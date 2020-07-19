#ifndef __dropdrag__h
#define __dropdrag__h
#ifdef _WIN32
#include <windows.h>
#include <string>
#include <vector>
#include <functional>
#include<Shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#pragma comment(lib,"Shell32.lib")
#include <Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#define SHCNRF_InterruptLevel  0x0001 //Interrupt level notifications from the file system
#define SHCNRF_ShellLevel   0x0002 //Shell-level notifications from the shell
#define SHCNRF_RecursiveInterrupt  0x1000 //Interrupt events on the whole subtree
#define SHCNRF_NewDelivery   0x8000 //Messages received use shared memory
#define WM_USERDEF_FILECHANGED (WM_USER + 105)


#define OFFSETOFCLASS0(base, derived) \
    ((DWORD)(DWORD_PTR)(STATIC_CAST(base*)((derived*)8))-8)
#define QITABENTMULTI0(Cthis, Ifoo, Iimpl) \
    { (IID*) &IID_##Ifoo, OFFSETOFCLASS0(Iimpl, Cthis) }

#define QITABENT0(Cthis, Ifoo) QITABENTMULTI0(Cthis, Ifoo, Ifoo)

#include <data/json_helper.h>
namespace hz
{
	/*

	typedef struct tSHChangeNotifyEntry
	{
	LPCITEMIDLIST pidl; //Pointer to an item identifier list (PIDL) for which to receive notifications
	BOOL fRecursive; //Flag indicating whether to post notifications for children of this PIDL
	}SHChangeNotifyEntry;
	*/

	typedef struct tSHNotifyInfo
	{
		DWORD dwItem1;  // dwItem1 contains the previous PIDL or name of the folder. 
		DWORD dwItem2;  // dwItem2 contains the new PIDL or name of the folder. 
	}SHNotifyInfo;

	typedef ULONG
	(WINAPI* pfnSHChangeNotifyRegister)
		(
			HWND hWnd,
			int  fSource,
			LONG fEvents,
			UINT wMsg,
			int  cEntries,
			SHChangeNotifyEntry* pfsne
			);
	typedef BOOL(WINAPI* pfnSHChangeNotifyDeregister)(ULONG ulID);

#ifdef MAKEINTRESOURCE
#undef MAKEINTRESOURCE
#define MAKEINTRESOURCE  MAKEINTRESOURCEA
#endif // !UNICODE

	class DropFile
	{
	public:
		DropFile() {}
		~DropFile() {}
		BOOL Init(HWND hwnd)
		{
			//加载Shell32.dll
			m_hShell32 = LoadLibraryA("Shell32.dll");
			if (m_hShell32 == NULL)
			{
				return FALSE;
			}

			//取函数地址

			m_pfnRegister = (pfnSHChangeNotifyRegister)GetProcAddress(m_hShell32, MAKEINTRESOURCE(2));
			m_pfnDeregister = (pfnSHChangeNotifyDeregister)GetProcAddress(m_hShell32, MAKEINTRESOURCE(4));
			if (m_pfnRegister == NULL || m_pfnDeregister == NULL)
			{
				return FALSE;
			}

			SHChangeNotifyEntry shEntry = { 0 };
			shEntry.fRecursive = TRUE;
			shEntry.pidl = 0;


			//注册Shell监视函数
			m_ulNotifyId = m_pfnRegister(
				hwnd,
				SHCNRF_InterruptLevel | SHCNRF_ShellLevel,
				SHCNE_ALLEVENTS,
				WM_USERDEF_FILECHANGED, //自定义消息
				1,
				&shEntry
			);
			if (m_ulNotifyId == 0)
			{
				MessageBoxA(0, "Register failed!", "ERROR", MB_OK | MB_ICONERROR);
				return FALSE;
			}
			return TRUE;
		}
		// 在CListCtrlEx类中再添加如下函数。该函数的作用是从PIDL中解出实际字符路径。
		std::string GetPathFromPIDL(size_t pidl)
		{
			char szPath[MAX_PATH];
			std::string strTemp = ("");
			if (SHGetPathFromIDListA((struct _ITEMIDLIST*)pidl, szPath))
			{
				strTemp = szPath;
			}
			return strTemp;
		}
		LRESULT OnFileChanged(WPARAM wParam, LPARAM lParam)
		{
			std::string  strOriginal = ("");
			std::string  strCurrent = ("");
			SHNotifyInfo* pShellInfo = (SHNotifyInfo*)wParam;

			strOriginal = GetPathFromPIDL(pShellInfo->dwItem1);
			if (strOriginal.empty())
			{
				return NULL;
			}

			switch (lParam)
			{
			case SHCNE_CREATE:
				break;

			case SHCNE_DELETE:
				break;
			case SHCNE_RENAMEITEM:
				break;
			}
			return NULL;
		}

	private:

		HMODULE  m_hShell32 = 0;
		pfnSHChangeNotifyRegister      m_pfnRegister = NULL;
		pfnSHChangeNotifyDeregister    m_pfnDeregister = NULL;

		BOOL m_ulNotifyId = 0;
	};


	//-----------------------------------------------------------------------------
	class MouseCursor
	{
	public:
		MouseCursor() {}
		~MouseCursor() {}

		void OnHoverCursor(HWND hWnd, LPCTSTR idc)
		{
			_hWnd = hWnd;

#ifdef _WIN64
			clsCur = (HCURSOR)GetClassLongPtrW(hWnd, GCLP_HCURSOR); //取当前的光标值
			SetClassLongPtrW(hWnd, GCLP_HCURSOR, NULL);   //关闭窗口类对光标的控制
#else
			SetLastError(0);
			clsCur = (HCURSOR)GetClassLongW(hWnd, GCL_HCURSOR); //取当前的光标值
			DWORD err = GetLastError();
			SetClassLongW(hWnd, GCL_HCURSOR, NULL);   //关闭窗口类对光标的控制
#endif
#ifdef CUR_NO
			clsCur = SetCursor(hl::HLCaret::getCursor(CUR_NO)); //设置光标为输入指针
#endif
		}

		void OnLeaveCursor()
		{
			// if (clsCur)
			{

#ifdef _WIN64
				SetClassLongPtrW(_hWnd, GCLP_HCURSOR, (LONG_PTR)clsCur);
#else
				SetClassLongW(_hWnd, GCL_HCURSOR, (LONG)clsCur);  //恢复窗口类对光标的控制
#endif
				SetCursor(clsCur); //恢复箭头光标
			}
		}
	private:
		HWND _hWnd = NULL;
		HCURSOR clsCur;
	};

	//-----------------------------------------------------------------------------
	class IDuiDropTarget
	{
	public:
		virtual	HRESULT OnDragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect) = 0;
		virtual HRESULT  OnDragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) = 0;
		virtual HRESULT  OnDragLeave() = 0;
		virtual HRESULT  OnDrop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) = 0;
	};

	// 接收拖动OLE目标
	class CDropTargetEx : public IDropTarget
	{
	public:
		CDropTargetEx(void)
		{
			m_lRefCount = 1;
			// Create an instance of the shell DnD helper object.
			if (SUCCEEDED(CoCreateInstance(CLSID_DragDropHelper, NULL,
				CLSCTX_INPROC_SERVER,
				IID_IDropTargetHelper,
				(void**)&m_piDropHelper)))
			{
				//   m_bUseDnDHelper = true;
			}
		}
		~CDropTargetEx(void)
		{
			if (m_piDropHelper)
			{
				m_piDropHelper->Release();
			}
			m_bUseDnDHelper = false;
			m_lRefCount = 0;
			DragDropRevoke(m_hWnd);
		}
		bool DragDropRegister(HWND hWnd, DWORD AcceptKeyState = MK_LBUTTON)
		{
			if (!IsWindow(hWnd))return false;

			HRESULT s = ::RegisterDragDrop(hWnd, this);
			m_hWnd = hWnd;
			if (SUCCEEDED(s))
			{
				m_dAcceptKeyState = AcceptKeyState;
				return true;
			}
			else { return false; }
		}
		void setOver(std::function<void(int x, int y)> overfunc)
		{
			_over_func = overfunc;
		}
		void SetDragDrop(IDuiDropTarget* pDuiDropTarget)
		{
			if (_isDrop)
			{
				m_pDuiDropTarget = pDuiDropTarget;
			}
			else
			{
				m_pDuiDropTarget = 0;
			}
		}
		bool DragDropRevoke(HWND hWnd)
		{
			if (!hWnd)
			{
				hWnd = m_hWnd;
			}
			if (!IsWindow(hWnd))return false;

			HRESULT s = ::RevokeDragDrop(hWnd);
			m_pDuiDropTarget = NULL;
			return SUCCEEDED(s);
		}
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, __RPC__deref_out void** ppvObject)
		{
			static const QITAB qit[2] =
			{
				QITABENT0(CDropTargetEx, IDropTarget),
				{ 0 }
			};

			return QISearch(this, qit, riid, ppvObject);
		}
		ULONG STDMETHODCALLTYPE AddRef()
		{
			return InterlockedIncrement(&m_lRefCount);
		}
		ULONG STDMETHODCALLTYPE Release()
		{
			ULONG lRef = InterlockedDecrement(&m_lRefCount);
			if (0 == lRef)
			{
				delete this;
			}
			return m_lRefCount;
		}

		//进入
		HRESULT STDMETHODCALLTYPE DragEnter(__RPC__in_opt IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD* pdwEffect)
		{
			_isDrop = true;
			//_cur.OnHoverCursor(m_hWnd, IDC_NO);
			if (m_bUseDnDHelper)
			{
				m_piDropHelper->DragEnter(m_hWnd, pDataObj, (LPPOINT)&pt, *pdwEffect);
			}
			printf("DragEnter\n");
			if (m_pDuiDropTarget)
				return m_pDuiDropTarget->OnDragEnter(pDataObj, grfKeyState, pt, pdwEffect);

			return S_OK;
		}

		//移动
		HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, __RPC__inout DWORD* pdwEffect)
		{
			//*pdwEffect = DROPEFFECT_NONE;//表示不接受拖放好像没用
			//*pdwEffect = DROPEFFECT_COPY;//复制
			//_cur.OnHoverCursor(m_hWnd, IDC_NO);
			if (_over_func)
			{
				_over_func(pt.x, pt.y);
			}
			if (m_bUseDnDHelper)
			{
				m_piDropHelper->DragOver((LPPOINT)&pt, *pdwEffect);
			}
			*pdwEffect = DROPEFFECT_NONE;

			//printf("DragOver\n");
			if (m_pDuiDropTarget)
				return m_pDuiDropTarget->OnDragOver(grfKeyState, pt, pdwEffect);
			return S_OK;
		}

		//离开
		HRESULT STDMETHODCALLTYPE DragLeave()
		{
			_isDrop = false;
			if (m_bUseDnDHelper)
			{
				m_piDropHelper->DragLeave();
			}
			HRESULT hr = S_OK;
			if (m_pDuiDropTarget)
			{
				hr = m_pDuiDropTarget->OnDragLeave();
				m_pDuiDropTarget = nullptr;
			}
			printf("DragLeave\n");
			return hr;
		}

		//释放
		HRESULT STDMETHODCALLTYPE Drop(__RPC__in_opt IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD* pdwEffect)
		{
			m_piDropHelper->Drop(pDataObj, (LPPOINT)&pt, *pdwEffect);
			HRESULT hr = S_OK;
			if (m_pDuiDropTarget)
				hr = m_pDuiDropTarget->OnDrop(pDataObj, grfKeyState, pt, pdwEffect);
			m_pDuiDropTarget = nullptr;
			_isDrop = false;
			printf("-------------Drop\n");
			return hr;
		}

		//void GetDropData(__RPC__in_opt IDataObject *pDataObj);
		//BOOL ProcessDrop(HDROP hDrop);
	private:
		HWND m_hWnd = nullptr;
		IDropTargetHelper* m_piDropHelper = 0;
		bool	m_bUseDnDHelper = false;
		IDuiDropTarget* m_pDuiDropTarget = 0;
		DWORD m_dAcceptKeyState = 0;
		ULONG  m_lRefCount = 0;
		bool _isDrop = false;
		MouseCursor _cur;

		std::function<void(int x, int y)> _over_func;
	};

	typedef struct _DRAGDATA
	{

		int cfFormat;
		STGMEDIUM stgMedium;
	}DRAGDATA, * LPDRAGDATA;

	//---------drag-----------------------------------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
	class SdkDropSource : public IDropSource
	{
	public:

		SdkDropSource()
		{
			m_lRefCount = 1;
		}
		~SdkDropSource()
		{
			m_lRefCount = 0;
		}

		// Methods of IUnknown
		IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
		{

			static const QITAB qit[] =
			{
				QITABENT0(SdkDropSource, IDropSource),
				{ 0 }
			};

			return QISearch(this, qit, riid, ppv);
		}
		IFACEMETHODIMP_(ULONG) AddRef(void)
		{
			return InterlockedIncrement(&m_lRefCount);
		}

		IFACEMETHODIMP_(ULONG) Release(void)
		{
			ULONG lRef = InterlockedDecrement(&m_lRefCount);
			if (0 == lRef)
			{
				delete this;
			}
			return m_lRefCount;
		}

		// Methods of IDropSource
		IFACEMETHODIMP QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
		{
			if (TRUE == fEscapePressed)
			{
				return DRAGDROP_S_CANCEL;
			}

			// If the left button of mouse is released
			if (0 == (grfKeyState & (MK_LBUTTON)))
			{
				return DRAGDROP_S_DROP;
			}

			return S_OK;
		}
		IFACEMETHODIMP GiveFeedback(DWORD dwEffect)
		{
			UNREFERENCED_PARAMETER(dwEffect);
			return DRAGDROP_S_USEDEFAULTCURSORS;
		}

	private:

		volatile LONG   m_lRefCount;        //!< The reference count
	};


	//-----------------------------------------------------------------------------

	typedef struct _DATASTORAGE
	{
		FORMATETC* m_formatEtc;
		STGMEDIUM* m_stgMedium;

	} DATASTORAGE_t, * LPDATASTORAGE_t;

	class SdkDataObject : public IDataObject
	{
	public:

		SdkDataObject(SdkDropSource* pDropSource = NULL)
		{
			m_pDropSource = pDropSource;
			m_lRefCount = 1;
		}
		BOOL IsDataAvailable(CLIPFORMAT cfFormat);
		BOOL GetGlobalData(CLIPFORMAT cfFormat, void** ppData);
		BOOL GetGlobalDataArray(CLIPFORMAT cfFormat,
			HGLOBAL* pDataArray, DWORD dwCount);
		BOOL SetGlobalData(CLIPFORMAT cfFormat, void* pData, BOOL fRelease = TRUE);
		BOOL SetGlobalDataArray(CLIPFORMAT cfFormat,
			HGLOBAL* pDataArray, DWORD dwCount, BOOL fRelease = TRUE);
		BOOL SetDropTip(DROPIMAGETYPE type, PCWSTR pszMsg, PCWSTR pszInsert);

		// The com interface.
		IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
		{
			static const QITAB qit[] =
			{
				QITABENT0(SdkDataObject, IDataObject),
				{ 0 },
			};
			return QISearch(this, qit, riid, ppv);
		}
		IFACEMETHODIMP_(ULONG) AddRef()
		{
			return InterlockedIncrement(&m_lRefCount);
		}
		IFACEMETHODIMP_(ULONG) Release()
		{
			ULONG lRef = InterlockedDecrement(&m_lRefCount);
			if (0 == lRef)
			{
				delete this;
			}
			return m_lRefCount;
		}
		IFACEMETHODIMP GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium)
		{
			if ((NULL == pformatetcIn) || (NULL == pmedium))
			{
				return E_INVALIDARG;
			}

			pmedium->hGlobal = NULL;

			int nSize = (int)m_dataStorageCL.size();
			for (int i = 0; i < nSize; ++i)
			{
				DATASTORAGE_t dataEntry = m_dataStorageCL.at(i);
				if ((pformatetcIn->tymed & dataEntry.m_formatEtc->tymed) &&
					(pformatetcIn->dwAspect == dataEntry.m_formatEtc->dwAspect) &&
					(pformatetcIn->cfFormat == dataEntry.m_formatEtc->cfFormat))
				{
					return CopyMedium(pmedium,
						dataEntry.m_stgMedium, dataEntry.m_formatEtc);
				}
			}

			return DV_E_FORMATETC;
		}
		IFACEMETHODIMP SetData(FORMATETC* pformatetc,
			STGMEDIUM* pmedium, BOOL fRelease)
		{
			if ((NULL == pformatetc) || (NULL == pmedium))
			{
				return E_INVALIDARG;
			}

			if (pformatetc->tymed != pmedium->tymed)
			{
				return E_FAIL;
			}

			FORMATETC* fetc = new FORMATETC;
			STGMEDIUM* pStgMed = new STGMEDIUM;
			ZeroMemory(fetc, sizeof(FORMATETC));
			ZeroMemory(pStgMed, sizeof(STGMEDIUM));

			*fetc = *pformatetc;

			if (TRUE == fRelease)
			{
				*pStgMed = *pmedium;
			}
			else
			{
				CopyMedium(pStgMed, pmedium, pformatetc);
			}

			DATASTORAGE_t dataEntry = { fetc, pStgMed };
			m_dataStorageCL.push_back(dataEntry);

			return S_OK;
		}
		IFACEMETHODIMP GetDataHere(FORMATETC* pformatetc, STGMEDIUM* pmedium)
		{
			UNREFERENCED_PARAMETER(pformatetc);
			UNREFERENCED_PARAMETER(pmedium);
			return E_NOTIMPL;
		}
		IFACEMETHODIMP QueryGetData(FORMATETC* pformatetc)
		{
			if (NULL == pformatetc)
			{
				return E_INVALIDARG;
			}
			if (!(DVASPECT_CONTENT & pformatetc->dwAspect))
			{
				return DV_E_DVASPECT;
			}
			HRESULT hr = DV_E_TYMED;
			size_t nSize = m_dataStorageCL.size();
			for (size_t i = 0; i < nSize; ++i)
			{
				DATASTORAGE_t dataEnrty = m_dataStorageCL.at(i);
				if (dataEnrty.m_formatEtc->tymed & pformatetc->tymed)
				{
					if (dataEnrty.m_formatEtc->cfFormat == pformatetc->cfFormat)
					{
						return S_OK;
					}
					else
					{
						hr = DV_E_CLIPFORMAT;
					}
				}
				else
				{
					hr = DV_E_TYMED;
				}
			}
			return hr;
		}
		IFACEMETHODIMP GetCanonicalFormatEtc(FORMATETC* pformatetcIn,
			FORMATETC* pformatetcOut)
		{
			pformatetcOut->ptd = NULL;

			return E_NOTIMPL;
		}
		IFACEMETHODIMP EnumFormatEtc(DWORD dwDirection,
			IEnumFORMATETC** ppenumFormatEtc)
		{
			if (NULL == ppenumFormatEtc)
			{
				return E_INVALIDARG;
			}
			*ppenumFormatEtc = NULL;
			HRESULT hr = E_NOTIMPL;
			if (DATADIR_GET == dwDirection)
			{
				FORMATETC rgfmtetc[] =
				{
					{ _cfFormat, NULL, DVASPECT_CONTENT, 0, TYMED_HGLOBAL },
				};
				hr = SHCreateStdEnumFmtEtc(ARRAYSIZE(rgfmtetc), rgfmtetc, ppenumFormatEtc);
			}
			return hr;
		}
		IFACEMETHODIMP DAdvise(FORMATETC* pformatetc, DWORD advf,
			IAdviseSink* pAdvSnk, DWORD* pdwConnection)
		{
			UNREFERENCED_PARAMETER(pformatetc);
			UNREFERENCED_PARAMETER(advf);
			UNREFERENCED_PARAMETER(pAdvSnk);
			UNREFERENCED_PARAMETER(pdwConnection);
			return E_NOTIMPL;
		}
		IFACEMETHODIMP DUnadvise(DWORD dwConnection)
		{
			UNREFERENCED_PARAMETER(dwConnection);
			return E_NOTIMPL;
		}
		IFACEMETHODIMP EnumDAdvise(IEnumSTATDATA** ppenumAdvise)
		{
			UNREFERENCED_PARAMETER(ppenumAdvise);
			return E_NOTIMPL;
		}

		HRESULT SetBlob(CLIPFORMAT cf, const void* pvBlob, UINT cbBlob)
		{
			void* pv = GlobalAlloc(GPTR, cbBlob);
			*(char*)pv = 0;
			HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
			if (SUCCEEDED(hr))
			{
				CopyMemory(pv, pvBlob, cbBlob);
				FORMATETC fmte = { cf, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
				_cfFormat = cf;
				// The STGMEDIUM structure is used to define how to handle a global memory transfer.
				// This structure includes a flag, tymed, which indicates the medium
				// to be used, and a union comprising pointers and a handle for getting whichever
				// medium is specified in tymed.
				STGMEDIUM medium = {};
				medium.tymed = TYMED_HGLOBAL;
				medium.hGlobal = pv;
				hr = this->SetData(&fmte, &medium, TRUE);
				if (FAILED(hr))
				{
					GlobalFree(pv);
				}
			}
			return hr;
		}
	private:

		~SdkDataObject(void)
		{
			m_lRefCount = 0;

			int nSize = (int)m_dataStorageCL.size();
			for (int i = 0; i < nSize; ++i)
			{
				DATASTORAGE_t dataEntry = m_dataStorageCL.at(i);
				ReleaseStgMedium(dataEntry.m_stgMedium);
				delete[](dataEntry.m_stgMedium);
				delete[](dataEntry.m_formatEtc);
			}
		}
		SdkDataObject(const SdkDataObject&);
		SdkDataObject& operator = (const SdkDataObject&);
		HRESULT CopyMedium(STGMEDIUM* pMedDest,
			STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc)
		{
			if ((NULL == pMedDest) || (NULL == pMedSrc) || (NULL == pFmtSrc))
			{
				return E_INVALIDARG;
			}
			switch (pMedSrc->tymed)
			{
			case TYMED_HGLOBAL:
				pMedDest->hGlobal = (HGLOBAL)OleDuplicateData(pMedSrc->hGlobal, pFmtSrc->cfFormat, NULL);
				break;
			case TYMED_GDI:
				pMedDest->hBitmap = (HBITMAP)OleDuplicateData(pMedSrc->hBitmap, pFmtSrc->cfFormat, NULL);
				break;
			case TYMED_MFPICT:
				pMedDest->hMetaFilePict = (HMETAFILEPICT)OleDuplicateData(pMedSrc->hMetaFilePict, pFmtSrc->cfFormat, NULL);
				break;
			case TYMED_ENHMF:
				pMedDest->hEnhMetaFile = (HENHMETAFILE)OleDuplicateData(pMedSrc->hEnhMetaFile, pFmtSrc->cfFormat, NULL);
				break;
			case TYMED_FILE:
				pMedSrc->lpszFileName = (LPOLESTR)OleDuplicateData(pMedSrc->lpszFileName, pFmtSrc->cfFormat, NULL);
				break;
			case TYMED_ISTREAM:
				pMedDest->pstm = pMedSrc->pstm;
				pMedSrc->pstm->AddRef();
				break;
			case TYMED_ISTORAGE:
				pMedDest->pstg = pMedSrc->pstg;
				pMedSrc->pstg->AddRef();
				break;
			case TYMED_NULL:
			default:
				break;
			}
			pMedDest->tymed = pMedSrc->tymed;
			pMedDest->pUnkForRelease = NULL;
			if (pMedSrc->pUnkForRelease != NULL)
			{
				pMedDest->pUnkForRelease = pMedSrc->pUnkForRelease;
				pMedSrc->pUnkForRelease->AddRef();
			}
			return S_OK;
		}

	private:

		//!< The reference of count
		volatile LONG           m_lRefCount;
		//!< The pointer to CDropSource object  
		SdkDropSource* m_pDropSource;
		//!< The collection of DATASTORAGE_t structure    
		std::vector<DATASTORAGE_t>   m_dataStorageCL;

		CLIPFORMAT _cfFormat = CF_UNICODETEXT;//CF_HDROP;
	};


	//----------接收拖动OLE-------------------------------------------------------------------
