#ifndef _WATCH_H_
#define _WATCH_H_
#ifdef _WIN32
namespace hz {

	// 通用的通知处理函数  
	typedef void(*funNotifyAction)(DWORD dwAction, LPWSTR wzSrcFile, LPWSTR wzDestFile);

	// 要监控的目录的信息  
	typedef struct _WATCH_PARAM
	{
		_WATCH_PARAM()
		{
			hFile = INVALID_HANDLE_VALUE;
			hEvent = NULL;
			//hWatch = NULL;
			//pBuffer = NULL;
			bIsExit = FALSE;
			dwBufferSize = 0;
			NotifyAction = NULL;
			ZeroMemory(&ol, sizeof(OVERLAPPED));
		}
		std::wstring  wzPath; // 路径  
		HANDLE hFile;            // 文件句柄  
		HANDLE hEvent;           // 事件句柄  
		std::vector<BYTE>  pBuffer;          // 缓存区  
		DWORD  dwBufferSize;     // 缓存区大小  
		OVERLAPPED ol;           // 异步结构  

		std::thread hWatch;           // 监控线程句柄  
		BOOL   bIsExit;          // 监控线程是否退出  

		funNotifyAction NotifyAction;

	}WATCH_PARAM, * PWATCH_PARAM;
	typedef enum _MyMSG
	{
		MSG_SUCCESS = (WM_USER)+0x100,
		MSG_STARTWATCH,
		MSG_STOPWATCH,
		MSG_EXIT
	};
#define MAX_BUFFER_SIZE 2048
	// 监控类  
	class CDirectoryWatch
	{
	public:
		CDirectoryWatch()
		{

		}
		virtual ~CDirectoryWatch()
		{
			// 释放资源  
			if (!m_vWatchList.empty())
			{
				for (auto it : m_vWatchList)
				{
					StopWatch(it.first);
				}
			}

			PostThreadMessageW(m_dwMSGTID, MSG_EXIT, NULL, NULL); // 退出 MSGThred 线程  
			WaitForSingleObject(m_hMSG, INFINITE);
			CloseHandle(m_hMSG);

			wprintf(L"<DBG> ~CDirectoryWatch() success! Close the MSGThred...\r\n");
		}

	public:
		BOOL StartWatch(LPWSTR wzPath, funNotifyAction lpfunNotifyAction)
		{
			if (wzPath == NULL && wzPath[0] == 0 && lpfunNotifyAction == NULL && !PathFileExistsW(wzPath))
				return FALSE;


			if (!PostThreadMessageW(m_dwMSGTID, MSG_STARTWATCH, (WPARAM)wzPath, (LPARAM)lpfunNotifyAction))
			{
				auto erstr = getLastError();
				wprintf(L"<ERR> StartWatch()->PostThreadMessageW() fail! ErrorCode:0x%X\r\n", GetLastError());
				return FALSE;
			}

			return TRUE;
		}
		BOOL StopWatch(const std::wstring& wzPath)
		{
			if (wzPath.empty() == NULL && !PathFileExistsW(wzPath.c_str()))
				return FALSE;

			/*BOOL bIsExist = FALSE;
			std::list<WATCH_PARAM*>::iterator iBegin = m_vWatchList.begin();
			std::list<WATCH_PARAM*>::iterator iEnd = m_vWatchList.end();
			while (iBegin != iEnd)
			{
				if (wcscmp((*iBegin)->wzPath, wzPath) == 0)
				{
					bIsExist = TRUE;
					break;
				}
				++iBegin;
			}

			if (!bIsExist)
			{
				wprintf(L"<ERR> StopWatch() fail! %s 没有监控...\r\n", wzPath);
				return FALSE;
			}

			if (!PostThreadMessageW(m_dwMSGTID, MSG_STOPWATCH, (WPARAM)wzPath, NULL))
			{
				wprintf(L"<ERR> StopWatch()->PostThreadMessageW() fail! ErrorCode:0x%X\r\n", GetLastError());
				return FALSE;
			}*/

			return TRUE;
		}

