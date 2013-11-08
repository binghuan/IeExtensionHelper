/*
 Copyright (C) 2009 Moutaz Haq <cefarix@gmail.com>
 This file is released under the Code Project Open License <http://www.codeproject.com/info/cpol10.aspx>

 This defines our implementation of the IClassFactory interface. The IClassFactory interface is used by COM to create objects of the DLL's main COM class.
*/

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