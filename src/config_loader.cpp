#include "config_loader.h"

static const long DEFAULT_BUTTON_ACTIONS[MAX_BUTTONS][3] = {
    {';', 0, 0},
    {WXK_NONE, 0, 0},
    {'\'', 0, 0},
    {' ', 0, 0},
    {WXK_NONE, 0, 0},
    {WXK_NONE, 0, 0},
    {WXK_NONE, 0, 0},
    {WXK_NONE, 0, 0},
};

static const long DEFAULT_ENCODER_ACTIONS[MAX_ENCODERS][2][5] = {
    {{WXK_RIGHT, 0, 1, WXK_RIGHT, Keystroke::Shift}, {WXK_LEFT, 0, 1, WXK_LEFT, Keystroke::Shift}},
    {{WXK_NONE, 0, 0, 0, 0}, {WXK_NONE, 0, 0, 0, 0}},
};

//----------------------------------------------------------------------
// ConfigLoader
//----------------------------------------------------------------------
ConfigLoader::ConfigLoader() {
    m_config = wxConfigBase::Create();
}


Action ConfigLoader::LoadAction(const wxString& path, const long defaultValues[]) {
    Action action;
    m_config->SetPath(path);
    long key, flags, modCount;
    size_t def_idx = 0;
    m_config->Read("KeyCode", &key, defaultValues[def_idx++]);
    m_config->Read("KeyFlags", &flags, defaultValues[def_idx++]);
    action.SetKeystroke(Keystroke(key, flags));
    m_config->Read("ModifierCount", &modCount, defaultValues[def_idx++]);
    for (int i = 0; i < modCount; i++) {
        long modifier;
        m_config->Read(wxString::Format("Modifier%d", i), &modifier, defaultValues[def_idx++]);
        m_config->Read(wxString::Format("Modifier%dKeyCode", i), &key, defaultValues[def_idx++]);
        m_config->Read(wxString::Format("Modifier%dKeyFlags", i), &flags, defaultValues[def_idx++]);
        action.SetModifiedKeystroke(modifier, Keystroke(key, flags));
    }
    return action;
}

void ConfigLoader::SaveAction(const wxString& path, const Action& value) {
    m_config->SetPath(path);
    m_config->Write("KeyCode", value.GetKeystroke().key);
    m_config->Write("KeyFlags", value.GetKeystroke().modifiers);
    m_config->Write("ModifierCount", value.GetKeystroke().modifiers);
    for (int i = 0; i < value.GetKeystroke().modifiers; i++) {
        m_config->Write(wxString::Format("Modifier%d", i), value.GetKeystroke().modifiers);
        m_config->Write(wxString::Format("Modifier%dKeyCode", i), value.GetKeystroke().key);
        m_config->Write(wxString::Format("Modifier%dKeyFlags", i), value.GetKeystroke().modifiers);
    }
}

ActionsTemplate ConfigLoader::LoadActiveTemplate() {
    ActionsTemplate template_;
    wxString strOldPath = m_config->GetPath();

    for (int i = 0; i < MAX_BUTTONS; i++) {
        template_.button_actions[i] = LoadAction(wxString::Format("/Active/Button%d", i), DEFAULT_BUTTON_ACTIONS[i]);
    }

    for (int i = 0; i < MAX_ENCODERS; i++) {
        template_.encoder_actions[i][ENCODER_CW] = LoadAction(wxString::Format("/Active/Encoder%d/CW", i), DEFAULT_ENCODER_ACTIONS[i][ENCODER_CW]);
        template_.encoder_actions[i][ENCODER_CCW] = LoadAction(wxString::Format("/Active/Encoder%d/CCW", i), DEFAULT_ENCODER_ACTIONS[i][ENCODER_CCW]);
    }
    m_config->SetPath(strOldPath);
    return template_;
}


void ConfigLoader::SaveActiveTemplate(const ActionsTemplate& template_) {
    wxString strOldPath = m_config->GetPath();

    for (int i = 0; i < MAX_BUTTONS; i++) {
        SaveAction(wxString::Format("/Active/Button%d", i), template_.button_actions[i]);
    }

    for (int i = 0; i < MAX_ENCODERS; i++) {
        SaveAction(wxString::Format("/Active/Encoder%d/CW", i), template_.encoder_actions[i][ENCODER_CW]);
        SaveAction(wxString::Format("/Active/Encoder%d/CCW", i), template_.encoder_actions[i][ENCODER_CCW]);
    }
    m_config->SetPath(strOldPath);
}