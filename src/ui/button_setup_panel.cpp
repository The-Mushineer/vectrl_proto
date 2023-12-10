#include "button_setup_panel.h"
#include "ui_common.h"

#ifdef __APPLE__
const wxString s_controlLabel = wxT("^");
const wxString s_altLabel = wxT("⌥");
const wxString s_shiftLabel = wxT("⇧");
const wxString s_commandLabel = wxT("⌘");
#else
const wxString s_controlLabel = wxT("Ctrl + ");
const wxString s_altLabel = wxT("Alt + ");
const wxString s_shiftLabel = wxT("Shift + ");
const wxString s_commandLabel = wxT("Win + ");
#endif

wxDEFINE_EVENT(EVT_SET_KEYSTROKE, SetKeystrokeEvent);
wxDEFINE_EVENT(EVT_SET_ACTION, SetActionEvent);

// ----------------------------------------------------------------------------
// Auxiliary functions
// ----------------------------------------------------------------------------
wxString GetKeyName(int key, bool isCharacter) {
    switch (key) {
        case WXK_BACK:
#ifdef __WXOSX__
            return wxT("Delete (backspace)");
#else
            return wxT("Backspace");
#endif
        case WXK_TAB:
            return wxT("Tab");
        case WXK_SPACE:
            return wxT("Space");
        case WXK_RETURN:
            return wxT("Enter");
    };
    if (isCharacter) {
        return (wchar_t)key;
    }
    switch (key) {
        case WXK_ESCAPE:
            return wxT("Esc");
        case WXK_DELETE:
#ifdef __WXOSX__
            return wxT("Delete (forward)");
#else
            return wxT("Delete");
#endif
        case WXK_START:
            return wxT("Start");
        case WXK_LBUTTON:
            return wxT("Left mouse button");
        case WXK_RBUTTON:
            return wxT("Right mouse button");
        case WXK_CANCEL:
            return wxT("Cancel");
        case WXK_MBUTTON:
            return wxT("Middle mouse button");
        case WXK_CLEAR:
            return wxT("Clear");
        case WXK_SHIFT:
            return wxT("Shift");
        case WXK_ALT:
#ifdef __WXOSX__
            return wxT("Option");
#else
            return wxT("Alt");
#endif
        case WXK_RAW_CONTROL:
            return wxT("Control");
        case WXK_MENU:
            return wxT("Menu");
        case WXK_PAUSE:
            return wxT("Pause");
        case WXK_CAPITAL:
            return wxT("Caps Lock");
        case WXK_END:
            return wxT("End");
        case WXK_HOME:
            return wxT("Home");
        case WXK_LEFT:
            return wxT("Left");
        case WXK_UP:
            return wxT("Up");
        case WXK_RIGHT:
            return wxT("Right");
        case WXK_DOWN:
            return wxT("Down");
        case WXK_SELECT:
            return wxT("Select");
        case WXK_PRINT:
            return wxT("Print");
        case WXK_EXECUTE:
            return wxT("Execute");
        case WXK_SNAPSHOT:
            return wxT("Print Screen");
        case WXK_INSERT:
            return wxT("Insert");
        case WXK_HELP:
            return wxT("Help");
        case WXK_NUMPAD0:
            return wxT("Numpad 0");
        case WXK_NUMPAD1:
            return wxT("Numpad 1");
        case WXK_NUMPAD2:
            return wxT("Numpad 2");
        case WXK_NUMPAD3:
            return wxT("Numpad 3");
        case WXK_NUMPAD4:
            return wxT("Numpad 4");
        case WXK_NUMPAD5:
            return wxT("Numpad 5");
        case WXK_NUMPAD6:
            return wxT("Numpad 6");
        case WXK_NUMPAD7:
            return wxT("Numpad 7");
        case WXK_NUMPAD8:
            return wxT("Numpad 8");
        case WXK_NUMPAD9:
            return wxT("Numpad 9");
        case WXK_MULTIPLY:
            return wxT("Numpad *");
        case WXK_ADD:
            return wxT("Numpad +");
        case WXK_SEPARATOR:
            return wxT("Separator");
        case WXK_SUBTRACT:
            return wxT("Numpad -");
        case WXK_DECIMAL:
            return wxT("Numpad .");
        case WXK_DIVIDE:
            return wxT("Numpad /");
        case WXK_F1:
            return wxT("F1");
        case WXK_F2:
            return wxT("F2");
        case WXK_F3:
            return wxT("F3");
        case WXK_F4:
            return wxT("F4");
        case WXK_F5:
            return wxT("F5");
        case WXK_F6:
            return wxT("F6");
        case WXK_F7:
            return wxT("F7");
        case WXK_F8:
            return wxT("F8");
        case WXK_F9:
            return wxT("F9");
        case WXK_F10:
            return wxT("F10");
        case WXK_F11:
            return wxT("F11");
        case WXK_F12:
            return wxT("F12");
        case WXK_F13:
            return wxT("F13");
        case WXK_F14:
            return wxT("F14");
        case WXK_F15:
            return wxT("F15");
        case WXK_F16:
            return wxT("F16");
        case WXK_F17:
            return wxT("F17");
        case WXK_F18:
            return wxT("F18");
        case WXK_F19:
            return wxT("F19");
        case WXK_F20:
            return wxT("F20");
        case WXK_F21:
            return wxT("F21");
        case WXK_F22:
            return wxT("F22");
        case WXK_F23:
            return wxT("F23");
        case WXK_F24:
            return wxT("F24");
        case WXK_NUMLOCK:
            return wxT("Numpad Lock");
        case WXK_SCROLL:
            return wxT("Scroll Lock");
        case WXK_PAGEUP:
            return wxT("Page Up");
        case WXK_PAGEDOWN:
            return wxT("Page Down");
        case WXK_NUMPAD_SPACE:
            return wxT("Numpad Space");
        case WXK_NUMPAD_TAB:
            return wxT("Numpad Tab");
        case WXK_NUMPAD_ENTER:
            return wxT("Numpad Enter");
        case WXK_NUMPAD_F1:
            return wxT("Numpad F1");
        case WXK_NUMPAD_F2:
            return wxT("Numpad F2");
        case WXK_NUMPAD_F3:
            return wxT("Numpad F3");
        case WXK_NUMPAD_F4:
            return wxT("Numpad F4");
        case WXK_NUMPAD_HOME:
            return wxT("Numpad Home");
        case WXK_NUMPAD_LEFT:
            return wxT("Numpad Left");
        case WXK_NUMPAD_UP:
            return wxT("Numpad Up");
        case WXK_NUMPAD_RIGHT:
            return wxT("Numpad Right");
        case WXK_NUMPAD_DOWN:
            return wxT("Numpad Down");
        case WXK_NUMPAD_PAGEUP:
            return wxT("Numpad Page Up");
        case WXK_NUMPAD_PAGEDOWN:
            return wxT("Numpad Page Down");
        case WXK_NUMPAD_END:
            return wxT("Numpad End");
        case WXK_NUMPAD_BEGIN:
            return wxT("Numpad Begin");
        case WXK_NUMPAD_INSERT:
            return wxT("Numpad Insert");
        case WXK_NUMPAD_DELETE:
            return wxT("Numpad Delete");
        case WXK_NUMPAD_EQUAL:
            return wxT("Numpad =");
        case WXK_NUMPAD_MULTIPLY:
            return wxT("Numpad *");
        case WXK_NUMPAD_ADD:
            return wxT("Numpad +");
        case WXK_NUMPAD_SEPARATOR:
            return wxT("Numpad Separator");
        case WXK_NUMPAD_SUBTRACT:
            return wxT("Numpad -");
        case WXK_NUMPAD_DECIMAL:
            return wxT("Numpad .");
        case WXK_NUMPAD_DIVIDE:
            return wxT("Numpad /");
        case WXK_WINDOWS_LEFT:
            return wxT("Left Windows");
        case WXK_WINDOWS_RIGHT:
            return wxT("Right Windows");
        case WXK_WINDOWS_MENU:
            return wxT("Windows Menu");
#ifdef __WXOSX__
        case WXK_COMMAND:
            return wxT("Command");
#endif
        case WXK_SPECIAL1:
            return wxT("Special 1");
        case WXK_SPECIAL2:
            return wxT("Special 2");
        case WXK_SPECIAL3:
            return wxT("Special 3");
        case WXK_SPECIAL4:
            return wxT("Special 4");
        case WXK_SPECIAL5:
            return wxT("Special 5");
        case WXK_SPECIAL6:
            return wxT("Special 6");
        case WXK_SPECIAL7:
            return wxT("Special 7");
        case WXK_SPECIAL8:
            return wxT("Special 8");
        case WXK_SPECIAL9:
            return wxT("Special 9");
        case WXK_SPECIAL10:
            return wxT("Special 10");
        case WXK_SPECIAL11:
            return wxT("Special 11");
        case WXK_SPECIAL12:
            return wxT("Special 12");
        case WXK_SPECIAL13:
            return wxT("Special 13");
        case WXK_SPECIAL14:
            return wxT("Special 14");
        case WXK_SPECIAL15:
            return wxT("Special 15");
        case WXK_SPECIAL16:
            return wxT("Special 16");
        case WXK_SPECIAL17:
            return wxT("Special 17");
        case WXK_SPECIAL18:
            return wxT("Special 18");
        case WXK_SPECIAL19:
            return wxT("Special 19");
        case WXK_SPECIAL20:
            return wxT("Special 20");
        case WXK_BROWSER_BACK:
            return wxT("Browser Back");
        case WXK_BROWSER_FORWARD:
            return wxT("Browser Forward");
        case WXK_BROWSER_REFRESH:
            return wxT("Browser Refresh");
        case WXK_BROWSER_STOP:
            return wxT("Browser Stop");
        case WXK_BROWSER_SEARCH:
            return wxT("Browser Search");
        case WXK_BROWSER_FAVORITES:
            return wxT("Browser Favorites");
        case WXK_BROWSER_HOME:
            return wxT("Browser Home");
        case WXK_VOLUME_MUTE:
            return wxT("Mute");
        case WXK_VOLUME_DOWN:
            return wxT("Volume Down");
        case WXK_VOLUME_UP:
            return wxT("Volume Up");
        case WXK_MEDIA_NEXT_TRACK:
            return wxT("Next Track");
        case WXK_MEDIA_PREV_TRACK:
            return wxT("Previous Track");
        case WXK_MEDIA_STOP:
            return wxT("Stop Media");
        case WXK_MEDIA_PLAY_PAUSE:
            return wxT("Play/Pause Media");
        case WXK_LAUNCH_MAIL:
            return wxT("Launch Mail");
        case WXK_LAUNCH_0:
            return wxT("Launch 0");
        case WXK_LAUNCH_1:
            return wxT("Launch 1");
        case WXK_LAUNCH_2:
            return wxT("Launch 2");
        case WXK_LAUNCH_3:
            return wxT("Launch 3");
        case WXK_LAUNCH_4:
            return wxT("Launch 4");
        case WXK_LAUNCH_5:
            return wxT("Launch 5");
        case WXK_LAUNCH_6:
            return wxT("Launch 6");
        case WXK_LAUNCH_7:
            return wxT("Launch 7");
        case WXK_LAUNCH_8:
            return wxT("Launch 8");
        case WXK_LAUNCH_9:
            return wxT("Launch 9");
        case WXK_LAUNCH_A:
            return wxT("Launch 10");
        case WXK_LAUNCH_B:
            return wxT("Launch 11");
        case WXK_LAUNCH_C:
            return wxT("Launch 12");
        case WXK_LAUNCH_D:
            return wxT("Launch 13");
        case WXK_LAUNCH_E:
            return wxT("Launch 14");
        case WXK_LAUNCH_F:
            return wxT("Launch 15");
    };
    return wxString::Format("Unknown (%d)", key);
}

