#ifndef __MAPVIEW__H__
#define __MAPVIEW__H__
#define MAPVIEW
#ifdef _WIN32
//#include <windows.h>
#else
#include <unistd.h>  
#include <sys/mman.h>
#include <sys/types.h>  
#include <sys/stat.h> 
#include <fcntl.h>
#endif
#include <string>
#include "rw.h"
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
	class mfile_t :public rw_t
	{
	private:
#ifdef _WIN32
		HANDLE hFile = nullptr; // 文件的句柄 
		HANDLE hMapFile = nullptr; // 文件内存映射区域的句柄 
#else
		int _fd = 0;
#endif // _WIN32
		std::string _fn;
		bool _is_rdonly = true;
		int _prot = 0, _flags = 0, _flags_fl = 0, _flags_o = 0;
		char* _pm = nullptr;
		// 映射的大小
		uint64_t _msize = 0;
		// 文件大小
		uint64_t _fsize = 0;
		// 1M
		int _block = 1024 * 1024;
		std::string _errstr;
	public:
		mfile_t();
		~mfile_t();
		/*
		is_rdonly=true则需要文件存在
		is_shared=是否共享，window设置false则会共享读
		is_async在window无效
		*/
		bool open_m(const std::string& fn, bool is_rdonly, bool is_shared = false, bool is_async = true);
		uint64_t get_file_size();
		// 修改文件大小
		int ftruncate_m(int64_t rs);
		// 创建映射，文件空的话pos必需为0
		char* map(uint64_t ms, int64_t pos);
		int flush();
		int unmap(bool isclose = false);
		void close_m();
		// up是否更新到文件
		size_t write_m(const char* data, size_t len, bool up = false);
		uint64_t get_size();
		char* open_d(const std::string& fn, bool is_rdonly);
		static std::string getLastError();
	private:

	};

	class MapView :public rw_t
	{
	private:
#ifdef _WIN32

		unsigned long dwCreationDisposition = OPEN_ALWAYS;
		HANDLE hMapFile = nullptr; // 文件内存映射区域的句柄 
		HANDLE hFile = nullptr; // 文件的句柄 
		SYSTEM_INFO SysInfo = {}; // 系统信息 

#endif // _WIN32
	 //   DWORD dBytesWritten; // 写入的字节数 
		int64_t dwFileSize = 0; // 文件大小 
		unsigned long dwFileMapSize = 0; // 文件映射的大小 
		int64_t dwMapViewSize = 0; // 视图（View）的大小 
		unsigned long dwFileMapStart = 0; // 文件映射视图的起始位置 
		unsigned long dwSysGran = 0; // 系统内存分配的粒度 
		void* lpMapAddress = 0; // 内在映射区域的起始位置   
		int iViewDelta = 0;
		bool isma[3]; //true为没有解除
		std::string _errstr;
		bool _read_only = true;

	protected:

		const int BUFFSIZE = 1024; // 内存大小 
		const int FILE_MAP_START = 0x28804; // 文件映射的起始的位置 

	public:
#ifdef _WIN32
		static std::string getLastError()
		{
			char* buf = 0;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buf, 0, NULL);
			std::string str = buf;
			LocalFree(buf);
			return str;
		}
