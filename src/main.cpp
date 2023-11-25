#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_THREADS
    #error "This sample requires thread support!"
#endif // wxUSE_THREADS

#include "serial.h"
#include "actions.h"

class ControllerApp : public wxApp
{
public:
    ControllerApp();
    virtual ~ControllerApp(){}

    virtual bool OnInit() override;
};

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

private:
    wxTextCtrl *m_txtctrl;
    wxLog *m_oldLogger;
    wxCriticalSection m_critsect;
    SerialThread *m_serialThread;
    Actions m_actions;
    ActionsTemplate m_current_template;
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    App_Quit = wxID_EXIT,
    App_About = wxID_ABOUT,
    App_StartWatching = 101,
    App_StopWatching,
};

// ----------------------------------------------------------------------------
// Event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(App_Quit,  MainFrame::OnQuit)
    EVT_MENU(App_About, MainFrame::OnAbout)
    EVT_MENU(App_StartWatching,  MainFrame::OnStartWatching)
    EVT_MENU(App_StopWatching, MainFrame::OnStopWatching)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(ControllerApp);

// ============================================================================
// Implementation
// ============================================================================

// ----------------------------------------------------------------------------
// ControllerApp
// ----------------------------------------------------------------------------


ControllerApp::ControllerApp() {
}

bool ControllerApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    MainFrame *frame = new MainFrame("Video Editing Controller Prototype");
    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// MainFrame
// ----------------------------------------------------------------------------

MainFrame::MainFrame(const wxString& title)
       : wxFrame(nullptr, wxID_ANY, title), m_serialThread(nullptr)
{
    m_current_template.button_actions[0].SetKeystroke(Keystroke(VK_OEM_2, false, false, false));
    //m_current_template.button_actions[1].SetKeystroke(Keystroke(VK_SPACE, false, false, false));
    m_current_template.button_actions[2].SetKeystroke(Keystroke(VK_OEM_3, false, false, false));
    m_current_template.button_actions[3].SetKeystroke(Keystroke(VK_SPACE, false, false, false));
    m_current_template.encoder_actions[0][ENCODER_CW].SetKeystroke(Keystroke(VK_RIGHT, false, false, false));
    m_current_template.encoder_actions[0][ENCODER_CW].SetModifiedKeystroke(1, Keystroke(VK_RIGHT, false, false, true));
    m_current_template.encoder_actions[0][ENCODER_CCW].SetKeystroke(Keystroke(VK_LEFT, false, false, false));
    m_current_template.encoder_actions[0][ENCODER_CCW].SetModifiedKeystroke(1, Keystroke(VK_LEFT, false, false, true));

    m_actions.LoadTemplate(m_current_template);
    m_oldLogger = wxLog::GetActiveTarget();
    //SetIcon(wxICON(sample));

#if wxUSE_MENUBAR
    wxMenu *fileMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(App_About, "&About\tF1", "Show about dialog");
    fileMenu->Append(App_StartWatching, "Start", "Start listening to the serial port");
    fileMenu->Append(App_StopWatching, "Stop", "Stop listening to the serial port");
    fileMenu->AppendSeparator();
    fileMenu->Append(App_Quit, "E&xit\tAlt-X", "Quit this program");
    
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    
    SetMenuBar(menuBar);
#else // !wxUSE_MENUBAR
    // If menus are not available add a button to access the about box
    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* aboutBtn = new wxButton(this, wxID_ANY, "About...");
    aboutBtn->Bind(wxEVT_BUTTON, &MyFrame::OnAbout, this);
    sizer->Add(aboutBtn, wxSizerFlags().Center());
    SetSizer(sizer);
#endif // wxUSE_MENUBAR/!wxUSE_MENUBAR

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    SetStatusText("Video Editing Controller");
#endif // wxUSE_STATUSBAR
    wxTextCtrl *header = new wxTextCtrl(this, wxID_ANY, "",
                                        wxDefaultPosition, wxDefaultSize,
                                        wxTE_READONLY);
    DoLogLine(header, "  Time", " Thread", "Message");
    m_txtctrl = new wxTextCtrl(this, wxID_ANY, "",
                               wxDefaultPosition, wxDefaultSize,
                               wxTE_MULTILINE | wxTE_READONLY);
    wxLog::SetActiveTarget(this);

    // use fixed width font to align output in nice columns
    wxFont font(wxFontInfo().Family(wxFONTFAMILY_TELETYPE));
    header->SetFont(font);
    m_txtctrl->SetFont(font);

    m_txtctrl->SetFocus();

    // layout and show the frame
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(header, wxSizerFlags().Expand());
    sizer->Add(m_txtctrl, wxSizerFlags(1).Expand());
    SetSizer(sizer);

    SetSize(600, 350);
}

MainFrame::~MainFrame()
{
    wxLog::SetActiveTarget(m_oldLogger);
    {
        wxCriticalSectionLocker locker(m_critsect);
        if ( !m_serialThread )
            return;
        m_serialThread->Terminate();
        m_serialThread->Wait();
        delete m_serialThread;
        m_serialThread = nullptr;
    }
}

// logging functions
void
MainFrame::DoLogLine(wxTextCtrl *text,
                   const wxString& timestr,
                   const wxString& threadstr,
                   const wxString& msg)
{
    text->AppendText(wxString::Format("%9s %10s %s", timestr, threadstr, msg));
}

void
MainFrame::DoLogRecord(wxLogLevel level,
                     const wxString& msg,
                     const wxLogRecordInfo& info)
{
    // let the default GUI logger treat warnings and errors as they should be
    // more noticeable than just another line in the log window and also trace
    // messages as there may be too many of them
    if ( level <= wxLOG_Warning || level == wxLOG_Trace )
    {
        m_oldLogger->LogRecord(level, msg, info);
        return;
    }

    DoLogLine
    (
        m_txtctrl,
        wxDateTime(wxLongLong(info.timestampMS)).FormatISOTime(),
        info.threadId == wxThread::GetMainId()
            ? wxString("main")
            : wxString::Format("%lx", info.threadId),
        msg + "\n"
    );
}


// event handlers

void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                 (
                    "Video Editing Controller Prototype\n"
                    "\n"
                    "Running under %s.",
                    wxVERSION_STRING,
                    wxGetOsDescription()
                 ),
                 "About Video Editing Controller Prototype",
                 wxOK | wxICON_INFORMATION,
                 this);
}

void MainFrame::OnStartWatching(wxCommandEvent& WXUNUSED(event))
{
        wxCriticalSectionLocker locker(m_critsect);
        if ( m_serialThread )
            return;
        m_serialThread = new SerialThread(this, "\\\\.\\COM4", &m_actions);
        m_serialThread->Run();
}

void MainFrame::OnStopWatching(wxCommandEvent& WXUNUSED(event))
{
        wxCriticalSectionLocker locker(m_critsect);
        if ( !m_serialThread )
            return;
        m_serialThread->Terminate();
        m_serialThread->Wait();
        delete m_serialThread;
        m_serialThread = nullptr;
}

