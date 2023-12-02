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
    int key;
    uint8_t modifiers;

    Keystroke(int _key = 0, uint8_t _modifiers = 0): key(_key), modifiers(_modifiers) {};

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
    Action();

    void SetKeystroke(Keystroke keystroke);
    void SetModifiedKeystroke(uint8_t modifier, Keystroke keystroke);
    Keystroke GetKeystroke() const;
    Keystroke GetModifiedKeystroke(uint8_t modifier) const;
    Keystroke GetDesiredKeystroke(std::unordered_set<uint8_t> pressed_keys);
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