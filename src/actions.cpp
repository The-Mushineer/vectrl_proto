#include "actions.h"
#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _WIN32
static bool isExtended(uint16_t vk_code) {
    switch (vk_code) {
        case VK_LEFT:
        case VK_UP:
        case VK_RIGHT:
        case VK_DOWN:
        case VK_PRIOR:
        case VK_NEXT:
        case VK_END:
        case VK_HOME:
        case VK_INSERT:
        case VK_DELETE:
        case VK_DIVIDE:
        case VK_NUMLOCK:
            return true;
    }
    return false;
}
#endif

static void issueKeystroke(Keystroke keystroke, bool pressed) {
#ifdef _WIN32
    INPUT input[4];
    size_t num_keys = 0;
    DWORD dwFlags = pressed ? 0 : KEYEVENTF_KEYUP;
    memset(input, 0, sizeof(input));
    if (keystroke.ctrl) {
        input[num_keys].type = INPUT_KEYBOARD;
        input[num_keys].ki.wVk = VK_CONTROL;
        input[num_keys].ki.dwFlags = dwFlags;
        num_keys++;
    }
    if (keystroke.alt) {
        input[num_keys].type = INPUT_KEYBOARD;
        input[num_keys].ki.wVk = VK_MENU;
        input[num_keys].ki.dwFlags = dwFlags;
        num_keys++;
    }
    if (keystroke.shift) {
        input[num_keys].type = INPUT_KEYBOARD;
        input[num_keys].ki.wVk = VK_SHIFT;
        input[num_keys].ki.dwFlags = dwFlags;
        num_keys++;
    }
    input[num_keys].type = INPUT_KEYBOARD;
    input[num_keys].ki.wVk = keystroke.key;
    input[num_keys].ki.dwFlags = dwFlags | (isExtended(keystroke.key) ? KEYEVENTF_EXTENDEDKEY : 0);
    num_keys++;
    if (!pressed) {
        std::reverse(input, input + num_keys);
    }
    wxLogMessage("Sending %d keys", (int) num_keys);
    for (int i = 0; i < num_keys; i++) {
        wxLogMessage(" - Key %d: %d", i, input[i].ki.wVk);
        SendInput(1, input + i, sizeof(input[0]));
    }
#endif
}

//----------------------------------------------------------------------
// Action
//----------------------------------------------------------------------

Action::Action(): m_keystroke(KEYSTROKE_NONE) {}

void Action::SetKeystroke(Keystroke keystroke) {
    m_keystroke = keystroke;
}

void Action::SetModifiedKeystroke(uint8_t modifier, Keystroke keystroke) {
    if (keystroke == KEYSTROKE_NONE)
        m_modifier_keystrokes.erase(modifier);
    m_modifier_keystrokes[modifier] = keystroke;
}

Keystroke Action::GetKeystroke() {
    return m_keystroke;
}

Keystroke Action::GetModifiedKeystroke(uint8_t modifier) {
    auto it = m_modifier_keystrokes.find(modifier);
    if (it == m_modifier_keystrokes.end())
        return KEYSTROKE_NONE;
    return it->second;
}

Keystroke Action::GetDesiredKeystroke(std::unordered_set<uint8_t> pressed_keys) {
    for (auto pressed_key : pressed_keys) {
        auto it = m_modifier_keystrokes.find(pressed_key);
            if (it != m_modifier_keystrokes.end()) {
                return it->second;
            }
    }

    return m_keystroke;
}

//----------------------------------------------------------------------
// Actions
//----------------------------------------------------------------------

Actions::Actions(): m_button_actions(), m_encoder_actions(), m_pressed_buttons() {}

void Actions::SetButtonKeystroke(uint8_t button, Keystroke action) {
    wxCriticalSectionLocker locker(m_button_critsect);
    m_button_actions[button].SetKeystroke(action);
}

void Actions::SetModifiedButtonKeystroke(uint8_t button, uint8_t modifier, Keystroke action) {
    wxCriticalSectionLocker locker(m_button_critsect);
    m_button_actions[button].SetModifiedKeystroke(modifier, action);
}

void Actions::SetEncoderKeystroke(uint8_t encoder, EncoderDirection direction, Keystroke action) {
    wxCriticalSectionLocker locker(m_encoder_critsect);
    m_encoder_actions[encoder][direction].SetKeystroke(action);
}

void Actions::SetModifiedEncoderKeystroke(uint8_t encoder, uint8_t modifier, EncoderDirection direction, Keystroke action) {
    wxCriticalSectionLocker locker(m_encoder_critsect);
    m_encoder_actions[encoder][direction].SetModifiedKeystroke(modifier, action);
}

 Keystroke Actions::GetButtonKeystroke(uint8_t button) {
    wxCriticalSectionLocker locker(m_button_critsect);
    return m_button_actions[button].GetDesiredKeystroke(m_pressed_buttons);
}

Keystroke Actions::GetEncoderKeystroke(uint8_t encoder, EncoderDirection direction) {
    wxCriticalSectionLocker locker(m_encoder_critsect);
    return m_encoder_actions[encoder][direction].GetDesiredKeystroke(m_pressed_buttons);
}

void Actions::IssueButton(uint8_t button, bool pressed) {
    auto keystroke = GetButtonKeystroke(button);
    {
        wxCriticalSectionLocker locker(m_pressed_critsect);
        if (pressed) {
            m_pressed_buttons.insert(button);
        } else {
            m_pressed_buttons.erase(button);
        }
    }
    if (keystroke != KEYSTROKE_NONE) {
        issueKeystroke(keystroke, pressed);
    }
}

void Actions::IssueEncoder(uint8_t encoder, int8_t count) {
    auto direction = count > 0 ? ENCODER_CW : ENCODER_CCW;
    auto abs_count = count > 0 ? count : -count;
    auto keystroke = GetEncoderKeystroke(encoder, count > 0 ? ENCODER_CW : ENCODER_CCW);
    if (keystroke != KEYSTROKE_NONE) {
        for (int i = 0; i < abs_count; i++) {
            issueKeystroke(keystroke, true);
            issueKeystroke(keystroke, false);
        }
    }
}
