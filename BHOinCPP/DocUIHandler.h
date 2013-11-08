#pragma once
#include <mshtmhst.h>
#include <atlcomcli.h>
#include "ExternalFunction.h"

class DocUIHandler: public IDocHostUIHandler
{
public:
	DocUIHandler(void);
	~DocUIHandler(void);

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void **ppvObject );

	virtual ULONG STDMETHODCALLTYPE AddRef( void );

	virtual ULONG STDMETHODCALLTYPE Release( void );

	virtual HRESULT STDMETHODCALLTYPE GetExternal( _Outptr_result_maybenull_ IDispatch **ppDispatch );

	virtual HRESULT STDMETHODCALLTYPE ShowContextMenu( _In_ DWORD dwID, _In_ POINT *ppt, _In_ IUnknown *pcmdtReserved, _In_ IDispatch *pdispReserved );

	virtual HRESULT STDMETHODCALLTYPE GetHostInfo( _Inout_ DOCHOSTUIINFO *pInfo );

	virtual HRESULT STDMETHODCALLTYPE ShowUI( _In_ DWORD dwID, _In_ IOleInPlaceActiveObject *pActiveObject, _In_ IOleCommandTarget *pCommandTarget, _In_ IOleInPlaceFrame *pFrame, _In_ IOleInPlaceUIWindow *pDoc );

	virtual HRESULT STDMETHODCALLTYPE HideUI( void );

	virtual HRESULT STDMETHODCALLTYPE UpdateUI( void );

	virtual HRESULT STDMETHODCALLTYPE EnableModeless( BOOL fEnable );

	virtual HRESULT STDMETHODCALLTYPE OnDocWindowActivate( BOOL fActivate );

	virtual HRESULT STDMETHODCALLTYPE OnFrameWindowActivate( BOOL fActivate );

	virtual HRESULT STDMETHODCALLTYPE ResizeBorder( _In_ LPCRECT prcBorder, _In_ IOleInPlaceUIWindow *pUIWindow, _In_ BOOL fRameWindow );

	virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator( LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID );

	virtual HRESULT STDMETHODCALLTYPE GetOptionKeyPath( _Out_ LPOLESTR *pchKey, DWORD dw );

	virtual HRESULT STDMETHODCALLTYPE GetDropTarget( _In_ IDropTarget *pDropTarget, _Outptr_ IDropTarget **ppDropTarget );

	virtual HRESULT STDMETHODCALLTYPE TranslateUrl( DWORD dwTranslate, _In_ LPWSTR pchURLIn, _Outptr_ LPWSTR *ppchURLOut );

	virtual HRESULT STDMETHODCALLTYPE FilterDataObject( _In_ IDataObject *pDO, _Outptr_result_maybenull_ IDataObject **ppDORet );

	ExternalFunction *pClientCall ;
};
