#ifndef __LOADDLL__H__
#define __LOADDLL__H__

/*
从内存、文件、资源加载DLL


// 示例
auto dll = hz::mem_pe::load_pe("okb64.dll");
//auto dll = hz::mem_pe::load_pe((char*)RES_ID);
auto msgBox = (int(*)(const wchar_t *str, const wchar_t *title))dll->getFunc("msgBox");
msgBox(L"内存加载DLL", L"DLL标题");

// 从系统库加载
	auto dllptr = hz::Shared::loadShared( "libnfcm.dll");
	auto nfc_ctx = get_nfc_ctx_func(hz::Shared::dllsym, dllptr);

*/
#ifdef _WIN32
#include <winsock2.h> 
#include <windows.h>
#pragma comment(lib,"user32.lib")
#else
#include <dlfcn.h>
#endif // _WIN32
#ifndef ASSERT
#define ASSERT assert
#endif
#include <string>
#include <map>
#include <mutex>
#include <thread>


#define _strKey_ (char)0x12
typedef struct tag_NAME_FUN
{
	char fname[128];//函数名
	void* fun;//保存函数指针变量地址
}*pN_FUN, N_FUN;

typedef struct tag_NAMES_FUN
{
	std::string fname;//函数名
	void* fun;//保存函数指针变量地址
}*pNS_FUN, NS_FUN;

typedef std::map<std::string, void*> ms_d_FUN; //函数表
typedef std::map<int64_t, ms_d_FUN> ms_dll_FUNs; //DLL


#ifdef _WIN32
typedef bool(__stdcall* ProcDllMain)(HINSTANCE, DWORD, LPVOID);
#endif
namespace hz
{
#if (/*!defined(QT_VERSION) &&*/ defined(_WIN32))
	class mem_pe
	{
	private:

	public:
		mem_pe() :_isMain(false)
		{
			if (sizeof(void*) == 8)
			{
				cur64 = true;
			}
			else
			{
				cur64 = false;
			}
		}
		mem_pe(bool isMain) :_isMain(isMain)
		{
			if (sizeof(void*) == 8)
			{
				cur64 = true;
			}
			else
			{
				cur64 = false;
			}
		}
		~mem_pe()
		{
			if (m_bIsLoadOk)
			{
				//	ASSERT(m_pImageBase != NULL);
				//	ASSERT(m_pDllMain   != NULL);
				//脱钩，准备卸载dll
				if (_isDll)
					m_pDllMain((HINSTANCE)m_pImageBase, DLL_PROCESS_DETACH, 0);
				VirtualFree((void*)m_pImageBase, 0, MEM_DECOMMIT | MEM_RELEASE);
			}
		}
		static void destroy(mem_pe* p)
		{
			if (p)
				delete p;
		}
	public:

		/*
		最好先加载依赖的DLL

		从内存、文件、资源加载DLL
		pe参数可以是文件名、内存数据(要有dwSize)、资源ID(可以指定类型，默认DLL)
		*/
		static mem_pe* load_pe(const char* pe, size_t dwSize = 0, char* fn_type = nullptr, bool isMain = false, HINSTANCE hinst = nullptr, int nm = 0)
		{
			HRSRC hr = NULL;
			HGLOBAL hg = NULL;
			LPVOID pBuffer = 0;
			std::vector<char> buf;
			size_t isfile = false;
			if (((DWORD)pe & 0xFFFF0000) == 0) //判断是资源ID还是指针
			{
				char dt[4] = { 'D' ^ _strKey_, 'L' ^ _strKey_, 'L' ^ _strKey_, 0x00 };
				if (!fn_type)
				{
					//对资源名称字符串进行简单的异或操作，达到不能通过外部字符串参考下断点
					for (size_t i = 0; i < sizeof(dt) - 1; i++) dt[i] ^= _strKey_;
					fn_type = dt;
				}
				if (!hinst)
					hinst = GetSelfModuleHandle();
				pe = (const char*)((DWORD)pe & 0x0000FFFF);
				hr = FindResourceA(hinst, MAKEINTRESOURCEA((int)pe), fn_type);
				if (NULL == hr) return nullptr;
				//获取资源的大小
				dwSize = SizeofResource(hinst, hr);
				if (0 == dwSize) return nullptr;
				hg = LoadResource(hinst, hr);
				if (NULL == hg) return nullptr;
				//锁定资源
				pBuffer = (LPSTR)LockResource(hg);
				if (NULL == pBuffer) return nullptr;
			}
			else
			{
				if (dwSize > 0)
				{
					pBuffer = (LPVOID)pe;
				}
				else
				{
#ifdef __FILE__h__
					File::read_binary_file(pe, buf);
					pBuffer = buf.data();
					dwSize = buf.size();
#else
					HANDLE   hFile = CreateFileA(pe, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile != INVALID_HANDLE_VALUE)
					{
						DWORD   dwHigh;
						dwSize = GetFileSize(hFile, &dwHigh);
						if (dwSize > 0)
						{
							buf.resize(dwSize);
							pBuffer = buf.data();
							memset(pBuffer, 0, dwSize);
							SetFilePointer(hFile, 0, 0, FILE_BEGIN);
							DWORD   dwWritten;
							ReadFile(hFile, pBuffer, dwSize, &dwWritten, NULL);
							CloseHandle(hFile);
							isfile = true;
						}
					}
#endif
				}
			}
			mem_pe* pMemLoadDll = new mem_pe(isMain);
			//对pBuffer进行处理
			if (pMemLoadDll->MemLoadLibrary(pBuffer, dwSize, nm)) //加载dll到当前进程的地址空间
			{
				pMemLoadDll->MemGetProcAddress();//提取导出表
#if 0
				void* SENSE3 = 0;
				pN_FUN ot = outfun;
				for (int j = 0; j < num; ++j)
				{
#if 0
					for (int i = 0; i < strlen(ot[j].fname) - 1; i++)
					{
						ot[j].fname[i] ^= _strKey_;
					}
#endif
					SENSE3 = pMemLoadDll->MemGetProcAddress(ot[j].fname);

					if (SENSE3 != NULL && ot[j].fun != NULL)
					{
						*((DWORD_PTR*)ot[j].fun) = (DWORD_PTR)SENSE3;
						++ret;
					}
				}
#endif
			}

			return pMemLoadDll;
		}

