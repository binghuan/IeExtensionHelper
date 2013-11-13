#pragma once


#include "Util.h"
#include "jsoncpp_5_0\include\json\json.h"
#include <string.h>

using namespace Json;
using namespace std;

#define SHARE_VARIABLE_SIZE 8192

class ExtStatus
{
public:
	ExtStatus(void);
	ExtStatus(string extensionID);

	~ExtStatus(void);

	boolean isPopoverVisible();
	void setPopoverVisible(boolean);
	void setActiveTabInfo(int tabId, wstring title, wstring url);
	TCHAR* getActiveTabInfo();

	void retrieveExtStatus();
	void commitChange();

private:
	TCHAR* m_ExtensionID;

	CHAR* m_ValueBuffer;
	Value m_manifestRoot;

	LPVOID m_pViewMMFFile;
	HANDLE m_hFileMMF;
};

