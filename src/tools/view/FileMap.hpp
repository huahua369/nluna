#ifndef __FileMap__h__
#define __FileMap__h__
//-----------------------------------------------------------------------------
#ifdef _WIN32
//#include <windows.h>
#include <io.h>
#include <direct.h>
#else

#include <sys/shm.h>
#endif

#define BUFFSIZE 1024 // 内存大小 
#define FILE_MAP_START 0x28804 // 文件映射的起始的位置 

namespace hz
{
#if 0
	static __int64 GetTime64()
	{
		static LARGE_INTEGER TicksPerSecond = { 0 };
		LARGE_INTEGER Tick;
		if (!TicksPerSecond.QuadPart)
		{
			QueryPerformanceFrequency(&TicksPerSecond);
		}
		QueryPerformanceCounter(&Tick);
		__int64 Seconds = Tick.QuadPart / TicksPerSecond.QuadPart;
		__int64 LeftPart = Tick.QuadPart - (TicksPerSecond.QuadPart*Seconds);
		__int64 MillSeconds = LeftPart * 1000 / TicksPerSecond.QuadPart;
		__int64 Ret = Seconds * 1000 + MillSeconds;
		return Ret;
	}
	static double GetTimeF()
	{
		static LARGE_INTEGER TicksPerSecond = { 0 };
		LARGE_INTEGER Tick;
		if (!TicksPerSecond.QuadPart)
		{
			QueryPerformanceFrequency(&TicksPerSecond);
		}
		QueryPerformanceCounter(&Tick);
		double Seconds = (double)Tick.QuadPart / TicksPerSecond.QuadPart;
		double LeftPart = (double)Tick.QuadPart - ((double)TicksPerSecond.QuadPart*Seconds);
		double MillSeconds = (double)LeftPart * 1000 / (double)TicksPerSecond.QuadPart;
		double Ret = (double)Seconds * 1000 + MillSeconds;
		return Ret;
	}
	static char* gettime(int64_t mss, int type = 0)
	{
		int d, h, m, s;
		int ms = mss / 1000;
		m = ms / 60;
		h = m / 60;
		//d = h / 24;
		//h %= 24;
		m %= 60;
		s = ms % 60;
		ms = mss % 1000;
		static	char time[256] = { 0 };
		//
		if (type == 5)
			sprintf(time, "%02d天%02d:%02d:%02d:%03d", d, h, m, s, ms);
		else if (type == 4)
			sprintf(time, "%02d:%02d:%02d:%03d", h, m, s, ms);
		else
			sprintf(time, "%02d:%02d:%02d", h, m, s);
		return time;
	}
#endif
#ifdef _WIN32
	class FileMap
	{
	public:


