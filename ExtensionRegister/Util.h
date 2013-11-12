#pragma once
#include <tchar.h>
#include <windows.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <math.h> 

using namespace std;

#define BUFFERSIZE 999999
//#define BUFFERSIZE 9999

class Util
{
public:
	Util(void) {};
	~Util(void) {};

	static DISPID FindId(IDispatch *pObj, LPOLESTR pName)
	{
		DISPID id = 0;
		if(FAILED(pObj->GetIDsOfNames(IID_NULL,&pName,1,LOCALE_SYSTEM_DEFAULT,&id))) {
			id = -1;
		}

		return id;
	};

	

	static HRESULT InvokeMethod(IDispatch *pObj, LPOLESTR pName, VARIANT *pVarResult, VARIANT *p, int cArgs)
	{
		DISPID dispid = FindId(pObj, pName);
		if(dispid == -1) return E_FAIL;

		DISPPARAMS ps;
		ps.cArgs = cArgs;
		ps.rgvarg = p;
		ps.cNamedArgs = 0;
		ps.rgdispidNamedArgs = NULL;

		return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &ps, pVarResult, NULL, NULL);
	}

	static HRESULT GetProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue)
	{
		DISPID dispid = FindId(pObj, pName);
		if(dispid == -1) return E_FAIL;

		DISPPARAMS ps;
		ps.cArgs = 0;
		ps.rgvarg = NULL;
		ps.cNamedArgs = 0;
		ps.rgdispidNamedArgs = NULL;

		return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &ps, pValue, NULL, NULL);
	}

	static HRESULT SetProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue)
	{
		DISPID dispid = FindId(pObj, pName);
		if(dispid == -1) return E_FAIL;

		DISPPARAMS ps;
		ps.cArgs = 1;
		ps.rgvarg = pValue;
		ps.cNamedArgs = 0;
		ps.rgdispidNamedArgs = NULL;

		return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUT, &ps, NULL, NULL, NULL);
	}

	static void rand_str(char *dest, size_t length) {
		char charset[] = 
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		while (length-- > 0) {
			size_t index = (size_t) rand() / RAND_MAX * (sizeof charset - 1);
			*dest++ = charset[index];
		}
		*dest = '\0';
	};


	static void wait()
	{
		cout<<"\r\nPress ENTER to continue....."<<endl<<endl;
		cin.ignore(1);
	};

	static wstring getModuleFolderPath(_TCHAR *folderPath) {
		string::size_type pos = wstring( folderPath ).find_last_of(_T("\\/") );
		return wstring( folderPath ).substr( 0, pos);
	};

	static boolean writeFile(char* DataBuffer, wstring fileName) {

		boolean isOK = true;


		HANDLE hFile; 
		DWORD dwBytesToWrite = (DWORD)strlen(DataBuffer);
		DWORD dwBytesWritten = 0;
		BOOL bErrorFlag = FALSE;

		printf("\n");

		hFile = CreateFile(fileName.c_str(),                // name of the write
			GENERIC_WRITE,          // open for writing
			0,                      // do not share
			NULL,                   // default security
			OPEN_EXISTING,             // create new file only
			FILE_ATTRIBUTE_NORMAL,  // normal file
			NULL);                  // no attr. template

		if (hFile == INVALID_HANDLE_VALUE) 
		{ 
			//DisplayError(TEXT("CreateFile"));
			_tprintf(TEXT("Terminal failure: Unable to open file \"%s\" for write.\n"), fileName.c_str());
			return false;
		}

		_tprintf(TEXT("Writing %d bytes to %s.\n"), dwBytesToWrite, fileName.c_str());

		bErrorFlag = WriteFile( 
			hFile,           // open file handle
			DataBuffer,      // start of data to write
			dwBytesToWrite,  // number of bytes to write
			&dwBytesWritten, // number of bytes that were written
			NULL);            // no overlapped structure

		if (FALSE == bErrorFlag)
		{
			//DisplayError(TEXT("WriteFile"));
			printf("Terminal failure: Unable to write to file.\n");
		}
		else
		{
			if (dwBytesWritten != dwBytesToWrite)
			{
				// This is an error because a synchronous write that results in
				// success (WriteFile returns TRUE) should write all data as
				// requested. This would not necessarily be the case for
				// asynchronous writes.
				printf("Error: dwBytesWritten != dwBytesToWrite\n");
			}
			else
			{
				_tprintf(TEXT("Wrote %d bytes to %s successfully.\n"), dwBytesWritten, fileName.c_str());
			}
		}

		CloseHandle(hFile);
		return isOK;
	}

	static char* readInputFile(wstring filePath) {
		HANDLE hFile; 
		DWORD  dwBytesRead = 0;
		CHAR *ReadBuffer = new CHAR[BUFFERSIZE];

		hFile = CreateFile(filePath.c_str(),               // file to open
			GENERIC_READ,          // open for reading
			FILE_SHARE_READ,       // share for reading
			NULL,                  // default security
			OPEN_EXISTING,         // existing file only
			FILE_ATTRIBUTE_NORMAL, // normal file
			NULL);                 // no attr. template

		if (hFile == INVALID_HANDLE_VALUE) 
		{ 
			//DisplayError(TEXT("CreateFile"));
			_tprintf(TEXT("Terminal failure: unable to open file \"%s\" for read.\n"), filePath);
			return ""; 
		} else {
			//DisplayError(_T("Success: CreateFile"));
		}

		// Read one character less than the buffer size to save room for
		// the terminating NULL character. 

		if( FALSE == ReadFile(hFile, ReadBuffer, BUFFERSIZE-1, &dwBytesRead, NULL) )
		{
			//DisplayError(TEXT("ReadFile"));
			_tprintf(_T("Terminal failure: Unable to read from file.\n"));
			CloseHandle(hFile);
			return "";
		} else {
			//DisplayError(TEXT("Success: ReadFile"));
		}

		// This is the section of code that assumes the file is ANSI text. 
		// Modify this block for other data types if needed
		if (dwBytesRead > 0 && dwBytesRead <= BUFFERSIZE-1)
		{
			ReadBuffer[dwBytesRead]='\0'; // NULL character

			_tprintf(TEXT("Data read from %s (%d bytes): \n"), filePath.c_str(), dwBytesRead);
			//DisplayError(TEXT("Success: Read Data"));
		}
		else if (dwBytesRead == 0)
		{
			_tprintf(TEXT("No data read from file %s\n"),filePath);
		}
		else
		{
			_tprintf(_T("\n ** Unexpected value for dwBytesRead ** \n"));
		}

		// It is always good practice to close the open file handles even though
		// the app will exit here and clean up open handles anyway.
		CloseHandle(hFile);

		return ReadBuffer; 
	};

	/***************************/
	/* ansi-unicode conversion */
	/***************************/

	static BOOL AnsiToUnicode16(CHAR *in_Src, WCHAR *out_Dst, INT in_MaxLen)
	{
		/* locals */
		INT lv_Len;

		// do NOT decrease maxlen for the eos
		if (in_MaxLen <= 0)
			return FALSE;

		// let windows find out the meaning of ansi
		// - the SrcLen=-1 triggers MBTWC to add a eos to Dst and fails if MaxLen is too small.
		// - if SrcLen is specified then no eos is added
		// - if (SrcLen+1) is specified then the eos IS added
		lv_Len = MultiByteToWideChar(CP_ACP, 0, in_Src, -1, out_Dst, in_MaxLen);

		// validate
		if (lv_Len < 0) {
			lv_Len = 0;
		}
			
		// ensure eos, watch out for a full buffersize
		// - if the buffer is full without an eos then clear the output like MBTWC does
		//   in case of too small outputbuffer
		// - unfortunately there is no way to let MBTWC return shortened strings,
		//   if the outputbuffer is too small then it fails completely
		if (lv_Len < in_MaxLen){
			out_Dst[lv_Len] = 0;
		} else if (out_Dst[in_MaxLen-1]) {
			out_Dst[0] = 0;
		}

		// done
		return TRUE;
	};


	static BOOL AnsiToUnicode16L(CHAR *in_Src, INT in_SrcLen, WCHAR *out_Dst, INT in_MaxLen)
	{
		/* locals */
		INT lv_Len;


		// do NOT decrease maxlen for the eos
		if (in_MaxLen <= 0)
			return FALSE;

		// let windows find out the meaning of ansi
		// - the SrcLen=-1 triggers MBTWC to add a eos to Dst and fails if MaxLen is too small.
		// - if SrcLen is specified then no eos is added
		// - if (SrcLen+1) is specified then the eos IS added
		lv_Len = MultiByteToWideChar(CP_ACP, 0, in_Src, in_SrcLen, out_Dst, in_MaxLen);

		// validate
		if (lv_Len < 0)
			lv_Len = 0;

		// ensure eos, watch out for a full buffersize
		// - if the buffer is full without an eos then clear the output like MBTWC does
		//   in case of too small outputbuffer
		// - unfortunately there is no way to let MBTWC return shortened strings,
		//   if the outputbuffer is too small then it fails completely
		if (lv_Len < in_MaxLen)
			out_Dst[lv_Len] = 0;
		else if (out_Dst[in_MaxLen-1])
			out_Dst[0] = 0;

		// done
		return TRUE;
	};
};

