// ExtensionRegister.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "json/json.h"
#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <shlwapi.h>

#include <ctime>
#include "Util.h"
#include "IeManifestParser.h"

#include "ExtStatus.h"

using namespace Json;
using namespace std;

LPVOID m_pViewMMFFile;
HANDLE m_hFileMMF;

int _tmain(int argc, const _TCHAR* argv[])
{
	IeManifestParser *parser = new IeManifestParser(NULL);
	IeExtBHOInfo bhoInfo = parser->getIeExtBHOInfo();
	IeExtToolbarButtonInfo toolbarButtonInfo = parser->getIeExtToolbarButtonInfo();
	IeExtContentScriptInfo contentScriptInfo = parser->getIeExtContentScriptInfo();

	ExtStatus *extStatus = new ExtStatus(parser->m_IeExtensionID);
	extStatus->isPopoverVisible();

	extStatus->setPopoverVisible(true);

	Util::wait();
	return 0;
}

