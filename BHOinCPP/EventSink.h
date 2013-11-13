#pragma once
#include "common.h"
#include <Exdisp.h>
#include <Exdispid.h>
#include "DocUIHandler.h"
#include "..\ExtensionRegister\IeManifestParser.h"
#include "..\ExtensionRegister\ExtStatus.h"

// Note we don't derive from our implementation of CUnknown
// This is because CEventSink always has only one instance, so we write a custom implementation of the IUnknown methods
class CEentSink : public DWebBrowserEvents2{
public:

	CEentSink(int componentID);

	// No constructor or destructor is needed
	// IUnknown methods
	STDMETHODIMP QueryInterface(REFIID riid,void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	// IDispatch methods
	STDMETHODIMP GetTypeInfoCount(UINT *pctinfo);
	STDMETHODIMP GetTypeInfo(UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
	STDMETHODIMP GetIDsOfNames(REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
	STDMETHODIMP Invoke(DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);


	// DWebBrowserEvents2 does not have any methods, IE calls our Invoke() method to notify us of events
	
	IWebBrowser2 *m_IWebBrowser2;
	DocUIHandler *extFun;
	ExtStatus *m_ExtStatus;

	void setContentScriptInfo(IeExtContentScriptInfo info);
	void exportExternalFunction(int componentID);
	void setTabID(int id);

	int m_componentID;

protected:

	CLSID *m_clsid;
	

	// Event handling methods
	bool Event_BeforeNavigate2(LPOLESTR url,LONG Flags,LPOLESTR TargetFrameName,PUCHAR PostData,LONG PostDataSize,LPOLESTR Headers,bool Cancel);

	IeExtContentScriptInfo m_IeExtContentScriptInfo;
	IeExtToolbarButtonInfo m_IeExtToolbarButtonInfo;
	boolean m_IsPopoverInitialized;
	boolean m_IsBackgroundPageInitialized;
};


// We only have one global object of this
//extern CEentSink EventSink;
