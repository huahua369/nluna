#ifndef __MEM__H__
#define __MEM__H__
#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <errno.h>
#include <sys/stat.h>  
#include <sys/types.h>  
#ifndef _WIN32
#include <unistd.h> 
#include <sys/ipc.h>  
#include <sys/shm.h>  
#define PERM S_IRUSR|S_IWUSR  
#endif
/*
进程间内存共享数据通讯

int main()
{
	char szMsg[2048];
	char a = 0;
	a = getchar();
	bool iw = a == 't';
#ifdef _WIN32
	SetConsoleTitle(iw?"写":"读");
#endif
	hz::MemShare ms("mems", 2048, iw);
	printf("key:%d\n", ms.key());
	int i = 0;
	while (1)
	{
		a = getchar();
		if (a == 'q')
		{
			break;
		}
		if (iw)
		{
			snprintf(szMsg, sizeof(szMsg), "hello %d", ++i);
			if (ms.write(szMsg, strlen(szMsg) + 1, 200) > 0)
			{
				printf("write msg is [%s]\n", szMsg);
			}
			else
			{
				printf("Wait for timeout!\n");
			}
		}
		else
		{
			if (ms.read(szMsg, 2048, 1000) > 0)
			{
				printf("msg from writer is [%s]\n", szMsg);
			}
			else
			{
				printf("Wait for timeout, not data!\n");
			}
		}
	}
	return 0;
}
*/
#define M_null								0
#define M_write                             0x0001                //写
#define M_read                              0x0002                //读
#define M_data								0x0004				  //有数据
namespace hz
{
	struct Mdata
	{
		int maxsize = 0;				//缓冲区总大小
		int bufsize = 0;				//缓冲区大小
		int dlen = 0;					//数据大小
		int status = 0;			    	//M_null
		int type = 0;
		char *data = 0;					//缓冲区首地址
	};
	class MemShare
	{
	public:
		MemShare(const char *name, int bufsize, bool isSer = true)
		{
			if (isSer)
			{
				//outfile.creatfile(hl::FileMap::getAppPathN(name).c_str());
				outfile.creatmap(bufsize, name);
			}
			else
			{
				//outfile.openfile(hl::FileMap::getAppPathN(name).c_str());
				outfile.openmap(bufsize, name);
			}
			md = (Mdata*)outfile.mapview();
			if (isSer && md)
			{
				md->maxsize = bufsize;
				md->data = ((char*)md) + 100;// sizeof(Mdata);
				md->bufsize = bufsize - sizeof(Mdata);
				md->status = 0;
				Flush();
			}
		}
		~MemShare()
		{
		}
		int write(const char *buf, int len, int wait = -1)
		{
			if (!waitW(0, wait))return -1;
			modtype(M_write);
			mapv();
			md->dlen = len;
			md->data = ((char*)md) + 100;
			char rb = buf[len - 1];
			memcpy(md->data, buf, len);
			modtype(M_data);
			return len;
		}
		int read(void *buf, int len, int wait = -1)
		{
			if (buf)
			{
				if (!waitR(M_data, wait))return -1;
				modtype(M_read);
				mapv();
				//if (len > md->dlen)
				len = md->dlen;
				md->data = ((char*)md) + 100;
				memcpy(buf, md->data, len);
				modtype(M_null);
				return len;
			}
			return 0;
		}
		int key()
		{
			return outfile.key();
		}
	protected:
		void mapv()
		{
			if (outfile.getIsUn())
			{
				md = (Mdata*)outfile.mapview();
				if (md)
				{
					int len = 100;// sizeof(Mdata);
					md->data = ((char*)md) + len;
				}
			}
		}
		bool waitW(int wtype = M_null, unsigned int cs = -1)
		{
			unsigned int c = 0;
			while (1)
			{
				mapv();
				if (md)
				{
					if (md->status == wtype)
						break;
				}
				//outfile.UnmapView();
				sleep(1);
				if (cs <= c++)
				{
					return false;
				}
			}
			return true;
		}
		bool waitR(int wtype = M_null, unsigned int cs = -1)
		{
			unsigned int c = 0;
			while (1)
			{
				mapv();
				if (md)
				{
					if (md->status == wtype)
						break;
					//outfile.UnmapView();
				}
				sleep(1);
				if (cs <= c++)
				{
					return false;
				}
			}
			return true;
		}
		void Flush(int size = -1)
		{
			outfile.FlushView(md, size);
			//outfile.UnmapView();
		}
		void modtype(int type)
		{
			md->status = type;
			if (type == M_data)
				Flush();
			else
				Flush(sizeof(Mdata));
		}
		static void sleep(int s)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(s));
		}
	private:
		Mdata		*md = 0;
		FileMap      outfile;
		int ofmsize = 0;
	};
};
#if 0
int main1()
{
	char szMsg[2048];
	char a = 0;
	a = getchar();
	bool iw = a == 't';
#ifdef _WIN32
	SetConsoleTitle(iw ? "写" : "读");
#endif
	hz::MemShare ms("mems", 2048, iw);
	printf("key:%d\n", ms.key());
	int i = 0;
	while (1)
	{
		if (iw)
		{
			a = getchar();
			if (a == 'q')
			{
				break;
			}
			snprintf(szMsg, sizeof(szMsg), "hello %d", ++i);
			if (ms.write(szMsg, strlen(szMsg) + 1, 200) > 0)
			{
				printf("write msg is [%s]\n", szMsg);
			}
			else
			{
				printf("Wait for timeout!\n");
			}
		}
		else
		{
			if (ms.read(szMsg, 2048, 1000) > 0)
			{
				printf("msg from writer is [%s]\n", szMsg);
			}
			else
			{
				//printf("Wait for timeout, not data!\n");
			}
		}
	}
	return 0;
}
#endif
#endif //!__MEM__H__