		static HMODULE GetSelfModuleHandle()
		{
			MEMORY_BASIC_INFORMATION mbi;
			return ((::VirtualQuery((void*)GetSelfModuleHandle, &mbi, sizeof(mbi)) != 0)
				? (HMODULE)mbi.AllocationBase : NULL);
		}
	public:
		void* getFunc(const std::string& fcn)
		{
			auto it = m_sd_fun.find(fcn);
			return it != m_sd_fun.end() ? it->second : 0;
		}
		int64_t size()
		{
			return m_sd_fun.size();
		}
		// 批量获取
		int64_t dllsyms(std::function<void(const char* fun, void* ptr)> func)
		{
			if (func)
			{
				for (auto& [k, v] : m_sd_fun)
				{
					func(k.c_str(), v);
				}
			}
			return m_sd_fun.size();
		}
		// 批量获取
		void dllsyms(const char** funs, void** outbuf, int n)
		{
			for (int i = 0; i < n; i++)
			{
				auto fcn = funs[i];
				if (fcn)
				{
					auto it = m_sd_fun.find(fcn);
					if (it != m_sd_fun.end())
					{
						outbuf[i] = it->second;
					}
				}
			}
		}
		std::map<std::string, std::vector<std::string>>& get_import()
		{
			return _import_map;
		}
		std::map<std::string, std::vector<std::string>>& get_export()
		{
			return _export_map;
		}
		bool is_64()
		{
			return is64;
		}
		static void* dllsym(void* ptr, const char* fn)
		{
			mem_pe* ctx = (mem_pe*)ptr;
			void* ret = nullptr;
			if (ctx)
			{
				ret = ctx->getFunc(fn);
			}
			return ret;
		}
		void* _dlsym(const char* fn)
		{
			return getFunc(fn);
		}
	public:
		ms_d_FUN* getFuncList()
		{
			if (m_sd_fun.empty())
				return 0;
			return &m_sd_fun;
		}
	public:


	public:
		// 	bool MemLoadLibrary(void* lpFileData, int DataLength); // Dll file data buffer
		// 	FARPROC MemGetProcAddress(const char * lpProcName = NULL);
		// 	bool	IsLoadOk();
			//CheckDataValide函数用于检查缓冲区中的数据是否有效的dll文件
			//返回值： 是一个可执行的dll则返回true，否则返回false。
			//lpFileData: 存放dll数据的内存缓冲区
			//DataLength: dll文件的长度
		static bool CheckDataValide1(void* lpFileData, size_t DataLength)
		{
			PIMAGE_DOS_HEADER pDosHeader = 0;
			PIMAGE_NT_HEADERS pNTHeader = 0;
			PIMAGE_SECTION_HEADER pSectionHeader = 0;
			//检查长度
			if (DataLength < sizeof(IMAGE_DOS_HEADER)) return false;
			pDosHeader = (PIMAGE_DOS_HEADER)lpFileData; // DOS头
														//检查dos头的标记
			if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) return false; //0x5A4D : MZ

																		  //检查长度
			if ((DWORD)DataLength < (pDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS))) return false;
			//取得pe头
			pNTHeader = (PIMAGE_NT_HEADERS)((uint64_t)lpFileData + pDosHeader->e_lfanew); // PE头
																							   //检查pe头的合法性

			if (pNTHeader->Signature != IMAGE_NT_SIGNATURE) return false; //0x00004550 : PE00
			if ((pNTHeader->FileHeader.Characteristics & IMAGE_FILE_DLL) == 0) //0x2000 : File is a DLL
				return false;
			if ((pNTHeader->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0) //0x0002 : 指出文件可以运行
				return false;
			if (pNTHeader->FileHeader.SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER)) return false;

