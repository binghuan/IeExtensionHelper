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
	}
}

void ExtStatus::commitChange( string changedValue )
{
	char *tempStr = const_cast<char* > (changedValue.c_str());
	strcpy_s( (LPSTR) m_pViewMMFFile, SHARE_VARIABLE_SIZE,tempStr);
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

void ExtStatus::setPopoverVisible( boolean willVisible)
{
	retrieveExtStatus();

	m_manifestRoot["isPopoverVisible"] = willVisible;
	StyledWriter writer;
	string resultJsonString = writer.write( m_manifestRoot );

	commitChange(resultJsonString);
}




