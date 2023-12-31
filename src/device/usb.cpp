#include "usb.h"

#include <hidapi.h>

class USBLib {
public:
    USBLib() {
        hid_init();
    };
    ~USBLib() {
        hid_exit();
    };
};
static USBLib usbLib;

//----------------------------------------------------------------------
// USBManagerThread
//----------------------------------------------------------------------

USBManagerThread::USBManagerThread(wxFrame *frame, Actions *actions)
    : wxThread(wxTHREAD_JOINABLE),
      m_frame(frame),
      m_actions(actions),
      m_terminate() {
}

void *USBManagerThread::Entry() {
    while (!TestDestroy() && !m_terminate) {
        auto firstDevice = hid_enumerate(USB_VENDOR_ID, USB_PRODUCT_ID);
        auto currentDevice = firstDevice;
        while (currentDevice) {
            auto path = wxString(currentDevice->path);
            if (MaybeAddDevice(path)) {
                auto manufacturer =
                    wxString(currentDevice->manufacturer_string);
                auto product = wxString(currentDevice->product_string);
                wxLogMessage("Found device %s (%s) at %s", product,
                             manufacturer, path);
            }
            currentDevice = currentDevice->next;
        }
        hid_free_enumeration(firstDevice);
        // TODO: Find better way to pool for devices
        m_terminate.WaitTimeout(500);
    }
    TerminateAll();
    return nullptr;
}

bool USBManagerThread::MaybeAddDevice(wxString path) {
    wxCriticalSectionLocker lock(m_criticalSection);
    auto finder = [path](const USBDeviceThread *thread) {
        return thread->GetPath() == path;
    };
    if (std::find_if(m_devices.begin(), m_devices.end(), finder) ==
        m_devices.end()) {
        auto newItem = new USBDeviceThread(this, path);
        m_devices.push_back(newItem);
        newItem->Create();
        newItem->Run();
        return true;
    }
    return false;
}

void USBManagerThread::TerminateAll() {
    std::vector<USBDeviceThread *> devices;
    {
        wxCriticalSectionLocker lock(m_criticalSection);
        std::swap(devices, m_devices);
    }
    for (auto &device : devices) {
        device->Terminate(true);
    }
    Sleep(20);
    devices.clear();
}

void USBManagerThread::OnExit() {
}

void USBManagerThread::OnDeviceExit(const wxString &path) {
    wxCriticalSectionLocker lock(m_criticalSection);
    auto finder = [path](const USBDeviceThread *thread) {
        return thread->GetPath() == path;
    };
    auto foundItem = std::find_if(m_devices.begin(), m_devices.end(), finder);
    if (foundItem != m_devices.end()) {
        wxLogMessage("Removed device %s", path);
        m_devices.erase(foundItem);
    }
}

void USBManagerThread::Terminate() {
    m_terminate.Signal();
}

//----------------------------------------------------------------------
// USBDeviceThread
//----------------------------------------------------------------------

USBDeviceThread::USBDeviceThread(USBManagerThread *parent, wxString path)
    : wxThread(wxTHREAD_DETACHED),
      m_parent(parent),
      m_path(path),
      m_terminate(false) {
}

void *USBDeviceThread::Entry() {
    auto dev = hid_open_path(m_path.c_str());
    if (!dev) {
        return nullptr;
    }
    std::vector<int> lastPressedButtons;
    while (!TestDestroy() && !m_terminate) {
        uint8_t data[10];
        // Checks if a packet is available
        int read = hid_read_timeout(dev, data, 10, 10);

        if (read < 0) {
            // An error occurred
            wxLogError("Error reading from device %s", m_path);
            break;
        }

        if (read < 2) {
            continue;
        }

        if (data[0] != 0) {
            wxLogMessage("Encoder 0: %d", (int8_t)data[0]);
            IssueEncoder(0, (int8_t)data[0]);
        }

        for (auto it = lastPressedButtons.begin();
             it != lastPressedButtons.end();) {
            if (std::find(data + 2, data + read, *it) == data + read) {
                wxLogMessage("Button %d: released", *it);
                IssueButton(*it - 1, false);
                it = lastPressedButtons.erase(it);
            } else {
                it++;
            }
        }
        for (int i = 0; i < read - 2; i++) {
            if (data[i + 2] != 0) {
                if (std::find(lastPressedButtons.begin(),
                              lastPressedButtons.end(),
                              data[i + 2]) == lastPressedButtons.end()) {
                    wxLogMessage("Button %d: pressed", data[i + 2]);
                    lastPressedButtons.push_back(data[i + 2]);
                    IssueButton(data[i + 2] - 1, true);
                }
            }
        }
    }
    hid_close(dev);
    return nullptr;
}

void USBDeviceThread::OnExit() {
    wxCriticalSectionLocker lock(m_criticalSection);
    if (m_parent) {
        m_parent->OnDeviceExit(m_path);
    }
}

void USBDeviceThread::Terminate(bool invalidateParent) {
    if (invalidateParent) {
        wxCriticalSectionLocker lock(m_criticalSection);
        m_parent = nullptr;
    }
    m_terminate = true;
}

void USBDeviceThread::IssueButton(int button, bool pressed) {
    wxCriticalSectionLocker lock(m_criticalSection);
    if (m_parent) {
        auto actions = m_parent->GetActions();
        if (actions) {
            actions->IssueButton(button, pressed);
        }
    }
}

void USBDeviceThread::IssueEncoder(uint8_t encoder, int8_t count) {
    wxCriticalSectionLocker lock(m_criticalSection);
    if (m_parent) {
        auto actions = m_parent->GetActions();
        if (actions) {
            actions->IssueEncoder(encoder, count);
        }
    }
}