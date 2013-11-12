#include "stdafx.h"
#include "IeManifestParser.h"


IeManifestParser::IeManifestParser(HINSTANCE moduleInstance)
{
	// Get the full path to this DLL's file so we can register it
	GetModuleFileName(moduleInstance,m_ExtensionFilePath,1024);
	wprintf_s(_T("GetModuleFileName:%s\n"), m_ExtensionFilePath);

	m_fileFolderPath = Util::getModuleFolderPath(m_ExtensionFilePath);

	wprintf_s(_T("fileFolderPath:%s\n"),m_fileFolderPath.c_str());

	m_IeManifestFilePath = m_fileFolderPath;
	//m_IeManifestFilePath+= L"\\msiemanifest.json";
	m_IeManifestFilePath+= L"\\msiemanifest3.json";

	char *fileContent =  Util::readInputFile(m_IeManifestFilePath);

	printf("Read Data from file:\n%s\n", fileContent);

	Json::Features features;
	Reader reader(features);
	reader.parse(fileContent, m_manifestRoot);

	Value extId = m_manifestRoot.get("extension_id", NULL);
	if(extId == NULL) {
		char *randomID = new char[10];
		Util::rand_str(randomID, 10);
		printf("getRandomNumber: %s", randomID);
		m_manifestRoot["extension_id"] = randomID;
		Json::StyledWriter writer;
		std::string outputConfig = writer.write( m_manifestRoot );
		printf("\r\n ### New IE Manifest: %s\r\n", outputConfig.c_str());

		char *tempStr = const_cast<char* > (outputConfig.c_str());
		Util::writeFile(tempStr, m_IeManifestFilePath);	

		extId = m_manifestRoot.get("extension_id", NULL);
	}

	m_IeExtensionID = const_cast<char* > (extId.asCString());
}


IeManifestParser::~IeManifestParser(void)
{
}

IeExtBHOInfo IeManifestParser::getIeExtBHOInfo()
{
	printf(("### IE Extension BHO INFO: \r\n"));

	IeExtBHOInfo info;
	info.clsidStr = new _TCHAR[MAX_CLSID_LENGTH];
	info.name = new _TCHAR[MAX_CLSID_LENGTH];
	info.page = new _TCHAR[MAX_PATH];

	Value extBackground = m_manifestRoot.get("background", NULL);
	if(extBackground != NULL) {

		Value debugMode = extBackground.get("debug", NULL);
		if(debugMode == NULL) {
			info.debug = false;
		} else {
			info.debug = debugMode.asBool();
		}
		printf("extBackground Debug:%d\r\n", info.debug);

		Value extBackgroundPage = extBackground.get("page", NULL);
		printf(("getBackground page: %s\r\n"), extBackgroundPage);
		char *tempStr = const_cast<char* > (extBackgroundPage.asCString());
		Util::AnsiToUnicode16(tempStr, info.page, MAX_PATH);

		Value extBackgroundPageClsid = extBackground.get("clsid", NULL);
		tempStr = const_cast<char* > (extBackgroundPageClsid.asCString());
		Util::AnsiToUnicode16(tempStr, info.clsidStr, MAX_CLSID_LENGTH);
		CLSIDFromString( info.clsidStr, &info.clsid);
		wprintf_s(_T("extBackgroundPageClsid: %s\r\n"), info.clsidStr);

		Value extBackgroundName = extBackground.get("name", NULL);
		tempStr = const_cast<char* > (extBackgroundName.asCString());
		Util::AnsiToUnicode16(tempStr, info.name, MAX_PATH);
		wprintf_s(_T("extBackgroundName: %s\r\n"), info.name);

		Value extId = m_manifestRoot.get("extension_id", NULL);
		info.extenionID = const_cast<char* > (extId.asCString());

		info.isDefined = true;

	} else {
		info.isDefined = false;
	}


	return info;
}

IeExtToolbarButtonInfo IeManifestParser::getIeExtToolbarButtonInfo()
{
	printf(("### IE Extension ToolBarButton INFO: \r\n"));

	IeExtToolbarButtonInfo info;
	info.clsidStr = new _TCHAR[MAX_CLSID_LENGTH];
	info.name = new _TCHAR[MAX_PATH];
	info.page = new _TCHAR[MAX_PATH];
	info.iconPath = new _TCHAR[MAX_PATH];
	info.popupHeight = -1;
	info.popupWidth = -1;

	Value extToolbarButton = m_manifestRoot.get("browser_action", NULL);
	if(extToolbarButton != NULL) {

		Value debugMode = extToolbarButton.get("debug", NULL);
		if(debugMode == NULL) {
			info.debug = false;
		} else {
			info.debug = debugMode.asBool();
		}
		printf("extBackground Debug:%d\r\n", info.debug);

		Value extDefaultPopup = extToolbarButton.get("default_popup", NULL);
		printf(("extDefaultPopup page: %s\r\n"), extDefaultPopup);
		char *tempStr = const_cast<char* > (extDefaultPopup.asCString());
		Util::AnsiToUnicode16(tempStr, info.page, MAX_PATH);

		Value extToolbarButtonClsid = extToolbarButton.get("clsid", NULL);
		tempStr = const_cast<char* > (extToolbarButtonClsid.asCString());
		Util::AnsiToUnicode16(tempStr, info.clsidStr, MAX_CLSID_LENGTH);
		CLSIDFromString(info.clsidStr, &info.clsid);

		wprintf_s(_T("extToolbarButtonClsid: %s\r\n"), info.clsidStr);

		Value extName = extToolbarButton.get("name", NULL);
		tempStr = const_cast<char* > (extName.asCString());
		Util::AnsiToUnicode16(tempStr, info.name, MAX_PATH);
		wprintf_s(_T("extToolbarButtonName: %s\r\n"), info.name);

		ZeroMemory(info.iconPath, MAX_PATH);
		swprintf_s( info.iconPath, MAX_PATH,  _T("%s\\icon.ico"),  m_fileFolderPath.c_str());

		Value extPopupWidth = extToolbarButton.get("width", NULL);
		info.popupWidth = extPopupWidth.asInt();
		Value extPopupHeight = extToolbarButton.get("height", NULL);
		info.popupHeight = extPopupHeight.asInt();

		Value extId = m_manifestRoot.get("extension_id", NULL);
		info.extenionID = const_cast<char* > (extId.asCString());

		info.isDefined = true;

	} else {
		info.isDefined = false;
	}

	return info;
}

IeExtContentScriptInfo IeManifestParser::getIeExtContentScriptInfo()
{
	IeExtContentScriptInfo info;
	Value extContentScripts = m_manifestRoot.get("content_scripts", NULL);
	if(extContentScripts != NULL) {
		Value extScriptAtEnd = extContentScripts.get("end", NULL);
		Value scripts = extScriptAtEnd.get("scripts", NULL);
		if(scripts.type() != arrayValue) {
			printf("Incorrect END Script Format !!");
		}

		int size = scripts.size();
		for ( int i =0; i < size; i++ )
		{
			char *tempStr = const_cast<char* > (scripts[i].asCString());
			TCHAR *buff = new TCHAR[MAX_PATH];
			Util::AnsiToUnicode16(tempStr, buff, MAX_PATH);
			wstring scrpitPath = buff;
			info.javascriptsAtEnd.push_back(scrpitPath);
		}

		Value extId = m_manifestRoot.get("extension_id", NULL);
		info.extenionID = const_cast<char* > (extId.asCString());

		info.isDefined = true;

	} else {

		// BH_Lin@20131112
		// if the user does not want to inject contentScript, the component must be registered to host BHO.
		info.isDefined = true;
	}


	return info;
}
