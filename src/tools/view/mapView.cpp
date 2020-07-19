//#include "pch.h"
#define MAPVIEW
#ifdef _WIN32
#include <WinSock2.h>
#include <windows.h>
#else
#include <unistd.h>  
#include <sys/mman.h>
#include <sys/types.h>  
#include <sys/stat.h> 
#include <fcntl.h>
#endif
#include <string>
#include <string.h>
#include "mapView.h"
//---------------------------------------------------------------------------------------------
/*
使用方法
MapView mv;
mv.openfile(str.c_str());			//打开文件
char *buf = (char*)mv.mapview();	//获取映射内容
if (buf)
{
guiSetStr(_edit_cmdout, buf);
}

*/
//---------------------------------------------------------------------------------------------
#ifndef NO_MAPFILE
namespace hz
{
	mfile_t::mfile_t()
	{
	}

	mfile_t::~mfile_t()
	{
		unmap();
		close_m();
	}
#ifdef _WIN32

	bool mfile_t::open_m(const std::string& fn, bool is_rdonly, bool is_shared, bool is_async)
	{
		bool ret = true;
		_is_rdonly = is_rdonly;
		int dwcd = OPEN_EXISTING;
		_flags_o = GENERIC_READ;
		if (!is_rdonly)
		{
			_flags_o |= GENERIC_WRITE;
			dwcd = OPEN_ALWAYS;
		}
		_flags = FILE_SHARE_READ;
		if (is_shared)
		{
			_flags |= FILE_SHARE_WRITE;
		}
		//打开磁盘上的文件得到句柄   
		hFile = CreateFileA(fn.c_str(), _flags_o, _flags, NULL, dwcd, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		//判断文件是否创建成功 
		if (hFile == INVALID_HANDLE_VALUE)
		{
			_errstr = "openFile error: " + getLastError();
			ret = false;
		}
		else
		{
			_fsize = get_file_size();
			set_end(_fsize);
		}

		return ret;
	}

	int mfile_t::ftruncate_m(int64_t rs)
	{
		int ret = 0;
		if (hFile && rs > 0)
		{
			LARGE_INTEGER ds;
			ds.QuadPart = rs;
			ret = SetFilePointerEx(hFile, ds, NULL, FILE_BEGIN);
			SetEndOfFile(hFile);
		}
		return ret;
	}
	char* mfile_t::map(uint64_t ms, int64_t pos)
	{
		_msize = ms;
		if (!_pm && ms)
		{
			if (!hMapFile && pos == 0)
			{
				// 创建File mapping 
				hMapFile = CreateFileMapping(hFile, // 需要映射的文件的句柄 
					NULL, // 安全选项：默认 
					_is_rdonly ? PAGE_READONLY : PAGE_READWRITE, // 可读，可写 
					(DWORD)(ms >> 32), // mapping对象的大小，高位
					(DWORD)(ms & 0xFFFFFFFF),  // mapping对象的大小，低位 
					NULL); // mapping对象的名字 
				if (hMapFile == NULL)
				{
					_errstr = "CreateFileMapping error: " + getLastError();
				}
			}
			if (hMapFile)
			{
				_pm = (char*)MapViewOfFile(hMapFile, // mapping对象的句柄 
					_is_rdonly ? SECTION_MAP_READ : FILE_MAP_ALL_ACCESS, // 可读，可写 
					(DWORD)(pos >> 32), // 映射的文件偏移，高32位
					(DWORD)(pos & 0xFFFFFFFF),// 映射的文件偏移，低32位 
					ms); // 映射到View的数据大小 
				set(_pm, ms + pos);
				seek(0, SEEK_SET);
			}
		}
		return _pm;
	}
	int mfile_t::flush()
	{
		int ret = 0;
		if (_pm && _msize)
		{
			ret = FlushViewOfFile(_pm, _msize);
		}
		return ret;
	}
	int mfile_t::unmap(bool isclose)
	{
		int ret = 0;
		if (_pm && hMapFile)
		{
			ret = UnmapViewOfFile(_pm);
			if (isclose)
			{
				// 关闭mapping对象 
				if (!CloseHandle(hMapFile))
				{
					_errstr = "CloseMapping error: " + getLastError();
				}
				hMapFile = nullptr;
			}
			set(nullptr, 0);
			_pm = nullptr;
			_msize = 0;
		}
		return ret;
	}
	void mfile_t::close_m()
	{
		// 关闭mapping对象 
		if (hMapFile && !CloseHandle(hMapFile))
		{
			_errstr = "CloseMapping error: " + getLastError();
		}
		if (!CloseHandle(hFile))
		{
			_errstr = "CloseFile error: " + getLastError();
		}
		_fsize = 0;
	}

	uint64_t mfile_t::get_file_size()
	{
		uint64_t ret = 0;
		if (hFile)
		{
#if 0
			unsigned long dwsize = 0, dwheihgt = 0;
			dwsize = GetFileSize(hFile, &dwheihgt);
			ret = dwheihgt;
			ret << 32;
			ret |= dwsize;
#else
			LARGE_INTEGER ds = {};
			GetFileSizeEx(hFile, &ds);
			ret = ds.QuadPart;
#endif // 0
		}
		return ret;
	}


#else
	bool mfile_t::open_m(const std::string& fn, bool is_rdonly, bool is_shared, bool is_async)
	{
		bool ret = true;
		_is_rdonly = is_rdonly;

		_flags_o = is_async ? O_ASYNC : O_SYNC;
		if (is_rdonly)
		{
			_flags_o |= O_RDONLY;
			_fd = open(fn.c_str(), _flags_o);
		}
		else
		{
			_flags_o |= O_RDWR;
			_fd = open(fn.c_str(), _flags_o);
			if (_fd == -1)
			{
				_flags_o |= O_CREAT;
				_fd = open(fn.c_str(), _flags_o, 00666);
			}
		}

		if (_fd == -1)
		{
			ret = false;
		}
		else
		{
			_fsize = get_file_size();
			set_end(_fsize);
		}
		_fn = fn;
		_prot = _is_rdonly ? PROT_READ : PROT_READ | PROT_WRITE;
		_flags = is_shared ? MAP_SHARED : MAP_PRIVATE;
		_flags_fl = is_shared ? MS_ASYNC : MS_SYNC;
		return ret;
	}
	int mfile_t::ftruncate_m(int64_t rs)
	{
		int ret = 0;
		if (_fd && rs > 0)
			ret = ftruncate64(_fd, rs);
		return ret;
	}
	char* mfile_t::map(uint64_t ms, int64_t pos)
	{
		_msize = ms;
		if (!_pm && ms > 0)
		{
			_pm = (char*)mmap64(0, ms, _prot, _flags, _fd, pos);
			auto er = errno;
			set(_pm, ms + pos);
			seek(0, SEEK_SET);
		}
		return _pm;
	}
	int  mfile_t::flush()
	{
		int ret = 0;
		if (_pm)
			ret = msync(_pm, _msize, _flags_fl);
		return ret;
	}
	int mfile_t::unmap(bool isclose)
	{
		int ret = 0;
		if (_pm && _msize)
			ret = munmap(_pm, _msize);
		set(nullptr, 0);
		_pm = nullptr;
		_msize = 0;
		return ret;
	}

	void mfile_t::close_m()
	{
		if (_fd)
			close(_fd);
		_fd = -1;
	}


	uint64_t mfile_t::get_file_size()
	{
		uint64_t ret = 0;
		if (_fd)
		{
			ret = lseek64(_fd, 0L, SEEK_END);
			lseek(_fd, 0L, SEEK_SET);
		}
		return ret;
	}


#endif // _WIN32


	size_t mfile_t::write_m(const char* data, size_t len, bool up)
	{
		const char* my = nullptr;
		size_t ret = 0;
		if (_pm)
		{
			my = (char*)memcpy(_pm, data, len);
			seek(len, SEEK_CUR);
			if (up)
			{
				flush();
			}
			ret = len;
		}
		return ret;
	}

	uint64_t mfile_t::get_size()
	{
		return _fsize;
	}

	char* mfile_t::open_d(const std::string& fn, bool is_rdonly)
	{
		if (open_m(fn, is_rdonly))
		{
			return map(size(), 0);
		}
		return nullptr;
	}

	std::string mfile_t::getLastError()
	{
		std::string str;
#ifdef _WIN32

		char* buf = 0;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buf, 0, NULL);
		if (buf)
			str = buf;
		LocalFree(buf);
#else
		str = strerror(errno);
#endif // _WIN32
		return str;
	}
}//!hz

#endif //!NO_MAPFILE