			//取得节表（段表）
			pSectionHeader = (PIMAGE_SECTION_HEADER)((size_t)pNTHeader + sizeof(IMAGE_NT_HEADERS));
			//验证每个节表的空间
			for (size_t i = 0; i < pNTHeader->FileHeader.NumberOfSections; i++)
			{
				if ((pSectionHeader[i].PointerToRawData + pSectionHeader[i].SizeOfRawData) > (DWORD)DataLength)return false;
			}
			return true;
		}

		//MemLoadLibrary函数从内存缓冲区数据中加载一个dll到当前进程的地址空间，缺省位置0x10000000
		//返回值： 成功返回true , 失败返回false
		//lpFileData: 存放dll文件数据的缓冲区
		//nDataLength: 缓冲区中数据的总长度
		bool MemLoadLibrary(void* lpFileData, size_t nDataLength, int nm)
		{
			if (m_pImageBase != NULL)
			{
				return false;  //已经加载一个dll，还没有释放，不能加载新的dll
			}
			//if (nm)
			//{
			//	_handle = MemoryLoadLibrary(lpFileData, nDataLength);
			//	is64 = true; MemoryGetBit(_handle);
			//	return _handle;
			//}
			//检查数据有效性，并初始化
			if (!CheckDataValide(lpFileData, nDataLength))
			{
				return false;
			}
			//计算所需的加载空间
			int nImageSize = CalcTotalImageSize();

			if (nImageSize == 0)
			{
				return false;
			}
			// 分配虚拟内存
			void* bt = (void*)(is64 ? m_pNTHeader.h64->OptionalHeader.ImageBase : m_pNTHeader.h32->OptionalHeader.ImageBase);
			uint64_t pMemoryAddress = /*cur64 == is64 ?*/ (uint64_t)VirtualAlloc(nullptr, nImageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE) /*: 0*/;

			if (pMemoryAddress == NULL)
			{
				return false;
			}
			else
			{
				//复制dll数据，并对齐每个段
				CopyDllDatas(pMemoryAddress, lpFileData);
				//重定位信息
				uint64_t va[] = { is64 ? m_pNTHeader.h64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress : m_pNTHeader.h32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress,
				is64 ? m_pNTHeader.h64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size : m_pNTHeader.h32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size };
				if (va[0] > 0 && va[1] > 0)
				{
					DoRelocation(pMemoryAddress);
				}
				//填充引入地址表
				if (!FillRavAddress(pMemoryAddress)) //修正引入地址表失败
				{
					VirtualFree((void*)pMemoryAddress, 0, MEM_DECOMMIT | MEM_RELEASE);
					return false;
				}
				//修改页属性。应该根据每个页的属性单独设置其对应内存页的属性。这里简化一下。
				//统一设置成一个属性PAGE_EXECUTE_READWRITE
				DWORD unOld = 0;

				VirtualProtect((void*)pMemoryAddress, nImageSize, PAGE_EXECUTE_READWRITE, &unOld);
			}
			//修正基地址
			uint64_t mad = 0;
			if (is64)
			{
				m_pNTHeader.h64->OptionalHeader.ImageBase = (ULONGLONG)pMemoryAddress;
				mad = m_pNTHeader.h64->OptionalHeader.AddressOfEntryPoint + pMemoryAddress;
			}
			else
			{
				m_pNTHeader.h32->OptionalHeader.ImageBase = (DWORD)pMemoryAddress;
				mad = m_pNTHeader.h32->OptionalHeader.AddressOfEntryPoint + pMemoryAddress;
			}
			//if (m_pNTHeader->OptionalHeader.AddressOfEntryPoint == 0)m_pNTHeader->OptionalHeader.AddressOfEntryPoint = 0x02acd0;

			//接下来要调用一下dll的入口函数，做初始化工作。
			/*DWORD temk = 0xf0000&(DWORD_PTR)pMemoryAddress;
			temk >>= 16;
			temk--;
			temk <<= 16;
			temk |= 0x3368;
			DWORD_PTR ptr = 0;
			ptr = (DWORD_PTR)pMemoryAddress;// 0x98cc;
			ptr >>= 20;
			ptr <<= 20;
			ptr |= temk;
			//  ptr -= 0x10000;
			DWORD pk = *(DWORD*)ptr;
			if (m_pNTHeader->OptionalHeader.AddressOfEntryPoint == 0)
			m_pNTHeader->OptionalHeader.AddressOfEntryPoint = pk;*/
			m_pDllMain = (ProcDllMain)mad;
			bool InitResult = 0;


			// if (m_pNTHeader->OptionalHeader.AddressOfEntryPoint)
			if (_isMain || _isDll)
			{
				if (cur64 == is64)
				{
					InitResult = m_pDllMain((HINSTANCE)pMemoryAddress, DLL_PROCESS_ATTACH, 0);
					if (!InitResult) //初始化失败
					{
						// if (m_pNTHeader->OptionalHeader.AddressOfEntryPoint)
						m_pDllMain((HINSTANCE)pMemoryAddress, DLL_PROCESS_DETACH, 0);
						VirtualFree((void*)pMemoryAddress, 0, MEM_DECOMMIT | MEM_RELEASE);
						m_pDllMain = NULL;
						return false;
					}
				}
				else
				{
					std::thread td([=]() {
						bool InitResult = m_pDllMain((HINSTANCE)pMemoryAddress, DLL_PROCESS_ATTACH, 0);
						if (!InitResult) //初始化失败
						{
							// if (m_pNTHeader->OptionalHeader.AddressOfEntryPoint)
							m_pDllMain((HINSTANCE)pMemoryAddress, DLL_PROCESS_DETACH, 0);
							VirtualFree((void*)pMemoryAddress, 0, MEM_DECOMMIT | MEM_RELEASE);
							m_pDllMain = NULL;
							return;
						}
						});
					td.detach();
				}
			}
			m_bIsLoadOk = true;
			m_pImageBase = (uint64_t)pMemoryAddress;
			return true;
		}

		bool IsLoadOk()
		{
			return m_bIsLoadOk;
		}
		//输入RVA（内存相对偏移地址），返回FOA（文件偏移地址）

#if 0
		DWORD RVAToFOA(DWORD imageAddr)
		{
			/*
			* 相对虚拟地址转文件偏移地址
			* ①获取Section数目
			* ②获取SectionAlignment
			* ③判断需要转换的RVA位于哪个Section中(section[n])，
			* offset = 需要转换的RVA-VirtualAddress，计算出RVA相对于本节的偏移地址
			* ④section[n].PointerToRawData + offset就是RVA转换后的FOA
			*/

			if (imageAddr > imageSize) {
				printf("RVAToFOA in_addr is error!%08X\n", imageAddr);
				exit(EXIT_FAILURE);
			}
			if (imageAddr < section_header[0].PointerToRawData) {
				return imageAddr;//在头部（包括节表与对齐）则直接返回
			}
			IMAGE_SECTION_HEADER* section = section_header;
			DWORD offset = 0;
			for (int i = 0; i < sectionNum; i++) {
				DWORD lower = section[i].VirtualAddress;//该节下限
				DWORD upper = section[i].VirtualAddress + section[i].Misc.VirtualSize;//该节上限
				if (imageAddr >= lower && imageAddr <= upper) {
					offset = imageAddr - lower + section[i].PointerToRawData;//计算出RVA的FOA
					break;
				}
			}
			return offset;
		}
