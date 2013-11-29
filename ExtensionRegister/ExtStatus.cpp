#include "stdafx.h"
#include "ExtStatus.h"

ExtStatus::ExtStatus(void)
{

}

ExtStatus::ExtStatus(string extensionID )
{
	m_ValueBuffer = new CHAR[SHARE_VARIABLE_SIZE];
	ZeroMemory(m_ValueBuffer, SHARE_VARIABLE_SIZE);
	m_ExtensionID = new TCHAR[MAX_PATH];

	char *tempStr = const_cast<char* > (extensionID.c_str());

	Util::AnsiToUnicode16(tempStr, m_ExtensionID, MAX_PATH);

	//Creation and Mapping of Memory Mapped File
	m_hFileMMF = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,SHARE_VARIABLE_SIZE,m_ExtensionID);              
	DWORD dwError = GetLastError();
	if ( ! m_hFileMMF )
	{
		printf("Creation of file mapping failed: %d", dwError);
	}
	else
	{
		m_pViewMMFFile = MapViewOfFile(m_hFileMMF,FILE_MAP_ALL_ACCESS,0,0,0);                         // map all file

		if(! m_pViewMMFFile )
		{
			printf("MapViewOfFile function failed");
		}
	}

	if(m_pViewMMFFile)
	{
		ZeroMemory(m_ValueBuffer, SHARE_VARIABLE_SIZE);
		strcpy_s(m_ValueBuffer, SHARE_VARIABLE_SIZE, (LPSTR) m_pViewMMFFile);

		if(m_ValueBuffer == NULL || strlen(m_ValueBuffer) < 2) {
			printf("!! init status value !!\r\n");
			strcpy_s( (LPSTR) m_pViewMMFFile, SHARE_VARIABLE_SIZE, "{isPopoverVisible: false}");
		}
	}
}

ExtStatus::~ExtStatus(void)
{

}

void ExtStatus::retrieveExtStatus()
{
	if(m_pViewMMFFile)
	{
		ZeroMemory(m_ValueBuffer, SHARE_VARIABLE_SIZE);
		strcpy_s(m_ValueBuffer, SHARE_VARIABLE_SIZE, (LPSTR) m_pViewMMFFile);

		Features features;
		Reader reader(features);
		reader.parse(m_ValueBuffer, m_manifestRoot);

		m_manifestRoot["isPopoverVisible"] = m_manifestRoot.get("isPopoverVisible", NULL);
		m_manifestRoot["activeTab"] = m_manifestRoot.get("activeTab", NULL);
		m_manifestRoot["localStorage"] = m_manifestRoot.get("localStorage", NULL);
		m_manifestRoot["sessionStorage"] = m_manifestRoot.get("sessionStorage", NULL);
		m_manifestRoot["tabCounter"] = m_manifestRoot.get("tabCounter", NULL);
	}
}

void ExtStatus::commitChange()
{
	StyledWriter writer;
	string resultJsonString = writer.write( m_manifestRoot );

	strcpy_s( (LPSTR) m_pViewMMFFile, SHARE_VARIABLE_SIZE,resultJsonString.c_str());
}

boolean ExtStatus::isPopoverVisible() {
	boolean result = false;
	retrieveExtStatus();

	Value popupVisible = m_manifestRoot.get("isPopoverVisible", NULL);
	if(popupVisible != NULL) {
		result = popupVisible.asBool();
	}

	printf("isPopoverVisible --> %d", result);
	return result;
};

void ExtStatus::setSessionStorageData(wstring storageStr) {
	retrieveExtStatus();

	int nIndex = WideCharToMultiByte(CP_ACP, 0, storageStr.c_str(), -1, NULL, 0, NULL, NULL);
	char *pAnsi = new char[nIndex + 1];
	WideCharToMultiByte(CP_ACP, 0, storageStr.c_str(), -1, pAnsi, nIndex, NULL, NULL);

	m_manifestRoot["sessionStorage"] = pAnsi;
	commitChange();
}

