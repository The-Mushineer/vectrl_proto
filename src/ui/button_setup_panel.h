#ifndef __VECTRL_BUTTON_SETUP_FRAME_H__
#define __VECTRL_BUTTON_SETUP_FRAME_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/artprov.h>
#include <wx/wx.h>
#endif

#include <vector>
#include "../common/actions.h"
#include "../config/controller_description.h"

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
    const Action& GetAction() const {
        return m_action;
    };

private:
    Action m_action;
    ButtonActionLine* m_mainLine;
    std::vector<ButtonActionLine*> m_modifierLines;
    std::vector<ButtonInfo> m_availableModifiers;
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

    wxStaticText* CreateSectionLabel(const wxString& label);
    ButtonActionPanel* CreateButtonActionPanel(
        int buttonNumber, std::vector<ButtonInfo> availableModifiers);
    EncoderControls CreateEncoderActionPanel(
        int encoderNumber, std::vector<ButtonInfo> availableModifiers);
};

#endif