// ----------------------------------------------------------------------------
// ButtonActionLine
// ----------------------------------------------------------------------------

ButtonActionLine::ButtonActionLine(wxWindow* parent, wxWindowID winid,
                                   const wxString& label, const wxPoint& pos,
                                   const wxSize& size)
    : wxPanel(parent, winid, pos, size),
      m_ownModifierNumber(-1),
      m_mouseEnterCount(0),
      m_mouseInside(false) {
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));
    SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT));
    m_labelControl = new wxStaticText(this, wxID_ANY, label);
    // m_labelControl->SetSizeHints(wxSize(100, -1));
    m_keystrokeDisplayControl =
        new wxStaticText(this, wxID_ANY, wxT("(none)"), wxDefaultPosition,
                         wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
    m_keystrokeDisplayControl->GetAlignment();
    // m_keystrokeDisplayControl->SetSizeHints(wxSize(100, -1));
    m_btnEdit =
        new wxBitmapButton(this, wxID_ANY, wxNullBitmap, wxDefaultPosition,
                           wxDefaultSize, wxBU_AUTODRAW | 0);
    m_btnEdit->SetBitmap(wxArtProvider::GetBitmap(wxART_EDIT, wxART_BUTTON));
    m_btnClear =
        new wxBitmapButton(this, wxID_ANY, wxNullBitmap, wxDefaultPosition,
                           wxDefaultSize, wxBU_AUTODRAW | 0);
    m_btnClear->SetBitmap(wxArtProvider::GetBitmap(wxART_DELETE, wxART_BUTTON));
    // Layout
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(m_labelControl, wxSizerFlags(3).Center().Border(wxALL, 4));
    sizer->Add(m_keystrokeDisplayControl,
               wxSizerFlags(1).Center().Border(wxALL, 4));
    sizer->Add(m_btnEdit, wxSizerFlags().Center().Border(wxALL, 4));
    sizer->Add(m_btnClear, wxSizerFlags().Center().Border(wxALL, 4));
    SetSizer(sizer);
    // Events
    SetupHoverEvents(m_labelControl);
    SetupHoverEvents(m_keystrokeDisplayControl);
    SetupHoverEvents(m_btnEdit);
    SetupHoverEvents(m_btnClear);
    SetupHoverEvents(this);
    Bind(wxEVT_IDLE, &ButtonActionLine::OnIdle, this);
    m_btnClear->Bind(wxEVT_BUTTON, &ButtonActionLine::OnBtnClear, this);
    m_btnEdit->Bind(wxEVT_BUTTON, &ButtonActionLine::OnBtnEdit, this);
}