wstring ExtStatus::getSessionStorageData()
{
	retrieveExtStatus();

	_TCHAR* result = new _TCHAR[SHARE_VARIABLE_SIZE];
	ZeroMemory(result, SHARE_VARIABLE_SIZE);

	Value storageData = m_manifestRoot.get("sessionStorage", NULL);
	if(storageData != NULL) {
		char* tempStr = const_cast<char*>(storageData.asCString());	
		Util::AnsiToUnicode16(tempStr, result, SHARE_VARIABLE_SIZE);
	}


	printf("getSessionStorageData --> %s", result);

	wstring data = result;

	if(result != NULL) {
		delete result;
	}

	return data;
}

void ExtStatus::setLocalStorageData(wstring localStorageStr) {
	retrieveExtStatus();

	int nIndex = WideCharToMultiByte(CP_ACP, 0, localStorageStr.c_str(), -1, NULL, 0, NULL, NULL);
	char *pAnsi = new char[nIndex + 1];
	ZeroMemory(pAnsi, nIndex + 1);
	WideCharToMultiByte(CP_ACP, 0, localStorageStr.c_str(), -1, pAnsi, nIndex, NULL, NULL);

	m_manifestRoot["localStorage"] = pAnsi;
	commitChange();
}

wstring ExtStatus::getLocalStorageData()
{
	retrieveExtStatus();

	_TCHAR* result = new _TCHAR[SHARE_VARIABLE_SIZE];
	ZeroMemory(result, SHARE_VARIABLE_SIZE);

	Value localStorageData = m_manifestRoot.get("localStorage", NULL);
	if(localStorageData != NULL) {
		char* tempStr = const_cast<char*>(localStorageData.asCString());	
		Util::AnsiToUnicode16(tempStr, result, SHARE_VARIABLE_SIZE);
	}

	printf("getLocalStorageData --> %s", result);
	wstring data = result;
	if(result != NULL) {
		delete result;
	}
	
	return data;
}

void ExtStatus::setPopoverVisible( boolean willVisible)
{
	retrieveExtStatus();

	m_manifestRoot["isPopoverVisible"] = willVisible;

	commitChange();
}

int ExtStatus::getTabCounter() {
	int result = false;
	retrieveExtStatus();

	Value tabCounter = m_manifestRoot.get("tabCounter", NULL);
	if(tabCounter != NULL) {
		result = tabCounter.asInt();
	}

	printf("isPopoverVisible --> %d", result);
	if(result < 0) 
	{ 
		result = 0;
	};
	return result;
};

void ExtStatus::tabCounterPlus()
{
	retrieveExtStatus();

	m_manifestRoot["tabCounter"] = getTabCounter() + 1;

	commitChange();
}

void ExtStatus::tabCounterMinus()
{
	retrieveExtStatus();

	m_manifestRoot["tabCounter"] = getTabCounter() - 1;

	commitChange();
}

void ExtStatus::setActiveTabInfo(int tabId, wstring title, wstring url)
{
	retrieveExtStatus();

	TCHAR *tabIDStr = new TCHAR[MAX_PATH];
	ZeroMemory(tabIDStr, MAX_PATH);
	swprintf_s( tabIDStr, MAX_PATH, _T("%d"),  tabId);

	wstring infoStr = L"{";
	infoStr += L"id:";
	infoStr += tabIDStr;
	infoStr += L",";
	infoStr += L"title:'" + title;
	infoStr += L"',";
	infoStr += L"url:'" + url;
	infoStr += L"'}";

	int nIndex = WideCharToMultiByte(CP_ACP, 0, infoStr.c_str(), -1, NULL, 0, NULL, NULL);
	char *pAnsi = new char[nIndex + 1];
	ZeroMemory(pAnsi, nIndex + 1);
	WideCharToMultiByte(CP_ACP, 0, infoStr.c_str(), -1, pAnsi, nIndex, NULL, NULL);
	//delete pAnsi;

	m_manifestRoot["activeTab"] = pAnsi;
	

	commitChange();
}

TCHAR* ExtStatus::getActiveTabInfo()
{
	retrieveExtStatus();
	_TCHAR* tabInfoStr = new _TCHAR[SHARE_VARIABLE_SIZE];

	Value activeTabInfo = m_manifestRoot.get("activeTab", NULL);
	if(activeTabInfo != NULL) {
		char* tempStr = const_cast<char*>(activeTabInfo.asCString());	
		Util::AnsiToUnicode16(tempStr, tabInfoStr, SHARE_VARIABLE_SIZE);
	}

	printf("getActiveTabInfo --> %s", tabInfoStr);
	return tabInfoStr;
}






