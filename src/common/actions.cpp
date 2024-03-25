#include "actions.h"

//----------------------------------------------------------------------
// Action
//----------------------------------------------------------------------
void Action::SetKeystroke(Keystroke keystroke) {
    m_keystroke = keystroke;
}

void Action::SetModifiedKeystroke(uint8_t modifier, Keystroke keystroke) {
    if (keystroke == KEYSTROKE_NONE)
        m_modifierKeystrokes.erase(modifier);
    m_modifierKeystrokes[modifier] = keystroke;
}

const Keystroke& Action::GetKeystroke() const {
    return m_keystroke;
}

const Keystroke& Action::GetModifiedKeystroke(uint8_t modifier) const {
    auto it = m_modifierKeystrokes.find(modifier);
    if (it == m_modifierKeystrokes.end())
        return KEYSTROKE_NONE;
    return it->second;
}

const std::map<uint8_t, Keystroke>& Action::GetModifiedKeystrokes() const {
    return m_modifierKeystrokes;
}

const Keystroke& Action::GetDesiredKeystroke(
    std::unordered_set<uint8_t> pressedKeys) const {
    for (auto pressedKey : pressedKeys) {
        auto it = m_modifierKeystrokes.find(pressedKey);
        if (it != m_modifierKeystrokes.end()) {
            return it->second;
        }
    }

    return m_keystroke;
}

//----------------------------------------------------------------------
// Actions
//----------------------------------------------------------------------

Actions::Actions() : m_buttonActions(), m_encoderActions(), m_pressedButtons() {
}

Keystroke Actions::GetButtonKeystroke(uint8_t button) {
    wxCriticalSectionLocker locker(m_actionsCriticalSection);
    if (button > GetButtonCount()) {
        return KEYSTROKE_NONE;
    }
    return m_buttonActions[button].GetDesiredKeystroke(m_pressedButtons);
}

Keystroke Actions::GetEncoderKeystroke(uint8_t encoder,
                                       EncoderDirection direction) {
    wxCriticalSectionLocker locker(m_actionsCriticalSection);
    if (encoder > GetEncoderCount()) {
        return KEYSTROKE_NONE;
    }
    return m_encoderActions[encoder][direction].GetDesiredKeystroke(
        m_pressedButtons);
}

void Actions::IssueButton(uint8_t button, bool pressed) {
    auto keystroke = GetButtonKeystroke(button);
    {
        wxCriticalSectionLocker locker(m_pressedCriticalSection);
        if (pressed) {
            m_pressedButtons.insert(button);
        } else {
            m_pressedButtons.erase(button);
        }
    }
    if (keystroke != KEYSTROKE_NONE) {
        issueKeystroke(keystroke, pressed);
    }
}

void Actions::LoadTemplate(const ActionsTemplate& actionsTemplate) {
    wxCriticalSectionLocker locker(m_actionsCriticalSection);
    m_buttonActions = actionsTemplate.buttonActions;
    m_encoderActions = actionsTemplate.encoderActions;
}

void Actions::IssueEncoder(uint8_t encoder, int8_t count) {
    auto direction = count > 0 ? ENCODER_CW : ENCODER_CCW;
    auto absCount = count > 0 ? count : -count;
    auto keystroke =
        GetEncoderKeystroke(encoder, count > 0 ? ENCODER_CW : ENCODER_CCW);
    if (keystroke != KEYSTROKE_NONE) {
        for (int i = 0; i < absCount; i++) {
            issueKeystroke(keystroke, true);
            issueKeystroke(keystroke, false);
        }
    }
}
