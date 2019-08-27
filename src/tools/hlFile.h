#ifndef __FILE__h__
#define __FILE__h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <set>
#include <list>
#include <regex>
#ifdef _WIN32
#include <io.h>
#include <ShlObj.h>
#elif defined(__ANDROID__)
#include <unistd.h>  
#include <dirent.h>
#else
#ifdef __GLIBC__
#undef __GLIBC__
#endif
#include <linux/stat.h>
#include <unistd.h>  
#include <dirent.h>
#include <sys/param.h>

#endif
#include <sys/types.h>  
#include <sys/stat.h>  
#include <errno.h>  

#include "Singleton.h"
#include "mapView.h"
#include "FileMap.hpp"
#include "mem.h"
#include "hlUtil.h"
#ifndef LOBYTE
#define LOBYTE(w)           ((unsigned char)(((size_t)(w)) & 0xff))
#define GetRValue(rgb)      (LOBYTE(rgb))
#define GetGValue(rgb)      (LOBYTE(((unsigned short)(rgb)) >> 8))
#define GetBValue(rgb)      (LOBYTE((rgb)>>16))
#endif // !LOBYTE

#ifdef _WIN32
#ifndef S_IRWXU
#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001
#endif
#endif

extern void* a_mgr;
/*

 */
namespace hz {
	class File :public Singleton<File>
	{
	private:
		std::set<std::string> dv;
	public:
		File()
		{
		}

		~File()
		{
		}
		void addDir(std::string dir)
		{
			dv.insert(dir);
		}
		static std::string getAP()
		{
			std::string ret;
#ifdef _WIN32
			char szAppPath[1024] = { 0 };
			GetModuleFileNameA(GetModuleHandleA(""), (char*)szAppPath, 1024);
			(strrchr((char*)szAppPath, '\\'))[1] = 0;
			ret = szAppPath;
#else
			// linux获取本进程目录
			size_t len = PATH_MAX;
			char processdir[PATH_MAX];
			char processname[1024];
			char* path_end;
			std::vector<std::string> vs;
			if (readlink("/proc/self/exe", processdir, len) <= 0)
				return ret;
			path_end = strrchr(processdir, '/');
			if (path_end == NULL)
				return ret;
			++path_end;
			strcpy(processname, path_end);
			*path_end = '\0';
			size_t l = (size_t)(path_end - processdir);
			vs.push_back(processdir);
			vs.push_back(processname);
			ret = processdir;
			//ret = getenv("HOME");
			//ret += "/";
#endif
			return ret;
		}
		static std::string getAP(std::string name, std::string pat = "")
		{
			std::string rap = getAP();
			std::string stp = rap.find('/') != -1 ? "/" : "\\";
			if (pat != "")
			{
				rap += pat + stp;
			}
			rap += name;
			return rap;
		}
		// 获取路径，去掉尾部n个文件夹
		static std::string getDic(const std::string& dic, int last = 1)
		{
			auto drs = hz::hstring::split_m(dic, "\\/");
			std::string sfg = dic.find('/') != std::string::npos ? "/" : "\\";
			for (int i = 0; i < last; i++)
			{
				drs.pop_back();
			}
			auto rdic = hz::jsonT::join(drs, sfg) + sfg;
			return rdic;
		}

		// 后台运行
		static void init_daemon()
		{
			int pid;
			int i;
#ifndef _WIN32
			pid = fork();
			if (pid < 0)
				exit(1);  //创建错误，退出
			else if (pid > 0) //父进程退出
				exit(0);
			setsid(); //使子进程成为组长
			pid = fork();
			if (pid > 0)
				exit(0); //再次退出，使进程不是组长，这样进程就不会打开控制终端
			else if (pid < 0)
				exit(1);
			//关闭进程打开的文件句柄
			for (i = 0; i < NOFILE; i++)
				close(i);
			signal(SIGHUP, SIG_IGN);
#endif
			return;
		}
#ifndef _WIN32
		static void _splitpath(const char* path, char* drive, char* dir, char* fname, char* ext)
		{
			drive[0] = '\0';
			dir[0] = '\0';
			fname[0] = '\0';
			ext[0] = '\0';
			if (NULL == path)
			{
				return;
			}
			const char* ps = path;
			if (path[1] == ':')
			{
				char d[8] = { path[0],':',0 };
				strcpy(drive, d);
				ps = path + 2;
			}
			if ('/' == ps[strlen(ps)])
			{
				strcpy(dir, ps);
				fname[0] = '\0';
				ext[0] = '\0';
				return;
			}
			auto tem = hstring::split_m(path, "/\\");
			std::string pstr = path, dirs;

			if (tem.size() > 0)
			{
				auto name = *tem.rbegin();
				auto pos = name.rfind('.');
				if (pos != -1)
				{
					std::string exts = name.substr(pos);
					strcpy(ext, exts.c_str());
					if (pos > 0)
						memcpy(fname, name.c_str(), pos);
				}
				pos = pstr.find(name);
				if (pos > 0)
				{
					memcpy(dir, pstr.c_str(), pos);
				}
			}
		}
		static void _splitpath0(const char* path, char* drive, char* dir, char* fname, char* ext)
		{
			char* p_whole_name;

			drive[0] = '\0';
			if (NULL == path)
			{
				dir[0] = '\0';
				fname[0] = '\0';
				ext[0] = '\0';
				return;
			}

			if ('/' == path[strlen(path)])
			{
				strcpy(dir, path);
				fname[0] = '\0';
				ext[0] = '\0';
				return;
			}

			p_whole_name = strchr((char*)path, '/');
			if (NULL != p_whole_name)
			{
				p_whole_name++;
				_split_whole_name(p_whole_name, fname, ext);

				snprintf(dir, p_whole_name - path, "%s", path);
			}
			else
			{
				_split_whole_name(path, fname, ext);
				dir[0] = '\0';
			}
		}