	public:
		FileMap()
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
		~FileMap()
		{
			//  FlushView();
			UnmapView();
			closemap();
			closefile();

		}
		BOOL openfile(LPCSTR lpcTheFile)
		{
			// 打开一个文件 
			dwCreationDisposition = OPEN_EXISTING;
			hFile = CreateFile(lpcTheFile,
				GENERIC_READ | GENERIC_WRITE,                  //打开磁盘上的大文件A，得到句柄    
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, dwCreationDisposition, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			//判断文件是否创建成功 
			if (hFile == INVALID_HANDLE_VALUE)
			{
				//printf("CreateFile error\n", GetLastError());
				return FALSE;
			}
			if (getFileSize() <= 0)return FALSE;
			creatmap();
			isma[1] = false;
			return TRUE;
		}
		void creatfile(LPCSTR lpcTheFile)
		{
			// 创建一个文件 
			dwCreationDisposition = OPEN_ALWAYS;
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
				//printf("CreateFile error\n", GetLastError());
				return;
			}
			getFileSize();
			creatmap();
			isma[1] = false;
		}
		int64_t getFileSize()
		{
			if (dwFileSize != -1 && dwFileSize > 0)return dwFileSize;
			if (hFile)
			{
				LARGE_INTEGER len64 = { 0 };
				if (GetFileSizeEx(hFile, &len64))
				{
					dwFileSize = len64.QuadPart;
				}
				/*DWORD dwsize = 0, dwheihgt = 0;
				dwsize = GetFileSize(hFile, &dwheihgt);
				dwFileSize = dwheihgt;
				dwFileSize << 32;
				dwFileSize |= dwsize;
				printf("s:%ud ,h:%ud\n", dwsize, dwheihgt);*/
			}
			// printf("GetFileSize error: %d\n",  GetLastError() ); 
			return dwFileSize;
		}
		LPVOID mapview()//返回映射位置
		{
			return mapview(0, dwFileSize);
		}
		LPVOID mapview(int64_t Filesize, SIZE_T dwNumberOfBytesToMap)
		{
			if (dwNumberOfBytesToMap <= 0 || dwNumberOfBytesToMap == -1)return 0;
			// 映射view 
			lpMapAddress = MapViewOfFile(hMapFile, // mapping对象的句柄 
				FILE_MAP_ALL_ACCESS, // 可读，可写 
				(DWORD)(Filesize >> 32), // 映射的文件偏移，高32位
				(DWORD)(Filesize & 0xFFFFFFFF),// 映射的文件偏移，低32位 
				dwNumberOfBytesToMap); // 映射到View的数据大小 
			if (lpMapAddress == NULL)
			{
				//printf("MapViewOfFile error: %d\n", GetLastError());
				closemap();
				closefile();

				dwMapViewSize = 0;
				//  return 0; 
			}
			else
			{
				dwMapViewSize = dwNumberOfBytesToMap;
			}
			isma[0] = false;

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
		void FlushView()//更新映射内容到文件
		{
			FlushViewOfFile(lpMapAddress, dwMapViewSize);
		}
		void FlushView(LPCVOID _lpMapAddress, SIZE_T _dwNumberOfBytesToFlush)//更新指定位置和大小到文件
		{
			if (-1 == _dwNumberOfBytesToFlush)
				_dwNumberOfBytesToFlush = dwMapViewSize;
			FlushViewOfFile(_lpMapAddress, _dwNumberOfBytesToFlush);
		}
		void UnmapView()
		{
			if (isma[0])
				return;
			UnmapViewOfFile(lpMapAddress);
			isma[0] = true;
		}
		bool getIsUn()
		{
			return isma[0];
		}
		void closemap()
		{
			if (isma[2])return;
			// 关闭mapping对象 
			if (!CloseHandle(hMapFile))
			{
				//printf("\nclosing the mapping object error %d!", 
				GetLastError();
			}   isma[2] = true;
		}
		void closefile()
		{
			//关闭文件 
			if (isma[1])return;
			if (!CloseHandle(hFile))
			{
				//printf("\nError %ld occurred closing the file!", 
				GetLastError();
			}
			isma[1] = true;
		}

		DWORD dwCreationDisposition = OPEN_ALWAYS;
		//protected:
		void  creatmap()//创建和文件一样大小的映射
		{
			creatmap(dwFileSize);
		}
		void  creatmap(int64_t Filesize)//创建指定大小映射
		{
			if (Filesize <= 0 || Filesize == -1)return;
			// 创建File mapping 
			hMapFile = CreateFileMapping(hFile, // 需要映射的文件的句柄 
				NULL, // 安全选项：默认 
				PAGE_READWRITE, // 可读，可写 
				(DWORD)(Filesize >> 32), // mapping对象的大小，高位
				(DWORD)(Filesize & 0xFFFFFFFF),  // mapping对象的大小，低位 
				NULL); // mapping对象的名字 
					   // if (dwFileSize<=0)

			if (hMapFile == NULL)
			{
				//printf("CreateFileMapping error: %d\n", GetLastError());
				dwFileSize = 0;
				//  1; 
			}
			else
			{
				dwFileSize = Filesize;
			}

			isma[2] = false;
		}
		void  creatmap(int64_t Filesize, const char *name)//创建指定大小映射
		{
			if (Filesize <= 0 || Filesize == -1)return;
			// 创建File mapping 
			hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, // 需要映射的文件的句柄 
				NULL, // 安全选项：默认 
				PAGE_READWRITE, // 可读，可写 
				(DWORD)(Filesize >> 32), // mapping对象的大小，高位
				(DWORD)(Filesize & 0xFFFFFFFF),  // mapping对象的大小，低位 
				name); // mapping对象的名字 
					   // if (dwFileSize<=0)

			if (hMapFile == NULL)
			{
				//printf("CreateFileMapping error: %d\n", GetLastError());
				dwFileSize = 0;
				//  1; 
			}
			else
			{
				dwFileSize = Filesize;
			}

			isma[2] = false;
		}
		void openmap(int64_t Filesize, const char *name)
		{
			if (hMapFile)
				CloseHandle(hMapFile);
			hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
			if (hMapFile)
			{
				dwFileSize = Filesize;
				isma[2] = false;
			}
		}
		int key()
		{
			return (int)hMapFile;
		}
	private:
		HANDLE hMapFile = 0; // 文件内存映射区域的句柄 
		HANDLE hFile = 0; // 文件的句柄 
	 //   DWORD dBytesWritten; // 写入的字节数 
		int64_t dwFileSize = 0; // 文件大小 
		DWORD dwFileMapSize = 0; // 文件映射的大小 
		int64_t dwMapViewSize = 0; // 视图（View）的大小 
		DWORD dwFileMapStart = 0; // 文件映射视图的起始位置 
		DWORD dwSysGran; // 系统内存分配的粒度 
		SYSTEM_INFO SysInfo; // 系统信息 
		LPVOID lpMapAddress; // 内在映射区域的起始位置   
		INT iViewDelta;
		bool isma[3]; //true为没有解除
	};
#else
	class FileMap
	{
	private:
		int shmid = 0;
		void *shm = 0;
		bool isma[3]; //true为没有解除
		int isfm = 0;
		int _k = 0;
	public:
		FileMap()
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
		~FileMap()
		{
			if (shm)
				shmdt(shm);
			if (shmid&&isfm == 1)
			{
				shmctl(shmid, IPC_RMID, 0);
			}
			//  FlushView();
			//UnmapView();
			//closemap();
			//closefile();

		}
	public:
		int key()
		{
			return _k;
		}
		void creatmap(int64_t Filesize, const char *name)//创建指定大小映射
		{
			key_t k = std::hash<std::string>()(name);
			_k = k;
			shmid = shmget(k, Filesize, 0666 | IPC_CREAT);
			isma[2] = false; isfm = 1;
		}
		void openmap(int64_t Filesize, const char *name)
		{
			key_t k = std::hash<std::string>()(name);
			_k = k;
			shmid = shmget(k, Filesize, 0666 | IPC_CREAT);
			isma[2] = false;
			isfm = 2;
		}
		//返回映射位置
		void* mapview()
		{
			if (!shm)
				shm = shmat(shmid, (void*)0, 0);
			return shm;
		}
		//更新映射内容到文件
		void FlushView()
		{
			// linux不需要这步
		}
		void FlushView(const void* _lpMapAddress, int64_t _dwNumberOfBytesToFlush)
		{

		}
		bool getIsUn()
		{
			return true;
		}
	private:
	};

#endif

};
#endif //! __FileMap__h__