#include "EventSink.h"
#include <tchar.h>
#include <atlbase.h>
#include <atlwin.h>

#include <stdio.h>
#include <vector>
using namespace std;

#include "..\ExtensionRegister\Util.h"
#include "EventNotifier.h"

// BH_Lin: testing GetExternal


CEentSink::CEentSink(int componentID)
{
	extFun = new DocUIHandler();
	m_componentID = componentID;


	IeManifestParser *manifestParser = new IeManifestParser(hInstance);

	switch(componentID) {
	case IE_EXT_COMPONENT_CONTENTSCRIPT:
		m_IeExtContentScriptInfo = manifestParser->getIeExtContentScriptInfo();
		m_ExtStatus = new ExtStatus(m_IeExtContentScriptInfo.extenionID);
		break;
	case IE_EXT_COMPONENT_TOOLBARBUTTON:
		m_IsPopoverInitialized = false;
		m_IeExtToolbarButtonInfo = manifestParser->getIeExtToolbarButtonInfo();
		break;
	case IE_EXT_COMPONENT_BHO:
		m_IsBackgroundPageInitialized = false;
		break;
	}

}

STDMETHODIMP CEentSink::QueryInterface(REFIID riid,void **ppvObject)
{
	// Check if ppvObject is a valid pointer
	if(IsBadWritePtr(ppvObject,sizeof(void*))) return E_POINTER;
	// Set *ppvObject to NULL
	(*ppvObject)=NULL;
	// See if the requested IID matches one that we support
	// If it doesn't return E_NOINTERFACE
	if(!IsEqualIID(riid,IID_IUnknown) && !IsEqualIID(riid,IID_IDispatch) && !IsEqualIID(riid,DIID_DWebBrowserEvents2)) return E_NOINTERFACE;
	// If it's a matching IID, set *ppvObject to point to the global EventSink object
	(*ppvObject)=(void*)this;
	return S_OK;
}

STDMETHODIMP_(ULONG) CEentSink::AddRef()
{
	return 1; // We always have just one static object
}

STDMETHODIMP_(ULONG) CEentSink::Release()
{
	return 1; // Ditto
}

// We don't need to implement the next three methods because we are just a pure event sink
// We only care about Invoke() which is what IE calls to notify us of events

STDMETHODIMP CEentSink::GetTypeInfoCount(UINT *pctinfo)
{
	UNREFERENCED_PARAMETER(pctinfo);

	return E_NOTIMPL;
}

STDMETHODIMP CEentSink::GetTypeInfo(UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo)
{
	UNREFERENCED_PARAMETER(iTInfo);
	UNREFERENCED_PARAMETER(lcid);
	UNREFERENCED_PARAMETER(ppTInfo);

	return E_NOTIMPL;
}

STDMETHODIMP CEentSink::GetIDsOfNames(REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId)
{
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(rgszNames);
	UNREFERENCED_PARAMETER(cNames);
	UNREFERENCED_PARAMETER(lcid);
	UNREFERENCED_PARAMETER(rgDispId);

	return E_NOTIMPL;
}

