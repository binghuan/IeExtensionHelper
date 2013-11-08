/*
 Copyright (C) 2009 Moutaz Haq <cefarix@gmail.com>
 This file is released under the Code Project Open License <http://www.codeproject.com/info/cpol10.aspx>

 This defines our implementation of the IObjectWithSite interface. 
 The IObjectWithSite interface is how Internet Explorer accesses our BHO.
*/

#ifndef __BHO_H__
#define __BHO_H__

#include <Ocidl.h>
#include <Exdisp.h>
#include "Unknown.h"
#include "..\ExtensionRegister\IeManifestParser.h"

class CBHO : public CUnknown<IObjectWithSite> {
public:
	// Constructor and destructor
	CBHO(IeExtBHOInfo info, IeExtContentScriptInfo info2);
	virtual ~CBHO();
	// IObjectWithSite methods
	STDMETHODIMP SetSite(IUnknown *pUnkSite);
	STDMETHODIMP GetSite(REFIID riid,void **ppvSite);

	IeExtBHOInfo m_IeExtBHOInfo;

protected:
	void ConnectEventSink(IWebBrowser2 *pSite); // used to start handling events from IE
	void DisconnectEventSink(); // used to stop handling events from IE
	IWebBrowser2 *pSite; // the currently set site
	IConnectionPoint *pCP; // the active connection point interface
	DWORD adviseCookie; // used by ConnectEventSink() and DisconnectEventSink() in conjunction with pCP

	void ConnectEventSink2BHO(IWebBrowser2 *pSite); // used to start handling events from IE
	void DisconnectEventSink2BHO(); // used to stop handling events from IE
	IWebBrowser2 *pSite2BHO; // the currently set site
	IConnectionPoint *pCP2BHO; // the active connection point interface
	DWORD adviseCookie2BHO; // used by ConnectEventSink() and DisconnectEventSink() in conjunction with pCP

	static const IID SupportedIIDs[2];
};

#endif // __BHO_H__
