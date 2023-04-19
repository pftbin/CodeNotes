#include "StdAfx.h"
#include "C_XLSPackBundle.h"
#include "C_PbXLSReader.h"
#include "C_PbXLSWriter.h"

C_XLSPackBundle::C_XLSPackBundle(void)
{
    REGISTER_PLUGIN_ENTRY()
}

C_XLSPackBundle::~C_XLSPackBundle(void)
{
	ReleaseAllDynamicPlugin();
}

I_PiPlugin* C_XLSPackBundle::DynamicCreatePlugin(LPCTSTR pszExtensionPointId, LPCTSTR pszPluginId)
{
	if (EMPTY_STR(pszExtensionPointId) || EMPTY_STR(pszPluginId))
		return NULL;

	I_PiPlugin* pPlugin = NULL;
 	T_DynamicCreatePlugin<C_PbXLSReader>(pPlugin, pszExtensionPointId, pszPluginId, (_tcscmp(_T("I_PbXLSReader"), pszExtensionPointId) == 0));
	T_DynamicCreatePlugin<C_PbXLSWriter>(pPlugin, pszExtensionPointId, pszPluginId, (_tcscmp(_T("I_PbXLSWriter"), pszExtensionPointId) == 0));
 	
	return pPlugin;
}

void C_XLSPackBundle::ReleaseDynamicPlugin(I_PiPlugin*& pPlugin)
{
 	T_ReleaseDynamicPlugin<C_PbXLSReader>(pPlugin);
	T_ReleaseDynamicPlugin<C_PbXLSWriter>(pPlugin);
}