#endif
		static void exports_func(const char* pn, void* ad, int iOrdinal, void* bptr, void* ud)
		{
			mem_pe* t = (mem_pe*)ud;
			t->m_sd_fun.insert(std::pair<std::string, void*>(pn, ad));
			char buf[64] = { 0 };
			std::string fmt = "%04X";
			if (t->is64)
			{
				fmt = "%08X";
			}
			else {
				fmt = "%04X";
			}
			sprintf(buf, fmt.c_str(), (uint64_t)1 + iOrdinal);
			auto hint = std::string(buf);
			if (t->is64)
			{
				fmt = "%016X";
			}
			else {
				fmt = "%08X";
			}
			sprintf(buf, fmt.c_str(), (uint64_t)bptr);
			auto foff = std::string(buf);
			t->_export_map["this"].push_back(hint + "," + foff + "," + pn);
		}
		//MemGetProcAddress函数从dll中获取指定函数的地址
		//返回值： 成功返回函数地址 , 失败返回NULL
		//lpProcName: 要查找函数的名字或者序号
		FARPROC MemGetProcAddress(const char* lpProcName = 0)
		{
			uint64_t dwOffsetStart = is64 ? m_pNTHeader.h64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress : m_pNTHeader.h32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
			uint64_t dwSize = is64 ? m_pNTHeader.h64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size : m_pNTHeader.h32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

			if (dwOffsetStart == 0 || dwSize == 0)
			{
				return NULL;
			}
			if (!m_bIsLoadOk)
			{
				return NULL;
			}

			PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)((uint64_t)m_pImageBase + dwOffsetStart);
			int iBase = pExport->Base;
			int iNumberOfFunctions = pExport->NumberOfFunctions;
			int iNumberOfNames = pExport->NumberOfNames; //<= iNumberOfFunctions
			int allcount = pExport->NumberOfFunctions;
			// 函数地址
			LPDWORD pAddressOfFunctions = (LPDWORD)(pExport->AddressOfFunctions + (uint64_t)m_pImageBase);
			// 函数序号地址
			LPWORD  pAddressOfOrdinals = (LPWORD)(pExport->AddressOfNameOrdinals + (uint64_t)m_pImageBase);
			// 函数名地址
			LPDWORD pAddressOfNames = (LPDWORD)(pExport->AddressOfNames + (uint64_t)m_pImageBase);
			/*std::vector<WORD> aoo;
			aoo.resize(allcount);
			memcpy(aoo.data(), pAddressOfOrdinals, sizeof(WORD)*allcount);
			std::vector<char*> fns;
			fns.resize(iNumberOfNames);
			memcpy(fns.data(), pAddressOfNames, sizeof(DWORD)*iNumberOfNames);
			for (int i = 0; i < fns.size(); i++)
			{
				fns[i] += m_pImageBase;
			}
			for (int i = 0; i < aoo.size(); i++)
			{
				if (i < iNumberOfNames)
					aoo[i] += 1;
			}
			std::vector<LPDWORD> adds;
			adds.resize(allcount);
			memcpy(adds.data(), pAddressOfFunctions, sizeof(DWORD)*allcount);
			for (int i = 0; i < adds.size(); i++)
			{
				adds[i] += m_pImageBase;
			}*/
			int iOrdinal = -1;
			if (lpProcName == NULL)
			{
				size_t iFound = -1;
				m_sd_fun.clear();
				for (size_t i = 0; i < iNumberOfNames; i++)
				{
					std::string pn;
					if (i < iNumberOfNames)
					{
						char* pName = (char*)(pAddressOfNames[i] + (uint64_t)m_pImageBase);
						pn = pName ? pName : "";
					}
					if (pn == "WriteStringStream") {
						printf("1\n");
					}
					iFound = i;
					if (iFound >= 0)
					{
						iOrdinal = (size_t)(pAddressOfOrdinals[iFound]);
						if (!(iOrdinal < 0 || iOrdinal >= iNumberOfFunctions))
						{
							DWORD pFunctionOffset = pAddressOfFunctions[iOrdinal];
							if (!(pFunctionOffset > dwOffsetStart && pFunctionOffset < (dwOffsetStart + dwSize)))//maybe Export Forwarding
							{
								m_sd_fun.insert(std::pair<std::string, void*>(pn, (void*)(pFunctionOffset + (uint64_t)m_pImageBase)));//

							}
							char buf[64] = { 0 };
							std::string fmt = "%04X";
							if (is64)
							{
								fmt = "%08X";
							}
							else {
								fmt = "%04X";
							}
							sprintf(buf, fmt.c_str(), (uint64_t)1 + iOrdinal);
							auto hint = std::string(buf);
							if (is64)
							{
								fmt = "%016X";
							}
							else {
								fmt = "%08X";
							}
							sprintf(buf, fmt.c_str(), (uint64_t)pFunctionOffset);
							auto foff = std::string(buf);
							_export_map["this"].push_back(hint + "," + foff + "," + pn);
						}
					}
				}
#if 0
				DWORD* addrFunc = (LPDWORD)(pExport->AddressOfFunctions + (uint64_t)m_pImageBase);// (DWORD *)(pFileBuffer + RVAToFOA(pExport->AddressOfFunctions));
				DWORD* addrName = (LPDWORD)(pExport->AddressOfNames + (uint64_t)m_pImageBase); //(DWORD *)(pFileBuffer + RVAToFOA(pExport->AddressOfNames));
				WORD* addrOrdi = (LPWORD)(pExport->AddressOfNameOrdinals + (uint64_t)m_pImageBase);//(WORD *)(pFileBuffer + RVAToFOA(pExport->AddressOfNameOrdinals));
				uint32_t bases = 1;
				//Base--->NumberOfFunctions
				DWORD i, j;
				for (i = 0; i < pExport->NumberOfFunctions; i++) {//导出时序号有NumberOfFunctions个
					if (addrFunc[i] == 0) {
						continue;//地址值为0代表该序号没有对应的函数，是空余的
					}
					for (j = 0; j < pExport->NumberOfNames; j++) {//序号表序号有NumberOfNames个
						if (addrOrdi[j] == i) {//序号表的值为地址表的索引
							printf("\t%04X\t%08X\t%08X\t%s\n", i + bases, addrFunc[i], addrName[j], m_pImageBase + addrName[j]);
							break;
						}
					}
					//存在addrOrdi[j]时,i(索引)等于addrOrdi[j](值),不存在，则i依旧有效,i+Base依旧是序号
					if (j != pExport->NumberOfNames) {
						continue;//在序号表中找到
					}
					else {//如果在序号表中没有找到地址表的索引，说明函数导出是以地址导出的，匿名函数
						printf("\t%04X\t%08X\t%s\t%s\n", i + bases, addrFunc[i], "--------", "--------");
					}
				}
#endif
				return NULL;
			}
			if (((DWORD)lpProcName & 0xFFFF0000) == 0) //IT IS A ORDINAL!
			{
				iOrdinal = (DWORD)lpProcName & 0x0000FFFF - iBase;
			}
			else  //use name
			{
				int iFound = -1;

				for (int i = 0; i < iNumberOfNames; i++)
				{
					char* pName = (char*)(pAddressOfNames[i] + (uint64_t)m_pImageBase);
					if (strcmp(pName, lpProcName) == 0)
					{
						iFound = i;
						break;
					}
				}
				if (iFound >= 0)
				{
					iOrdinal = (int)(pAddressOfOrdinals[iFound]);
				}
			}

			if (iOrdinal < 0 || iOrdinal >= iNumberOfFunctions)
			{
				return NULL;
			}
			else
			{
				DWORD pFunctionOffset = pAddressOfFunctions[iOrdinal];

				if (pFunctionOffset > dwOffsetStart && pFunctionOffset < (dwOffsetStart + dwSize))//maybe Export Forwarding
				{
					return NULL;
				}
				else
				{
					return (FARPROC)(pFunctionOffset + (uint64_t)m_pImageBase);
				}
			}

		}
	private:
		// 	bool CheckDataValide(void* lpFileData, int DataLength);
		// 	int CalcTotalImageSize();
		// 	void CopyDllDatas(void* pDest, void* pSrc);
		// 	bool FillRavAddress(void* pBase);
		// 	void DoRelocation(void* pNewBase);
		// 	int GetAlignedSize(int Origin, int Alignment);

	// 重定向PE用到的地址
		void DoRelocation(uint64_t pNewBase)
		{
			/* 重定位表的结构：
			// DWORD sectionAddress, DWORD size (包括本节需要重定位的数据)
			// 例如 1000节需要修正5个重定位数据的话，重定位表的数据是
			// 00 10 00 00   14 00 00 00      xxxx xxxx xxxx xxxx xxxx 0000
			// -----------   -----------      ----
			// 给出节的偏移  总尺寸=8+6*2     需要修正的地址           用于对齐4字节
			// 重定位表是若干个相连，如果address 和 size都是0 表示结束
			// 需要修正的地址是12位的，高4位是形态字，intel cpu下是3
			*/
			//假设NewBase是0x600000,而文件中设置的缺省ImageBase是0x400000,则修正偏移量就是0x200000

			//注意重定位表的位置可能和硬盘文件中的偏移地址不同，应该使用加载后的地址
			uint64_t dva = is64 ? m_pNTHeader.h64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress : m_pNTHeader.h32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
			PIMAGE_BASE_RELOCATION pLoc = (PIMAGE_BASE_RELOCATION)(pNewBase + dva);

			while ((pLoc->VirtualAddress + pLoc->SizeOfBlock) != 0) //开始扫描重定位表
			{
				WORD* pLocData = (WORD*)((uint64_t)pLoc + sizeof(IMAGE_BASE_RELOCATION));
				//计算本节需要修正的重定位项（地址）的数目
				int nNumberOfReloc = (pLoc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);

				for (int i = 0; i < nNumberOfReloc; i++)
				{
					// 每个WORD由两部分组成。高4位指出了重定位的类型，WINNT.H中的一系列IMAGE_REL_BASED_xxx定义了重定位类型的取值。
					// 低12位是相对于VirtualAddress域的偏移，指出了必须进行重定位的位置。
					if ((DWORD)(pLocData[i] & 0x0000F000) == 0x0000A000)
					{
						// 64位dll重定位，IMAGE_REL_BASED_DIR64
						// 对于IA-64的可执行文件，重定位似乎总是IMAGE_REL_BASED_DIR64类型的。
						if (is64)
						{
							ULONGLONG* pAddress = (ULONGLONG*)((uint64_t)pNewBase + pLoc->VirtualAddress + (pLocData[i] & 0x0FFF));
							ULONGLONG ullDelta = (ULONGLONG)pNewBase - m_pNTHeader.h64->OptionalHeader.ImageBase;
							*pAddress += ullDelta;
						}
					}
					else if ((DWORD)(pLocData[i] & 0x0000F000) == 0x00003000) //这是一个需要修正的地址
					{
						// 32位dll重定位，IMAGE_REL_BASED_HIGHLOW
						// 对于x86的可执行文件，所有的基址重定位都是IMAGE_REL_BASED_HIGHLOW类型的。
						if (!is64)
						{
							DWORD* pAddress = (DWORD*)((uint64_t)pNewBase + pLoc->VirtualAddress + (pLocData[i] & 0x0FFF));
							DWORD dwDelta = (DWORD)pNewBase - m_pNTHeader.h32->OptionalHeader.ImageBase;
							*pAddress += dwDelta;
						}
					}
				}
				//转移到下一个节进行处理
				pLoc = (PIMAGE_BASE_RELOCATION)((uint64_t)pLoc + pLoc->SizeOfBlock);
			}
		}

		//填充引入地址表
		bool FillRavAddress(uint64_t pImageBase)
		{

			// 引入表实际上是一个 IMAGE_IMPORT_DESCRIPTOR 结构数组，全部是0表示结束
			// 数组定义如下：
			// 
			// DWORD   OriginalFirstThunk;         // 0表示结束，否则指向未绑定的IAT结构数组
			// DWORD   TimeDateStamp; 
			// DWORD   ForwarderChain;             // -1 if no forwarders
			// DWORD   Name;                       // 给出dll的名字
			// DWORD   FirstThunk;                 // 指向IAT结构数组的地址(绑定后，这些IAT里面就是实际的函数地址)
			uint64_t nOffset = is64 ? m_pNTHeader.h64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress : m_pNTHeader.h32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

			if (nOffset == 0)
			{
				return true; //No Import Table
			}

			PIMAGE_IMPORT_DESCRIPTOR pID = (PIMAGE_IMPORT_DESCRIPTOR)((uint64_t)pImageBase + nOffset);

			while (pID->Characteristics != 0)
			{
				union {
					PIMAGE_THUNK_DATA32 h32;
					PIMAGE_THUNK_DATA64 h64;
				}pRealIAT;

				union {
					PIMAGE_THUNK_DATA32 h32; PIMAGE_THUNK_DATA64 h64;
				}
				pOriginalIAT;

				pRealIAT.h64 = (PIMAGE_THUNK_DATA64)((uint64_t)pImageBase + pID->FirstThunk);
				pOriginalIAT.h64 = (PIMAGE_THUNK_DATA64)((uint64_t)pImageBase + pID->OriginalFirstThunk);

				//获取dll的名字

				std::string szBuf; //dll name;
				BYTE* pName = (BYTE*)((uint64_t)pImageBase + pID->Name);
				szBuf = (char*)pName;

				HMODULE hDll = nullptr;
				if (is64 == cur64)
				{
					hDll = GetModuleHandleA(szBuf.c_str());
					if (hDll == NULL)
						hDll = LoadLibraryA(szBuf.c_str());
				}
				//获取DLL中每个导出函数的地址，填入IAT
				//每个IAT结构是 ：
				// union { PBYTE  ForwarderString;
				//   PDWORD Function;
				//   DWORD Ordinal;
				//   PIMAGE_IMPORT_BY_NAME  AddressOfData;
				// } u1;
				// 长度是一个DWORD ，正好容纳一个地址。
				for (int i = 0;; i++)
				{
					if ((is64 ? pOriginalIAT.h64[i].u1.Function : pOriginalIAT.h32[i].u1.Function) == 0)
					{
						break;
					}
					FARPROC lpFunction = NULL;
					bool ordinal = is64 ? IMAGE_SNAP_BY_ORDINAL64(pOriginalIAT.h64[i].u1.Ordinal)
						: IMAGE_SNAP_BY_ORDINAL32(pOriginalIAT.h32[i].u1.Ordinal);
					if (ordinal) //这里的值给出的是导出序号
					{
						if (is64)
						{
							if (hDll)
								lpFunction = GetProcAddress(hDll, (const char*)(pOriginalIAT.h64[i].u1.Ordinal & 0x0000FFFF));
							_import_map[szBuf].push_back(std::to_string((pOriginalIAT.h64[i].u1.Ordinal & 0x0000FFFF)));
						}
						else
						{
							if (hDll)
								lpFunction = GetProcAddress(hDll, (const char*)(pOriginalIAT.h32[i].u1.Ordinal & 0x0000FFFF));
							_import_map[szBuf].push_back(std::to_string((pOriginalIAT.h32[i].u1.Ordinal & 0x0000FFFF)));
						}
					}
					else //按照名字导入
					{
						//获取此IAT项所描述的函数名称
						auto iataddr = (is64 ? pOriginalIAT.h64[i].u1.AddressOfData : pOriginalIAT.h32[i].u1.AddressOfData);
						PIMAGE_IMPORT_BY_NAME pByName = (PIMAGE_IMPORT_BY_NAME)((uint64_t)pImageBase + iataddr);
						if (hDll)
							lpFunction = GetProcAddress(hDll, (char*)pByName->Name);
						auto hint = std::to_string(pByName->Hint);
						_import_map[szBuf].push_back(hint + "," + (char*)pByName->Name);

					}
					if (lpFunction != NULL)   //找到了！
					{
						if (is64)
							pRealIAT.h64[i].u1.Function = (uint64_t)lpFunction;
						else
							pRealIAT.h32[i].u1.Function = (uint32_t)lpFunction;

					}
					else
					{
						continue;
					}
				}

				//move to next 
				pID = (PIMAGE_IMPORT_DESCRIPTOR)((uint64_t)pID + sizeof(IMAGE_IMPORT_DESCRIPTOR));
			}

			return true;
		}

		//CheckDataValide函数用于检查缓冲区中的数据是否有效的dll文件
		//返回值： 是一个可执行的dll则返回true，否则返回false。
		//lpFileData: 存放dll数据的内存缓冲区
		//nDataLength: dll文件的长度
		bool CheckDataValide(void* lpFileData, size_t nDataLength)
		{
			//检查长度
			if (nDataLength < sizeof(IMAGE_DOS_HEADER))
			{
				return false;
			}
			m_pDosHeader = (PIMAGE_DOS_HEADER)lpFileData;  // DOS头
														   //检查dos头的标记
			if (m_pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
			{
				return false;  //0x5A4D : MZ
			}

			//检查长度
			if ((DWORD)nDataLength < (m_pDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS)))
			{
				return false;
			}
			//取得pe头
			m_pNTHeader.h64 = (PIMAGE_NT_HEADERS64)((uint64_t)lpFileData + m_pDosHeader->e_lfanew); // PE头
			int s[] = { sizeof(IMAGE_NT_HEADERS32),sizeof(IMAGE_NT_HEADERS64) };
			//检查pe头的合法性
			if (m_pNTHeader.h64->Signature != IMAGE_NT_SIGNATURE)
			{
				return false;  //0x00004550 : PE00
			}
			if (m_pNTHeader.h64->FileHeader.Machine == IMAGE_FILE_MACHINE_IA64
				|| m_pNTHeader.h64->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64
#ifdef IMAGE_FILE_MACHINE_ARM64
				|| m_pNTHeader.h64->FileHeader.Machine == IMAGE_FILE_MACHINE_ARM64
#endif
				|| m_pNTHeader.h64->FileHeader.Machine == IMAGE_FILE_MACHINE_ALPHA64)
			{
				is64 = true;
			}
			if ((m_pNTHeader.h64->FileHeader.Characteristics & IMAGE_FILE_DLL) == 0) //0x2000  : File is a DLL
			{
				_isDll = false;
				//return false;
			}
			if ((m_pNTHeader.h64->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0) //0x0002 : 指出文件可以运行
			{
				return false;
			}
			int us[] = { sizeof(IMAGE_OPTIONAL_HEADER32),sizeof(IMAGE_OPTIONAL_HEADER64) };
			if (!(m_pNTHeader.h64->FileHeader.SizeOfOptionalHeader == us[0] || (m_pNTHeader.h64->FileHeader.SizeOfOptionalHeader == us[1])))
			{
				return false;
			}

			//取得节表（段表）
			m_pSectionHeader = (PIMAGE_SECTION_HEADER)((uint64_t)m_pNTHeader.h64 + (is64 ? sizeof(IMAGE_NT_HEADERS64) : sizeof(IMAGE_NT_HEADERS32)));
			//验证每个节表的空间
			for (int i = 0; i < m_pNTHeader.h64->FileHeader.NumberOfSections; i++)
			{
				if ((m_pSectionHeader[i].PointerToRawData + m_pSectionHeader[i].SizeOfRawData) > nDataLength)
				{
					return false;
				}
			}

			return true;
		}

		//计算对齐边界
		int GetAlignedSize(int nOrigin, int nAlignment)
		{
			return (nOrigin + nAlignment - 1) / nAlignment * nAlignment;
		}

		//计算整个dll映像文件的尺寸
		int CalcTotalImageSize()
		{
			int nSize = 0;

			if (m_pNTHeader.h32 == NULL)
			{
				return 0;
			}
			//段对齐字节数
			int nAlign = is64 ? m_pNTHeader.h64->OptionalHeader.SectionAlignment : m_pNTHeader.h32->OptionalHeader.SectionAlignment;

			// 计算所有头的尺寸。包括dos, coff, pe头 和 段表的大小
			nSize = GetAlignedSize(is64 ? m_pNTHeader.h64->OptionalHeader.SizeOfHeaders : m_pNTHeader.h32->OptionalHeader.SizeOfHeaders, nAlign);
			// 计算所有节的大小
			for (int i = 0; i < m_pNTHeader.h64->FileHeader.NumberOfSections; ++i)
			{
				//得到该节的大小
				int nCodeSize = m_pSectionHeader[i].Misc.VirtualSize;
				int nLoadSize = m_pSectionHeader[i].SizeOfRawData;
				int nMaxSize = (nLoadSize > nCodeSize) ? (nLoadSize) : (nCodeSize);
				int nSectionSize = GetAlignedSize(m_pSectionHeader[i].VirtualAddress + nMaxSize, nAlign);

				if (nSize < nSectionSize)
				{
					nSize = nSectionSize;  //Use the Max;
				}
			}

			return nSize;
		}

		//CopyDllDatas函数将dll数据复制到指定内存区域，并对齐所有节
		//pSrc: 存放dll数据的原始缓冲区
		//pDest:目标内存地址
		void CopyDllDatas(uint64_t pDest, void* pSrc)
		{
			// 计算需要复制的PE头+段表字节数
			int  nHeaderSize = is64 ? m_pNTHeader.h64->OptionalHeader.SizeOfHeaders : m_pNTHeader.h32->OptionalHeader.SizeOfHeaders;
			int  nSectionSize = m_pNTHeader.h64->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
			int  nMoveSize = nHeaderSize + nSectionSize;
			//复制头和段信息
			memcpy((void*)pDest, pSrc, nMoveSize);

			//复制每个节
			for (int i = 0; i < m_pNTHeader.h64->FileHeader.NumberOfSections; ++i)
			{
				if (m_pSectionHeader[i].VirtualAddress == 0 || m_pSectionHeader[i].SizeOfRawData == 0)
				{
					continue;
				}
				// 定位该节在内存中的位置
				void* pSectionAddress = (void*)((uint64_t)pDest + m_pSectionHeader[i].VirtualAddress);
				// 复制段数据到虚拟内存
				memcpy((void*)pSectionAddress, (void*)((uint64_t)pSrc + m_pSectionHeader[i].PointerToRawData),
					m_pSectionHeader[i].SizeOfRawData);
			}

			//修正指针，指向新分配的内存
			//新的dos头
			m_pDosHeader = (PIMAGE_DOS_HEADER)pDest;
			//新的pe头地址
			m_pNTHeader.h64 = (PIMAGE_NT_HEADERS64)((uint64_t)pDest + (m_pDosHeader->e_lfanew));
			//新的节表地址
			m_pSectionHeader = (PIMAGE_SECTION_HEADER)((uint64_t)m_pNTHeader.h64 + (is64 ? sizeof(IMAGE_NT_HEADERS64) : sizeof(IMAGE_NT_HEADERS32)));
		}

	private:
		// todo 数据区
		ProcDllMain m_pDllMain = nullptr;
		bool m_bIsLoadOk = false;
		bool _isDll = true, _isMain = false;
		uint64_t m_pImageBase = 0;

		PIMAGE_SECTION_HEADER m_pSectionHeader = nullptr;
		PIMAGE_DOS_HEADER m_pDosHeader = nullptr;
		union {
			PIMAGE_NT_HEADERS32 h32;
			PIMAGE_NT_HEADERS64 h64 = 0;
		}m_pNTHeader;
		// pe数据是否为64位
		bool is64 = false;
		// 当前程序是否为64位
		bool cur64 = false;
		ms_d_FUN                     m_sd_fun;         //函数表

		// 导入表\导出表
		std::map<std::string, std::vector<std::string>> _import_map, _export_map;
	};
