#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_THREADS
#error "This sample requires thread support!"
#endif  // wxUSE_THREADS

#include "common/actions.h"
#include "ui/main_frame.h"

class ControllerApp : public wxApp {
public:
    ControllerApp();
    virtual ~ControllerApp() {
    }

    virtual bool OnInit() override;
};

// ----------------------------------------------------------------------------
// Event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(ControllerApp);

// ----------------------------------------------------------------------------
// ControllerApp
// ----------------------------------------------------------------------------

ControllerApp::ControllerApp() {
    this->SetAppName("VECtrl");
    this->SetAppDisplayName("Video Editing Controller Prototype");
    this->SetVendorName("LeonOliver");
    this->SetVendorDisplayName("Leon Oliver");
}

bool ControllerApp::OnInit() {
    if (!wxApp::OnInit())
        return false;
    initializeActionsSupport();

    MainFrame *frame = new MainFrame("Video Editing Controller Prototype");
    frame->Show(true);

    return true;
}