void ButtonActionLine::SetLabel(const wxString& label) {
    m_labelControl->SetLabel(label);
}

void ButtonActionLine::SetOwnModifierNumber(int number) {
    m_ownModifierNumber = number;
}

void ButtonActionLine::SetKeystroke(const Keystroke& keystroke) {
    m_keystroke = keystroke;
    if (keystroke == KEYSTROKE_NONE) {
        m_keystrokeDisplayControl->SetLabel(wxT("(none)"));
    } else {
        wxString keystrokeString;
        if (keystroke.modifiers & Keystroke::Control) {
            keystrokeString += s_controlLabel;
        }
        if (keystroke.modifiers & Keystroke::Alt) {
            keystrokeString += s_altLabel;
        }
        if (keystroke.modifiers & Keystroke::Shift) {
            keystrokeString += s_shiftLabel;
        }
        if (keystroke.modifiers & Keystroke::Command) {
            keystrokeString += s_commandLabel;
        }
        keystrokeString += GetKeyName(keystroke.key, keystroke.isCharacter);
        m_keystrokeDisplayControl->SetLabel(keystrokeString);
    }
    Layout();
}

void ButtonActionLine::SetupHoverEvents(wxWindow* window) {
    window->Bind(wxEVT_ENTER_WINDOW, &ButtonActionLine::OnMouseEnter, this);
    window->Bind(wxEVT_LEAVE_WINDOW, &ButtonActionLine::OnMouseLeave, this);
}