		static void _split_whole_name(const char* whole_name, char* fname, char* ext)
		{
			char* p_ext;

			p_ext = strchr((char*)whole_name, '.');
			if (NULL != p_ext)
			{
				strcpy(ext, p_ext);
				snprintf(fname, p_ext - whole_name + 1, "%s", whole_name);
			}
			else
			{
				ext[0] = '\0';
				strcpy(fname, whole_name);
			}
		}
#endif

#ifndef _MAX_PATH
		//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		//
		// Path Manipulation
		//
		//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		// Sizes for buffers used by the _makepath() and _splitpath() functions.
		// note that the sizes include space for 0-terminator
#define _MAX_PATH   260 // max. length of full pathname
#define _MAX_DRIVE  3   // max. length of drive component
#define _MAX_DIR    256 // max. length of path component
#define _MAX_FNAME  256 // max. length of file name component
#define _MAX_EXT    256 // max. length of extension component
#endif
		enum PathE
		{
			pathdrive = 0x01,
			pathdir = 0x02,
			pathfname = 0x04,
			pathext = 0x08,
			path_all = pathdrive | pathdir,
		};
		static std::string getPath(const char* path_buffer, unsigned int ty = pathext)
		{
			//    char path_buffer0[_MAX_PATH];
			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];
			// _makepath( path_buffer0, "c", "\\sample\\crt\\", "makepath", "c" );
			//    printf( "Path created with _makepath: %s\n\n", path_buffer );
			if (!(path_buffer && *path_buffer))
				return "";

			//_splitpath0(path_buffer, drive, dir, fname, ext);
			_splitpath(path_buffer, drive, dir, fname, ext);
#if 0
			printf("Path extracted with _splitpath:\n");
			printf("  Drive: %s\n", drive);
			printf("  Dir: %s\n", dir);
			printf("  Filename: %s\n", fname);
			printf("  Ext: %s\n", ext);
#endif
			std::string ret;
			if (ty & pathdrive)ret = drive;
			if (ty & pathdir)ret += dir;
			if (ty & pathfname)ret += fname;
			if (ty & pathext)ret += ext;
			return ret;
		}
		//如果文件具有指定的访问权限，则函数返回0  
		//如果文件不存在或者不能访问指定的权限，则返回-1  

		//备注  
		//当path为文件时，_access函数判断文件是否存在，并判断文件是否可以用mode值指定的模式进行访问  
		//当path为目录时，_access只判断指定的目录是否存在，在WindowsNT和Windows2000中，所有目录都有读写权限  

		//mode值  
		//00    只检查文件是否存在  
		//02    写权限  
		//04    读权限  
		//06    读写权限