	private:
		unsigned  MSGThred(LPVOID lpParam)   // 消息线程  
		{
			MSG msg;
			while (GetMessageW(&msg, NULL, 0, 0))
			{
				switch (msg.message)
				{
				default:
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					break;

					// 开启监控  
				case MSG_STARTWATCH:
				{
					WATCH_PARAM* SwatchParam = new(std::nothrow) WATCH_PARAM;
					if (SwatchParam == 0)
					{
						wprintf(L"<ERR> MSGThred()->New buffer fail!\r\n");
						break;
					}
					//ZeroMemory(SwatchParam, sizeof(WATCH_PARAM));

					SwatchParam->wzPath = (LPCWSTR)msg.wParam;
					SwatchParam->hFile = CreateFileW(
						SwatchParam->wzPath.c_str(),
						FILE_LIST_DIRECTORY,
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, // 注意这里的 异步标志  
						NULL
					);
					if (SwatchParam->hFile == INVALID_HANDLE_VALUE)
					{
						auto erstr = getLastError();
						wprintf(L"<ERR> MSGThred()->CreateFileW fail! ErrorCode:0x%X\r\n", GetLastError());
						delete[] SwatchParam;
						break;
					}
					SwatchParam->hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
					if (SwatchParam->hEvent == NULL)
					{
						auto erstr = getLastError();
						wprintf(L"<ERR> MSGThred()->CreateEventW fail! ErrorCode:0x%X\r\n", GetLastError());
						CloseHandle(SwatchParam->hFile);
						delete[] SwatchParam;
						break;
					}
					SwatchParam->ol.hEvent = SwatchParam->hEvent;

					// 接收通知信息的缓存区  
					SwatchParam->pBuffer.resize(MAX_BUFFER_SIZE);
					SwatchParam->dwBufferSize = MAX_BUFFER_SIZE;
					SwatchParam->NotifyAction = (funNotifyAction)msg.lParam;

					// 开启监控线程 每个文件都有一个  
					std::thread thr([=]() {
						WatchThred(SwatchParam);
						});
					SwatchParam->hWatch.swap(thr);
					LOCK(_hMutex);
					m_vWatchList.insert(std::pair<std::wstring, PWATCH_PARAM>(SwatchParam->wzPath, SwatchParam)); // 添加到监控列表 便于控制
					wprintf(L"<DBG> StartWatch success!-> %s\r\n", SwatchParam->wzPath.c_str());
					break;
				}

				// 停止监控  
				case MSG_STOPWATCH:
				{
					std::wstring wzPath = (WCHAR*)msg.wParam;
					LOCK(_hMutex);
					auto it = m_vWatchList.find(wzPath);
					if (it != m_vWatchList.end())
					{
						auto nt = it->second;
						nt->bIsExit = TRUE;
						SetEvent(nt->hEvent); // 设置状态 不让 监控函数堵塞住  
						//WaitForSingleObject((*iBegin)->hWatch, INFINITE);

						CloseHandle(nt->hFile);
						CloseHandle(nt->hEvent);

						wprintf(L"<DBG> StopWatch success!-> %s\r\n", nt->wzPath.c_str());

						delete nt;
						m_vWatchList.erase(it);
					}

					break;
				}
				case MSG_EXIT:
				{
					//_endthreadex(0);
					return 0;
				}
				}
			}
			return 0;
		}

