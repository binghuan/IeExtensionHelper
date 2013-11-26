#ifndef __CLASSFACTORY_H__
#define __CLASSFACTORY_H__

#include "Unknown.h"
#include "..\ExtensionRegister\IeManifestParser.h"

class CClassFactory : public CUnknown<IClassFactory> {
public:
	// Constructor and destructor
	CClassFactory();
	CClassFactory(CLSID clsid, HINSTANCE instance);
	virtual ~CClassFactory();
	// IClassFactory methods
	STDMETHODIMP CreateInstance(IUnknown *pUnkOuter,REFIID riid,void **ppvObject);
	STDMETHODIMP LockServer(BOOL fLock);
private:
	static const IID SupportedIIDs[2];
	CLSID m_clsid;
	IeManifestParser *m_ManifestParser;

	IeExtBHOInfo m_IeExtBHOInfo;
	IeExtToolbarButtonInfo m_IeExtToolbarButtonInfo;
	IeExtContentScriptInfo m_IeExtContentScriptInfo;
};

#endif // __CLASSFACTORY_H__