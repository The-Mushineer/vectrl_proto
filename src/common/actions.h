#ifndef __VECTRL_ACTIONS_H__
#define __VECTRL_ACTIONS_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/log.h"
#include "wx/thread.h"
#endif

#include <array>
#include <cstdint>
#include <map>
#include <unordered_set>

const size_t MAX_BUTTONS = 8;
const size_t MAX_ENCODERS = 2;

struct Keystroke {
    enum KeyModifiers { Control = 1, Alt = 2, Shift = 4, Command = 8 };
    bool isCharacter;
    int key;
    uint8_t modifiers;

    constexpr Keystroke(int _key = 0, uint8_t _modifiers = 0)
        : key(_key), isCharacter(false), modifiers(_modifiers){};
    constexpr Keystroke(wchar_t _key, uint8_t _modifiers = 0)
        : key(_key), isCharacter(true), modifiers(_modifiers){};
    Keystroke(wxUniChar _key, uint8_t _modifiers = 0)
        : key(_key.GetValue()), isCharacter(true), modifiers(_modifiers){};

    bool operator==(const Keystroke& other) const {
        return key == other.key && modifiers == other.modifiers;
    }
    bool operator!=(const Keystroke& other) const {
        return !(*this == other);
    }
};

const Keystroke KEYSTROKE_NONE = Keystroke();

enum EncoderDirection { ENCODER_CW, ENCODER_CCW };

class Action {
public:
    Action() : m_keystroke(KEYSTROKE_NONE){};
    Action(const Keystroke& keystroke) : m_keystroke(keystroke){};
    Action(const Keystroke& keystroke,
           const std::map<uint8_t, Keystroke>& modifierKeystrokes)
        : m_keystroke(keystroke), m_modifierKeystrokes(modifierKeystrokes){};

    void SetKeystroke(Keystroke keystroke);
    void SetModifiedKeystroke(uint8_t modifier, Keystroke keystroke);
    const Keystroke& GetKeystroke() const;
    const Keystroke& GetModifiedKeystroke(uint8_t modifier) const;
    const std::map<uint8_t, Keystroke>& GetModifiedKeystrokes() const;
    const Keystroke& GetDesiredKeystroke(
        std::unordered_set<uint8_t> pressedKeys) const;

private:
    Keystroke m_keystroke;
    std::map<uint8_t, Keystroke> m_modifierKeystrokes;
};

class ActionsTemplate {
public:
    std::array<Action, MAX_BUTTONS> buttonActions;
    std::array<std::array<Action, 2>, MAX_ENCODERS> encoderActions;

    size_t GetButtonCount() const {
        return MAX_BUTTONS;
    }
    size_t GetEncoderCount() const {
        return MAX_ENCODERS;
    }
};

class Actions {
public:
    Actions();

    Keystroke GetButtonKeystroke(uint8_t button);
    Keystroke GetEncoderKeystroke(uint8_t encoder, EncoderDirection direction);

    size_t GetButtonCount() const {
        return MAX_BUTTONS;
    }
    size_t GetEncoderCount() const {
        return MAX_ENCODERS;
    }

    void IssueButton(uint8_t button, bool pressed);
    void IssueEncoder(uint8_t encoder, int8_t count);

    void LoadTemplate(const ActionsTemplate& actionsTemplate);

private:
    std::array<Action, MAX_BUTTONS> m_buttonActions;
    std::array<std::array<Action, 2>, MAX_ENCODERS> m_encoderActions;
    std::unordered_set<uint8_t> m_pressedButtons;
    wxCriticalSection m_actionsCriticalSection;
    wxCriticalSection m_pressedCriticalSection;
};

void issueKeystroke(Keystroke keystroke, bool pressed);
bool initializeActionsSupport();

#endif