#ifndef __VECTRL_MAIN_FRAME_H__
#define __VECTRL_MAIN_FRAME_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../common/actions.h"
#include "../config/config_loader.h"
#include "../device/usb.h"
#include "button_setup_panel.h"
#include "ui_common.h"

class MainFrame : public wxFrame, public wxLog {
public:
    MainFrame(const wxString& title);
    virtual ~MainFrame();

protected:
    virtual void DoLogRecord(wxLogLevel level, const wxString& msg,
                             const wxLogRecordInfo& info) override;

private:
    void DoLogLine(wxTextCtrl* text, const wxString& timestr,
                   const wxString& threadstr, const wxString& msg);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnStartWatching(wxCommandEvent& event);
    void OnStopWatching(wxCommandEvent& event);
    void OnThreadMessage(wxThreadEvent& event);
    void OnRestoreDefaultTemplate(wxCommandEvent& event);
    void OnApplyTemplate(wxCommandEvent& event);

private:
    wxTextCtrl* m_textLogControl;
    ButtonSetupPanel* m_buttonSetupPanel;
    wxLog* m_oldLogger;
    wxCriticalSection m_criticalSection;
    USBManagerThread* m_usbThread;
    Actions m_actions;
    ActionsTemplate m_currentTemplate;
    ConfigLoader m_configLoader;
    wxDECLARE_EVENT_TABLE();
};

#endif