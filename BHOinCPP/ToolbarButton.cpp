/*
 Copyright (C) 2009 Moutaz Haq <cefarix@gmail.com>
 This file is released under the Code Project Open License <http://www.codeproject.com/info/cpol10.aspx>

 This file contains our implementation of the IObjectWithSite interface, CToolbarButton.
*/

#include "common.h"
#include "ToolbarButton.h"
#include "EventSink.h"
#include <atlbase.h>
#include <atlwin.h>
#include "EventNotifier.h"

// The single global object of CEventSink
CEentSink *EventSink;

const IID CToolbarButton::SupportedIIDs[]={IID_IUnknown, IID_IOleCommandTarget};

CComModule _Module;
_ATL_OBJMAP_ENTRY* ObjectMap;
int wmId, wmEvent;
PAINTSTRUCT ps;
HDC hdc;
CComPtr<IWebBrowser2> m_IWebBrowser2Popover;  

IeExtToolbarButtonInfo m_IeExtToolbarButtonInfo;

IConnectionPoint *pCP; // the active connection point interface
DWORD adviseCookie; // used by ConnectEventSink() and DisconnectEventSink() in conjunction with pCP


boolean m_IsPopoverInitialized = false;

CToolbarButton::CToolbarButton(IeExtToolbarButtonInfo info) : CUnknown<IOleCommandTarget>(SupportedIIDs,2)
{
	EventSink = new CEentSink(IE_EXT_COMPONENT_TOOLBARBUTTON);
	m_IeExtToolbarButtonInfo = info;
	m_IsPopoverInitialized = false;
}

CToolbarButton::~CToolbarButton()
{

}

#define MYMENU_EXIT         (WM_APP + 101)
#define MYMENU_MESSAGEBOX   (WM_APP + 102) 

HINSTANCE  g_PopupInstance;          //Injected Modules Handle
HWND g_PopupHwnd;

//WndProc for the new window
LRESULT CALLBACK DLLWindowProc (HWND, UINT, WPARAM, LPARAM);

//Creating our windows Menu
HMENU CreateDLLWindowMenu()
{
	HMENU hMenu;
	hMenu = CreateMenu();
	HMENU hMenuPopup;
	if(hMenu==NULL)
		return FALSE;
	hMenuPopup = CreatePopupMenu();
	AppendMenu (hMenuPopup, MF_STRING, MYMENU_EXIT, TEXT("Exit"));
	AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hMenuPopup, TEXT("File")); 

	hMenuPopup = CreatePopupMenu();
	AppendMenu (hMenuPopup, MF_STRING,MYMENU_MESSAGEBOX, TEXT("MessageBox")); 
	AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hMenuPopup, TEXT("Test")); 
	return hMenu;
}

void ConnectEventSink(IWebBrowser2 *pSite)
{
	EventSink->m_IWebBrowser2 = pSite;

	HRESULT hr;
	IConnectionPointContainer* pCPC;

	if(pSite==NULL) return; // If we don't have a site, don't do anything
	// Get an IConnectionPointContainer interface pointer from the site
	hr=pSite->QueryInterface(IID_IConnectionPointContainer,(void**)&pCPC);
	if(FAILED(hr)) return; // If we couldn't get it, abort
	// Now we use the IConnectionPointContainer interface to get an IConnectionPoint interface pointer that will handle DWebBrowserEvents2 "dispatch interface" events.
	// That means we have to plug our implementation of DWebBrowserEvents2 into the returned IConnectionPoint interface using its Advise() method, as below
	hr=pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2,&pCP);
	if(FAILED(hr)) { // If it failed, release the pCPC interface pointer and abort
		pCPC->Release();
		return;
	}
	// Finally we can plug our event handler object EventSink into the connection point and start receiving IE events
	// The advise cookie is just a return value we use when we want to "unplug" our event handler object from the connection point
	pCP->Advise((IUnknown*)EventSink,&adviseCookie);
}

void DisconnectEventSink(IWebBrowser2 *pSite)
{
	if(pCP) { // if we have a valid connection point, unplug the event handler from it, then Release() it
		pCP->Unadvise(adviseCookie);
		adviseCookie=0;
		pCP->Release();
		pCP=NULL;
	}
	if(pSite) { // if we have a valid site, Release() it
		pSite->Release();
		pSite=NULL;
	}
}

#define WM_SP_POPOVER	0x10001

