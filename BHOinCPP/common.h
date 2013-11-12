/*
 Copyright (C) 2009 Moutaz Haq <cefarix@gmail.com>
 This file is released under the Code Project Open License <http://www.codeproject.com/info/cpol10.aspx>
*/

#ifndef __COMMON_H__
#define __COMMON_H__

#include <windows.h>
#include <tchar.h>
#include <atlcomcli.h>
#include <exdisp.h>

// Our main CLSID in string format
#define CLSID_IE_BHO_Str _T("{0CA63DE6-4947-4096-959C-002FEB7C2F90}")
#define CLSID_IE_TOOBAR_Str _T("{25F07CA0-787A-4770-B329-941EB72FDA05}")
extern const CLSID CLSID_IE_BHO;
extern const CLSID CLSID_IE_TOOBAR;
extern volatile LONG DllRefCount;
extern HINSTANCE hInstance;

#define MAX_STRING_LENGTH 1024

#define IE_EXT_COMPONENT_BHO 1
#define IE_EXT_COMPONENT_TOOLBARBUTTON 2
#define IE_EXT_COMPONENT_CONTENTSCRIPT 3

extern CComPtr<IWebBrowser2> m_IWebBrowser2BHO;
extern CComPtr<IWebBrowser2> m_IWebBrowser2ContentScript;
extern CComPtr<IWebBrowser2> m_IWebBrowser2Popover;


#define IS_CONTENTSCRIPT_ISOLATEDWORLD_ENABLED true

#define IE_EXT_EVENT_POPOVER L"onIeExtensionEventPopover"
#define IE_EXT_EVENT_TAB_INACTIVATE L"onIeExtensionEventTabInactivate"
#define IE_EXT_EVENT_TAB_ACTIVATE L"onIeExtensionEventTabActivate"
#define IE_EXT_EVENT_TAB_OPEN L"onIeExtensionEventTabOpen"

#endif // __COMMON_H__
