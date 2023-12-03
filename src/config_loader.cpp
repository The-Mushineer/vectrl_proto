#include "config_loader.h"

// Here we arbitrarily use  0x80000000 to indicate that the key is a character
#define CHAR_DEFAULT(c) ((long)((c) | 0x80000000))
#define IS_CHAR_DEFAULT(c) ((c) & 0x80000000)

static const long DEFAULT_BUTTON_ACTIONS[MAX_BUTTONS][3] = {
    {CHAR_DEFAULT(';'), 0, 0},
    {WXK_NONE, 0, 0},
    {CHAR_DEFAULT('\''), 0, 0},
    {CHAR_DEFAULT(' '), 0, 0},
    {WXK_NONE, 0, 0},
    {WXK_NONE, 0, 0},
    {WXK_NONE, 0, 0},
    {WXK_NONE, 0, 0},
};

static const long DEFAULT_ENCODER_ACTIONS[MAX_ENCODERS][2][6] = {
    {{WXK_RIGHT, 0, 1, 1, WXK_RIGHT, Keystroke::Shift}, {WXK_LEFT, 0, 1, 1, WXK_LEFT, Keystroke::Shift}},
    {{WXK_NONE, 0, 0, 0, 0, 0}, {WXK_NONE, 0, 0, 0, 0, 0}},
};

static wxString GetDefKeyValue(long defKeyLong) {
    wxString defKey;
    if (IS_CHAR_DEFAULT(defKeyLong)) {
        defKey = wxString::Format("%c", defKeyLong & 0x7fffffff);
    } else {
        defKey = wxString::Format("0x%x", defKeyLong);
    }
    return defKey;
}

static wxString GetKeystrokeCode(const Keystroke& keystroke) {
    wxString keyCode;
    if (keystroke.is_character) {
        keyCode = wxString::Format("%c", keystroke.key);
    } else {
        keyCode = wxString::Format("0x%x", keystroke.key);
    }
    return keyCode;
}

static int GetLongFromHex(const wxString& hex) {
    long value = 0;
    if (hex.length() > 2 && hex[0] == '0' && hex[1] == 'x') {
        hex.ToLong(&value, 16);
    }
    return static_cast<int>(value);
}

//----------------------------------------------------------------------
// ConfigLoader
//----------------------------------------------------------------------
ConfigLoader::ConfigLoader() {
    m_config = wxConfigBase::Create();
}


Action ConfigLoader::LoadAction(const wxString& path, const long defaultValues[]) {
    Action action;
    m_config->SetPath(path);
    long flags, modCount;
    wxString key;
    size_t def_idx = 0;
    m_config->Read("KeyCode", &key, GetDefKeyValue(defaultValues[def_idx++]));
    m_config->Read("KeyFlags", &flags, defaultValues[def_idx++]);
    if (key.starts_with("0x")) {
        action.SetKeystroke(Keystroke(GetLongFromHex(key), flags));
    } else {
        wchar_t keyChar = key.length() > 0 ? key[0] : '\0';
        action.SetKeystroke(Keystroke(keyChar, flags));
    }
    m_config->Read("ModifierCount", &modCount, defaultValues[def_idx++]);
    for (int i = 0; i < modCount; i++) {
        long modifier;
        m_config->Read(wxString::Format("Modifier%d", i), &modifier, defaultValues[def_idx++]);
        m_config->Read(wxString::Format("Modifier%dKeyCode", i), &key, GetDefKeyValue(defaultValues[def_idx++]));
        m_config->Read(wxString::Format("Modifier%dKeyFlags", i), &flags, defaultValues[def_idx++]);
        if (key.starts_with("0x")) {
            action.SetModifiedKeystroke(modifier, Keystroke(GetLongFromHex(key), flags));
        } else {
            wchar_t keyChar = key[0];
            action.SetModifiedKeystroke(modifier, Keystroke(keyChar, flags));
        }
    }
    return action;
}

void ConfigLoader::SaveAction(const wxString& path, const Action& value) {
    m_config->SetPath(path);
    const auto& keystroke = value.GetKeystroke();
    const auto& modifiedKeystrokes = value.GetModifiedKeystrokes();
    m_config->Write("KeyCode", GetKeystrokeCode(keystroke));
    m_config->Write("KeyFlags", keystroke.modifiers);
    m_config->Write("ModifierCount", modifiedKeystrokes.size());
    int i = 0;
    for (auto item : modifiedKeystrokes) {
        m_config->Write(wxString::Format("Modifier%d", i), item.first);
        m_config->Write(wxString::Format("Modifier%dKeyCode", i), GetKeystrokeCode(item.second));
        m_config->Write(wxString::Format("Modifier%dKeyFlags", i), item.second.modifiers);
        i++;
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