void ButtonActionLine::OnMouseEnter(wxMouseEvent& event) {
    m_mouseEnterCount++;
}

void ButtonActionLine::OnMouseLeave(wxMouseEvent& event) {
    m_mouseEnterCount--;
}

void ButtonActionLine::OnIdle(wxIdleEvent& event) {
    bool newMouseInside = m_mouseEnterCount > 0;
    if (newMouseInside != m_mouseInside) {
        m_mouseInside = newMouseInside;
        SetBackgroundColour(
            m_mouseInside
                ? wxSystemSettings::GetColour(wxSYS_COLOUR_MENUHILIGHT)
                : wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));
        SetForegroundColour(
            m_mouseInside
                ? wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT)
                : wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT));
        Refresh();
    }
}

void ButtonActionLine::OnBtnEdit(wxCommandEvent& event) {
}

void ButtonActionLine::OnBtnClear(wxCommandEvent& event) {
    SetKeystrokeEvent newEvent{EVT_SET_KEYSTROKE, GetId()};
    SetKeystroke(KEYSTROKE_NONE);
    newEvent.SetKeystroke(m_keystroke);
    newEvent.SetModifierNumber(m_ownModifierNumber);
    AddPendingEvent(newEvent);
}

// ----------------------------------------------------------------------------
// ButtonActionPanel
// ----------------------------------------------------------------------------

