#include "common.h"
#include "BHO.h"
#include "EventSink.h"
#include <tchar.h>
#include <atlbase.h>
#include <atlwin.h>


// The single global object of CEventSink
CEentSink *EventSink4ContentScript;
CEentSink *EventSink4BHO;

const IID CBHO::SupportedIIDs[]={IID_IUnknown,IID_IObjectWithSite};

CBHO::CBHO(IeExtBHOInfo ieExtBhoInfo, IeExtContentScriptInfo ieExtContentScriptInfo) : CUnknown<IObjectWithSite>(SupportedIIDs,2)
{
	adviseCookie=0;
	pSite=NULL;
	pCP=NULL;

	EventSink4ContentScript = new CEentSink(IE_EXT_COMPONENT_CONTENTSCRIPT);
	EventSink4ContentScript->setContentScriptInfo(ieExtContentScriptInfo);

	EventSink4BHO = new CEentSink(IE_EXT_COMPONENT_BHO);

	m_IeExtBHOInfo = ieExtBhoInfo;

	extStatusBHO = new ExtStatus(m_IeExtBHOInfo.extenionID);

	/*
	boolean isTrue = extStatusBHO->isPopoverVisible();
	if(isTrue == TRUE) {
		MessageBox(NULL, _T("isPopoverVisible: true"), _T("Information"), MB_OK);
	} else {
		MessageBox(NULL, _T("isPopoverVisible: false"), _T("Information"), MB_OK);
	}
	*/
}

CBHO::~CBHO()
{
	DisconnectEventSink();
}

CComPtr<IWebBrowser2> m_IWebBrowser2BHO;	
CComPtr<IWebBrowser2> m_IWebBrowser2ContentScript;

// Called by IE to give us IE's site object, through which we get access to IE itself
// If have a previous site set, we should unset it
// If pUnkSite is NULL, we should unset any previous site and not set a new site
STDMETHODIMP CBHO::SetSite(IUnknown *pUnkSite)
{
	HRESULT hr;

	// BH_Lin@201310121	------------------------------------------------->
	// purpose: create web container for BHO

	CAxWindow m_axWnd;
	if(m_IWebBrowser2BHO == NULL) {
		CAxWindow m_axWnd;
		hr = CoCreateInstance (CLSID_WebBrowser, NULL, CLSCTX_SERVER, IID_IWebBrowser2, (LPVOID*)&m_IWebBrowser2BHO);
		m_axWnd.Create(GetDesktopWindow());
		CComPtr<IUnknown> pUnk;
		hr = m_axWnd.AttachControl(m_IWebBrowser2BHO, &pUnk);
		hr = m_IWebBrowser2BHO->put_Silent(VARIANT_TRUE);

		if(m_IeExtBHOInfo.debug == TRUE) {
			m_axWnd.MoveWindow(0, 0, 800, 600);
			m_axWnd.ShowWindowAsync(SW_SHOW);
		}

		VARIANT varMyURL;
		VariantInit(&varMyURL);
		varMyURL.vt = VT_BSTR;
		varMyURL.bstrVal = SysAllocString(m_IeExtBHOInfo.page);
		hr = m_IWebBrowser2BHO-> Navigate2(&varMyURL,0,0,0,0);

		ConnectEventSink2BHO(m_IWebBrowser2BHO);
	}
	// BH_Lin@201310121	-------------------------------------------------<

	if(pUnkSite) pUnkSite->AddRef(); // if a new site object is given, AddRef() it to make sure the object doesn't get deleted while we are working with it
	DisconnectEventSink(); // disconnect any previous connection with IE
	if(pUnkSite==NULL) return S_OK; // if only unsetting the site, return S_OK
	hr=pUnkSite->QueryInterface(IID_IWebBrowser2,(void**)&pSite); // query the site object for the IWebBrowser2 interface, from which we can access IE
	pUnkSite->Release(); // we are done working with pUnkSite, so call Release() since we called AddRef() before
	if(FAILED(hr)) return hr; // if we couldn't find the IWebBrowser2 interface, return the error
	ConnectEventSink(pSite); // connect the new connection with IE

	return S_OK;
}