		static bool fn_access(const char* fn)
		{
			return (access(fn, 0) != -1);
		}
		//验证
		static std::string getFn(std::string fnc, bool isutf8 = false)
		{
			std::string tem = fnc;
#ifndef __ANDROID__
			if (0 != access(fnc.c_str(), 0))
			{
				std::set<std::string> fv;
				fv.insert(getAP(fnc));
				for (auto it : s()->dv)
					fv.insert(getAP(fnc, it));
				fnc = "";
				for (auto& it : fv)
				{
					if (0 == access(it.c_str(), 0))
					{
						fnc = it;
						break;
					}
				}
			}
#endif
			if (fnc == "")
			{
				fnc = tem;
			}
#ifdef strutil
			return isutf8 ? hz::strutil::AnsiToUtf8(fnc.c_str()) : fnc;
#else
			return fnc;
#endif
		}
		// 验证目录是否存在，不存在则创建
		static void check_make_path(const std::string& filename, unsigned int mod = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
		{
			char* file_name = (char*)filename.c_str();
			char* t = file_name;
			char chr = '/';
			char* ch = strchr(t, '\\');
			if (ch)
			{
				chr = *ch;
			}

			for (; t;)
			{
				t = strchr(++t, chr);
				if (t)
					* t = 0;
				if (access(file_name, 0) != -1)
				{
					if (t)
						* t = chr;
					continue;
				}
#ifdef _WIN32
				mkdir(file_name);
#else
				mkdir(file_name, mod);
#endif
				if (t)
					* t = chr;
			}
		}
		static bool read_binary_file(std::string filename, std::vector<char>& result, bool ismv = false)
		{
			int64_t size = 0;
			uint64_t  retval;
			void* buff = 0;
			result.clear();
			//LOGW(("read_binary_file0:" + filename).c_str());
#if (defined(VK_USE_PLATFORM_IOS_MVK) || defined(VK_USE_PLATFORM_MACOS_MVK))
			filename = [[[NSBundle mainBundle]resourcePath] stringByAppendingPathComponent:@(filename)] .UTF8String;
#endif
			filename = s()->getFn(filename);
#ifdef _WIN32
			if (filename.find("/"))
			{
				filename = hstring::replace(filename, "/", "\\");
			}
#endif
			const char* fn = filename.c_str();
			FILE* fp = ismv ? 0 : fopen(filename.c_str(), "rb");
			if (!fp)
			{
#ifdef __ANDROID__

				LOGE("func=%s,line=%d,file=%s\n", __FUNCTION__, __LINE__, __FILE__);
				LOGI("a_mgr%p,%s", a_mgr, filename.c_str());
				AAsset* asset = AAssetManager_open((AAssetManager*)a_mgr, filename.c_str(), AASSET_MODE_STREAMING);

				assert(asset);
				if (!asset)return false;
				size_t size = AAsset_getLength(asset);
				assert(size > 0);
				LOGI("%p", size);
				result.resize(size);
				void* textureData = result.data();
				AAsset_read(asset, textureData, size);
				AAsset_close(asset);
				LOGW("read_binary_file1: %d", (int)size);
				return true;
#else
#ifdef _WIN32
				hz::MapView mv;
				if (mv.openfile(fn))			//打开文件					
				{
					size = mv.getFileSize();
					char* buf = (char*)mv.mapview();	//获取映射内容
					if (buf)
					{
						result.resize(size);
						memcpy(&result[0], buf, size);
						return true;
					}
				}
#endif
				//LOGW(("fail to open file: " + filename).c_str());
				return false;
#endif
			}
#ifdef _WIN32
			_fseeki64(fp, 0L, SEEK_END);
			size = _ftelli64(fp);
			_fseeki64(fp, 0L, SEEK_SET);
#else			
			fseeko64(fp, 0L, SEEK_END);
			size = ftello64(fp);
			fseeko64(fp, 0L, SEEK_SET);
#endif // _WIN32
			result.resize(size);
			buff = &result[0];
			retval = fread(buff, size, 1, fp);
			assert(retval == 1);
			fclose(fp);
			return true;
		}
		static bool save_binary_file(std::string filename, const char* data, uint64_t size, uint64_t pos = 0, bool is_plus = false)
		{
			uint64_t  retval;
			if (!data || !size)
			{
				return false;
			}
			//LOGW(("read_binary_file0:" + filename).c_str());
#if (defined(VK_USE_PLATFORM_IOS_MVK) || defined(VK_USE_PLATFORM_MACOS_MVK))
			filename = [[[NSBundle mainBundle]resourcePath] stringByAppendingPathComponent:@(filename)] .UTF8String;
#endif
			//filename = s()->getFn(filename);
#ifdef _WIN32
			if (filename[1] != ':')
				filename = getAP(filename);
#endif // _WIN32
			check_make_path(getPath(filename.c_str(), pathdrive | pathdir));
			const char* fn = filename.c_str();
			FILE* fp = fopen(filename.c_str(), is_plus ? "ab" : "wb");

			if (!fp && !is_plus)
			{
#ifdef _WIN32 
				hz::MapView mv;
				if (mv.openfile(fn) || mv.createfile(fn))			//打开文件					
				{
					mv.creatmap(size);
					char* buf = (char*)mv.mapview();	//获取映射内容
					if (buf)
					{
						memcpy(buf, data, size);
						mv.FlushView();
						return true;
					}
				}
#endif
				return false;// "fail to open file: " + filename;
			}
#ifdef _WIN32
			if (is_plus)
				_fseeki64(fp, pos, SEEK_SET);
#else			
			if (is_plus)
				fseeko64(fp, pos, SEEK_SET);
#endif // _WIN32
			retval = fwrite(data, size, 1, fp);
			assert(retval == 1);
			fclose(fp);
			return true;
		}
		static std::vector<char> read_binary_file(std::string filename, bool ismv = false)
		{
			std::vector<char> ret;
			read_binary_file(filename, ret, ismv);
			return ret;
		}
#ifndef NO_BF 
#ifdef _WIN32
		static int CALLBACK _SHBrowseForFolderCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
		{
			switch (uMsg)
			{
			case BFFM_INITIALIZED:
				::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);   //传递默认打开路径 break;  
			case BFFM_SELCHANGED:    //选择路径变化，BFFM_SELCHANGED  
			{
				char curr[MAX_PATH];
				SHGetPathFromIDList((LPCITEMIDLIST)lParam, (LPSTR)(LPCTSTR)curr);
				::SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)& curr[0]);
			}
			break;
			default:
				break;
			}
			return 0;
		}
#endif
		static int BrowseForFolder(const std::string& strCurrentPath, std::function<void(const std::string&)> rfunc, const std::string& title = "")
		{
#ifdef _WIN32
			BROWSEINFO bi;
			char Buffer[MAX_PATH];
			char szResult[MAX_PATH];
			ITEMIDLIST* ppidl;
			HWND hWnd = GetForegroundWindow();
			SHGetSpecialFolderLocation(hWnd, CSIDL_DRIVES, &ppidl);
			if (ppidl == NULL)
				MessageBox(hWnd, "启动路径浏览失败", "提示", MB_OK);

			//初始化入口参数bi开始  
			bi.hwndOwner = hWnd;
			bi.pidlRoot = ppidl;//根目录  
			bi.pszDisplayName = Buffer;//此参数如为NULL则不能显示对话框  
			bi.lpszTitle = title.c_str();//\0浏览文件夹";//下标题  
			bi.ulFlags = //BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
				BIF_RETURNONLYFSDIRS | BIF_USENEWUI /*包含一个编辑框 用户可以手动填写路径 对话框可以调整大小之类的..*/ |
				BIF_UAHINT /*带TIPS提示*/ | BIF_NONEWFOLDERBUTTON /*不带新建文件夹按钮*/;
			bi.lpfn = _SHBrowseForFolderCallbackProc;
			//bi.iImage=IDR_MAINFRAME;  
			bi.lParam = LPARAM(strCurrentPath.c_str());    //设置默认路径  

														   //初始化入口参数bi结束  
			LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//调用显示选择对话框  
			if (pIDList)
			{
				//取得文件夹路径到Buffer里  
				SHGetPathFromIDList(pIDList, szResult);
				//sFolderPath就是我们选择的路径  

				rfunc((std::string)szResult);
				printf("Select path %s\n", szResult);
			}

			LPMALLOC lpMalloc;
			if (FAILED(SHGetMalloc(&lpMalloc)))
				return -1;
			lpMalloc->Free(pIDList);
			lpMalloc->Release();
#endif
			return 0;
		}