#if 1
	class GuiDropTarget :public IDuiDropTarget, public Res
	{
	public:
		std::function<void(const char*, size_t, bool, size_t num)> pOnChar = nullptr;
		std::function<bool(int x, int y)> pOnOver = nullptr;

		bool _isDrag = false;
	public:
		static GuiDropTarget* create()
		{
			return new GuiDropTarget();
		}
		bool isDrop()
		{
			return _isDrag;
		}
	public:
		GuiDropTarget() {}
		~GuiDropTarget() {}

		HRESULT OnDragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect)
		{
			_isDrag = true;
			return S_OK;
		}
		HRESULT OnDragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
		{
			/*
			DROPEFFECT_NONE=0 表示此窗口不能接受拖放。
			DROPEFFECT_COPY=1 表示拖放将引起源对象的。
			DROPEFFECT_MOVE=2 表示拖放的结果将使源对象被删除。
			DROPEFFECT_LINK =4 表示拖放源对象创建了一个对自己的连接。
			DROPEFFECT_SCROLL=0x80000000表示拖放目标窗口正在或将要进行卷滚。此标志可以和其他几个合用。
			对于拖放对象来说，一般只要使用DROPEFFECT_NONE和DROPEFFECT_COPY即可。
			*/
			DWORD effect = DROPEFFECT_COPY;
			if (pOnOver)
				effect = pOnOver(pt.x, pt.y) ? DROPEFFECT_COPY : DROPEFFECT_NONE;
			*pdwEffect = effect;
			return S_OK;
		}
		HRESULT OnDragLeave()
		{
			_isDrag = false;
			return S_OK;
		}
		HRESULT OnDrop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
		{
			GetDropData(pDataObj);
			_isDrag = false;
			return S_OK;
		}
		void GetDropData(__RPC__in_opt IDataObject* pDataObj)
		{
			IEnumFORMATETC* pEnumFmt = NULL;
			HRESULT ret = pDataObj->EnumFormatEtc(DATADIR_GET, &pEnumFmt);
			if (SUCCEEDED(ret))
			{
				pEnumFmt->Reset();
				HRESULT Ret = S_OK;
				FORMATETC cFmt = { (CLIPFORMAT)CF_TEXT, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

				STGMEDIUM stgMedium;
				//
				ULONG Fetched = 0;
				size_t inc = 0;
				do {
					Ret = pEnumFmt->Next(1, &cFmt, &Fetched);
					if (SUCCEEDED(ret))
					{
						if (cFmt.cfFormat == CF_HDROP ||
							cFmt.cfFormat == CF_TEXT || cFmt.cfFormat == CF_OEMTEXT ||
							cFmt.cfFormat == CF_UNICODETEXT
							)
						{
							ret = pDataObj->GetData(&cFmt, &stgMedium);

							if (FAILED(ret))
								return;

							if (stgMedium.pUnkForRelease != NULL)
								return;
							///////////////////////////////////////////
							switch (stgMedium.tymed)
							{
							case TYMED_HGLOBAL:
							{
								DRAGDATA pData;
								pData.cfFormat = cFmt.cfFormat;
								memcpy(&pData.stgMedium, &stgMedium, sizeof(STGMEDIUM));

								if (cFmt.cfFormat == CF_HDROP)
								{
									ProcessDrop((HDROP)stgMedium.hGlobal);
								}
								if (cFmt.cfFormat == CF_TEXT || cFmt.cfFormat == CF_OEMTEXT)
								{
									char* pBuff = 0;

									HGLOBAL  hText = stgMedium.hGlobal;
									pBuff = (LPSTR)GlobalLock(hText);
									if (pOnChar)
									{
										size_t tlen = GlobalSize(hText);
										pOnChar(pBuff, tlen, false, inc++);
									}
									GlobalUnlock(hText);
								}
								if (cFmt.cfFormat == CF_UNICODETEXT)
								{
									HGLOBAL  hText = stgMedium.hGlobal;
									LPWSTR pW = (LPWSTR)GlobalLock(hText);
									size_t tlen = GlobalSize(hText);
									std::wstring wstr(pW, tlen / sizeof(wchar_t));
									std::string str = w_u8(wstr.c_str());
									if (pOnChar)
									{
										pOnChar(str.c_str(), str.length(), false, inc++);
									}
									GlobalUnlock(hText);
								}

								return;
								break;
							}

							default:                        // type not supported, so return error
							{
								::ReleaseStgMedium(&stgMedium);
							}
							break;
							}
						}
					}
				} while (Ret == S_OK);
			}
		}
		BOOL ProcessDrop(HDROP hDrop)
		{
			UINT iFiles, ich = 0;
			char Buffer[MAX_PATH] = "";
			memset(&iFiles, 0xff, sizeof(iFiles));
			int Count = ::DragQueryFileA(hDrop, iFiles, Buffer, 0); //Get the Drag _Files Number.
			if (Count)
			{
				std::vector<std::string> fns;
				char* t = nullptr;
				size_t inc = 0;
				for (int i = 0; i < Count; ++i)
				{
					if (::DragQueryFileA(hDrop, i, Buffer, sizeof(Buffer)))
					{
						//Got the FileName in Buffer
						char* pBuff = 0;
						pBuff = (char*)Buffer;
						fns.push_back(pBuff);
					}
				}
				if (pOnChar)
				{

					pOnChar((char*)&fns, fns.size(), true, Count);
				}
			}
			::DragFinish(hDrop);
			return true;
		}
	};