		static std::string getLastError()
		{
			char* buf = 0;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buf, 0, NULL);
			std::string str = buf;
			LocalFree(buf);
			return str;
		}
	public:
		static void createWatchFile(const std::wstring& wpath)
		{
			WATCH_PARAM* SwatchParam = new(std::nothrow) WATCH_PARAM;
			if (SwatchParam == 0)
			{
				wprintf(L"<ERR> MSGThred()->New buffer fail!\r\n");
				return;
			}
			//ZeroMemory(SwatchParam, sizeof(WATCH_PARAM));

			SwatchParam->wzPath = wpath;
			SwatchParam->hFile = CreateFileW(
				SwatchParam->wzPath.c_str(),
				FILE_LIST_DIRECTORY,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_BACKUP_SEMANTICS,//| FILE_FLAG_OVERLAPPED, // 注意这里的 异步标志  
				NULL
			);
			if (SwatchParam->hFile == INVALID_HANDLE_VALUE)
			{
				auto erstr = getLastError();
				wprintf(L"<ERR> MSGThred()->CreateFileW fail! ErrorCode:0x%X\r\n", GetLastError());
				delete[] SwatchParam;
				return;
			}
			SwatchParam->hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
			if (SwatchParam->hEvent == NULL)
			{
				auto erstr = getLastError();
				wprintf(L"<ERR> MSGThred()->CreateEventW fail! ErrorCode:0x%X\r\n", GetLastError());
				CloseHandle(SwatchParam->hFile);
				delete[] SwatchParam;
				return;
			}
			SwatchParam->ol.hEvent = SwatchParam->hEvent;

			// 接收通知信息的缓存区  
			SwatchParam->pBuffer.resize(MAX_BUFFER_SIZE);
			SwatchParam->dwBufferSize = MAX_BUFFER_SIZE;
			SwatchParam->NotifyAction = (funNotifyAction)nullptr;

			// 开启监控线程 每个文件都有一个  
			std::thread thr([=]() {
				WatchThred(SwatchParam);
				});
			thr.join();
		}
		static unsigned  WatchThred(LPVOID lpParam) // 监控线程  
		{
			WATCH_PARAM* pSwatchParam = (WATCH_PARAM*)lpParam;

			DWORD dwByteRet = 0;
			while (true)
			{
				if (pSwatchParam->bIsExit) break; // 退出标记  

				if (!ReadDirectoryChangesW(pSwatchParam->hFile,
					pSwatchParam->pBuffer.data(),
					pSwatchParam->dwBufferSize,
					TRUE,
					FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
					FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
					FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS |
					FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY,
					&dwByteRet,
					0,//&pSwatchParam->ol, // 异步结构  
					NULL
				))
				{
					auto erstr = getLastError();
					wprintf(L"<ERR> WatchThred()->ReadDirectoryChangesW() fail! ErrorCode:0x%X path:%s\r\n", GetLastError(), pSwatchParam->wzPath.c_str());
					continue;
				}
				if (!GetOverlappedResult(pSwatchParam->hFile, &pSwatchParam->ol, &dwByteRet, TRUE))
				{
					auto erstr = getLastError();
					wprintf(L"<ERR> WatchThred()->GetOverlappedResult() fail! ErrorCode:0x%X path:%s\r\n", GetLastError(), pSwatchParam->wzPath.c_str());
					continue;
				}

				FILE_NOTIFY_INFORMATION* pFileNotify = (FILE_NOTIFY_INFORMATION*)pSwatchParam->pBuffer.data();
				if (!pFileNotify->Action) continue;


				// 这里要注意的就是 FILE_NOTIFY_INFORMATION.FileName 不是以'\0'结尾  
				WCHAR wzSrcFile[MAX_PATH] = { 0 },
					wzDestFile[MAX_PATH] = { 0 };
				wmemcpy(wzSrcFile, pFileNotify->FileName, pFileNotify->FileNameLength);

				// 如果是重命名会将新的信息存储到下一个结构  
				if (pFileNotify->Action == FILE_ACTION_RENAMED_OLD_NAME)
				{
					FILE_NOTIFY_INFORMATION* pNextFileNotify = (FILE_NOTIFY_INFORMATION*)((BYTE*)pFileNotify + pFileNotify->NextEntryOffset);
					wmemcpy(wzDestFile, pNextFileNotify->FileName, pNextFileNotify->FileNameLength);
				}
				std::string srcfile = w_gbk(wzSrcFile);
				std::string srcnfile = w_gbk(wzDestFile);
				switch (pFileNotify->Action)
				{
				case FILE_ACTION_REMOVED: //文件删除        
				{
					printf("删除了一个文件:");
				}
				break;
				case FILE_ACTION_ADDED: //文件添加          
				{
					printf("添加了一个文件:");
				}
				break;
				case FILE_ACTION_MODIFIED:  //文件修改       
				{
					printf("修改了一个文件:");
				}
				break;
				case FILE_ACTION_RENAMED_OLD_NAME: //文件改名                     
				{
					printf("改名了一个文件:(OLD_NAME)");
				}
				break;
				case FILE_ACTION_RENAMED_NEW_NAME: //文件改名:新名字
				{
					printf("改名了一个文件:(NEW_NAME)");
				}
				break;
				default:
					break;
				}
				printf("%s->%s\n", srcfile.c_str(), srcnfile.c_str());
				// 通用处理函数  
				if (pSwatchParam->NotifyAction)
					pSwatchParam->NotifyAction(pFileNotify->Action, wzSrcFile, wzDestFile);

				memset(pSwatchParam->pBuffer.data(), 0, MAX_BUFFER_SIZE);
			}

			return 0;
		}

		void NotifyAction(DWORD dwAction, LPWSTR wzSrcFile, LPWSTR wzDestFile)
		{
			switch (dwAction)
			{
			case FILE_ACTION_ADDED:
				wprintf(L"FILE_ACTION_ADDED:%s \r\n", wzSrcFile);
				break;

			case FILE_ACTION_REMOVED:
				wprintf(L"FILE_ACTION_REMOVED:%s \r\n", wzSrcFile);
				break;

			case FILE_ACTION_MODIFIED:
				wprintf(L"FILE_ACTION_MODIFIED:%s \r\n", wzSrcFile);
				break;

			case FILE_ACTION_RENAMED_OLD_NAME:
				wprintf(L"FILE_ACTION_RENAMED:%s to %s \r\n", wzSrcFile, wzDestFile);
				break;

			default:
				break;
			}
		}

	private:
		std::map<std::wstring, WATCH_PARAM*> m_vWatchList; // 监控的列表  
		HANDLE m_hMSG = 0;                        // 消息线程句柄  
		DWORD  m_dwMSGTID = 0;                    // 消息线程ID  
		Lock _hMutex;                      // 互斥锁(list保护)  
	};

};//!hz
#endif // _WIN32
#endif