#endif
	// DLL动态加载
	class Shared //:public Res
	{
	private:
		void* _ptr = 0;
		std::once_flag oc;
		bool isinit = false;
	public:
		static Shared* loadShared(const std::string& fnstr, std::vector<std::string>* pdir = nullptr)
		{
#ifdef NDEBUG
			auto nd = "debug";
#else
			auto nd = "release";
#endif // NDEBUG
			Shared* ptr = new Shared();
			bool is = ptr->loadFile(fnstr);
			if (!is && pdir)
			{
				for (auto it : *pdir)
				{
					auto nit = hstring::toLower(it);
					if (nit.find(nd) != std::string::npos)
					{
						continue;
					}
					if ('/' != *it.rbegin() && '\\' != *it.rbegin())
					{
						it.push_back('/');
					}
					is = ptr->loadFile(it + fnstr, 2);
					if (is)
					{
						break;
					}
				}
			}
			if (!is)
			{
				delete ptr;
				ptr = nullptr;
			}
			if (ptr) {

			}
			return ptr;
		}
		static void destroy(Shared* p)
		{
			if (p)
				delete p;
		}
	public:
		bool loadFile(const std::string& fnstr, int cs = 2)
		{
#ifdef _WIN32
#define _DL_OPEN(d) LoadLibraryA(d)
			const char* sysdirstr = nullptr;
#else
#define _DL_OPEN(d) dlopen(d,RTLD_NOW)
			const char* sysdirstr = "/usr/local/lib/";
#endif
			//std::call_once(oc, [=]() {
			int inc = 0;
			std::string dfn = fnstr;
			std::string errstr;
			isinit = false;
			do
			{
				_ptr = _DL_OPEN(dfn.c_str());
				if (_ptr)break;
				dfn = File::getAP(dfn);
				_ptr = _DL_OPEN(dfn.c_str());
				if (_ptr)break;
				if (sysdirstr)
				{
					dfn = sysdirstr + fnstr;
					_ptr = _DL_OPEN(dfn.c_str());
				}
				if (!_ptr) {
					errstr = mfile_t::getLastError();
					printf("Could not load %s dll library!\n", fnstr.c_str());
				}
			} while (0);

			if (_ptr)
			{
				isinit = true;
			}
			return isinit;
		}
		void dll_close()
		{
			if (_ptr)
			{

#ifdef _WIN32
				FreeLibrary((HMODULE)_ptr);
#else
				dlclose(_ptr);
#endif
				_ptr = 0;
			}
		}
		void* _dlsym(const char* funcname)
		{
#if defined(_WIN32)
#define __dlsym GetProcAddress
#define LIBPTR HMODULE
#else
#define __dlsym dlsym
#define LIBPTR void*
#endif
			void* func = (void*)__dlsym((LIBPTR)_ptr, funcname);
			if (!func)
				func = (void*)__dlsym((LIBPTR)_ptr, funcname);
			return func;
#undef __dlsym
#undef LIBPTR
		}

		// 批量获取
		void dllsyms(const char** funs, void** outbuf, int n)
		{
			for (int i = 0; i < n; i++)
			{
				auto fcn = funs[i];
				if (fcn)
				{
					auto it = _dlsym(fcn);
					if (it)
					{
						outbuf[i] = it;
					}
				}
			}
		}
		static void* dllsym(void* ptr, const char* fn)
		{
			Shared* ctx = (Shared*)ptr;
			void* ret = nullptr;
			if (ctx)
			{
				ret = ctx->_dlsym(fn);
			}
			return ret;
		}
	public:
		Shared()
		{
		}

		~Shared()
		{
			dll_close();
		}

	private:

	};

}//!hz
#endif //!__LOADDLL__H__
