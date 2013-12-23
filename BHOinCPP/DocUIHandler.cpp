#include "DocUIHandler.h"
#include <tchar.h>
#include <atlcomcli.h>
#include <oaidl.h>
#include <guiddef.h>
#include <minwinbase.h>


DocUIHandler::DocUIHandler(void)
{
}

DocUIHandler::DocUIHandler(int componentID)
{
	m_registeredComponentID = componentID;
}


DocUIHandler::~DocUIHandler(void)
{
}

HRESULT STDMETHODCALLTYPE DocUIHandler::QueryInterface( REFIID riid, void **ppvObject )
{
	bool hit=false;

	// Check if ppvObject is a valid pointer
	if(IsBadWritePtr(ppvObject,sizeof(void*))) return E_POINTER;
	if(IsBadReadPtr(this,sizeof(void*))) return E_POINTER;
	// Set *ppvObject to NULL
	(*ppvObject)=NULL;
	// See if the requested IID matches one that we support
	// If it doesn't return E_NOINTERFACE
	if(IsEqualIID(riid,IID_IUnknown))
	{
		// want IUNKNOWN
		hit = true;
		//return E_NOINTERFACE;
		(*ppvObject)=this;
	} else if (IsEqualIID(riid,IID_IDispatch)) {
		// want IID_IDispatch
		(*ppvObject)=(IDispatch*)this;
		hit = true;
	} else if (IsEqualIID(riid,IID_IDocHostUIHandler)) {
		// want IID_IDocHostUIHandler
		(*ppvObject)=(IDocHostUIHandler*)this;
		hit = true;
	} else {
		hit = true;
		return E_NOINTERFACE;
	}

	if (*ppvObject)  
	{  
		AddRef();  
		return ERROR_SUCCESS;  
	}  
	else  
	{  
		return E_NOINTERFACE;  
	} 
}

ULONG STDMETHODCALLTYPE DocUIHandler::AddRef( void )
{
	return 1;
}

ULONG STDMETHODCALLTYPE DocUIHandler::Release( void )
{
	return 1; // Ditto
}



HRESULT STDMETHODCALLTYPE DocUIHandler::GetExternal( _Outptr_result_maybenull_ IDispatch **ppDispatch )
{
	// IE will call this when the user asks for the external dispatch 
	// of the window, either via script (window.external) or via
	// the Tools menu.

	//this->QueryInterface(IID_IDispatch, (void**)&ppDispatch);
	
	pClientCall = new ExternalFunction(m_registeredComponentID);
	*ppDispatch = pClientCall;


	return S_OK;

	/*
	if (m_spExtDispatch)
		return m_spExtDispatch.CopyTo(ppDispatch);

	if (m_spDefaultDocHostUIHandler)
		return m_spDefaultDocHostUIHandler->GetExternal(ppDispatch);
	return S_FALSE;
	*/
}

