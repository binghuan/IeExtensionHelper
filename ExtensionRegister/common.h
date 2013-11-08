/*
 Copyright (C) 2009 Moutaz Haq <cefarix@gmail.com>
 This file is released under the Code Project Open License <http://www.codeproject.com/info/cpol10.aspx>
*/

#ifndef __COMMON_H__
#define __COMMON_H__

#define _WIN32_WINNT 0x0600

#include <windows.h>
#include <tchar.h>



// Our main CLSID in string format
extern _TCHAR *CLSID_IE_BHO_Str;
extern _TCHAR *CLSID_IE_TOOBAR_Str;
extern CLSID CLSID_IE_BHO;
extern CLSID CLSID_IE_TOOBAR;
extern volatile LONG DllRefCount;
extern HINSTANCE hInstance;

#define MAX_STRING_LENGTH 1024

#define IE_EXT_COMPONENT_BHO 1
#define IE_EXT_COMPONENT_TOOLBARBUTTON 2
#define IE_EXT_COMPONENT_CONTENTSCRIPT 3


#endif // __COMMON_H__
