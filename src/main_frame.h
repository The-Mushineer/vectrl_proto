#ifndef __VECTRL_MAIN_FRAME_H__
#define __VECTRL_MAIN_FRAME_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "usb.h"
#include "actions.h"
#include "config_loader.h"
#include "ui_common.h"

class MainFrame : public wxFrame, public wxLog
{
public:
    MainFrame(const wxString& title);
    virtual ~MainFrame();

protected:
    virtual void DoLogRecord(wxLogLevel level,
                             const wxString& msg,
                             const wxLogRecordInfo& info) override;

private:
    void DoLogLine(wxTextCtrl *text,
                    const wxString& timestr,
                    const wxString& threadstr,
                    const wxString& msg);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnStartWatching(wxCommandEvent& event);
    void OnStopWatching(wxCommandEvent& event);
    void OnThreadMessage(wxThreadEvent& event);

private:
    wxTextCtrl *m_txtctrl;
    wxLog *m_oldLogger;
    wxCriticalSection m_critsect;
    USBManagerThread *m_usbThread;
    Actions m_actions;
    ActionsTemplate m_current_template;
    ConfigLoader m_configLoader;
    wxDECLARE_EVENT_TABLE();
};

#endif