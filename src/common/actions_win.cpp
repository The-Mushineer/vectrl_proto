#include "actions.h"

#include <windows.h>

// Uses the private wxWidgets function WXToVK to convert a wxWidgets keycode to
// a Windows virtual key code. Might be better to implement this ourselves, but
// this is easier for now.
#include <wx/msw/private/keyboard.h>

bool initializeActionsSupport() {
    return true;
}

void issueKeystroke(Keystroke keystroke, bool pressed) {
    INPUT input[4];
    size_t numKeys = 0;
    DWORD dwFlags = pressed ? 0 : KEYEVENTF_KEYUP;
    bool isExtended = false;
    WORD keyCode = 0;
    if (keystroke.isCharacter) {
        wxUniChar uc(keystroke.key);
        keyCode = VkKeyScanW((wchar_t)uc);
    } else {
        keyCode = wxMSWKeyboard::WXToVK(keystroke.key, &isExtended);
    }
    memset(input, 0, sizeof(input));
    if (keystroke.modifiers & Keystroke::Control) {
        input[numKeys].type = INPUT_KEYBOARD;
        input[numKeys].ki.wVk = VK_CONTROL;
        input[numKeys].ki.dwFlags = dwFlags;
        numKeys++;
    }
    if (keystroke.modifiers & Keystroke::Alt) {
        input[numKeys].type = INPUT_KEYBOARD;
        input[numKeys].ki.wVk = VK_MENU;
        input[numKeys].ki.dwFlags = dwFlags;
        numKeys++;
    }
    if (keystroke.modifiers & Keystroke::Shift) {
        input[numKeys].type = INPUT_KEYBOARD;
        input[numKeys].ki.wVk = VK_SHIFT;
        input[numKeys].ki.dwFlags = dwFlags;
        numKeys++;
    }
    input[numKeys].type = INPUT_KEYBOARD;
    input[numKeys].ki.wVk = keyCode;
    input[numKeys].ki.dwFlags =
        dwFlags | (isExtended ? KEYEVENTF_EXTENDEDKEY : 0);
    numKeys++;
    if (!pressed) {
        std::reverse(input, input + numKeys);
    }
    wxLogMessage("Sending %d keys", (int)numKeys);
    for (size_t i = 0; i < numKeys; i++) {
        wxLogMessage(" - Key %d: %d", i, input[i].ki.wVk);
        SendInput(1, input + i, sizeof(input[0]));
    }
}