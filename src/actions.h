#ifndef __VECTRL_ACTIONS_H__
#define __VECTRL_ACTIONS_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/thread.h"
#include "wx/log.h"
#endif

#include <cstdint>
#include <map>
#include <unordered_set>
#include <array>

const size_t MAX_BUTTONS = 8;
const size_t MAX_ENCODERS = 2;

struct Keystroke {
    enum KeyModifiers {
        Control = 1,
        Alt = 2,
        Shift = 4,
        Command = 8
    };
    bool is_character;
    int key;
    uint8_t modifiers;

    constexpr Keystroke(int _key = 0, uint8_t _modifiers = 0): key(_key), is_character(false), modifiers(_modifiers) {};
    constexpr Keystroke(wchar_t _key, uint8_t _modifiers = 0): key(_key), is_character(true), modifiers(_modifiers) {};

    bool operator==(const Keystroke& other) const {
        return key == other.key && modifiers == other.modifiers;
    }
    bool operator!=(const Keystroke& other) const {
        return !(*this == other);
    }
};

const Keystroke KEYSTROKE_NONE = Keystroke();

enum EncoderDirection {
    ENCODER_CW,
    ENCODER_CCW
};

class Action {
public:
    Action(): m_keystroke(KEYSTROKE_NONE) {};
    Action(const Keystroke& keystroke): m_keystroke(keystroke) {};
    Action(const Keystroke& keystroke, const std::map<uint8_t, Keystroke>& modifier_keystrokes):
        m_keystroke(keystroke), m_modifier_keystrokes(modifier_keystrokes) {};

    void SetKeystroke(Keystroke keystroke);
    void SetModifiedKeystroke(uint8_t modifier, Keystroke keystroke);
    const Keystroke& GetKeystroke() const;
    const Keystroke& GetModifiedKeystroke(uint8_t modifier) const;
    const std::map<uint8_t, Keystroke>& GetModifiedKeystrokes() const;
    const Keystroke&  GetDesiredKeystroke(std::unordered_set<uint8_t> pressed_keys) const;
private:
    Keystroke m_keystroke;
    std::map<uint8_t, Keystroke> m_modifier_keystrokes;
};

class ActionsTemplate {
public:
    std::array<Action, MAX_BUTTONS> button_actions;
    std::array<std::array<Action, 2>, MAX_ENCODERS> encoder_actions;

    size_t GetButtonCount() const { return MAX_BUTTONS; }
    size_t GetEncoderCount() const { return MAX_ENCODERS; }
};

class Actions {
public:
    Actions();

    Keystroke GetButtonKeystroke(uint8_t button);
    Keystroke GetEncoderKeystroke(uint8_t encoder, EncoderDirection direction);

    size_t GetButtonCount() const { return MAX_BUTTONS; }
    size_t GetEncoderCount() const { return MAX_ENCODERS; }

    void IssueButton(uint8_t button, bool pressed);
    void IssueEncoder(uint8_t encoder, int8_t count);

    void LoadTemplate(const ActionsTemplate& actions_template);
private:
    std::array<Action, MAX_BUTTONS> m_button_actions;
    std::array<std::array<Action, 2>, MAX_ENCODERS> m_encoder_actions;
    std::unordered_set<uint8_t> m_pressed_buttons;
    wxCriticalSection m_actions_critsect;
    wxCriticalSection m_pressed_critsect;
};

void issueKeystroke(Keystroke keystroke, bool pressed);
bool initializeActionsSupport();

#endif