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
    uint16_t key;
    bool ctrl;
    bool alt;
    bool shift;

    Keystroke(uint16_t _key, bool _ctrl, bool _alt, bool _shift): key(_key), ctrl(_ctrl), alt(_alt), shift(_shift) {};
    Keystroke(): key(0), ctrl(false), alt(false), shift(false) {};

    bool operator==(const Keystroke& other) const {
        return key == other.key && ctrl == other.ctrl && alt == other.alt && shift == other.shift;
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
    Keystroke GetKeystroke();
    Keystroke GetModifiedKeystroke(uint8_t modifier);
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

#endif