#ifdef _WIN32
		static int openFileName(const std::string& strCurrentPath, const char* filter, HWND hWnd, std::function<void(const std::vector<std::string>&)> rfunc)
		{
			OPENFILENAME opfn;
			CHAR strFilename[MAX_PATH * 100];//存放文件名  
											 //初始化  
			ZeroMemory(&opfn, sizeof(OPENFILENAME));
			opfn.lStructSize = sizeof(OPENFILENAME);//结构体大小  
			opfn.hwndOwner = hWnd;

			//设置过滤  
			opfn.lpstrFilter = filter ? filter : "所有文件\0*.*\0\0文本文件\0*.txt\0";
			//默认过滤器索引设为1  
			opfn.nFilterIndex = 1;
			//文件名的字段必须先把第一个字符设为 \0  
			opfn.lpstrFile = strFilename;
			opfn.lpstrFile[0] = '\0';
			opfn.nMaxFile = sizeof(strFilename);
			//设置标志位，检查目录或文件是否存在  
			opfn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT;
			opfn.lpstrInitialDir = strCurrentPath.c_str();
			// 显示对话框让用户选择文件  
			if (GetOpenFileName(&opfn))
			{
				//在文本框中显示文件路径  
				if (rfunc)
				{
					char* t = strFilename;
					std::string st;
					std::vector<std::string> sfn;
					for (; *t;)
					{
						st = t;
						t += st.size() + 1;
						sfn.push_back(st);
					}
					if (sfn.size() > 1)
					{
						std::string dirs = sfn[0] + "\\";
						std::vector<std::string> ts;
						for (size_t i = 1; i < sfn.size(); i++)
						{
							ts.push_back(dirs + sfn[i]);
						}
						sfn = ts;
					}
					rfunc(sfn);
				}
			}
			return 0;
		}
#endif

#endif
		/*static void deleteBom(std::string &str)
		{
			//UTF-8
			char utf8[] = { 0xEF ,0xBB ,0xBF };
			//UTF-16（大端序）
			char utf16d[] = { 0xFE ,0xFF };
			//UTF-16（小端序）
			char utf16[] = { 0xFF ,0xFE };
			//UTF-32（大端序）
			char utf32d[] = { 0x00 ,0x00 ,0xFE ,0xFF };
			//UTF-32（小端序）
			char utf32[] = { 0xFF ,0xFE ,0x00 ,0x00 };

			int bomlen[] = { 3,2,2,4,4 };
			char *bom[] = { utf8,utf16d,utf16,utf32d,utf32 };
			int u = -1;
			for (int i = 0; i < 5; ++i)
			{
				if (0 == memcmp(str.c_str(), bom[i], bomlen[i]))
				{
					u = i;
				}
			}
			if (u >= 0)
				str.replace(0, bomlen[u], "");
		}*/

		bool isFileExistInternal(const std::string& strFilePath) const
		{
			if (strFilePath.empty())
			{
				return false;
			}

			bool bFound = false;

			// Check whether file exists in apk.

#ifdef __ANDROID__
			if (strFilePath[0] != '/')
			{
				const char* s = strFilePath.c_str();
				if (strFilePath.find("assets/") == 0)
				{
					s += 7;
				}
				AAsset* aa = AAssetManager_open((AAssetManager*)a_mgr, s, AASSET_MODE_UNKNOWN);
				if (aa)
				{
					bFound = true;
					AAsset_close(aa);
				}
			}
			else
#endif
			{
				FILE* fp = fopen(strFilePath.c_str(), "r");
				if (fp)
				{
					bFound = true;
					fclose(fp);
				}
			}
			return bFound;
		}

#ifdef __ANDROID__
		bool isDirectoryExistInternal(const std::string& dirPath) const
		{
			if (dirPath.empty())
			{
				return false;
			}

			const char* s = dirPath.c_str();
			bool startWithAssets = (dirPath.find("assets/") == 0);
			int lenOfAssets = 7;

			// find absolute path in flash memory
			if (s[0] == '/')
			{
				struct stat st;
				if (stat(s, &st) == 0)
				{
					return S_ISDIR(st.st_mode);
				}
			}
			// find it in apk's assets dir
			// Found "assets/" at the beginning of the path and we don't want it

			if (startWithAssets)
			{
				s += lenOfAssets;
			}
			AAssetDir* aa = AAssetManager_openDir((AAssetManager*)a_mgr, s);
			if (aa && AAssetDir_getNextFileName(aa))
			{
				AAssetDir_close(aa);
				return true;
			}
			return false;
		}