#endif

	//-----------------------------------------------------------------------------
	struct DragDropData
	{
		CLIPFORMAT _cf = 0;
		void* _data = 0;
		size_t _size = 0;
		int* _outsta = 0;
		DragDropData(CLIPFORMAT cf, void* data, size_t  size, int* outsta) :_cf(cf)
			, _data(data), _size(size), _outsta(outsta)
		{}
	};
	static void* DoDragDropBegin(DragDropData d, std::function<void(int effect)> pOnEnd)
	{
		//OleInitialize(NULL);
		SdkDataObject* pDataObject = new SdkDataObject(NULL);
		pDataObject->SetBlob(d._cf, d._data, d._size);
		SdkDropSource* pSource = new SdkDropSource();
		DWORD  pdwEffect = 0;
		//if (d._outsta)
		//	(*d._outsta) = 0;
		DoDragDrop(pDataObject, pSource, DROPEFFECT_MOVE | DROPEFFECT_COPY, &pdwEffect);

		pDataObject->Release();
		pSource->Release();
		//if (d._outsta)
		{
			if (pOnEnd/* && (*d._outsta) == 1*/)
			{
				pOnEnd(pdwEffect);
			}
		}
		//OleUninitialize();
		return nullptr;
	}
#if 0
	//示例：
	//	接收拖动OLE

	CDropTargetEx _WDrop;
	void init()
	{
		::DragAcceptFiles(_hWnd, false);

		_WDrop.DragDropRegister(_hWnd);
		_WDrop.setOver([this](int x, int y)
			{
				// 拖动的时候鼠标事件
				POINT pt = { x,y };
				ScreenToClient(_hWnd, &pt);
				//printf("drop:x<%d>,y<%d>\n", pt.x, pt.y);
				Node* n = getPickNode({ pt.x, pt.y });
				onMouseMoveCallBack(0, pt.x, pt.y);

			});
	}
	// 创建接收处理事件
	_drop = GuiDropTarget::create();
	_drop->pOnChar = pOnChar;
	_drop->pOnOver = pOnOver;

	// 设置当前接收
	IDuiDropTarget* pDuiDropTarget = _drop : nullptr;
	_WDrop.SetDragDrop(pDuiDropTarget);


	//拖动_mouse_txt文本到OLE,类型可用CF_UNICODETEXT或CF_TEXT
	DoDragDropBegin(DragDropData{ CF_UNICODETEXT, (void*)_mouse_txt.c_str(), (int)_mouse_txt.length(), &_isDragEnd }, [this]()
		{
			if (!getReadonly())
				this->on_ime_char("", 0);
		});
#endif
}

#endif

#endif /*end __dropdrag__h*/