HRESULT STDMETHODCALLTYPE DocUIHandler::ShowContextMenu( _In_ DWORD dwID, 
														_In_ POINT *ppt, 
														_In_ IUnknown *pcmdtReserved, 
														_In_ IDispatch *pdispReserved )
{
	/*
	UNREFERENCED_PARAMETER(dwID);
	UNREFERENCED_PARAMETER(ppt);
	UNREFERENCED_PARAMETER(pcmdtReserved);
	UNREFERENCED_PARAMETER(pdispReserved);
	*/

	if (m_spDefaultDocHostUIHandler) {
		return m_spDefaultDocHostUIHandler->ShowContextMenu(dwID, ppt, pcmdtReserved, pdispReserved);
	}

	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE DocUIHandler::GetHostInfo( _Inout_ DOCHOSTUIINFO *pInfo )
{
	//UNREFERENCED_PARAMETER(pInfo);

	if (m_spDefaultDocHostUIHandler) {
		return m_spDefaultDocHostUIHandler->GetHostInfo(pInfo);
	}

	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE DocUIHandler::ShowUI( _In_ DWORD dwID, 
											   _In_ IOleInPlaceActiveObject *pActiveObject, 
											   _In_ IOleCommandTarget *pCommandTarget, 
											   _In_ IOleInPlaceFrame *pFrame, 
											   _In_ IOleInPlaceUIWindow *pDoc )
{
	/*
	UNREFERENCED_PARAMETER(dwID);
	UNREFERENCED_PARAMETER(pActiveObject);
	UNREFERENCED_PARAMETER(pCommandTarget);
	UNREFERENCED_PARAMETER(pFrame);
	UNREFERENCED_PARAMETER(pDoc);
	*/

	if (m_spDefaultDocHostUIHandler) {
		return m_spDefaultDocHostUIHandler->ShowUI(dwID, pActiveObject, pCommandTarget, pFrame, pDoc );
	}

	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE DocUIHandler::HideUI( void )
{
	if (m_spDefaultDocHostUIHandler) {
		return m_spDefaultDocHostUIHandler->HideUI();
	}

	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE DocUIHandler::UpdateUI( void )
{
	if (m_spDefaultDocHostUIHandler) {
		return m_spDefaultDocHostUIHandler->UpdateUI();
	}

	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE DocUIHandler::EnableModeless( BOOL fEnable )
{
	//UNREFERENCED_PARAMETER(fEnable);

	if (m_spDefaultDocHostUIHandler) {
		return m_spDefaultDocHostUIHandler->EnableModeless(fEnable);
	}

	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE DocUIHandler::OnDocWindowActivate( BOOL fActivate )
{
	//UNREFERENCED_PARAMETER(fActivate);

	if (m_spDefaultDocHostUIHandler) {
		return m_spDefaultDocHostUIHandler->OnDocWindowActivate(fActivate);
	}

	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE DocUIHandler::OnFrameWindowActivate( BOOL fActivate )
{
	//UNREFERENCED_PARAMETER(fActivate);

	if (m_spDefaultDocHostUIHandler) {
		return m_spDefaultDocHostUIHandler->OnFrameWindowActivate(fActivate);
	}

	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE DocUIHandler::ResizeBorder( _In_ LPCRECT prcBorder, _In_ IOleInPlaceUIWindow *pUIWindow, _In_ BOOL fRameWindow )
{
	/*
	UNREFERENCED_PARAMETER(prcBorder);
	UNREFERENCED_PARAMETER(pUIWindow);
	UNREFERENCED_PARAMETER(fRameWindow);
	*/

	if (m_spDefaultDocHostUIHandler) {
		return m_spDefaultDocHostUIHandler->ResizeBorder(prcBorder, pUIWindow, fRameWindow);
	}

	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE DocUIHandler::TranslateAccelerator( LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID )
{
	/*
	UNREFERENCED_PARAMETER(pguidCmdGroup);
	UNREFERENCED_PARAMETER(nCmdID);

	HRESULT hr = S_FALSE;
	if (lpMsg && lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_TAB) {
		// The message was handled.
		hr = S_OK;
	}
	*/

	if (m_spDefaultDocHostUIHandler) {
		return m_spDefaultDocHostUIHandler->TranslateAccelerator(lpMsg, pguidCmdGroup, nCmdID);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE DocUIHandler::GetOptionKeyPath( _Out_ LPOLESTR *pchKey, DWORD dw )
{
	UNREFERENCED_PARAMETER(pchKey);
	UNREFERENCED_PARAMETER(dw);

	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE DocUIHandler::GetDropTarget( _In_ IDropTarget *pDropTarget, _Outptr_ IDropTarget **ppDropTarget )
{
	UNREFERENCED_PARAMETER(pDropTarget);
	UNREFERENCED_PARAMETER(ppDropTarget);

	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE DocUIHandler::TranslateUrl( DWORD dwTranslate, _In_ LPWSTR pchURLIn, _Outptr_ LPWSTR *ppchURLOut )
{
	/*
	UNREFERENCED_PARAMETER(dwTranslate);
	UNREFERENCED_PARAMETER(pchURLIn);
	UNREFERENCED_PARAMETER(ppchURLOut);
	*/

	if (m_spDefaultDocHostUIHandler) {
		return m_spDefaultDocHostUIHandler->TranslateUrl(dwTranslate, pchURLIn, ppchURLOut);
	}

	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE DocUIHandler::FilterDataObject( _In_ IDataObject *pDO, _Outptr_result_maybenull_ IDataObject **ppDORet )
{
	/*
	UNREFERENCED_PARAMETER(pDO);
	UNREFERENCED_PARAMETER(ppDORet);
	*/

	if (m_spDefaultDocHostUIHandler) {
		return m_spDefaultDocHostUIHandler->FilterDataObject(pDO, ppDORet);
	}

	return E_NOINTERFACE;
}

