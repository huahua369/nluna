#ifndef __MAPVIEW__H__
#define __MAPVIEW__H__
#define MAPVIEW
#ifdef _WIN32
//#include <windows.h>
#else
#include <unistd.h>  
#include <sys/mman.h>
#endif
#include <string>
//---------------------------------------------------------------------------------------------
/*
ʹ�÷���
MapView mv;
mv.openfile(str.c_str());			//���ļ�
char *buf = (char*)mv.mapview();	//��ȡӳ������
if (buf)
{
guiSetStr(_edit_cmdout, buf);
}

*/
//---------------------------------------------------------------------------------------------
#ifdef _WIN32
namespace hz
{
	class MapView
	{
	protected:

		const int BUFFSIZE = 1024; // �ڴ��С 
		const int FILE_MAP_START = 0x28804; // �ļ�ӳ�����ʼ��λ�� 

	public:
#ifdef _WIN32
		static std::string getLastError()
		{
			char *buf = 0;
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
		~MapView()
		{
			UnmapView();
			closemap();
			closefile();
		}
		bool openfile(const char* lpcTheFile)
		{
#ifdef _WIN32
			// ��һ���ļ� 
			dwCreationDisposition = OPEN_EXISTING;
			hFile = CreateFile(lpcTheFile,
				GENERIC_READ | GENERIC_WRITE,                  //�򿪴����ϵĴ��ļ�A���õ����    
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, dwCreationDisposition, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			//�ж��ļ��Ƿ񴴽��ɹ� 
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
			void *start;
			struct stat sb;
			fd = open("/etc/passwd", O_RDONLY); /*��/etc/passwd */
			fstat(fd, &sb); /* ȡ���ļ���С */
			start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
			if (start == MAP_FAILED) /* �ж��Ƿ�ӳ��ɹ� */
#endif

		}
		bool createfile(const char* lpcTheFile)
		{
#ifdef _WIN32
			// ����һ���ļ� 
			dwCreationDisposition = CREATE_ALWAYS;
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
				_errstr = "CreateFile error: " + getLastError();
				return false;
			}
			getFileSize();
			creatmap();
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
			return dwFileSize;
#else

#endif

		}

		void* mapview()//����ӳ��λ��
		{
#ifdef _WIN32
			if (!dwFileSize)
			{
				getFileSize();
			}
			return mapview(0, dwFileSize);
#else

#endif

		}
		void* mapview(int64_t offset, int64_t Filesize)
		{
#ifdef _WIN32
			if (Filesize <= 0 || Filesize == -1)return 0;
			// ӳ��view 
			lpMapAddress = MapViewOfFile(hMapFile, // mapping����ľ�� 
				FILE_MAP_ALL_ACCESS, // �ɶ�����д 
				(DWORD)(offset >> 32), // ӳ����ļ�ƫ�ƣ���32λ
				(DWORD)(offset & 0xFFFFFFFF),// ӳ����ļ�ƫ�ƣ���32λ 
				Filesize); // ӳ�䵽View�����ݴ�С 
			if (lpMapAddress == NULL)
			{
				printf("MapViewOfFile error: %d\n",
					GetLastError());
				closemap();
				closefile();

				dwMapViewSize = 0;
				//  return 0; 
			}
			else
			{
				dwMapViewSize = Filesize;
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
#else

#endif

		}

		void FlushView()//����ӳ�����ݵ��ļ�
		{
#ifdef _WIN32
			FlushViewOfFile(lpMapAddress, dwMapViewSize);
#else

#endif

		}
		void FlushView(void* start, int64_t length)//����ָ��λ�úʹ�С���ļ�
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
			// �ر�mapping���� 
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
			//�ر��ļ� 
			if (isma[1])return;
			if (!CloseHandle(hFile))
			{
				_errstr = "CloseFile error: " + getLastError();
			}
			isma[1] = true;
#else

#endif

		}

		std::string _errstr;
		unsigned long dwCreationDisposition = OPEN_ALWAYS;
		//protected:
		void  creatmap()//�������ļ�һ����С��ӳ��
		{
#ifdef _WIN32
			creatmap(dwFileSize);
#else

#endif

		}
		void  creatmap(uint64_t Filesize)//����ָ����Сӳ��
		{
#ifdef _WIN32
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
	private:
		HANDLE hMapFile = nullptr; // �ļ��ڴ�ӳ������ľ�� 
		HANDLE hFile = nullptr; // �ļ��ľ�� 
	 //   DWORD dBytesWritten; // д����ֽ��� 
		int64_t dwFileSize = 0; // �ļ���С 
		unsigned long dwFileMapSize = 0; // �ļ�ӳ��Ĵ�С 
		int64_t dwMapViewSize = 0; // ��ͼ��View���Ĵ�С 
		unsigned long dwFileMapStart = 0; // �ļ�ӳ����ͼ����ʼλ�� 
		unsigned long dwSysGran = 0; // ϵͳ�ڴ��������� 
		SYSTEM_INFO SysInfo; // ϵͳ��Ϣ 
		void* lpMapAddress = 0; // ����ӳ���������ʼλ��   
		int iViewDelta = 0;
		bool isma[3]; //trueΪû�н��
	};
}
#endif
#endif /* end __MAPVIEW__H__*/