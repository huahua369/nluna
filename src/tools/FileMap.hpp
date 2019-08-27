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

#define BUFFSIZE 1024 // �ڴ��С 
#define FILE_MAP_START 0x28804 // �ļ�ӳ�����ʼ��λ�� 

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
			sprintf(time, "%02d��%02d:%02d:%02d:%03d", d, h, m, s, ms);
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
			//��ȡϵͳ��Ϣ���ڴ�������� 
			//��ȡ�������ȣ���������ļ������㣬 
			//Ŀ����Ϊ��ӳ���������ϵͳ�ڴ�������ȶ��룬����ڴ����Ч�� 
			/*GetSystemInfo(&SysInfo);
			//dwSysGran = SysInfo.dwAllocationGranularity;

			//����mapping����ʼλ��
			dwFileMapStart = (FILE_MAP_START / dwSysGran) * dwSysGran;
			// ����mapping view�Ĵ�С
			dwMapViewSize = (FILE_MAP_START % dwSysGran) + BUFFSIZE;
			// ����mapping�Ĵ�С
			dwFileMapSize = FILE_MAP_START + BUFFSIZE; //*/
			// ������Ҫ��ȡ�����ݵ�ƫ�� 
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
			// ��һ���ļ� 
			dwCreationDisposition = OPEN_EXISTING;
			hFile = CreateFile(lpcTheFile,
				GENERIC_READ | GENERIC_WRITE,                  //�򿪴����ϵĴ��ļ�A���õ����    
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, dwCreationDisposition, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			//�ж��ļ��Ƿ񴴽��ɹ� 
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
			// ����һ���ļ� 
			dwCreationDisposition = OPEN_ALWAYS;
			hFile = CreateFile(lpcTheFile,
				GENERIC_READ | GENERIC_WRITE,                  //�򿪴����ϵĴ��ļ�A���õ����    
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, dwCreationDisposition, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			/*  GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL); */
			//�ж��ļ��Ƿ񴴽��ɹ� 
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
		LPVOID mapview()//����ӳ��λ��
		{
			return mapview(0, dwFileSize);
		}
		LPVOID mapview(int64_t Filesize, SIZE_T dwNumberOfBytesToMap)
		{
			if (dwNumberOfBytesToMap <= 0 || dwNumberOfBytesToMap == -1)return 0;
			// ӳ��view 
			lpMapAddress = MapViewOfFile(hMapFile, // mapping����ľ�� 
				FILE_MAP_ALL_ACCESS, // �ɶ�����д 
				(DWORD)(Filesize >> 32), // ӳ����ļ�ƫ�ƣ���32λ
				(DWORD)(Filesize & 0xFFFFFFFF),// ӳ����ļ�ƫ�ƣ���32λ 
				dwNumberOfBytesToMap); // ӳ�䵽View�����ݴ�С 
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
			printf ("�ļ�map view������ļ�����ʼλ�ã� 0x%x\n",
			dwFileMapStart);
			printf ("�ļ�map view�Ĵ�С��0x%x\n", dwMapViewSize);
			printf ("�ļ�mapping����Ĵ�С��0x%x\n", dwFileMapSize);
			printf ("�������map view 0x%x �ֽڵ�λ�ö�ȡ���ݣ�", iViewDelta);

			// ��ָ�����ݵ�ָ��ƫ�ƣ��������ǹ��ĵĵط�
			// pData = (PCHAR) lpMapAddress ;//+ iViewDelta;
			// ��ȡ���ݣ���ֵ������
			//  iData = *(PINT)pData;
			// ��ʾ��ȡ������
			// printf ("Ϊ��0x%.8x\n", iData);

			// ��mapping�и������ݣ�32���ֽڣ�����ӡ
			//  CopyMemory(cMapBuffer,lpMapAddress,32);
			//  printf("lpMapAddress��ʼ��32�ֽ��ǣ�");
			for(i=0; i<32; i++)
			{
			printf("%.2x ",cMapBuffer[i]);
			}
			// ��mapping��ǰ32���ֽ���0xff���
			pData+=5;
			FillMemory(pData,32,(BYTE)0x35);
			// ��ӳ�������д�ص�Ӳ����
			FlushViewOfFile(lpMapAddress,dwMapViewSize);
			printf("\n�Ѿ���lpMapAddress��ʼ��32�ֽ�ʹ��0xff��䡣\n");

			*/
		}
		void FlushView()//����ӳ�����ݵ��ļ�
		{
			FlushViewOfFile(lpMapAddress, dwMapViewSize);
		}
		void FlushView(LPCVOID _lpMapAddress, SIZE_T _dwNumberOfBytesToFlush)//����ָ��λ�úʹ�С���ļ�
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
			// �ر�mapping���� 
			if (!CloseHandle(hMapFile))
			{
				//printf("\nclosing the mapping object error %d!", 
				GetLastError();
			}   isma[2] = true;
		}
		void closefile()
		{
			//�ر��ļ� 
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
		void  creatmap()//�������ļ�һ����С��ӳ��
		{
			creatmap(dwFileSize);
		}
		void  creatmap(int64_t Filesize)//����ָ����Сӳ��
		{
			if (Filesize <= 0 || Filesize == -1)return;
			// ����File mapping 
			hMapFile = CreateFileMapping(hFile, // ��Ҫӳ����ļ��ľ�� 
				NULL, // ��ȫѡ�Ĭ�� 
				PAGE_READWRITE, // �ɶ�����д 
				(DWORD)(Filesize >> 32), // mapping����Ĵ�С����λ
				(DWORD)(Filesize & 0xFFFFFFFF),  // mapping����Ĵ�С����λ 
				NULL); // mapping��������� 
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
		void  creatmap(int64_t Filesize, const char *name)//����ָ����Сӳ��
		{
			if (Filesize <= 0 || Filesize == -1)return;
			// ����File mapping 
			hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, // ��Ҫӳ����ļ��ľ�� 
				NULL, // ��ȫѡ�Ĭ�� 
				PAGE_READWRITE, // �ɶ�����д 
				(DWORD)(Filesize >> 32), // mapping����Ĵ�С����λ
				(DWORD)(Filesize & 0xFFFFFFFF),  // mapping����Ĵ�С����λ 
				name); // mapping��������� 
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
		HANDLE hMapFile = 0; // �ļ��ڴ�ӳ������ľ�� 
		HANDLE hFile = 0; // �ļ��ľ�� 
	 //   DWORD dBytesWritten; // д����ֽ��� 
		int64_t dwFileSize = 0; // �ļ���С 
		DWORD dwFileMapSize = 0; // �ļ�ӳ��Ĵ�С 
		int64_t dwMapViewSize = 0; // ��ͼ��View���Ĵ�С 
		DWORD dwFileMapStart = 0; // �ļ�ӳ����ͼ����ʼλ�� 
		DWORD dwSysGran; // ϵͳ�ڴ��������� 
		SYSTEM_INFO SysInfo; // ϵͳ��Ϣ 
		LPVOID lpMapAddress; // ����ӳ���������ʼλ��   
		INT iViewDelta;
		bool isma[3]; //trueΪû�н��
	};
#else
	class FileMap
	{
	private:
		int shmid = 0;
		void *shm = 0;
		bool isma[3]; //trueΪû�н��
		int isfm = 0;
		int _k = 0;
	public:
		FileMap()
		{
			isma[2] = true;
			isma[1] = true;
			isma[0] = true;
			//��ȡϵͳ��Ϣ���ڴ�������� 
			//��ȡ�������ȣ���������ļ������㣬 
			//Ŀ����Ϊ��ӳ���������ϵͳ�ڴ�������ȶ��룬����ڴ����Ч�� 
			/*GetSystemInfo(&SysInfo);
			//dwSysGran = SysInfo.dwAllocationGranularity;

			//����mapping����ʼλ��
			dwFileMapStart = (FILE_MAP_START / dwSysGran) * dwSysGran;
			// ����mapping view�Ĵ�С
			dwMapViewSize = (FILE_MAP_START % dwSysGran) + BUFFSIZE;
			// ����mapping�Ĵ�С
			dwFileMapSize = FILE_MAP_START + BUFFSIZE; //*/
			// ������Ҫ��ȡ�����ݵ�ƫ�� 
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
		void creatmap(int64_t Filesize, const char *name)//����ָ����Сӳ��
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
		//����ӳ��λ��
		void* mapview()
		{
			if (!shm)
				shm = shmat(shmid, (void*)0, 0);
			return shm;
		}
		//����ӳ�����ݵ��ļ�
		void FlushView()
		{
			// linux����Ҫ�ⲽ
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