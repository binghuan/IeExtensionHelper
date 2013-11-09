#include "common.h"
#include <Olectl.h>
#include "ClassFactory.h"

#include <iostream>
#include "json/json.h"
using namespace Json;

#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
using namespace std;

#include <Shlwapi.h>
#include <comcat.h>

#include "Cathelp.h"
#include "../ExtensionRegister/IeManifestParser.h"

_TCHAR msgBuff[4096];

// Our DLL-global reference count. 
// This is incremented and decremented as objects are created and destroyed by the DLL.
volatile LONG DllRefCount=0;
// Our DLL's HINSTANCE
HINSTANCE hInstance=NULL;
// This our BHO's CLSID. 
// It is a globally unique identifier (GUID) which identifies our BHO's main class to the system. 
// It was generated by the guidgen.exe utility.
// {3543619C-D563-43f7-95EA-4DA7E1CC396A}

const CLSID CLSID_IE_BHO = { 0xca63de6, 0x4947, 0x4096, { 0x95, 0x9c, 0x0, 0x2f, 0xeb, 0x7c, 0x2f, 0x90 } }; // The CLSID in binary format
const CLSID CLSID_IE_TOOBAR = { 0x25f07ca0, 0x787a, 0x4770, { 0xb3, 0x29, 0x94, 0x1e, 0xb7, 0x2f, 0xda, 0x5 } }; // The CLSID in binary format