ButtonActionPanel::ButtonActionPanel(wxWindow* parent, wxWindowID winid,
                                     const wxString& label,
                                     std::vector<ButtonInfo> availableModifiers,
                                     const wxPoint& pos, const wxSize& size)
    : wxPanel(parent, winid, pos, size),
      m_availableModifiers(availableModifiers),
      m_buttonIndex(-1) {
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));
    SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT));
    // Creates the main line
    m_mainLine = new ButtonActionLine(this, wxID_ANY, label);
    m_mainLine->SetOwnModifierNumber(-1);
    // Creates the modifier lines
    for (auto& modifier : m_availableModifiers) {
        ButtonActionLine* modifierLine = new ButtonActionLine(
            this, wxID_ANY, label + wxT(" + ") + modifier.label);
        modifierLine->SetOwnModifierNumber(modifier.number);
        m_modifierLines.push_back(modifierLine);
    }
    // Layout
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_mainLine, wxSizerFlags().Expand());
    for (auto& modifierLine : m_modifierLines) {
        sizer->Add(modifierLine, wxSizerFlags().Expand());
    }
    SetSizer(sizer);
    Bind(EVT_SET_KEYSTROKE, &ButtonActionPanel::OnSetKeystroke, this);
}

void ButtonActionPanel::SetLabel(const wxString& label) {
    m_mainLine->SetLabel(label);
    for (size_t i = 0; i < m_modifierLines.size(); i++) {
        m_modifierLines[i]->SetLabel(label + wxT(" + ") +
                                     m_availableModifiers[i].label);
    }
}

void ButtonActionPanel::SetAction(const Action& action) {
    m_action = action;
    m_mainLine->SetKeystroke(action.GetKeystroke());
    for (size_t i = 0; i < m_modifierLines.size(); i++) {
        m_modifierLines[i]->SetKeystroke(
            action.GetModifiedKeystroke(m_availableModifiers[i].number));
    }
}

void ButtonActionPanel::OnSetKeystroke(SetKeystrokeEvent& event) {
    wxLogDebug("ButtonActionPanel::OnSetKeystroke - %d",
               event.GetModifierNumber());
    if (event.GetModifierNumber() < 0) {
        m_action.SetKeystroke(event.GetKeystroke());
    } else {
        m_action.SetModifiedKeystroke(event.GetModifierNumber(),
                                      event.GetKeystroke());
    }
    SetActionEvent newEvent{EVT_SET_ACTION, GetId()};
    newEvent.SetAction(m_action);
    newEvent.SetIndex(m_buttonIndex);
    AddPendingEvent(newEvent);
}

// ----------------------------------------------------------------------------
// ButtonSetupPanel
// ----------------------------------------------------------------------------

