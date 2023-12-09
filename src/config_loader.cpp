#include "config_loader.h"

static const Action DEFAULT_BUTTON_ACTIONS[MAX_BUTTONS] = {
    Action(Keystroke(';')),      Action(Keystroke(WXK_NONE)),
    Action(Keystroke('\'')),     Action(Keystroke(' ')),
    Action(Keystroke(WXK_NONE)), Action(Keystroke(WXK_NONE)),
};

static const Action DEFAULT_ENCODER_ACTIONS[MAX_ENCODERS][2] = {
    {
        Action(Keystroke(WXK_RIGHT),
               {{1, Keystroke(WXK_RIGHT, Keystroke::Shift)}}),
        Action(Keystroke(WXK_LEFT),
               {{1, Keystroke(WXK_LEFT, Keystroke::Shift)}}),
    },
    {Action(Keystroke(WXK_NONE)), Action(Keystroke(WXK_NONE))},
};

static wxString GetKeystrokeCode(const Keystroke& keystroke) {
    wxString keyCode;
    if (keystroke.isCharacter) {
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

Action ConfigLoader::LoadAction(const wxString& path,
                                const Action& defaultValue) {
    Action action;
    if (!m_config->Exists(path)) {
        return defaultValue;
    };
    m_config->SetPath(path);
    long flags, modCount;
    wxString key;
    m_config->Read("KeyCode", &key, L"0x0");
    m_config->Read("KeyFlags", &flags, 0);
    if (key.starts_with("0x")) {
        action.SetKeystroke(Keystroke(GetLongFromHex(key), flags));
    } else {
        wchar_t keyChar = key.length() > 0 ? key[0] : '\0';
        action.SetKeystroke(Keystroke(keyChar, flags));
    }
    m_config->Read("ModifierCount", &modCount, 0);
    for (int i = 0; i < modCount; i++) {
        long modifier;
        m_config->Read(wxString::Format("Modifier%d", i), &modifier, 0);
        m_config->Read(wxString::Format("Modifier%dKeyCode", i), &key, L"0x0");
        m_config->Read(wxString::Format("Modifier%dKeyFlags", i), &flags, 0);
        if (key.starts_with("0x")) {
            action.SetModifiedKeystroke(modifier,
                                        Keystroke(GetLongFromHex(key), flags));
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
    for (const auto& item : modifiedKeystrokes) {
        m_config->Write(wxString::Format("Modifier%d", i), item.first);
        m_config->Write(wxString::Format("Modifier%dKeyCode", i),
                        GetKeystrokeCode(item.second));
        m_config->Write(wxString::Format("Modifier%dKeyFlags", i),
                        item.second.modifiers);
        i++;
    }
}

ActionsTemplate ConfigLoader::LoadActiveTemplate() {
    ActionsTemplate template_;
    wxString strOldPath = m_config->GetPath();

    for (int i = 0; i < MAX_BUTTONS; i++) {
        template_.buttonActions[i] = LoadAction(
            wxString::Format("/Active/Button%d", i), DEFAULT_BUTTON_ACTIONS[i]);
    }

    for (int i = 0; i < MAX_ENCODERS; i++) {
        template_.encoderActions[i][ENCODER_CW] =
            LoadAction(wxString::Format("/Active/Encoder%d/CW", i),
                       DEFAULT_ENCODER_ACTIONS[i][ENCODER_CW]);
        template_.encoderActions[i][ENCODER_CCW] =
            LoadAction(wxString::Format("/Active/Encoder%d/CCW", i),
                       DEFAULT_ENCODER_ACTIONS[i][ENCODER_CCW]);
    }
    m_config->SetPath(strOldPath);
    return template_;
}

void ConfigLoader::SaveActiveTemplate(const ActionsTemplate& template_) {
    wxString strOldPath = m_config->GetPath();

    for (int i = 0; i < MAX_BUTTONS; i++) {
        SaveAction(wxString::Format("/Active/Button%d", i),
                   template_.buttonActions[i]);
    }

    for (int i = 0; i < MAX_ENCODERS; i++) {
        SaveAction(wxString::Format("/Active/Encoder%d/CW", i),
                   template_.encoderActions[i][ENCODER_CW]);
        SaveAction(wxString::Format("/Active/Encoder%d/CCW", i),
                   template_.encoderActions[i][ENCODER_CCW]);
    }
    m_config->SetPath(strOldPath);
}