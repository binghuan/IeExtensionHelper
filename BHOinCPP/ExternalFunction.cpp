#include "ExternalFunction.h"
#include <tchar.h>
#include <atlcomcli.h>
#include <oaidl.h>
#include <mshtmlc.h>
#include "common.h"
#include "..\ExtensionRegister\Util.h"
#include "..\ExtensionRegister\ExtStatus.h"


ExternalFunction::ExternalFunction(void)
{
	IeManifestParser *manifestParser = new IeManifestParser(hInstance);
	m_IeExtContentScriptInfo = manifestParser->getIeExtContentScriptInfo();
}


ExternalFunction::~ExternalFunction(void)
{
}

HRESULT STDMETHODCALLTYPE ExternalFunction::GetTypeInfoCount( __RPC__out UINT *pctinfo )
{
	UNREFERENCED_PARAMETER(pctinfo);

	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE ExternalFunction::GetTypeInfo( UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo )
{
	UNREFERENCED_PARAMETER(iTInfo);
	UNREFERENCED_PARAMETER(lcid);
	UNREFERENCED_PARAMETER(ppTInfo);

	return E_NOINTERFACE;
}

#define MAX_STRING_LENGTH 1024
#define DISPID_CB_CUSTOMFUNTION 123001
#define DISPID_DISPATCH_MSG_TO_BACKGROUND 123002
#define DISPID_DISPATCH_MSG_TO_CONTENTSCRIPT 123003
#define DISPID_GET_BACKGROUND_PAGE 123004

#define DISPID_GET_ACTIVE_TAB 123005
#define DISPID_OPEN_NEW_TAB 123006

#define DISPID_IS_POPOVER_VISIBLE 123007

#define DISPID_EXECUTE_SCRIPT_IN_TAB 123008

#define DISPID_GET_SELF_TAB 123009

HRESULT STDMETHODCALLTYPE ExternalFunction::GetIDsOfNames( 
	__RPC__in REFIID riid, 
	__RPC__in_ecount_full(cNames ) LPOLESTR *rgszNames, 
	__RPC__in_range(0,16384) UINT cNames, 
	LCID lcid, 
	__RPC__out_ecount_full(cNames) DISPID *rgDispId )
{
	UNREFERENCED_PARAMETER(cNames);
	UNREFERENCED_PARAMETER(lcid);
	UNREFERENCED_PARAMETER(riid);


	HRESULT hr = NOERROR;

	if(lstrcmp(rgszNames[0], L"messagebox")==0){
		_tprintf(_T("get function call: messagebox"));
		*rgDispId = DISPID_CB_CUSTOMFUNTION;
	} else if(lstrcmp(rgszNames[0], L"dispatchMessage2ContentScript")==0){
		*rgDispId = DISPID_DISPATCH_MSG_TO_CONTENTSCRIPT;
	} else if(lstrcmp(rgszNames[0], L"dispatchMessage2Background")==0){
		*rgDispId = DISPID_DISPATCH_MSG_TO_BACKGROUND;
	} else if(lstrcmp(rgszNames[0], L"getBackgroundPage")==0){
		*rgDispId = DISPID_GET_BACKGROUND_PAGE;
	} else if(lstrcmp(rgszNames[0], L"getActiveTab")==0){
		*rgDispId = DISPID_GET_ACTIVE_TAB;
	} else if(lstrcmp(rgszNames[0], L"getSelfTab")==0){
		*rgDispId = DISPID_GET_SELF_TAB;
	} else if(lstrcmp(rgszNames[0], L"openNewTab")==0){
		*rgDispId = DISPID_OPEN_NEW_TAB;
	} else if(lstrcmp(rgszNames[0], L"isPopoverVisible")==0){
		*rgDispId = DISPID_IS_POPOVER_VISIBLE;
	} else if(lstrcmp(rgszNames[0], L"executeScriptInTab")==0){
		*rgDispId = DISPID_EXECUTE_SCRIPT_IN_TAB;
	} else {
		*rgDispId = DISP_E_UNKNOWNNAME;
		hr = ResultFromScode(DISP_E_UNKNOWNNAME);
	}

	return hr;
}

void CppCall()
{
	MessageBox(NULL, L"You call external messagebox", L"Oh My god", 0);
}


int m_TabID = 99999999;

HRESULT STDMETHODCALLTYPE ExternalFunction::Invoke( _In_ DISPID dispIdMember, 
												   _In_ REFIID riid, 
												   _In_ LCID lcid, 
												   _In_ WORD wFlags, 
												   _In_ DISPPARAMS *pDispParams, 
												   _Out_opt_ VARIANT *pVarResult, 
												   _Out_opt_ EXCEPINFO *pExcepInfo, 
												   _Out_opt_ UINT *puArgErr )
{
	UNREFERENCED_PARAMETER(lcid);
	UNREFERENCED_PARAMETER(pExcepInfo);
	UNREFERENCED_PARAMETER(puArgErr);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(wFlags);

	HRESULT hr = S_OK;

	switch(dispIdMember) {
	case DISPID_EXECUTE_SCRIPT_IN_TAB:
		{
			CComPtr<IDispatch>   pHtmlDocDispatch;  
			CComPtr<IHTMLDocument2>   m_pDocument;  
			CComPtr<IDispatch>   m_pScript;  
			HRESULT hr = m_IWebBrowser2ContentScript->get_Document((IDispatch**)&m_pDocument);

			if(SUCCEEDED(hr)) {
				CComQIPtr<IHTMLWindow2> pWindow;	
				m_pDocument->get_parentWindow(&pWindow);
				CComBSTR bstrLanguage = _T("javascript");
				VARIANT vEmpty = {0};
				CComBSTR bstrScript(pDispParams->rgvarg[0].bstrVal);
				hr = pWindow->execScript(bstrScript,bstrLanguage,&vEmpty);
			}
		}
		break;
	case DISPID_IS_POPOVER_VISIBLE:
		{
			if(pVarResult != NULL)
			{
				ExtStatus *extStatus = new ExtStatus(m_IeExtContentScriptInfo.extenionID);

				VariantInit(pVarResult);
				V_VT(pVarResult)=VT_BOOL;
				V_BOOL(pVarResult) = extStatus->isPopoverVisible();
			}
		}
		break;
	case DISPID_OPEN_NEW_TAB:
		{
			VARIANT varMyURL;
			VariantInit(&varMyURL);
			varMyURL.vt = VT_BSTR;
			varMyURL.bstrVal = SysAllocString(pDispParams->rgvarg[0].bstrVal);

			VARIANT vFlags = {0};
			VariantInit(&vFlags);
			vFlags.vt = VT_I4;
			vFlags.intVal |= navOpenInNewTab;

			m_IWebBrowser2ContentScript-> Navigate2(&varMyURL,&vFlags,NULL,NULL,NULL);
		}
		break;
	case DISPID_CB_CUSTOMFUNTION:
		{
			CppCall();
		}
		break;
	case DISPID_GET_SELF_TAB:
		{
			printf("DISPID_GET_SELF_TAB.\n");
			CComPtr<IDispatch>   pHtmlDocDispatch;  
			CComPtr<IHTMLDocument2>   m_pDocument;  
			CComPtr<IDispatch>   m_pScript;  
			HRESULT hr = m_IWebBrowser2BHO->get_Document((IDispatch**)&m_pDocument);
			if(SUCCEEDED(hr)) {
				CComPtr<IHTMLWindow2> pWindow;	
				m_pDocument->get_parentWindow(&pWindow);
				CComBSTR bstrLanguage = _T("javascript");

				if(pVarResult != NULL)
				{
					CComBSTR bstrName;
					CComBSTR bstrUrl;
					m_IWebBrowser2ContentScript->get_LocationName(&bstrName);
					m_IWebBrowser2ContentScript->get_LocationURL(&bstrUrl);

					TCHAR *tabIDStr = new TCHAR[MAX_PATH];
					ZeroMemory(tabIDStr, MAX_PATH);
					swprintf_s( tabIDStr, MAX_PATH, _T("%d"),  m_TabID);
					

					wstring tabStr = L"{";
					tabStr += L"title: ";
					tabStr += L"'";
					tabStr += bstrName;
					tabStr += L"',";
					tabStr += L"url: ";
					tabStr += L"'";
					tabStr += bstrUrl;
					tabStr += L"',";
					tabStr += L"tabId: ";
					tabStr += tabIDStr;
					tabStr += L"}";

					VariantInit(pVarResult);
					V_VT(pVarResult)=VT_BSTR;

					CComBSTR returnObj = tabStr.c_str();
					V_BSTR(pVarResult) = returnObj;
				}
			}
		}
		break;
	case DISPID_GET_ACTIVE_TAB:
		{
			printf("DISPID_GET_ACTIVE_TABID.\n");
			CComPtr<IDispatch>   pHtmlDocDispatch;  
			CComPtr<IHTMLDocument2>   m_pDocument;  
			CComPtr<IDispatch>   m_pScript;  
			HRESULT hr = m_IWebBrowser2BHO->get_Document((IDispatch**)&m_pDocument);
			if(SUCCEEDED(hr)) {
				CComPtr<IHTMLWindow2> pWindow;	
				m_pDocument->get_parentWindow(&pWindow);
				CComBSTR bstrLanguage = _T("javascript");

				if(pVarResult != NULL)
				{
					VariantInit(pVarResult);
					V_VT(pVarResult)=VT_INT;
					V_INT(pVarResult) = m_TabID;
				}
			}
		}
		

		break;

	case DISPID_DISPATCH_MSG_TO_BACKGROUND:
		{
			CComPtr<IDispatch>   pHtmlDocDispatch;  
			CComPtr<IHTMLDocument2>   m_pDocument;  
			CComPtr<IDispatch>   m_pScript;  
			HRESULT hr = m_IWebBrowser2BHO->get_Document((IDispatch**)&m_pDocument);
			CComBSTR bstrMember = _T("onIeExtensionMsgBackgroundReceive");

			if (SUCCEEDED(hr))  
			{  
				CComQIPtr<IHTMLWindow2> pWindow;	
				m_pDocument->get_parentWindow(&pWindow);
				CComBSTR bstrLanguage = _T("javascript");
				VARIANT vEmpty = {0};

				TCHAR *tabIDStr = new TCHAR[MAX_PATH];
				ZeroMemory(tabIDStr, MAX_PATH);
				swprintf_s( tabIDStr, MAX_PATH, _T("%d"),  m_TabID);

				wstring script ;
				script = L"onIeExtensionMsgBackgroundReceive(";

				script += L"{";
				script += L"name:'";
				script += pDispParams->rgvarg[1].bstrVal;
				script += L"', tabId:";
				script += tabIDStr;
				script += L", data:";
				script += L"'";
				script += pDispParams->rgvarg[0].bstrVal;
				script += L"'";
				script += L"}";
				script += L");";

				CComBSTR bstrScript(script.c_str());
				hr = pWindow->execScript(bstrScript,bstrLanguage,&vEmpty);
			}  

			/*
			m_pDocument->get_Script(&m_pScript);
			if(m_pScript!=NULL)  
			{  
				hr = m_pScript->GetIDsOfNames(IID_NULL,&bstrMember,1,LOCALE_SYSTEM_DEFAULT,&dispid);  
				if (SUCCEEDED(hr))  
				{  
					DISPPARAMS dispparams;  
					
					memset(&dispparams, 0, sizeof(DISPPARAMS));  
					dispparams.cArgs = 0;  
					dispparams.cNamedArgs = 0;  

					EXCEPINFO excepInfo;  
					memset(&excepInfo, 0, sizeof(EXCEPINFO));  
					CComVariant vaResult;  
					// initialize to invalid arg  
					UINT nArgErr = (UINT)-1;  
					hr = m_pScript->Invoke(dispid,IID_NULL,0,DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);  
				}  
			}*/
			
		}
		break;
	case DISPID_DISPATCH_MSG_TO_CONTENTSCRIPT:
		{

			CComPtr<IHTMLDocument2>   m_pDocument;  
			CComPtr<IDispatch>   m_pScript;  
			HRESULT hr = m_IWebBrowser2ContentScript->get_Document((IDispatch**)&m_pDocument);

			m_pDocument->get_Script(&m_pScript);  


			VARIANT extensionObj;
			VariantInit(&extensionObj);
			char* tempStr = const_cast<char*>(m_IeExtContentScriptInfo.extenionID.c_str());
			_TCHAR* propertyName = new _TCHAR[MAX_PATH];
			Util::AnsiToUnicode16(tempStr, propertyName, MAX_PATH);

			hr = Util::GetProperty(m_pScript, propertyName, &extensionObj);

			CComBSTR bstrMember = _T("onIeExtensionMsgContentScriptReceive");
			DISPID dispid;

			hr = extensionObj.pdispVal->GetIDsOfNames(IID_NULL,&bstrMember,1,LOCALE_SYSTEM_DEFAULT,&dispid);  
			if (SUCCEEDED(hr))  
			{  
				CComQIPtr<IHTMLWindow2> pWindow;	
				m_pDocument->get_parentWindow(&pWindow);
				CComBSTR bstrLanguage = _T("javascript");
				VARIANT vEmpty = {0};

				wstring script ;
				script = L"onIeExtensionMsgContentScriptReceive(";
				
				script += L"{";
				script += L"name:'";
				script += pDispParams->rgvarg[1].bstrVal;
				script += L"', message:";
				script += L"'";
				script += pDispParams->rgvarg[0].bstrVal;
				script += L"'";
				script += L"}";
				script += L");";
				
				CComBSTR bstrScript(script.c_str());
				hr = pWindow->execScript(bstrScript,bstrLanguage,&vEmpty);
			}  
		}
		break;
	case DISPID_GET_BACKGROUND_PAGE:
		{
			printf("DISPID_GET_BACKGROUND_PAGE.\n");
			CComPtr<IDispatch>   pHtmlDocDispatch;  
			CComPtr<IHTMLDocument2>   m_pDocument;  
			CComPtr<IDispatch>   m_pScript;  
			HRESULT hr = m_IWebBrowser2BHO->get_Document((IDispatch**)&m_pDocument);
			if(SUCCEEDED(hr)) {
				CComPtr<IHTMLWindow2> pWindow;	
				m_pDocument->get_parentWindow(&pWindow);
				CComBSTR bstrLanguage = _T("javascript");

				//pVarResult = pWindow;
				pVarResult->vt = VT_DISPATCH;
				pVarResult->pdispVal = (IDispatch *)pWindow;
			}
		}
		break;
	default:
		{
			hr = E_NOINTERFACE;
		}

		break;
	}

	return hr;
}

HRESULT STDMETHODCALLTYPE ExternalFunction::QueryInterface( REFIID riid, void **ppvObject )
{
	*ppvObject = NULL;
	if (riid == IID_IUnknown)    *ppvObject = this;
	else if (riid == IID_IDispatch)    *ppvObject = (IDispatch*)this;
	if(*ppvObject)
	{
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;

}

ULONG STDMETHODCALLTYPE ExternalFunction::AddRef( void )
{
	return 1;
}

ULONG STDMETHODCALLTYPE ExternalFunction::Release( void )
{
	return 1;
}