void DisplayError(_TCHAR *lpszFunction) 
	// Routine Description:
	// Retrieve and output the system error message for the last-error code
{ 
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, 
		NULL );

	lpDisplayBuf = 
		(LPVOID)LocalAlloc( LMEM_ZEROINIT, 
		( lstrlen((LPCTSTR)lpMsgBuf)
		+ lstrlen((LPCTSTR)lpszFunction)
		+ 40) // account for format string
		* sizeof(TCHAR) );

	if (FAILED( StringCchPrintf((LPTSTR)lpDisplayBuf, 
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error code %d as follows:\n%s"), 
		lpszFunction, 
		dw, 
		lpMsgBuf)))
	{
		printf("FATAL ERROR: Unable to output error code.\n");
	}

	//_tprintf(TEXT("ERROR: %s\n"), (LPCTSTR)lpDisplayBuf);
	MessageBox(NULL,  (LPCTSTR)lpDisplayBuf, _T("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

// Called when the DLL is loaded into the process, attached or detached from a thread, and unloaded from the process
BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	UNREFERENCED_PARAMETER(lpvReserved);
	TCHAR mainexe[1024];
	int len;

	if(fdwReason==DLL_PROCESS_ATTACH) { // we only care about when the DLL is loaded into the process
		hInstance=hinstDLL; // store our HINSTANCE
		DisableThreadLibraryCalls(hInstance); // Disable calls to DllMain on thread attach/detach. Increases performance since we don't need those notifications anyways.
		// Get the full path of the main executable module of the process that loaded us
		// Since explorer.exe sometimes also loads BHOs, we want to stop the DLL from loading if we are being loaded by explorer.exe
		// Note that we can't check if we are being loaded into iexplore.exe, because other processes can have legitimate reasons for loading us as well
		//  such as regsvr32.exe for registering and unregistering our COM class.
		GetModuleFileName(NULL,mainexe,1024);
		len=_tcsnlen(mainexe,1024);
		if(len>12 && _tcsicmp(mainexe+len-12,_T("explorer.exe"))==0) return FALSE;
	}

	return TRUE;
}

// Called by COM to get a reference to our CClassFactory object
STDAPI DllGetClassObject(REFIID rclsid,REFIID riid,LPVOID *ppv)
{
	HRESULT hr;

	// We only support one class, make sure rclsid matches CLSID_IEPlugin
	// We only support one class, make sure rclsid matches CLSID_IEPlugin
	if(IsEqualCLSID(rclsid,CLSID_IE_BHO) ||
		IsEqualCLSID(rclsid,CLSID_IE_TOOBAR)) 
	{
		// ok
	} else {
		return CLASS_E_CLASSNOTAVAILABLE;
	}

	// Make sure the ppv pointer is valid
	if(IsBadWritePtr(ppv,sizeof(LPVOID))) return E_POINTER;
	// Set *ppv to NULL
	(*ppv)=NULL;
	// Create a new CClassFactory object
	CClassFactory *pFactory=new CClassFactory(rclsid, hInstance);
	// If we couldn't allocate the new object, return an out-of-memory error
	if(pFactory==NULL) return E_OUTOFMEMORY;
	// Query the pFactory object for the requested interface
	hr=pFactory->QueryInterface(riid,ppv);
	// If the requested interface isn't supported by pFactory, delete the newly created object
	if(FAILED(hr)) delete pFactory;
	// Return the same HRESULT as CClassFactory::QueryInterface
	return hr;
}

// This function is called by COM to determine if the DLL safe to unload.
// We return true if no objects from this DLL are being used and the DLL is unlocked.
STDAPI DllCanUnloadNow()
{
	if(DllRefCount>0) return S_FALSE;
	return S_OK;
}

CLSID StringToGUID(_TCHAR *guidStr)
{
	CLSID clsid;
	CLSIDFromString(guidStr, &clsid); 
	return clsid;
}

DEFINE_GUID(CATID_AppContainerCompatible, 0x59fb2056,0xd625,0x48d0,0xa9,0x44,0x1a,0x85,0xb5,0xab,0x26,0x40);
const CATID CATID_SafeForScripting     =
{0x7dd95801,0x9882,0x11cf,{0x9f,0xa9,0x00,0xaa,0x00,0x6c,0x42,0xc4}};
const CATID CATID_SafeForInitializing  =
{0x7dd95802,0x9882,0x11cf,{0x9f,0xa9,0x00,0xaa,0x00,0x6c,0x42,0xc4}};


// This function is called to register our DLL in the system, 
// for example, by regsvr32.exe
// We register ourselves with both COM and Internet Explorer
STDAPI DllRegisterServer()
{
	MessageBox(NULL, _T("DllRegisterServer"), _T("Information"), MB_OK);

	DWORD n;

	// Get the full path to this DLL's file so we can register it
	TCHAR *dllpath = new TCHAR[MAX_PATH];
	GetModuleFileName(hInstance,dllpath,1024);

	IeManifestParser *parser = new IeManifestParser(hInstance);

	//TCHAR *dllpath = parser->m_ExtensionFilePath;

	IeExtBHOInfo bhoInfo = parser->getIeExtBHOInfo();
	IeExtToolbarButtonInfo toolbarButtonInfo = parser->getIeExtToolbarButtonInfo();

	_TCHAR *registryPath = new _TCHAR[MAX_PATH];

	if(bhoInfo.isDefined == TRUE) {

		HKEY hk;
		// Create our key under HKCR\\CLSID
		ZeroMemory(registryPath, MAX_PATH);
		swprintf_s( registryPath, MAX_PATH, _T("CLSID\\%s"),  bhoInfo.clsidStr);

		if(RegCreateKeyEx(
			HKEY_CLASSES_ROOT,
			registryPath,
			0,
			NULL,
			0,
			KEY_ALL_ACCESS,
			NULL,
			&hk,
			NULL)!=ERROR_SUCCESS) 
		{
			return SELFREG_E_CLASS;
		}
		// Set the name of our BHO
		RegSetValueEx(
			hk,
			NULL,
			0,
			REG_SZ,
			//(const BYTE*)_T("BHBHO"),
			(const BYTE*)bhoInfo.name,
			24*sizeof(TCHAR));
		RegCloseKey(hk);

		// Create the InProcServer32 key
		ZeroMemory(registryPath, MAX_PATH);
		swprintf_s( registryPath, MAX_PATH, _T("CLSID\\%s\\InProcServer32"),  bhoInfo.clsidStr);
		if(RegCreateKeyEx(HKEY_CLASSES_ROOT,
			//_T("CLSID\\") CLSID_IE_BHO_Str _T("\\InProcServer32"),
			registryPath,
			0,
			NULL,
			0,
			KEY_ALL_ACCESS,NULL,&hk,NULL)!=ERROR_SUCCESS) return SELFREG_E_CLASS;
		// Set the path to this DLL
		RegSetValueEx(hk,
			NULL,
			0,
			REG_SZ,
			(const BYTE*)dllpath,
			(_tcslen(dllpath)+1)*sizeof(TCHAR));
		// Set the ThreadingModel to Apartment
		RegSetValueEx(hk,
			_T("ThreadingModel"),
			0,
			REG_SZ,
			(const BYTE*)_T("Apartment"),
			10*sizeof(TCHAR));
		RegCloseKey(hk);

		// Now register the BHO with Internet Explorer
		ZeroMemory(registryPath, MAX_PATH);
		swprintf_s( registryPath, MAX_PATH, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects\\%s"),  bhoInfo.clsidStr);
		if(RegCreateKeyEx(
			HKEY_LOCAL_MACHINE,
			//_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects\\") CLSID_IE_BHO_Str,
			registryPath,
			0,
			NULL,
			0,
			KEY_ALL_ACCESS,NULL,
			&hk,
			NULL)!=ERROR_SUCCESS) 
		{
			return SELFREG_E_CLASS;
		}
		// I believe the following tells explorer.exe not to load our BHO
		n=1;
		RegSetValueEx(hk,_T("NoExplorer"),0,REG_DWORD,(const BYTE*)&n,sizeof(DWORD));
		RegCloseKey(hk);
	}

	//if(false) {
	if(toolbarButtonInfo.isDefined == TRUE) {

		HKEY hk;

		// Create the InProcServer32 key
		ZeroMemory(registryPath, MAX_PATH);
		swprintf_s( registryPath, MAX_PATH, _T("CLSID\\%s\\InProcServer32"),  toolbarButtonInfo.clsidStr);
		if(RegCreateKeyEx(HKEY_CLASSES_ROOT,
			registryPath,
			0,
			NULL,
			0,
			KEY_ALL_ACCESS,NULL,&hk,NULL)!=ERROR_SUCCESS) {
				return SELFREG_E_CLASS;
		}

		// Set the path to this DLL
		
		RegSetValueEx(hk,
			NULL,
			0,
			REG_SZ,
			(const BYTE*)dllpath,
			(_tcslen(dllpath)+1)*sizeof(TCHAR));
		// Set the ThreadingModel to Apartment
		RegSetValueEx(hk,
			_T("ThreadingModel"),
			0,
			REG_SZ,
			(const BYTE*)_T("Apartment"),
			10*sizeof(TCHAR));
		RegCloseKey(hk);

		ZeroMemory(registryPath, MAX_PATH);
		swprintf_s( registryPath, MAX_PATH, _T("SOFTWARE\\Microsoft\\Internet Explorer\\Extensions\\%s"),  toolbarButtonInfo.clsidStr);
		if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,
			registryPath,
			0,
			NULL,
			0,
			KEY_ALL_ACCESS,
			NULL,
			&hk,
			NULL)!=ERROR_SUCCESS) 
		{
			return SELFREG_E_CLASS;
		}
		// I believe the following tells explorer.exe not to load our BHO
		n=1;
		RegSetValueEx(hk,_T("ButtonText"),0,REG_SZ,(const BYTE*)toolbarButtonInfo.name,MAX_PATH);
		RegSetValueEx(hk,_T("Icon"),0,REG_SZ,(const BYTE*)toolbarButtonInfo.iconPath,MAX_PATH);


		// Use the value {1FBA04EE-3024-11D2-8F1F-0000F87ABD16} to identify this toolbar button as pointing to an executable. 
		// In this case, you must also include the Exec value to identify the executable.
		// Use the CLSID for the DLL or the value {1FBA04EE-3024-11d2-8F1F-0000F87ABD16} to identify this toolbar button as pointing to a COM DLL. The class should implement the IOleCommandTarget interface. 
		RegSetValueEx(hk,_T("CLSID"),0,REG_SZ,(const BYTE*)_T("{1FBA04EE-3024-11d2-8F1F-0000F87ABD16}"),sizeof(_T("{1FBA04EE-3024-11d2-8F1F-0000F87ABD16}")));
		RegSetValueEx(hk,_T("ClsidExtension"),0,REG_SZ,(const BYTE*)toolbarButtonInfo.clsidStr,MAX_PATH);
		RegSetValueEx(hk,_T("Default Visible"),0,REG_SZ,(const BYTE*)_T("Yes"),sizeof(_T("Yes")));

		RegCloseKey(hk);

	}

	// Register Context MENU
	if(false) {

		HKEY hk;
		// Now register the BHO with Internet Explorer
		if(RegCreateKeyEx(
			HKEY_CURRENT_USER,
			_T("Software\\Microsoft\\Internet Explorer\\MenuExt\\Inspect Element"),
			0,
			NULL,
			0,
			KEY_ALL_ACCESS,NULL,
			&hk,
			NULL)!=ERROR_SUCCESS) 
		{
			return SELFREG_E_CLASS;
		}
		// I believe the following tells explorer.exe not to load our BHO
		n=1;
		if(RegSetValueEx(hk,_T("Contexts"),0,REG_DWORD,(const BYTE*)&n,sizeof(DWORD)) != ERROR_SUCCESS) {
			return SELFREG_E_CLASS;
		};

		swprintf_s( registryPath, MAX_PATH, _T("%s\\contextMenu.html"),  parser->m_fileFolderPath);
		if(RegSetValueEx(hk,NULL,0,REG_SZ,(const BYTE*)_T("C:\\Users\\binghuan\\Desktop\\temp\\contextMenu.html"),256*sizeof(TCHAR)) != ERROR_SUCCESS) {
			return SELFREG_E_CLASS;
		};
		RegCloseKey(hk);
	}


	// EPM
	//HRESULT hr =CreateComponentCategory(CATID_SafeForScripting,L"Controls that are safely scriptable");
	//hr = CreateComponentCategory(CATID_SafeForInitializing,L"Controls safely initializable from persistent data");
	HRESULT hr = CreateComponentCategory(CATID_AppContainerCompatible,L"IE Extension Component");
	  //hr = RegisterCLSIDInCategory(CLSID_IE_BHO, CATID_SafeForScripting);
	  //hr = RegisterCLSIDInCategory(CLSID_IE_TOOBAR, CATID_SafeForScripting);
	  //hr = RegisterCLSIDInCategory(CLSID_IE_BHO, CATID_SafeForInitializing);
	  //hr = RegisterCLSIDInCategory(CLSID_IE_TOOBAR, CATID_SafeForInitializing);
	   hr = RegisterCLSIDInCategory(CLSID_IE_BHO, CATID_AppContainerCompatible);
	   hr = RegisterCLSIDInCategory(CLSID_IE_TOOBAR, CATID_AppContainerCompatible);
	return S_OK;
}

