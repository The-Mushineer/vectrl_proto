#ifndef __VECTRL_BUTTON_SETUP_FRAME_H__
#define __VECTRL_BUTTON_SETUP_FRAME_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/artprov.h>
#include <wx/event.h>
#include <wx/wx.h>
#endif

#include <vector>
#include "../common/actions.h"
#include "../config/controller_description.h"

class SetKeystrokeEvent;
class SetActionEvent;
wxDECLARE_EVENT(EVT_SET_KEYSTROKE, SetKeystrokeEvent);
wxDECLARE_EVENT(EVT_SET_ACTION, SetActionEvent);

class SetKeystrokeEvent : public wxCommandEvent {
public:
    SetKeystrokeEvent(wxEventType commandType = EVT_SET_KEYSTROKE, int id = 0)
        : wxCommandEvent(commandType, id) {
    }

    // You *must* copy here the data to be transported
    SetKeystrokeEvent(const SetKeystrokeEvent& event) : wxCommandEvent(event) {
        this->SetKeystroke(event.GetKeystroke());
        this->SetModifierNumber(event.GetModifierNumber());
    }

    // Required for sending with wxPostEvent()
    wxEvent* Clone() const {
        return new SetKeystrokeEvent(*this);
    }

    const Keystroke& GetKeystroke() const {
        return m_newKeystroke;
    }
    int GetModifierNumber() const {
        return m_modifierNumber;
    }
    void SetKeystroke(const Keystroke& rp) {
        m_newKeystroke = rp;
    }
    void SetModifierNumber(int number) {
        m_modifierNumber = number;
    }

private:
    Keystroke m_newKeystroke;
    int m_modifierNumber;
};

class SetActionEvent : public wxCommandEvent {
public:
    SetActionEvent(wxEventType commandType = EVT_SET_ACTION, int id = 0)
        : wxCommandEvent(commandType, id) {
    }

    // You *must* copy here the data to be transported
    SetActionEvent(const SetActionEvent& event) : wxCommandEvent(event) {
        this->SetAction(event.GetAction());
        this->SetIndex(event.GetIndex());
    }

    // Required for sending with wxPostEvent()
    wxEvent* Clone() const {
        return new SetActionEvent(*this);
    }

    const Action& GetAction() const {
        return m_newAction;
    }
    const int GetIndex() const {
        return m_index;
    }
    void SetAction(const Action& rp) {
        m_newAction = rp;
    }
    void SetIndex(int index) {
        m_index = index;
    }

private:
    Action m_newAction;
    int m_index;
};

class ButtonActionLine : public wxPanel {
public:
    ButtonActionLine(wxWindow* parent, wxWindowID winid = wxID_ANY,
                     const wxString& label = wxEmptyString,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize);

    void SetLabel(const wxString& label);
    void SetOwnModifierNumber(int number);
    void SetKeystroke(const Keystroke& keystroke);

    const int GetOwnModifierNumber() const {
        return m_ownModifierNumber;
    };
    const Keystroke& GetKeystroke() const {
        return m_keystroke;
    };

private:
    Keystroke m_keystroke;
    wxStaticText* m_labelControl;
    wxStaticText* m_keystrokeDisplayControl;
    wxBitmapButton* m_btnEdit;
    wxBitmapButton* m_btnClear;
    int m_ownModifierNumber;
    int m_mouseEnterCount;
    bool m_mouseInside;
    void SetupHoverEvents(wxWindow* window);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnBtnEdit(wxCommandEvent& event);
    void OnBtnClear(wxCommandEvent& event);
};

class ButtonActionPanel : public wxPanel {
public:
    ButtonActionPanel(
        wxWindow* parent, wxWindowID winid = wxID_ANY,
        const wxString& label = wxEmptyString,
        std::vector<ButtonInfo> availableModifiers = std::vector<ButtonInfo>(),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize);

    void SetLabel(const wxString& label);
    void SetAction(const Action& action);
    void SetButtonIndex(int index) {
        m_buttonIndex = index;
    };
    const Action& GetAction() const {
        return m_action;
    };
    const int GetButtonIndex() const {
        return m_buttonIndex;
    };

private:
    Action m_action;
    ButtonActionLine* m_mainLine;
    std::vector<ButtonActionLine*> m_modifierLines;
    std::vector<ButtonInfo> m_availableModifiers;
    int m_buttonIndex;

    void OnSetKeystroke(SetKeystrokeEvent& event);
};

class ButtonSetupPanel : public wxScrolled<wxPanel> {
public:
    ButtonSetupPanel(wxWindow* parent, wxWindowID winid = wxID_ANY,
                     const ControllerDescription& deviceDescription =
                         ControllerDescription(),
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize);

    void SetActionsTemplate(const ActionsTemplate& actionsTemplate);
    const ActionsTemplate& GetActionsTemplate() const {
        return m_editingTemplate;
    };

private:
    struct EncoderControls {
        ButtonActionPanel* button;
        ButtonActionPanel* cw;
        ButtonActionPanel* ccw;
    };
    ControllerDescription m_deviceDescription;
    ActionsTemplate m_editingTemplate;
    std::vector<ButtonActionPanel*> m_buttonActionPanels;
    std::vector<EncoderControls> m_encoderActionPanels;
    std::vector<wxStaticText*> m_sectionLabels;

    void OnSetButtonAction(SetActionEvent& event);
    void OnSetEncoderCwAction(SetActionEvent& event);
    void OnSetEncoderCcwAction(SetActionEvent& event);
    void OnSetEncoderButtonAction(SetActionEvent& event);

    wxStaticText* CreateSectionLabel(const wxString& label);
    ButtonActionPanel* CreateButtonActionPanel(
        int buttonNumber, std::vector<ButtonInfo> availableModifiers);
    EncoderControls CreateEncoderActionPanel(
        int encoderNumber, std::vector<ButtonInfo> availableModifiers);
};

#endif