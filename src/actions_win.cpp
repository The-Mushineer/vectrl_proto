#include "actions.h"
#include <windows.h>

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

void issueKeystroke(Keystroke keystroke, bool pressed) {
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
    for (size_t i = 0; i < num_keys; i++) {
        wxLogMessage(" - Key %d: %d", i, input[i].ki.wVk);
        SendInput(1, input + i, sizeof(input[0]));
    }
}