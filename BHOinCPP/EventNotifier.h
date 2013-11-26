#pragma once
#include "common.h"
#include <exdisp.h>
#include <mshtmlc.h>

class EventNotifier
{
public:
	EventNotifier(void){

	};
	~EventNotifier(void){
	};

	static HRESULT issueEvent(IWebBrowser2* webView, TCHAR* eventName) {
		HRESULT hr;
		CComPtr<IHTMLDocument2>   m_pDocument; 
		hr = webView->get_Document((IDispatch**)&m_pDocument);
		if (hr != S_OK) {
			return hr;
		}

		CComBSTR bstrMember(eventName);
		DISPID dispid;  
		CComPtr<IDispatch>   m_pScript;  
		hr = m_pDocument->get_Script(&m_pScript);  
		if(!SUCCEEDED(hr)) {
			return hr;
		}
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
				if(hr == S_OK) {
					printf("S_OK");
				} else {
					printf("!S_OK");
				}
			}  
		}

		return hr;
	}
};

