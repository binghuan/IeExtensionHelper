#pragma once
#include <oaidl.h>
#include "..\ExtensionRegister\IeManifestParser.h"

class ExternalFunction: public IDispatch
{
public:
	ExternalFunction(void);
	~ExternalFunction(void);

	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( __RPC__out UINT *pctinfo );

	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo );

	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( __RPC__in REFIID riid, __RPC__in_ecount_full(cNames ) LPOLESTR *rgszNames, __RPC__in_range(0,16384) UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId);

	virtual HRESULT STDMETHODCALLTYPE Invoke( _In_ DISPID dispIdMember, _In_ REFIID riid, _In_ LCID lcid, _In_ WORD wFlags, _In_ DISPPARAMS *pDispParams, _Out_opt_ VARIANT *pVarResult, _Out_opt_ EXCEPINFO *pExcepInfo, _Out_opt_ UINT *puArgErr );

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void **ppvObject );

	virtual ULONG STDMETHODCALLTYPE AddRef( void );

	virtual ULONG STDMETHODCALLTYPE Release( void );


	IeExtContentScriptInfo m_IeExtContentScriptInfo;
	IeExtToolbarButtonInfo m_IeExtToolbarButtonInfo;
	IeExtBHOInfo m_IeExtBHOInfo;
};