ButtonSetupPanel::ButtonSetupPanel(
    wxWindow* parent, wxWindowID winid,
    const ControllerDescription& deviceDescription, const wxPoint& pos,
    const wxSize& size)
    : wxScrolled<wxPanel>(parent, winid, pos, size,
                          wxScrolledWindowStyle | wxTAB_TRAVERSAL),
      m_deviceDescription(deviceDescription) {
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
    SetScrollRate(5, 5);
    // Computes the list of available modifiers by joining the list of
    // encoder buttons and regular buttons.
    auto allAvailableModifiers = m_deviceDescription.buttons;
    for (auto& encoder : m_deviceDescription.encoders) {
        if (encoder.buttonNumber < 0) {
            continue;
        }
        struct ButtonInfo encoderButton = {
            encoder.buttonNumber,
            encoder.label + wxT(" (pressed)"),
        };
        allAvailableModifiers.push_back(encoderButton);
    }
    // Layouts while creating the controls
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    // Creates the action panels for encoders
    for (size_t idx = 0; idx < m_deviceDescription.encoders.size(); idx++) {
        wxStaticText* label =
            CreateSectionLabel(m_deviceDescription.encoders[idx].label);
        EncoderControls encoderControls =
            CreateEncoderActionPanel(idx, allAvailableModifiers);
        // Layout
        sizer->Add(label, wxSizerFlags().Border(wxALL, 8).Expand());
        sizer->Add(encoderControls.cw, wxSizerFlags().Expand());
        sizer->Add(encoderControls.ccw, wxSizerFlags().Expand());
        sizer->Add(encoderControls.button, wxSizerFlags().Expand());

        m_encoderActionPanels.push_back(encoderControls);
        m_sectionLabels.push_back(label);
    }
    // Creates the action panels for all buttons
    wxStaticText* buttonSectionLabel = CreateSectionLabel(wxT("Buttons"));
    sizer->Add(buttonSectionLabel, wxSizerFlags().Border(wxALL, 8).Expand());
    m_sectionLabels.push_back(buttonSectionLabel);
    for (size_t idx = 0; idx < m_deviceDescription.buttons.size(); idx++) {
        ButtonActionPanel* buttonPanel =
            CreateButtonActionPanel(idx, allAvailableModifiers);
        // Layout
        sizer->Add(buttonPanel, wxSizerFlags().Expand());

        m_buttonActionPanels.push_back(buttonPanel);
    }

    SetSizer(sizer);
}

void ButtonSetupPanel::SetActionsTemplate(
    const ActionsTemplate& actionsTemplate) {
    m_editingTemplate = actionsTemplate;
    for (size_t i = 0; i < m_encoderActionPanels.size(); i++) {
        auto buttonNumber = m_deviceDescription.encoders[i].buttonNumber;
        auto encoderNumber = m_deviceDescription.encoders[i].number;
        m_encoderActionPanels[i].button->SetAction(
            actionsTemplate.buttonActions[buttonNumber]);
        m_encoderActionPanels[i].cw->SetAction(
            actionsTemplate.encoderActions[encoderNumber][ENCODER_CW]);
        m_encoderActionPanels[i].ccw->SetAction(
            actionsTemplate.encoderActions[encoderNumber][ENCODER_CCW]);
    }
    for (size_t i = 0; i < m_buttonActionPanels.size(); i++) {
        auto buttonNumber = m_deviceDescription.buttons[i].number;
        m_buttonActionPanels[i]->SetAction(
            actionsTemplate.buttonActions[buttonNumber]);
    }
}

void ButtonSetupPanel::OnSetButtonAction(SetActionEvent& event) {
    if (event.GetIndex() < 0) {
        return;
    }
    auto buttonNumber = m_deviceDescription.buttons[event.GetIndex()].number;
    wxLogDebug("ButtonSetupPanel::OnSetButtonAction - %d", buttonNumber);
    m_editingTemplate.buttonActions[buttonNumber] = event.GetAction();
}

void ButtonSetupPanel::OnSetEncoderCwAction(SetActionEvent& event) {
    if (event.GetIndex() < 0) {
        return;
    }
    auto encoderNumber = m_deviceDescription.encoders[event.GetIndex()].number;
    wxLogDebug("ButtonSetupPanel::OnSetEncoderCwAction - %d", encoderNumber);
    m_editingTemplate.encoderActions[encoderNumber][ENCODER_CW] =
        event.GetAction();
}