#endif
		// 返回值:  
		// 0:成功,指定的文件或目录路径存在  
		// ENOENT(2):指定的文件或目录路径不存在  
		// EINVAL(22):参数非法  
		// ENODEV(19):指定的路径存在,但是类型与指定的类型不一致  
		// 其他:其他错误  
		int IsPathExist(std::string strPathName, bool bCheckFilePath = true)
		{
			std::string strPathTmp(strPathName);
#ifdef _WIN32			
			if (1 == strPathTmp.length()) // 驱动器盘符  
			{
				strPathTmp += (":\\");
			}
			else if (2 == strPathTmp.length() && strPathTmp[1] == ':')
			{
				strPathTmp += ("\\");
			}
			struct _stat stPath = { 0 };
			int nRet = ::_stat(strPathTmp.c_str(), &stPath);
#else
			struct stat stPath = { 0 };
			int nRet = ::stat(strPathTmp.c_str(), &stPath);

#endif

			do
			{
				if (0 != nRet)
				{
					nRet = errno;
					break;
				}

				bool IsFilePath = (S_IFREG == (S_IFREG & stPath.st_mode));
				bool IsDirPath = (S_IFDIR == (S_IFDIR & stPath.st_mode));

				if ((bCheckFilePath && !IsFilePath)
					|| (!bCheckFilePath && !IsDirPath))
				{
					nRet = ENODEV;
				}

			} while (false);

			return nRet;
		}

		static char* stristr(const char* str1, const char* str2)
		{
			char* cp = (char*)str1;
			char* s1, * s2;

			if (!*str2)
				return((char*)str1);

			while (*cp)
			{
				s1 = cp;
				s2 = (char*)str2;

				while (*s1 && *s2)
				{
					char ch1 = *s1, ch2 = *s2;
					if (isascii(*s1) && isupper(*s1)) ch1 = _tolower(*s1);
					if (isascii(*s2) && isupper(*s2)) ch2 = _tolower(*s2);

					if (ch1 - ch2 == 0) s1++, s2++;
					else break;
				}
				if (!*s2)
					return(cp);
				cp++;
			}
			return(NULL);
		}

		static int IterFiles(std::string srcPath, std::vector<std::string>* vfn = 0, njson * sundry = 0, std::string ext = "")//std::function<void(const char*fn, const char* dirfn)> func)
		{
			int Status = 0; int inc = 0;
#ifdef _WIN32
			std::list<std::string> vdir;
			auto vexts = hstring::split(ext, ";");
			std::set<std::string> sv;
			std::vector<std::string> afs;
			std::regex rg1("^[A-Za-z0-9//_.]+$"); std::smatch r1;
			for (auto it : vexts)
			{
				auto rk = hstring::toLower(it);
				if (rk.size() && rk.find("*") != -1)
					rk = hstring::replace(rk, "*", "((?:.*))");// "[A-Za-z0-9//]+");
				sv.insert(rk);
			}
			do {
				_finddata_t fd;
				if (srcPath[srcPath.size() - 1] != '\\' && srcPath[srcPath.size() - 1] != '/')
				{
					srcPath.push_back('\\');
				}
				std::string dpath = srcPath + "*", srcNewPath = srcPath;
				intptr_t pf = _findfirst(dpath.c_str(), &fd);
				if (pf < 0)
				{
					//break;
				}
				else
				{
					std::string fdn;
					do {
						fdn = fd.name;
						//if (strcmp(fd.name, ".") == 0 || strcmp(fd.name, "..") == 0)
						if (fdn == "." || fdn == "..")
						{
							continue;
						}
						else if (fd.attrib & _A_SUBDIR)
						{
							srcNewPath = srcPath + fd.name;
							vdir.push_back(srcNewPath);
							inc++;
						}
						else
						{
							if (ext != "" && ext != "*")
							{
								std::string fdnl = hstring::toLower(fdn);
								afs.push_back(fdnl);
								int ii = 0;
								for (auto it : sv)
								{
									std::string rk = it;
									try
									{
										std::regex reg1("^" + rk + "$");
										std::smatch r1;
										if (std::regex_match(fdnl, r1, reg1))
										{
											ii++;
										}
									}
									catch (const std::exception& e)
									{
										printf("router message: %s\n", e.what());
									}
								}
								if (!ii)
								{
									continue;
								}
								//auto fext = getPath(fdnl.c_str());
								//if (sv.find(fext) == sv.end())
								//	continue;
							}
							/* do something */
							//func(fd.name, srcPath.c_str());
							if (sundry)
							{
								njson fit;
								fit["path"] = srcPath;
								fit["name"] = fd.name;
								fit["size"] = fd.size;
								fit["time_access"] = fd.time_access;
								fit["time_create"] = fd.time_create;
								fit["time_write"] = fd.time_write;
								fit["attrib"] = fd.attrib;
								sundry->push_back(fit);
							}
							if (vfn)
								vfn->push_back(srcPath + fd.name);
						}
						if (Status != 0)
						{
							break;
						}
					} while (!_findnext(pf, &fd));
					_findclose(pf);
				}
				if (vdir.size())
				{
					srcPath = *vdir.begin();
					vdir.pop_front();
				}
			} while (vdir.size());
#else
			DIR* dir = 0;
			if (srcPath[srcPath.size() - 1] != '/')
			{
				srcPath.push_back('/');
			}
			if (srcPath[0] != '/')
			{
				srcPath = getAP() + srcPath;
			}
			if ((dir = opendir(srcPath.c_str())) == NULL)
			{
				//assert(false);
				return Status;
			}
			std::string srcNewPath = srcPath;
			struct dirent* d_ent;
			char fullpath[256];
			while ((d_ent = readdir(dir)) != NULL)
			{
				struct stat file_stat;
				// if ( strncmp(d_ent->d_name, ".", 1) == 0 )  
				// {  
				//  continue;   // 忽略"."目录  
				// }  			

				memset(fullpath, '\0', sizeof(fullpath));
				strcpy(fullpath, srcPath.c_str());
				if (!strcmp(fullpath, "/"))
				{
					fullpath[0] = '\0';
				}
				strcat(fullpath, "/");
				strcat(fullpath, d_ent->d_name);
				if (lstat(fullpath, &file_stat) < 0)
				{
					//assert(false);
					return Status;
				}
				//保存信息到自己的数据结构，在函数外面保存文件名  
				if (sundry)
				{
					njson fit;
					fit["path"] = srcPath;
					fit["name"] = d_ent->d_name;
					fit["blksize"] = file_stat.st_blksize;
					fit["size"] = file_stat.st_size;
					// 访问时间
					fit["time_access"] = file_stat.st_atime;
					// 修改文件权限时间
					fit["ctime"] = file_stat.st_ctime;
					// 修改时间
					fit["time_write"] = file_stat.st_mtime;
					fit["mode"] = file_stat.st_mode;
					sundry->push_back(fit);
				}
				if (vfn)
					vfn->push_back(d_ent->d_name);
			}
			closedir(dir);
#endif
			return inc;
		}
		static njson getTsort(njson vs, const std::string& like, int t = 1, const std::string k = "time_write")
		{
			njson ret;
			if (vs.is_array())
			{
				std::map<int64_t, njson> out;
				for (auto it : vs)
				{
					auto md = toInt(it["mode"]);
					auto name = toStr(it["name"]);
					if (like.size() && name.find(like) == std::string::npos)
					{
						continue;
					}
					if ((t == 1) && md & S_IFDIR)
					{
						continue;
					}
					if ((t == 2) && md & S_IFREG)
					{
						continue;
					}
					out[toInt(it[k])] = it;
				}
				for (auto it : out)
				{
					ret.push_back(it.second);
				}
			}
			return ret;
		}
	public:
#ifdef _WIN32
#ifdef _WATCH_H_
		static int inotify(const std::vector<std::string>& pathname, uint32_t mask = 0)
		{
			CDirectoryWatch::createWatchFile(jsonT::AtoW(pathname[0]));
			return 0;
		}
#endif
#else
		static int inotify(const std::vector<std::string>& pathname, uint32_t mask = IN_ALL_EVENTS)
		{
#define EVENT_NUM 12  

			char* event_str[EVENT_NUM] =
			{
				"IN_ACCESS",
				"IN_MODIFY",
				"IN_ATTRIB",
				"IN_CLOSE_WRITE",
				"IN_CLOSE_NOWRITE",
				"IN_OPEN",
				"IN_MOVED_FROM",
				"IN_MOVED_TO",
				"IN_CREATE",
				"IN_DELETE",
				"IN_DELETE_SELF",
				"IN_MOVE_SELF"
			};
			int fd;
			int wd;
			int len;
			int nread;
			char buf[BUFSIZ];
			struct inotify_event* event;

			fd = inotify_init();
			if (fd < 0)
			{
				fprintf(stderr, "inotify_init failed\n");
				return -1;
			}
			for (auto it : pathname)
			{
				wd = inotify_add_watch(fd, it.c_str(), mask);
				if (wd < 0)
				{
					fprintf(stderr, "inotify_add_watch %s failed\n", it.c_str());
					//return -1;
				}
			}

			buf[sizeof(buf) - 1] = 0;
			while ((len = read(fd, buf, sizeof(buf) - 1)) > 0)
			{
				nread = 0;
				while (len > 0)
				{
					event = (struct inotify_event*) & buf[nread];
					for (int i = 0; i < EVENT_NUM; i++)
					{
						if ((event->mask >> i) & 1)
						{
							if (event->len > 0)
								fprintf(stdout, "%s --- %s\n", event->name, event_str[i]);
							else
								fprintf(stdout, "%s --- %s\n", " ", event_str[i]);
						}
					}
					nread = nread + sizeof(struct inotify_event) + event->len;
					len = len - sizeof(struct inotify_event) - event->len;
				}
			}

			return 0;
		}

#endif // _WIN32
	private:
	};



	//DLL动态加载
