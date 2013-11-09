#ifndef __TOOLBAR_H__
#define __TOOLBAR_H__

#include <Ocidl.h>
#include <Exdisp.h>
#include <atlcomcli.h>
#include "Unknown.h"
#include <shlguid.h>
#include "..\ExtensionRegister\IeManifestParser.h"
#include "..\ExtensionRegister\ExtStatus.h"

class CToolbarButton : public CUnknown<IOleCommandTarget>  {
public:
	// Constructor and destructor
	CToolbarButton(IeExtToolbarButtonInfo info);
	virtual ~CToolbarButton();

	// IOleCommandTarget
	STDMETHODIMP Exec(const GUID *pguidCmdGroup, DWORD nCmdID, 
		DWORD nCmdExecOpt, VARIANTARG *pvaIn, VARIANTARG *pvaOut);
	STDMETHODIMP QueryStatus(const GUID *pguidCmdGroup, 
		ULONG cCmds, OLECMD *prgCmds, OLECMDTEXT *pCmdText);

	

protected:

	static const IID SupportedIIDs[2];	
	
};

#endif // __TOOLBAR_H__
