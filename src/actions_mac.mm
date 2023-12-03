#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h>
#include "actions.h"

// Uses the private wxWidgets function wxCharCodeWXToOSX to convert non-printable wxWidgets keycodes to macOS key codes.
// Might be better to implement this ourselves, but this is easier for now.
#include <wx/osx/core/private.h>

CFMutableDictionaryRef charToCodeDict = NULL;

/* Returns string representation of key, if it is printable.
 * Ownership follows the Create Rule; that is, it is the caller's
 * responsibility to release the returned object. */
CFStringRef createStringForKey(const UCKeyboardLayout *keyboardLayout, CGKeyCode keyCode, int modifiers)
{
    UInt32 keysDown = 0;
    UniChar chars[4] = {0};
    UniCharCount realLength = sizeof(chars) / sizeof(chars[0]);

    UCKeyTranslate(keyboardLayout,
                   keyCode,
                   kUCKeyActionDisplay,
                   modifiers,
                   LMGetKbdType(),
                   kUCKeyTranslateNoDeadKeysBit,
                   &keysDown,
                   sizeof(chars) / sizeof(chars[0]),
                   &realLength,
                   chars); 

    return CFStringCreateWithCharacters(kCFAllocatorDefault, chars, 1);
}

/* Returns key code for given character via the above function, or UINT16_MAX
 * on error. */
CGKeyCode keyCodeForChar(UniChar character)
{
    //static CFMutableDictionaryRef charToCodeDict = NULL;
    uintptr_t code;
    CFStringRef charStr = NULL;

    if (charToCodeDict == NULL) {
        return UINT16_MAX;
    }

    charStr = CFStringCreateWithCharacters(kCFAllocatorDefault, &character, 1);

    /* Our values may be NULL (0), so we need to use this function. */
    if (!CFDictionaryGetValueIfPresent(charToCodeDict, charStr,
                                       (const void **)&code)) {
        code = UINT16_MAX;
    }

    CFRelease(charStr);
    return (CGKeyCode) code;
}

CGEventFlags modifiersToCGEventFlags(uint8_t modifiers) {
    CGEventFlags flags = 0;
    if (modifiers & Keystroke::Alt) {
        flags |= kCGEventFlagMaskAlternate;
    }
    if (modifiers & Keystroke::Shift) {
        flags |= kCGEventFlagMaskShift;
    }
    if (modifiers & Keystroke::Control) {
        flags |= kCGEventFlagMaskControl;
    }
    if (modifiers & Keystroke::Command) {
        flags |= kCGEventFlagMaskCommand;
    }
    return flags;
}

bool initializeActionsSupport() {
    static const int modifiers[] = {
        0,
        kCGEventFlagMaskAlternate >> 16,
        kCGEventFlagMaskShift >> 16,
        (kCGEventFlagMaskShift | kCGEventFlagMaskAlternate) >> 16,
    };

    NSDictionary *options = @{(id)kAXTrustedCheckOptionPrompt: @YES};
    Boolean accessibilityEnabled = AXIsProcessTrustedWithOptions((CFDictionaryRef)options);

    /* Generate table of keycodes and characters. */
    if (charToCodeDict == NULL) {
        TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardLayoutInputSource();

        if (!currentKeyboard) {
            return false;
        }

        CFDataRef layoutData =
            (CFDataRef) TISGetInputSourceProperty(currentKeyboard,
                                    kTISPropertyUnicodeKeyLayoutData);

        if (!layoutData) {
            CFRelease(currentKeyboard);
            return false;
        }

        const UCKeyboardLayout *keyboardLayout =
            reinterpret_cast<const UCKeyboardLayout*>(CFDataGetBytePtr(layoutData));

        if (!keyboardLayout) {
            CFRelease(currentKeyboard);
            return false;
        }


        charToCodeDict = CFDictionaryCreateMutable(kCFAllocatorDefault,
                                                   128,
                                                   &kCFCopyStringDictionaryKeyCallBacks,
                                                   NULL);
        if (charToCodeDict == NULL) {
            CFRelease(currentKeyboard);
            return false;
        }

        /* Loop through every keycode (0 - 127) to find its current mapping. */
        for (size_t i = 0; i < 128; ++i) {
            for (size_t j = 0; j < sizeof(modifiers) / sizeof(modifiers[0]); ++j) {
                CFStringRef string = createStringForKey(keyboardLayout, (CGKeyCode)i, modifiers[j]);
                if (string != NULL) {
                    CFDictionaryAddValue(charToCodeDict, string, (const void *)i);
                    CFRelease(string);
                }
            }
        }
        CFRelease(currentKeyboard);   
    }

    return (!!accessibilityEnabled);
}

void issueKeystroke(Keystroke keystroke, bool pressed) {
    bool isTrusted = AXIsProcessTrusted();
    CGKeyCode keyCode = UINT16_MAX;
    if (keystroke.is_character) {
        // Printable key, use keyCodeForChar to convert to macOS keycode.
        keyCode = keyCodeForChar((unichar)keystroke.key);
    } else {
        // Non-printable key, use wxCharCodeWXToOSX to convert to macOS keycode.
        keyCode = (CGKeyCode) wxCharCodeWXToOSX((wxKeyCode) keystroke.key);
    }
    wxLogMessage("Sending key code: %d (%x)", (unsigned) keyCode, (unsigned) keyCode);
    if (keyCode != UINT16_MAX) {
        CGEventRef event = CGEventCreateKeyboardEvent(NULL, keyCode, pressed);
        CGEventSetFlags(event, modifiersToCGEventFlags(keystroke.modifiers));
        CGEventPost(kCGHIDEventTap, event);
        CFRelease(event);
    }
}