void ButtonSetupPanel::OnSetEncoderCcwAction(SetActionEvent& event) {
    if (event.GetIndex() < 0) {
        return;
    }
    auto encoderNumber = m_deviceDescription.encoders[event.GetIndex()].number;
    wxLogDebug("ButtonSetupPanel::OnSetEncoderCcwAction - %d", encoderNumber);
    m_editingTemplate.encoderActions[encoderNumber][ENCODER_CCW] =
        event.GetAction();
}

void ButtonSetupPanel::OnSetEncoderButtonAction(SetActionEvent& event) {
    if (event.GetIndex() < 0) {
        return;
    }
    auto buttonNumber =
        m_deviceDescription.encoders[event.GetIndex()].buttonNumber;
    wxLogDebug("ButtonSetupPanel::OnSetEncoderButtonAction - %d", buttonNumber);
    m_editingTemplate.buttonActions[buttonNumber] = event.GetAction();
}

wxStaticText* ButtonSetupPanel::CreateSectionLabel(const wxString& label) {
    wxStaticText* sectionLabel = new wxStaticText(this, wxID_ANY, label);
    sectionLabel->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(),
                                 wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                                 wxFONTWEIGHT_BOLD, false, wxEmptyString));
    return sectionLabel;
}

ButtonActionPanel* ButtonSetupPanel::CreateButtonActionPanel(
    int buttonNumber, std::vector<ButtonInfo> availableModifiers) {
    const auto& button = m_deviceDescription.buttons[buttonNumber];
    auto remove_iterator =
        std::remove_if(availableModifiers.begin(), availableModifiers.end(),
                       [button](ButtonInfo& modifierButton) {
                           return modifierButton.number == button.number;
                       });
    availableModifiers.erase(remove_iterator, availableModifiers.end());
    // Creates the action panel for the button
    auto result =
        new ButtonActionPanel(this, wxID_ANY, button.label, availableModifiers);
    // Events
    result->SetButtonIndex(buttonNumber);
    result->Bind(EVT_SET_ACTION, &ButtonSetupPanel::OnSetButtonAction, this);
    return result;
}

ButtonSetupPanel::EncoderControls ButtonSetupPanel::CreateEncoderActionPanel(
    int encoderNumber, std::vector<ButtonInfo> availableModifiers) {
    EncoderControls encoderControls;
    const auto& encoder = m_deviceDescription.encoders[encoderNumber];
    // Creates the action panels for turning the encoder
    encoderControls.cw = new ButtonActionPanel(
        this, wxID_ANY, wxT("Turn clockwise"), availableModifiers);
    encoderControls.ccw = new ButtonActionPanel(
        this, wxID_ANY, wxT("Turn counter-clockwise"), availableModifiers);
    // Computes the list of available modifiers for the encoder button
    auto remove_iterator =
        std::remove_if(availableModifiers.begin(), availableModifiers.end(),
                       [encoder](ButtonInfo& modifierButton) {
                           return modifierButton.number == encoder.buttonNumber;
                       });
    availableModifiers.erase(remove_iterator, availableModifiers.end());
    // Creates the action panels for pressing the encoder
    encoderControls.button =
        new ButtonActionPanel(this, wxID_ANY, wxT("Press"), availableModifiers);

    // Events
    encoderControls.cw->SetButtonIndex(encoderNumber);
    encoderControls.ccw->SetButtonIndex(encoderNumber);
    encoderControls.button->SetButtonIndex(encoderNumber);
    encoderControls.cw->Bind(EVT_SET_ACTION,
                             &ButtonSetupPanel::OnSetEncoderCwAction, this);
    encoderControls.ccw->Bind(EVT_SET_ACTION,
                              &ButtonSetupPanel::OnSetEncoderCcwAction, this);
    encoderControls.button->Bind(
        EVT_SET_ACTION, &ButtonSetupPanel::OnSetEncoderButtonAction, this);
    return encoderControls;
}