#endif

		MapView()
		{

			isma[2] = true;
			isma[1] = true;
			isma[0] = true;
			//获取系统信息，内存分配粒度 
			//获取分配粒度，进行下面的几个计算， 
			//目的是为了映射的数据与系统内存分配粒度对齐，提高内存访问效率 
			/*GetSystemInfo(&SysInfo);
			//dwSysGran = SysInfo.dwAllocationGranularity;

			//计算mapping的起始位置
			dwFileMapStart = (FILE_MAP_START / dwSysGran) * dwSysGran;
			// 计算mapping view的大小
			dwMapViewSize = (FILE_MAP_START % dwSysGran) + BUFFSIZE;
			// 计算mapping的大小
			dwFileMapSize = FILE_MAP_START + BUFFSIZE; //*/
			// 计算需要读取的数据的偏移 
			//    iViewDelta = FILE_MAP_START - dwFileMapStart; 
		}
		~MapView()
		{
			UnmapView();
			closemap();
			closefile();
		}
		static MapView* create()
		{
			return new MapView();
		}
		static void destroy(MapView* p)
		{
			if (p)
				delete p;
		}
		bool openfile(const char* lpcTheFile, bool read_only)
		{
#ifdef _WIN32
			// 打开一个文件 
			dwCreationDisposition = OPEN_EXISTING;
			uint32_t f1 = GENERIC_READ, f2 = FILE_SHARE_READ | FILE_SHARE_WRITE;
			if (!read_only)
			{
				f1 |= GENERIC_WRITE;
			}  //打开磁盘上的大文件A，得到句柄   
			hFile = CreateFile(lpcTheFile, f1, f2, NULL, dwCreationDisposition, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			//判断文件是否创建成功 
			if (hFile == INVALID_HANDLE_VALUE)
			{
				_errstr = "openFile error: " + getLastError();
				return false;
			}
			if (getFileSize() <= 0)return false;
			creatmap();
			isma[1] = false;
			return true;
#else
			int fd;
			void* start;
			struct stat sb;
			fd = open("/etc/passwd", O_RDONLY); /*打开/etc/passwd */
			fstat(fd, &sb); /* 取得文件大小 */
			start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
			if (start == MAP_FAILED) /* 判断是否映射成功 */
			{
				return false;
			}
			isma[1] = false;
			return true;
#endif

		}
		bool createfile(const char* lpcTheFile)
		{
#ifdef _WIN32
			// 创建一个文件 
			dwCreationDisposition = CREATE_ALWAYS;
			hFile = CreateFile(lpcTheFile,
				GENERIC_READ | GENERIC_WRITE,                  //打开磁盘上的大文件A，得到句柄    
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, dwCreationDisposition, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			/*  GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL); */
			//判断文件是否创建成功 
			if (hFile == INVALID_HANDLE_VALUE)
			{
				_errstr = "CreateFile error: " + getLastError();
				return false;
			}
			_read_only = false;
			getFileSize();
			if (dwFileSize < 1)
			{
				dwFileSize = 1024 * 4;
			}
			creatmap(dwFileSize);
			isma[1] = false;
#else

#endif
			return true;
		}
		int64_t getFileSize()
		{
#ifdef _WIN32
			if (dwFileSize != -1 && dwFileSize > 0)return dwFileSize;
			if (hFile)
			{
				unsigned long dwsize = 0, dwheihgt = 0;
				dwsize = GetFileSize(hFile, &dwheihgt);
				dwFileSize = dwheihgt;
				dwFileSize << 32;
				dwFileSize |= dwsize;
			}
			// printf("GetFileSize error: %d\n",  GetLastError() ); 

#else

#endif
			set_end(dwFileSize);
			return dwFileSize;
		}

		void* mapview()//返回映射位置
		{
#ifdef _WIN32
			if (!dwFileSize)
			{
				getFileSize();
			}
#else

#endif
			return mapview(0, dwFileSize);

		}
		void* mapview(int64_t offset, int64_t Filesize)
		{
#ifdef _WIN32
			if (Filesize <= 0 || Filesize == -1)return 0;
			// 映射view 
			lpMapAddress = MapViewOfFile(hMapFile, // mapping对象的句柄 
				_read_only ? SECTION_MAP_READ : FILE_MAP_ALL_ACCESS, // 可读，可写 
				(DWORD)(offset >> 32), // 映射的文件偏移，高32位
				(DWORD)(offset & 0xFFFFFFFF),// 映射的文件偏移，低32位 
				Filesize); // 映射到View的数据大小 
			if (lpMapAddress == NULL)
			{
				_errstr = "CloseMapping error: " + getLastError();
				//printf("MapViewOfFile error: %d\n", GetLastError());
				closemap();
				closefile();

				dwMapViewSize = 0;
				//  return 0; 
			}
			else
			{
				dwMapViewSize = Filesize;
			}
			set(lpMapAddress, dwMapViewSize);
			isma[0] = false;

#else

#endif
			return lpMapAddress;
			/*
			printf ("文件map view相对于文件的起始位置： 0x%x\n",
			dwFileMapStart);
			printf ("文件map view的大小：0x%x\n", dwMapViewSize);
			printf ("文件mapping对象的大小：0x%x\n", dwFileMapSize);
			printf ("从相对于map view 0x%x 字节的位置读取数据，", iViewDelta);

			// 将指向数据的指针偏移，到达我们关心的地方
			// pData = (PCHAR) lpMapAddress ;//+ iViewDelta;
			// 读取数据，赋值给变量
			//  iData = *(PINT)pData;
			// 显示读取的数据
			// printf ("为：0x%.8x\n", iData);

			// 从mapping中复制数据，32个字节，并打印
			//  CopyMemory(cMapBuffer,lpMapAddress,32);
			//  printf("lpMapAddress起始的32字节是：");
			for(i=0; i<32; i++)
			{
			printf("%.2x ",cMapBuffer[i]);
			}
			// 将mapping的前32个字节用0xff填充
			pData+=5;
			FillMemory(pData,32,(BYTE)0x35);
			// 将映射的数据写回到硬盘上
			FlushViewOfFile(lpMapAddress,dwMapViewSize);
			printf("\n已经将lpMapAddress开始的32字节使用0xff填充。\n");

			*/

		}
		char* get_map_ptr()
		{
			return (char*)lpMapAddress;
		}
		void FlushView()//更新映射内容到文件
		{
#ifdef _WIN32
			FlushViewOfFile(lpMapAddress, dwMapViewSize);
#else

#endif

		}
		void FlushView(void* start, int64_t length)//更新指定位置和大小到文件
		{
#ifdef _WIN32
			FlushViewOfFile(start, length);
#else
			munmap(start, length);
#endif
		}
		void UnmapView()
		{
#ifdef _WIN32
			if (isma[0])
				return;
			UnmapViewOfFile(lpMapAddress);
			isma[0] = true;
#else

#endif

		}

		bool getIsUn()
		{
			return isma[0];
		}
		void closemap()
		{
#ifdef _WIN32
			if (isma[2])return;
			// 关闭mapping对象 
			if (!CloseHandle(hMapFile))
			{
				_errstr = "CloseMapping error: " + getLastError();
			}   isma[2] = true;
#else

#endif

		}
		void closefile()
		{
#ifdef _WIN32
			//关闭文件 
			if (isma[1])return;
			if (!CloseHandle(hFile))
			{
				_errstr = "CloseFile error: " + getLastError();
			}
			isma[1] = true;
#else

#endif

		}

		//protected:
		void  creatmap()//创建和文件一样大小的映射
		{
#ifdef _WIN32
			creatmap(dwFileSize);
#else

#endif

		}
		void  creatmap(uint64_t Filesize)//创建指定大小映射
		{
#ifdef _WIN32
			if (Filesize <= 0 || Filesize == -1)return;
			// 创建File mapping 
			hMapFile = CreateFileMapping(hFile, // 需要映射的文件的句柄 
				NULL, // 安全选项：默认 
				_read_only ? PAGE_READONLY : PAGE_READWRITE, // 可读，可写 
				(DWORD)(Filesize >> 32), // mapping对象的大小，高位
				(DWORD)(Filesize & 0xFFFFFFFF),  // mapping对象的大小，低位 
				NULL); // mapping对象的名字 
					   // if (dwFileSize<=0)

			if (hMapFile == NULL)
			{
				_errstr = "CreateFileMapping error: " + getLastError();
				dwFileSize = 0;
			}
			else
			{
				dwFileSize = Filesize;
			}

			isma[2] = false;
#else

#endif

		}
	public:
		virtual int64_t write(const void* buf, int64_t len)
		{

			return rw_t::write(buf, len);
		}
		virtual int resize(int64_t s)
		{
			int ret = -1;
			if (s > 0)
			{
				if (dwFileSize < s)
					creatmap(s);
				mapview(0, s);
			}
			return ret;
		}
	private:
	};
}//!hz

#endif
#endif /* end __MAPVIEW__H__*/