#ifdef _WIN32
	class DllLoader
	{
	public:
		std::string dllname;
		void* dll = 0;
		std::vector<void*> funclist;
		std::map<std::string, void*> objfunc;
	public:

		static DllLoader* loadDlld3d(std::string d, std::vector<std::string>* vecfunc, bool isOrigin = false)
		{
			static std::once_flag flag;
			static njson ds;
			static std::map<std::string, DllLoader*> dlln;

			auto dt = dlln.find(d);
			if (dt != dlln.end())
			{
				if (vecfunc)
				{
					dt->second->GetFuncs(*vecfunc);
					//LOGPrint(1, "DLL[%s]函数数量：%d", d.c_str(), (int)vecfunc->size());
				}
				return dt->second;
			}
			std::call_once(flag, [&]() {
				auto dij = File::getAP("d3d_info.b");
				auto dsd = File::read_binary_file(dij);
				if (dsd.size())
				{
					try
					{
						//dsd.push_back(0);
						ds = njson::from_cbor(dsd.begin(), dsd.end());
						//ds = njson::parse(dsd.data());
					}
					catch (njson::exception& e)
					{
						// output exception information  
						njson errordata = { {"error","json error"},{"message",e.what()},{"exception_id",e.id} };
						printf("line:%d\t%s\n", __LINE__, errordata.dump(2).c_str());
					}
				}
				if (ds.empty())
				{
					ds = enum_sys_file("d3d*dll", true);
					//auto dstr = ds.dump(2);
					if (ds.size())
					{
						auto xj = njson::to_cbor(ds);
						hz::File::save_binary_file(dij, (char*)xj.data(), xj.size());
						//File::save_binary_file(dij, dstr.data(), dstr.size());
					}
				}
				});
			std::map<int64_t, std::string> dln;
			for (auto it : ds)
			{
				std::string fn = toStr(it["name"]);
				fn = hstring::toLower(fn);
				if (fn.find(d) != std::string::npos)
				{
					dln[toInt(it["time_write"])] = toStr(it["path"]) + fn;
				}
			}
			if (dln.empty())
			{
				return nullptr;
			}
			DllLoader* dl = new DllLoader();
			dl->dllname = d;
			//LOGPrint(1, "枚举DLL[%s]", d.c_str());
			std::string dllname = dln.rbegin()->second;
			dl->dll = LoadLibraryA(dllname.c_str());
			//dl->enumFile(isOrigin);
			if (vecfunc)
			{
				dl->GetFuncs(*vecfunc);
				//LOGPrint(1, "DLL[%s]函数数量：%d", d.c_str(), (int)vecfunc->size());
			}
			return dl;
		}
		static void freeDll(DllLoader* p)
		{
			delete p;
		}

		void GetFuncs(std::vector<std::string>& vecfunc)
		{
			for (auto it : vecfunc)
			{
				void* f = GetFunc(it.c_str());
				objfunc[it] = f;
				funclist.push_back(f);
			}
		}
		void* GetFunc(const char* funcname)
		{
			return dll ? GetProcAddress((HMODULE)dll, funcname) : 0;
		}

		DllLoader()
		{
		}
		~DllLoader()
		{
			if (dll)
				FreeLibrary((HMODULE)dll);
		}
	private:
		//不区分大小字符串比较
		char* stristr(const char* str1, const char* str2)
		{
			char* cp = (char*)str1;
			char* s1, * s2;

			if (!*str2)
				return((char*)str1);

			while (*cp)
			{
				s1 = cp;
				s2 = (char*)str2;

				while (*s1 && *s2)
				{
					char ch1 = *s1, ch2 = *s2;
					if (isascii(*s1) && isupper(*s1)) ch1 = _tolower(*s1);
					if (isascii(*s2) && isupper(*s2)) ch2 = _tolower(*s2);

					if (ch1 - ch2 == 0) s1++, s2++;
					else break;
				}
				if (!*s2)
					return(cp);
				cp++;
			}
			return(NULL);
		}
#define key __time64_t
#define value char*
		typedef struct _KeyValue
		{
			key _key;
			value _value;
		}KeyValue;
		typedef struct _kvNode
		{
			KeyValue* d;
			struct _kvNode* next;
		}kvNode;
#ifdef value
		value KVcreateValue(const char* str)
		{
			return strdup(str);
		}
		void KVdeleteValue(value v)
		{
			if (v)
			{
				free(v);
			}
		}
#endif
		KeyValue* KVcreate(key k, value v)
		{
			KeyValue* kv = (KeyValue*)calloc(1, sizeof(KeyValue));
			kv->_key = k; kv->_value = KVcreateValue(v);
			return kv;
		}
		void KVdestroy(KeyValue* p)
		{
			if (p)
			{
				KVdeleteValue(p->_value);
				free(p);
			}
		}

		kvNode* KVcreateNode(key k, value v)
		{
			kvNode* p = (kvNode*)calloc(1, sizeof(kvNode));
			KeyValue* d = KVcreate(k, v);
			if (p && d)
			{
				p->d = d;
			}
			return p;
		}
		void KVdeleteNode(kvNode* p)
		{
			if (p && p->d)
			{
				KVdestroy(p->d);
			}
			free(p);

		}
		void KVdeleteAllNode(kvNode* p)
		{
			if (p)
			{
				kvNode* t;
				t = p->next;
				KVdeleteNode(p);
				for (; t;)
				{
					p = t;
					t = p->next;
					KVdeleteNode(p);
				}
			}
		}
		int KVless(kvNode* p1, kvNode* p2)
		{
			return (p1->d->_key > p2->d->_key);
		}
		kvNode* InsertSort(kvNode* head, kvNode* n)
		{
			kvNode* first; /*为原链表剩下用于直接插入排序的节点头指针*/
			kvNode* t; /*临时指针变量：插入节点*/
			kvNode* p; /*临时指针变量*/
			kvNode* q; /*临时指针变量*/

			first = n;// head->next; /*原链表剩下用于直接插入排序的节点链表：可根据图12来理解。*/
					  //head->next = NULL; /*只含有一个节点的链表的有序链表：可根据图11来理解。*/

			while (first != NULL) /*遍历剩下无序的链表*/
			{
				/*注意：这里for语句就是体现直接插入排序思想的地方*/
				for (t = first, q = head; ((q != NULL) && KVless(q, t)); p = q, q = q->next); /*无序节点在有序链表中找插入的位置*/

																							  /*退出for循环，就是找到了插入的位置*/
																							  /*注意：按道理来说，这句话可以放到下面注释了的那个位置也应该对的，但是就是不能。原因：你若理解了上面的第3条，就知道了。*/
				first = first->next; /*无序链表中的节点离开，以便它插入到有序链表中。*/

				if (q == head) /*插在第一个节点之前*/
				{
					head = t;
				}
				else /*p是q的前驱*/
				{
					p->next = t;
				}
				t->next = q; /*完成插入动作*/
							 /*first = first->next;*/
			}
			return head;
		}
		void KVaddNode(kvNode** head, key k, value v, int sort)
		{
			kvNode* p = KVcreateNode(k, v);
			if (*head)
			{
				if (sort)
					* head = InsertSort(*head, p);
			}
			else
			{
				*head = p;
			}
		}

		//-----------------------------------------------------------------------------------------------------------
		/*
		_A_ARCH（存档）

		_A_HIDDEN（隐藏）

		_A_NORMAL（正常）

		_A_RDONLY（只读）

		_A_SUBDIR（文件夹）

		_A_SYSTEM（系统）
		*/
#ifndef __cplusplus
#ifdef _finddata_t
#undef _finddata_t
#endif
#ifdef _finddatai64_t
#undef _finddatai64_t
#endif
#ifdef _USE_32BIT_TIME_T
#define _finddata_t     _finddata32_t
#define _finddatai64_t  _finddata32i64_t
#else
#define _finddata_t     struct _finddata64i32_t
#define _finddatai64_t  struct __finddata64_t
#endif
#endif
		int IterFiles(const char* srcPath, kvNode** headout)
		{
			int Status = 0;
			_finddata_t fd;
			char* dpath = strdup(srcPath);
			dpath = (char*)calloc(strlen(srcPath) + 10, sizeof(char*));
			strcpy(dpath, srcPath);
			strcat(dpath, "/*");

			char* srcNewPath = (char*)calloc(strlen(srcPath) + 255, sizeof(char*));
			strcpy(srcNewPath, srcPath);
			intptr_t pf = _findfirst(dpath, &fd);
			do {
				sprintf(srcNewPath, "%s%s%s", srcPath, "/", fd.name);
				if (strcmp(fd.name, ".") == 0 || strcmp(fd.name, "..") == 0)
				{
					continue;
				}
				else if (fd.attrib & 0x10)//_A_SUBDIR)
				{
					Status = IterFiles(srcNewPath, headout);
				}
				else
				{
					/* do something */
					//查找dll
					if (stristr(fd.name, dllname.c_str()) && stristr(fd.name, ".dll"))
					{
						KVaddNode(headout, fd.time_write, fd.name, 1);
					}
				}
				if (Status != 0)
				{
					break;
				}
			} while (pf != -1 && !_findnext(pf, &fd));
			_findclose(pf);
			free(dpath);
			free(srcNewPath);
			return Status;
		}
		void* enumFile(bool isOrigin = false)
		{
			void* r = 0;
#ifdef _WIN32
			char sWinDir[1255];
			GetSystemDirectoryA(sWinDir, sizeof(sWinDir));
			kvNode* node = 0;
			std::vector<std::string> vecdir;
			if (isOrigin)
				vecdir.push_back(File::getAP());
			vecdir.push_back(sWinDir);
			for (auto it : vecdir)
			{
				sprintf(sWinDir, "%s", it.c_str());
				IterFiles(sWinDir, &node);
				if (node)
				{
					strcat(sWinDir, "\\");
					strcat(sWinDir, node->d->_value);
					r = LoadLibraryA(sWinDir);
					//printf("+++++++++\n++++++++%s \n", sWinDir);
					KVdeleteAllNode(node);
					break;
				}
			}
#endif
			dll = r;
			return r;
		}
		static njson enum_sys_file(const std::string& ext, bool isOrigin = false)
		{
			void* r = 0;
#ifdef _WIN32
			char sWinDir[1255];
			GetSystemDirectoryA(sWinDir, sizeof(sWinDir));
			std::vector<std::string> vecdir;
			if (isOrigin)
				vecdir.push_back(File::getAP());
			vecdir.push_back(sWinDir);
			std::vector<std::string> vfn;
			njson jfn;
			for (auto it : vecdir)
			{
				sprintf(sWinDir, "%s", it.c_str());
				File::IterFiles(sWinDir, 0, &jfn, ext);
			}
#endif
			return jfn;
		}

#undef key
#undef value
#if 0
		void* enumFile0()
		{
			char sWinDir[255];
			GetWindowsDirectory(sWinDir, sizeof(sWinDir));
			std::string sd = sWinDir;
			sd += "\\Fonts";
			std::vector<std::string> vf;
			File::IterFiles(sd, vf); /*[&vf](const char*fn, const char* dirfn) {
				vf.push_back(fn);
			});*/
		}
#endif
		//---------------------------------------------------------------------------------------------------------------------------------

	};
#endif
}
#endif //__FILE__h__