//Our new windows proc
LRESULT CALLBACK DLLWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SP_POPOVER:
		{
			EventNotifier::issueEvent(m_IWebBrowser2Popover, IE_EXT_EVENT_POPOVER);
		}
		break;
	case WM_PAINT:
		{
			//printf("WM_PAINT");
		}
		break;
	case WM_KILLFOCUS:
		//CloseWindow(hwnd);
		break;

	case WM_SHOWWINDOW:
		{
			if(wParam == TRUE) {
				_tprintf(_T("PopupWindow is visible"));
				if(m_IsPopoverInitialized == true) {
					SendMessageTimeout(g_PopupHwnd, WM_SP_POPOVER, 0, 0, SMTO_ABORTIFHUNG, 500, NULL);	
				} 
				m_IsPopoverInitialized = true;

			} else {
				_tprintf(_T("PopupWindow not visible"));
			}
		}
		break;
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_ACTIVE )
		{
			_tprintf(_T("PopupWindow is active"));
			if(m_pViewMMFFile ) {
				lstrcpy( (LPTSTR) m_pViewMMFFile, L"OPEN");
			}
		}
		else 
		{
			_tprintf(_T("PopupWindow is inactive"));
			ShowWindow(hwnd, SW_HIDE);

			if(m_pViewMMFFile ) {
				lstrcpy( (LPTSTR) m_pViewMMFFile, L"CLOSE");
			}
		}
		break ;
	case WM_CREATE:
		{
			RECT rc;
			VARIANT varMyURL;
			CAxWindow WinContainer;
			LPOLESTR pszName=OLESTR("shell.Explorer.2");
			GetClientRect(hwnd, &rc);
			GUID guid;

			_Module.Init(ObjectMap, hInstance,&guid);
			WinContainer.Create(hwnd, rc, 0,WS_CHILD |WS_VISIBLE);
			WinContainer.CreateControl(pszName);
			if(m_IWebBrowser2Popover == NULL) {
				WinContainer.QueryControl(__uuidof(IWebBrowser2),(void**)&m_IWebBrowser2Popover);
				m_IWebBrowser2Popover->put_Silent(VARIANT_TRUE);
			}
			VariantInit(&varMyURL);
			varMyURL.vt = VT_BSTR;
			varMyURL.bstrVal = SysAllocString(m_IeExtToolbarButtonInfo.page);
			m_IWebBrowser2Popover-> Navigate2(&varMyURL,0,0,0,0);

			// BH_Lin@20131014
			ConnectEventSink(m_IWebBrowser2Popover);

			VariantClear(&varMyURL);
		}
		break;	
	case WM_COMMAND:
		switch(wParam)
		{
		case MYMENU_EXIT:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case MYMENU_MESSAGEBOX:
			MessageBox(hwnd, L"Test", L"MessageBox",MB_OK);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage (0);
		break;
	default:
		return DefWindowProc (hwnd, message, wParam, lParam);
	}
	return 0;
}

//#define DEF_POPUP_WINDOW_NAME  _T("IeExtensionPopupWindow")
//#define DEF_POPUP_CLASS_NAME  _T("IeExtensionPopupClass")

BOOL  RegisterAndCreateWindow( HINSTANCE hModule) {
	
	//CreateThread(0, NULL, ThreadProc, (LPVOID)L"Popover Page", NULL, NULL);
	//ThreadProc((LPVOID)L"Popover Page");// BH_Lin@20131014

	BOOL result = FALSE;
	MSG messages;
	//HMENU hMenu = CreateDLLWindowMenu();
	g_PopupInstance = hModule;

	WNDCLASSEX wc;

	char *tempStr = const_cast<char* > (m_IeExtToolbarButtonInfo.extenionID.c_str());
	TCHAR *className = new TCHAR[MAX_CLSID_LENGTH];
	Util::AnsiToUnicode16(tempStr, className, MAX_CLSID_LENGTH);

	if(GetClassInfoEx(g_PopupInstance, className, &wc) == 0) {
		wc.hInstance =  g_PopupInstance;
		wc.lpszClassName = className;
		wc.lpfnWndProc = DLLWindowProc;
		wc.style = CS_DBLCLKS;
		wc.cbSize = sizeof (WNDCLASSEX);
		wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
		wc.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor (NULL, IDC_ARROW);
		wc.lpszMenuName = NULL;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
		if (RegisterClassEx (&wc)) {
			_tprintf(_T("Success: PopupWindowClass is registered"));
		} else {
			_tprintf(_T("Fail: PopupWindowClass not registered"));
		}
	}

	g_PopupHwnd = FindWindow(className, className);

	// try to get Cursor  position.
	POINT p;
	if (GetCursorPos(&p))
	{
		//cursor position now in p.x and p.y
		_tprintf(_T("getCursor X:%d,Y:%d"), p.x, p.y);
	}
	int POS_X_OF_POPUP = p.x;
	int POS_Y_OF_POPUP = p.y;
	int popupWidth = m_IeExtToolbarButtonInfo.popupWidth;
	int popupHeight = m_IeExtToolbarButtonInfo.popupHeight;

	if(g_PopupHwnd == NULL) {
		g_PopupHwnd = CreateWindowEx (
			0, 
			className, 
			className,//WindowName 
			WS_EX_PALETTEWINDOW, 
			POS_X_OF_POPUP, 
			POS_Y_OF_POPUP, 
			popupWidth, //nWidth 
			popupHeight, // nHeight 
			g_PopupHwnd, 
			NULL,
			g_PopupInstance, 
			NULL );		
	}

	SetWindowPos(g_PopupHwnd, NULL, POS_X_OF_POPUP, POS_Y_OF_POPUP, popupWidth, popupHeight, TRUE);
	ShowWindow (g_PopupHwnd, SW_SHOW);
	SetForegroundWindow(g_PopupHwnd);
	SetActiveWindow(g_PopupHwnd);

	while (GetMessage (&messages, NULL, 0, 0))
	{
		TranslateMessage(&messages);

		if((GetKeyState(VK_CONTROL) & 0x8000) && messages.wParam == 0x56) {
			m_IWebBrowser2Popover->ExecWB(OLECMDID_PASTE, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL); 
		}

		DispatchMessage(&messages);
	}

	
	
	return TRUE;	
}

//The new thread
DWORD WINAPI ThreadProc( LPVOID lpParam ) {

	RegisterAndCreateWindow(hInstance);

	return 0;
}

STDMETHODIMP CToolbarButton::Exec( const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdExecOpt, VARIANTARG *pvaIn, VARIANTARG *pvaOut )
{
	//CreateThread(0, NULL, ThreadProc, NULL, NULL, NULL);

	RegisterAndCreateWindow(hInstance);

	return S_OK;
}

STDMETHODIMP CToolbarButton::QueryStatus( const GUID *pguidCmdGroup, ULONG cCmds, OLECMD *prgCmds, OLECMDTEXT *pCmdText )
{
	return S_OK;
}


