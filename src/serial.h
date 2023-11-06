#ifndef __VECTRL_SERIAL_H__
#define __VECTRL_SERIAL_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_THREADS
    #error "This sample requires thread support!"
#endif // wxUSE_THREADS

#include "wx/thread.h"
#include <atomic>

class SerialPort {
public:
    SerialPort(wxString portName);
    ~SerialPort();

    bool Open();
    void Close();
    void Write(uint8_t *data, size_t size);
    size_t Read(uint8_t *data, size_t size);
private:
    wxString m_portName;
    #ifdef _WIN32
    void *m_handle;
    #else
    int m_handle;
    #endif
};

class SerialThread : public wxThread
{
public:
    SerialThread(wxFrame *frame, wxString portName);

    virtual void *Entry() override;

    virtual void OnExit() override;

    void Terminate();

private:
    wxFrame *m_frame;
    uint8_t m_buttons;
    SerialPort m_serialPort;
    std::atomic<bool> m_terminate;
};

#endif