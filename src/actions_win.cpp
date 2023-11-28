#include "actions.h"
#include <windows.h>

// Uses the private wxWidgets function WXToVK to convert a wxWidgets keycode to a Windows virtual key code.
// Might be better to implement this ourselves, but this is easier for now.
#include <wx/msw/private/keyboard.h>

void issueKeystroke(Keystroke keystroke, bool pressed) {
    INPUT input[4];
    size_t num_keys = 0;
    DWORD dwFlags = pressed ? 0 : KEYEVENTF_KEYUP;
    bool isExtended = false;
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
    input[num_keys].ki.wVk = wxMSWKeyboard::WXToVK(keystroke.key, &isExtended);
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