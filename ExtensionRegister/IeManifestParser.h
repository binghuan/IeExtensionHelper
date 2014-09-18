#pragma once
#include <string>
#include <wchar.h>
#include <stdio.h>
#include "json/json.h"
#include <algorithm>

#include "..\ExtensionRegister\Util.h"

using namespace Json;
using namespace std;


#define MAX_CLSID_LENGTH 128

struct IeExtBHOInfo{
	CLSID clsid;
	_TCHAR* page;
	_TCHAR *clsidStr;
	_TCHAR *name;
	boolean isDefined;
	boolean debug;
	string extenionID;
	wstring rootFolderPath;
} ;

struct IeExtToolbarButtonInfo{
	CLSID clsid;
	_TCHAR* page;
	_TCHAR *clsidStr;
	_TCHAR *name;
	boolean isDefined;
	_TCHAR* iconPath;
	int popupWidth;
	int popupHeight;
	boolean debug;
	string extenionID;
	boolean isInitialized;
	wstring rootFolderPath;
} ;

struct IeExtContentScriptInfo{
	vector<wstring> javascriptsAtEnd;
	vector<wstring> javascriptsAtStart;
	boolean isDefined;
	string extenionID;
	wstring rootFolderPath;
} ;

class IeManifestParser
{
public:
	IeManifestParser(HINSTANCE instance);
	~IeManifestParser(void);

	IeExtBHOInfo getIeExtBHOInfo();
	IeExtToolbarButtonInfo getIeExtToolbarButtonInfo();
	IeExtContentScriptInfo getIeExtContentScriptInfo();

	wstring m_fileFolderPath;
	wstring m_IeManifestFilePath;
	char* m_IeExtensionID;
	Value m_manifestRoot;
	TCHAR m_ExtensionFilePath[MAX_PATH];
};