void CEentSink::exportExternalFunction(int componentID) {
	_tprintf(TEXT("IE_EXT_COMPONENT_TOOLBARBUTTON"));		

	HRESULT hr;
	CComPtr<IHTMLDocument2>   m_pDocument; 
	hr = m_IWebBrowser2->get_Document((IDispatch**)&m_pDocument);

	// BH_Lin@20131011	---------------------------------------->
	// purpose: test iDispatch
	/*
	CComPtr<IDispatch>   pHtmlDocDispatch;   
	CComPtr<IDispatch>   m_pScript;  
	m_pDocument->get_Script(&m_pScript);  

	CComPtr<IHTMLElement> htmlElement;
	m_pDocument->get_body(&htmlElement);
	BSTR webContetn = NULL;
	htmlElement->get_innerHTML(&webContetn);
	*/

	// BH_Lin@20131014	--------------------------------->
	// purpose: try to implement UI handler.

	// If this is an HTML document...
	CComPtr<IDispatch> spDocument;

	hr = m_IWebBrowser2->get_Document(&spDocument);
	if (SUCCEEDED(hr) && (spDocument != nullptr))
	{
		// Request default handler from MSHTML client site
		CComPtr<IOleObject> spOleObject;
		spOleObject = spDocument;
		CComPtr<IOleClientSite> spClientSite;
		hr = spOleObject->GetClientSite(&spClientSite);
		if (SUCCEEDED(hr) && spClientSite)
		{
			// Save pointer for delegation to default 
			//m_spDefaultDocHostUIHandler = spClientSite;
		}

		// Set the new custom IDocHostUIHandler
		CComPtr<ICustomDoc> spCustomDoc;
		spCustomDoc = spDocument;
		CComPtr<IDocHostUIHandler> spDocHostUIHandler;
		//QueryInterface(IID_IDocHostUIHandler, (void **)&spDocHostUIHandler);  

		// NOTE: spHandler is user-defined class
		if(extFun) {
			spCustomDoc->SetUIHandler(extFun);
		} else {
			//bool notgood = true;
		}
	} 
	// BH_Lin@20131014	----------------------------------------<

	/*
	CComBSTR bstrMember = _T("helloworld");
	DISPID dispid;  
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
	}
	*/
					
	// BH_Lin@20131022	----------------------------------------------------------->
	// try to inject javascript
					

	//CComBSTR bstrScript = _T("alert(\"tuma\");");
	//hr = pWindow->execScript(bstrScript,bstrLanguage,&vEmpty);


	if((componentID == IE_EXT_COMPONENT_TOOLBARBUTTON) &&
		(m_IeExtToolbarButtonInfo.isDefined == TRUE) &&
		(m_IeExtToolbarButtonInfo.debug == TRUE)) {
			CComQIPtr<IHTMLWindow2> pWindow;	
			m_pDocument->get_parentWindow(&pWindow);
			CComBSTR bstrLanguage = _T("javascript");
			VARIANT vEmpty = {0};

			string contentString ;
			contentString += "var head = document.getElementsByTagName('head')[0];";
			contentString += "var scriptOfFirebugLite = document.createElement('script');";
			contentString += "scriptOfFirebugLite.src = \"https://getfirebug.com/firebug-lite.js#enableTrace,overrideConsole,startOpened,saveCookies,ignoreFirebugElements\";";
			contentString += "head.appendChild(scriptOfFirebugLite );";
			CComBSTR bstrScript(contentString.c_str());
			hr = pWindow->execScript(bstrScript,bstrLanguage,&vEmpty);
	}

	// BH_Lin@20131030	--------------------------------------------->
	// purpose: inject content script.
					
	if((componentID == IE_EXT_COMPONENT_CONTENTSCRIPT) &&
		(m_IeExtContentScriptInfo.isDefined == TRUE)
		//&& false
		) {

		CComQIPtr<IHTMLWindow2> pWindow;	
		m_pDocument->get_parentWindow(&pWindow);
		CComBSTR bstrLanguage = _T("javascript");
		VARIANT vEmpty = {0};

		string contentString ;

		if(IS_CONTENTSCRIPT_ISOLATEDWORLD_ENABLED) {
			contentString += "var " ;
			contentString += m_IeExtContentScriptInfo.extenionID;
			contentString += " = (function(){ " ;
		}

		for(int i = 0; i< (int)m_IeExtContentScriptInfo.javascriptsAtEnd.size(); i++)
		{
			char *scriptBuff = Util::readInputFile(m_IeExtContentScriptInfo.javascriptsAtEnd[i]);
			string script(scriptBuff);
			contentString += script;
		}

		if(IS_CONTENTSCRIPT_ISOLATEDWORLD_ENABLED) {
			contentString += "return {";
			contentString += "onIeExtensionMsgContentScriptReceive: function() { if(this.hasOwnProperty(\"onIeExtensionMsgContentScriptReceive\") === true ) { return onIeExtensionMsgContentScriptReceive();}else{console.warn(\"IeExtAPI: no implement- onIeExtensionMsgContentScriptReceive\")} }";
			contentString += "};";
			contentString += "})();";
		}

		CComBSTR bstrScript(contentString.c_str());
		hr = pWindow->execScript(bstrScript,bstrLanguage,&vEmpty);
	}
					
					
	// BH_Lin@20131030	---------------------------------------------<

	_tprintf(TEXT("Done: IE_EXT_COMPONENT_CONTENTSCRIPT"));
}

