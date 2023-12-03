#include "actions.h"
#include <windows.h>

// Uses the private wxWidgets function WXToVK to convert a wxWidgets keycode to a Windows virtual key code.
// Might be better to implement this ourselves, but this is easier for now.
#include <wx/msw/private/keyboard.h>

bool initializeActionsSupport() {
    return true;
}

void issueKeystroke(Keystroke keystroke, bool pressed) {
    INPUT input[4];
    size_t num_keys = 0;
    DWORD dwFlags = pressed ? 0 : KEYEVENTF_KEYUP;
    bool isExtended = false;
    WORD keyCode = 0;
    if (keystroke.is_character) {
        keyCode = VkKeyScanW(keystroke.key);
    } else {
        keyCode = wxMSWKeyboard::WXToVK(keystroke.key, &isExtended);
    }
    memset(input, 0, sizeof(input));
    if (keystroke.modifiers & Keystroke::Control) {
        input[num_keys].type = INPUT_KEYBOARD;
        input[num_keys].ki.wVk = VK_CONTROL;
        input[num_keys].ki.dwFlags = dwFlags;
        num_keys++;
    }
    if (keystroke.modifiers & Keystroke::Alt) {
        input[num_keys].type = INPUT_KEYBOARD;
        input[num_keys].ki.wVk = VK_MENU;
        input[num_keys].ki.dwFlags = dwFlags;
        num_keys++;
    }
    if (keystroke.modifiers & Keystroke::Shift) {
        input[num_keys].type = INPUT_KEYBOARD;
        input[num_keys].ki.wVk = VK_SHIFT;
        input[num_keys].ki.dwFlags = dwFlags;
        num_keys++;
    }
    input[num_keys].type = INPUT_KEYBOARD;
    input[num_keys].ki.wVk = keyCode;
    input[num_keys].ki.dwFlags = dwFlags | (isExtended ? KEYEVENTF_EXTENDEDKEY : 0);
    num_keys++;
    if (!pressed) {
        std::reverse(input, input + num_keys);
    }
    wxLogMessage("Sending %d keys", (int) num_keys);
    for (size_t i = 0; i < num_keys; i++) {
        wxLogMessage(" - Key %d: %d", i, input[i].ki.wVk);
        SendInput(1, input + i, sizeof(input[0]));
    }
}