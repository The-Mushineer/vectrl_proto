#ifndef __VECTRL_CONFIG_LOADER_H__
#define __VECTRL_CONFIG_LOADER_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/config.h"
#endif

#include "../common/actions.h"

class ConfigLoader {
public:
    ConfigLoader();

    ActionsTemplate LoadActiveTemplate();
    void SaveActiveTemplate(const ActionsTemplate& template_);

    ActionsTemplate GetDefaultTemplate();

private:
    wxConfigBase* m_config;
    Action LoadAction(const wxString& path, const Action& defaultValue);
    void SaveAction(const wxString& path, const Action& value);
};

#endif