// This function is called to unregister our DLL in the system, 
// for example, by regsvr32.exe
// We remove our registration entries from both COM and Internet Explorer
STDAPI DllUnregisterServer()
{

	IeManifestParser *parser = new IeManifestParser(hInstance);
	IeExtBHOInfo bhoInfo = parser->getIeExtBHOInfo();
	IeExtToolbarButtonInfo toolbarButtonInfo = parser->getIeExtToolbarButtonInfo();

	// Remove the Internet Explorer BHO registration
	_TCHAR *registryPath = new _TCHAR[MAX_PATH];
	ZeroMemory(registryPath, MAX_PATH);
	swprintf_s( registryPath, MAX_PATH, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects\\%s"),  bhoInfo.clsidStr);
	//RegDeleteKey(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects\\") bhoInfo.clsidStr);
	RegDeleteKey(HKEY_LOCAL_MACHINE,registryPath);

	// Remove the COM registration, starting with the deeper key first since RegDeleteKey is not recursive
	ZeroMemory(registryPath, MAX_PATH);
	swprintf_s( registryPath, MAX_PATH, _T("CLSID\\%s\\InProcServer32"),  bhoInfo.clsidStr);
	//RegDeleteKey(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_IE_BHO_Str _T("\\InProcServer32"));
	RegDeleteKey(HKEY_CLASSES_ROOT,registryPath);

	ZeroMemory(registryPath, MAX_PATH);
	swprintf_s( registryPath, MAX_PATH, _T("CLSID\\%s"),  bhoInfo.clsidStr);
	//RegDeleteKey(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_IE_BHO_Str);
	RegDeleteKey(HKEY_CLASSES_ROOT, registryPath);

	ZeroMemory(registryPath, MAX_PATH);
	swprintf_s( registryPath, MAX_PATH, _T("Software\\Microsoft\\Internet Explorer\\Extensions\\%s"),  toolbarButtonInfo.clsidStr);
	//RegDeleteKey(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Internet Explorer\\Extensions\\") CLSID_IE_TOOBAR_Str );
	RegDeleteKey(HKEY_LOCAL_MACHINE,registryPath );

	ZeroMemory(registryPath, MAX_PATH);
	swprintf_s( registryPath, MAX_PATH, _T("CLSID\\%s\\InProcServer32"),  toolbarButtonInfo.clsidStr);
	//RegDeleteKey(HKEY_CLASSES_ROOT,_T("CLSID\\") CLSID_IE_TOOBAR_Str _T("\\InProcServer32"));
	RegDeleteKey(HKEY_CLASSES_ROOT,registryPath);

	ZeroMemory(registryPath, MAX_PATH);
	swprintf_s( registryPath, MAX_PATH, _T("CLSID\\%s"),  toolbarButtonInfo.clsidStr);
	RegDeleteKey(HKEY_CLASSES_ROOT,registryPath);

	// remove ContextMenu
	RegDeleteKey(
		HKEY_CURRENT_USER,
		_T("Software\\Microsoft\\Internet Explorer\\MenuExt\\Inspect Element")
		);

	return S_OK;
}
