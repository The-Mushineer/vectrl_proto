#ifndef __VECTRL_USB_H__
#define __VECTRL_USB_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/frame.h"
#include "wx/thread.h"
#endif

#if !wxUSE_THREADS
#error "This program requires thread support!"
#endif  // wxUSE_THREADS

#include <atomic>
#include <vector>

#include "../common/actions.h"
#include "../common/thread_utils.h"

class USBManagerThread;
class USBDeviceThread;

class USBManagerThread : public wxThread {
public:
    USBManagerThread(wxFrame *frame, Actions *actions);

    virtual void *Entry() override;

    virtual void OnExit() override;
    void OnDeviceExit(const wxString &path);

    void Terminate();
    Actions *GetActions() {
        return m_actions;
    }

private:
    wxFrame *m_frame;
    Actions *m_actions;
    wxCriticalSection m_criticalSection;
    std::vector<USBDeviceThread *> m_devices;
    BooleanCondition m_terminate;

    bool MaybeAddDevice(wxString path);
    void TerminateAll();
};

class USBDeviceThread : public wxThread {
public:
    USBDeviceThread(USBManagerThread *parent, wxString path);

    virtual void *Entry() override;

    virtual void OnExit() override;

    void Terminate(bool invalidateParent = false);

    const wxString &GetPath() const {
        return m_path;
    }

private:
    USBManagerThread *m_parent;
    wxCriticalSection m_criticalSection;
    wxString m_path;
    std::atomic<bool> m_terminate;

    void IssueButton(int button, bool pressed);
    void IssueEncoder(uint8_t encoder, int8_t count);
};

#define USB_VENDOR_ID \
    0x05dc  // from
            // https://github.com/obdev/v-usb/blob/master/usbdrv/USB-IDs-for-free.txt
#define USB_PRODUCT_ID 0x16c0  // Generic HID device

#endif