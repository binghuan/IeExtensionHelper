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

#include "common.h"
#include "Util.h"
#include "IeManifestParser.h"


using namespace Json;
using namespace std;

CLSID CLSID_IE_BHO;
CLSID CLSID_IE_TOOBAR;

int _tmain(int argc, const _TCHAR* argv[])
{
	IeManifestParser *parser = new IeManifestParser(NULL);
	IeExtBHOInfo bhoInfo = parser->getIeExtBHOInfo();
	IeExtToolbarButtonInfo toolbarButtonInfo = parser->getIeExtToolbarButtonInfo();
	IeExtContentScriptInfo contentScriptInfo = parser->getIeExtContentScriptInfo();

	printf("seed: %d\r\n",  rand()*10000);
	printf("seed: %d\r\n",  rand()*10000);
	printf("seed: %d\r\n",  rand()*10000);
	printf("seed: %d\r\n",  rand()*10000);

	Util::wait();
	return 0;
}

