#pragma once

#include "PluginClient\T_BundleTemplateWithDynamicPlugin.h"

class C_XLSPackBundle : public T_BundleTemplateWithDynamicPlugin<T_BundleTemplate<I_PiBundle> >
{
public:
    C_XLSPackBundle(void);
    virtual ~C_XLSPackBundle(void);

protected:
	I_PiPlugin* DynamicCreatePlugin(LPCTSTR pszExtensionPointId, LPCTSTR pszPluginId);
	void ReleaseDynamicPlugin(I_PiPlugin*& pPlugin);

protected:
	BEGIN_REGISTER_PLUGIN()
	END_REGISTER_PLUGIN()
};