// This is called by IE to get an interface from the currently set site object
STDMETHODIMP CBHO::GetSite(REFIID riid,void **ppvSite)
{
	// Validate the ppvSite pointer
	if(IsBadWritePtr(ppvSite,sizeof(void*))) return E_POINTER;
	// Set *ppvSite to NULL
	(*ppvSite)=NULL;
	// If we don't have a current site set we must return E_FAIL
	if(pSite==NULL) return E_FAIL;
	// Otherwise we let the site's QueryInterface method take care of it
	return pSite->QueryInterface(riid,ppvSite);
}

// This is called by us to get a connection to IE and start handling IE events
void CBHO::ConnectEventSink(IWebBrowser2 *pSite)
{
	HRESULT hr;
	IConnectionPointContainer* pCPC;
	if(pSite==NULL) return; // If we don't have a site, don't do anything
	// Get an IConnectionPointContainer interface pointer from the site
	hr=pSite->QueryInterface(IID_IConnectionPointContainer,(void**)&pCPC);
	if(FAILED(hr)) return; // If we couldn't get it, abort
	// Now we use the IConnectionPointContainer interface to get an IConnectionPoint interface pointer that will handle DWebBrowserEvents2 "dispatch interface" events.
	// That means we have to plug our implementation of DWebBrowserEvents2 into the returned IConnectionPoint interface using its Advise() method, as below
	hr=pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2,&pCP);
	if(FAILED(hr)) { // If it failed, release the pCPC interface pointer and abort
		pCPC->Release();
		return;
	}

	pSite->put_Silent(VARIANT_TRUE);
	EventSink4ContentScript->m_IWebBrowser2 = pSite;
	m_IWebBrowser2ContentScript = pSite;

	// Finally we can plug our event handler object EventSink into the connection point and start receiving IE events
	// The advise cookie is just a return value we use when we want to "unplug" our event handler object from the connection point
	pCP->Advise((IUnknown*)EventSink4ContentScript,&adviseCookie);
}

// This is called by us to remove our connection to IE, if it exists, and stop handling IE events
void CBHO::DisconnectEventSink()
{
	if(pCP) { // if we have a valid connection point, unplug the event handler from it, then Release() it
		pCP->Unadvise(adviseCookie);
		adviseCookie=0;
		pCP->Release();
		pCP=NULL;
	}
	if(pSite) { // if we have a valid site, Release() it
		pSite->Release();
		pSite=NULL;
	}
}

// BH_Lin:
// purpose:
// This is called by us to get a connection to IE and start handling IE events
void CBHO::ConnectEventSink2BHO(IWebBrowser2 *pSite)
{
	EventSink4BHO->m_IWebBrowser2 = pSite;

	HRESULT hr;
	IConnectionPointContainer* pCPC;

	if(pSite==NULL) return; // If we don't have a site, don't do anything
	// Get an IConnectionPointContainer interface pointer from the site
	hr=pSite->QueryInterface(IID_IConnectionPointContainer,(void**)&pCPC);
	if(FAILED(hr)) return; // If we couldn't get it, abort
	// Now we use the IConnectionPointContainer interface to get an IConnectionPoint interface pointer that will handle DWebBrowserEvents2 "dispatch interface" events.
	// That means we have to plug our implementation of DWebBrowserEvents2 into the returned IConnectionPoint interface using its Advise() method, as below
	hr=pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2,&pCP2BHO);
	if(FAILED(hr)) { // If it failed, release the pCPC interface pointer and abort
		pCPC->Release();
		return;
	}
	// Finally we can plug our event handler object EventSink into the connection point and start receiving IE events
	// The advise cookie is just a return value we use when we want to "unplug" our event handler object from the connection point
	pCP2BHO->Advise((IUnknown*)EventSink4BHO,&adviseCookie2BHO);
}

// This is called by us to remove our connection to IE, if it exists, and stop handling IE events
void CBHO::DisconnectEventSink2BHO()
{
	if(pCP2BHO) { // if we have a valid connection point, unplug the event handler from it, then Release() it
		pCP2BHO->Unadvise(adviseCookie);
		adviseCookie=0;
		pCP2BHO->Release();
		pCP2BHO=NULL;
	}
	if(pSite2BHO) { // if we have a valid site, Release() it
		pSite2BHO->Release();
		pSite2BHO=NULL;
	}
}