// This is called by IE to notify us of events
// Full documentation about all the events supported by DWebBrowserEvents2 can be found at
//  http://msdn.microsoft.com/en-us/library/aa768283(VS.85).aspx
STDMETHODIMP CEentSink::Invoke(DISPID dispIdMember,
									   REFIID riid,
									   LCID lcid,
									   WORD wFlags,
									   DISPPARAMS *pDispParams,
									   VARIANT *pVarResult,
									   EXCEPINFO *pExcepInfo,
									   UINT *puArgErr)
{

	UNREFERENCED_PARAMETER(lcid);
	UNREFERENCED_PARAMETER(wFlags);
	UNREFERENCED_PARAMETER(pVarResult);
	UNREFERENCED_PARAMETER(pExcepInfo);
	UNREFERENCED_PARAMETER(puArgErr);
	VARIANT v[5]; // Used to hold converted event parameters before passing them onto the event handling method
	int n;
	bool b;
	PVOID pv;
	LONG lbound,ubound,sz;

	if(!IsEqualIID(riid,IID_NULL)) return DISP_E_UNKNOWNINTERFACE; // riid should always be IID_NULL
	// Initialize the variants
	for(n=0;n<5;n++) VariantInit(&v[n]);

	switch(dispIdMember) {
	case DISPID_NAVIGATEERROR:
		{
			_tprintf(TEXT("DISPID_NAVIGATEERROR"));
			switch(m_componentID) {
			case IE_EXT_COMPONENT_BHO:
				_tprintf(TEXT("BHO"));
				Release();
				break;
			}
			
		}
		break;
	case DISPID_BEFORENAVIGATE2:
		{
			_tprintf(TEXT("DISPID_BEFORENAVIGATE2"));
		}
		break;
	case DISPID_DOWNLOADCOMPLETE:
		{
			_tprintf(TEXT("DISPID_DOWNLOADCOMPLETE"));
		}
		break;
	case DISPID_DOCUMENTCOMPLETE:
		{
			_tprintf(TEXT("DISPID_DOCUMENTCOMPLETE"));

			switch(m_componentID) {
			case IE_EXT_COMPONENT_TOOLBARBUTTON:
				{
					if(m_IsPopoverInitialized == false) {
						m_IsPopoverInitialized = true;
						EventNotifier::issueEvent(m_IWebBrowser2Popover, IE_EXT_EVENT_POPOVER);
					}
				}
				break;
			case IE_EXT_COMPONENT_BHO:
				{
					_tprintf(TEXT("IE_EXT_COMPONENT_BHO"));
					if(m_IsBackgroundPageInitialized == false) {
						m_IsBackgroundPageInitialized = true;
						EventNotifier::issueEvent(m_IWebBrowser2BHO, IE_EXT_EVENT_TAB_OPEN);
					}
				}
				break;
			}

		}
		break;
	case DISPID_NAVIGATECOMPLETE2:
		{
			_tprintf(TEXT("DISPID_NAVIGATECOMPLETE2"));
			switch (m_componentID)
			{
			case IE_EXT_COMPONENT_BHO:
				{
					_tprintf(TEXT("IE_EXT_COMPONENT_BHO"));
				}
				break;
			case IE_EXT_COMPONENT_CONTENTSCRIPT:
				{
					_tprintf(TEXT("IE_EXT_COMPONENT_CONTENTSCRIPT"));
				}
				break;
			case IE_EXT_COMPONENT_TOOLBARBUTTON:
				{
					_tprintf(TEXT("IE_EXT_COMPONENT_TOOLBARBUTTON"));
				}
				break;
			}
			exportExternalFunction(m_componentID);
		}
		break;
		// DWebBrowserEvents2
	case DISPID_WINDOWSTATECHANGED: 
		{
			// the tab state will be notified by IE to contentscript . 
			// so , we pass the event to background BHO. 
			if(m_componentID == IE_EXT_COMPONENT_CONTENTSCRIPT) {
				if (pDispParams) {
					DWORD dwMask  = pDispParams->rgvarg[0].lVal;
					DWORD dwFlags = pDispParams->rgvarg[1].lVal;

					// We only care about WINDOWSTATE_USERVISIBLE.
					if (dwMask & OLECMDIDF_WINDOWSTATE_USERVISIBLE)
					{
						bool visible = !!(dwFlags & OLECMDIDF_WINDOWSTATE_USERVISIBLE);

						// ... your code here ...
						if(visible) {
							_tprintf(TEXT("Tab is visible: %d"), m_componentID);

							CComBSTR bstrName;
							CComBSTR bstrUrl;
							m_IWebBrowser2ContentScript->get_LocationName(&bstrName);
							m_IWebBrowser2ContentScript->get_LocationURL(&bstrUrl);

							// need to implement dynamic tabID. 
							m_ExtStatus->setActiveTabInfo( 99999999 , bstrName.m_str, bstrUrl.m_str);

							if(m_IWebBrowser2BHO != NULL) {
								EventNotifier::issueEvent(m_IWebBrowser2BHO,IE_EXT_EVENT_TAB_ACTIVATE);
							}
						} else {
							_tprintf(TEXT("Tab is not visible: %d"), m_componentID);
							EventNotifier::issueEvent(m_IWebBrowser2BHO,IE_EXT_EVENT_TAB_INACTIVATE);
						}
					}
				}
			}
			
			break;
		}
	}

	if(dispIdMember==DISPID_BEFORENAVIGATE2) { // Handle the BeforeNavigate2 event

		VariantChangeType(&v[0],&pDispParams->rgvarg[5],0,VT_BSTR); // url
		VariantChangeType(&v[1],&pDispParams->rgvarg[4],0,VT_I4); // Flags
		VariantChangeType(&v[2],&pDispParams->rgvarg[3],0,VT_BSTR); // TargetFrameName
		VariantChangeType(&v[3],&pDispParams->rgvarg[2],0,VT_UI1|VT_ARRAY); // PostData
		VariantChangeType(&v[4],&pDispParams->rgvarg[1],0,VT_BSTR); // Headers
		if(v[3].vt!=VT_EMPTY) {
			SafeArrayGetLBound(v[3].parray,0,&lbound);
			SafeArrayGetUBound(v[3].parray,0,&ubound);
			sz=ubound-lbound+1;
			SafeArrayAccessData(v[3].parray,&pv);
		} else {
			sz=0;
			pv=NULL;
		}
		b=Event_BeforeNavigate2((LPOLESTR)v[0].bstrVal,v[1].lVal,(LPOLESTR)v[2].bstrVal,(PUCHAR)pv,sz,(LPOLESTR)v[4].bstrVal,((*(pDispParams->rgvarg[0].pboolVal))!=VARIANT_FALSE));
		if(v[3].vt!=VT_EMPTY) SafeArrayUnaccessData(v[3].parray);
		if(b) *(pDispParams->rgvarg[0].pboolVal)=VARIANT_TRUE;
		else *(pDispParams->rgvarg[0].pboolVal)=VARIANT_FALSE;
	}
	// Free the variants
	for(n=0;n<5;n++) VariantClear(&v[n]);
	return S_OK;
}

// Return true to prevent the url from being opened
bool CEentSink::Event_BeforeNavigate2(
	LPOLESTR url,
	LONG Flags,
	LPOLESTR TargetFrameName,
	PUCHAR PostData,
	LONG PostDataSize,
	LPOLESTR Headers,
	bool Cancel)
{
	UNREFERENCED_PARAMETER(url);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(TargetFrameName);
	UNREFERENCED_PARAMETER(PostData);
	UNREFERENCED_PARAMETER(PostDataSize);
	UNREFERENCED_PARAMETER(Headers);

	return Cancel;
}

void CEentSink::setContentScriptInfo( IeExtContentScriptInfo info )
{
	m_IeExtContentScriptInfo = info;
}

