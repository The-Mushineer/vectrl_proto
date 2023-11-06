#include "serial.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

#define NUM_BUTTONS 3
#ifdef _WIN32
const int button_keys[NUM_BUTTONS] = { VK_RIGHT, VK_SPACE, VK_LEFT };
#endif

//----------------------------------------------------------------------
// SerialPort
//----------------------------------------------------------------------

SerialPort::SerialPort(wxString portName)
    : m_portName(portName)
{
}

SerialPort::~SerialPort()
{
    Close();
}

bool SerialPort::Open()
{
    #ifdef _WIN32
    m_handle = CreateFile(m_portName.c_str(),
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          NULL,
                          OPEN_EXISTING,
                          0,
                          NULL);
    if (m_handle == INVALID_HANDLE_VALUE) {
        return false;
    }
    DCB dcb;
    memset(&dcb, 0, sizeof(dcb));
    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(m_handle, &dcb)) {
        Close();
        return false;
    }
    dcb.BaudRate = CBR_115200;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    if (!SetCommState(m_handle, &dcb)) {
        Close();
        return false;
    }
    COMMTIMEOUTS timeouts;
    memset(&timeouts, 0, sizeof(timeouts));
    timeouts.ReadIntervalTimeout = 100;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    if (!SetCommTimeouts(m_handle, &timeouts)) {
        Close();
        return false;
    }
    #else
    m_handle = open(m_portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (m_handle == -1) {
        return false;
    }
    fcntl(m_handle, F_SETFL, 0);
    struct termios options;
    tcgetattr(m_handle, &options);
    cfsetispeed(&options, B115200);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_lflag &= ~ICANON; /* Set non-canonical mode */
    options.c_cflag |= CS8;
    options.c_cc[VTIME] = 1; /* Set timeout of 0.1 seconds */
    tcsetattr(m_handle, TCSANOW, &options);
    #endif
    return true;
}

void SerialPort::Close()
{
    #ifdef _WIN32
    if (m_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
    }
    #else
    if (m_handle != -1) {
        close(m_handle);
        m_handle = -1;
    }
    #endif
}

void SerialPort::Write(uint8_t *data, size_t size)
{
    #ifdef _WIN32
    DWORD written;
    WriteFile(m_handle, data, size, &written, NULL);
    #else
    write(m_handle, data, size);
    #endif
}

size_t SerialPort::Read(uint8_t *data, size_t size)
{
    #ifdef _WIN32
    DWORD read;
    ReadFile(m_handle, data, size, &read, NULL);
    return read;
    #else
    return read(m_handle, data, size);
    #endif
}

//----------------------------------------------------------------------
// SerialThread
//----------------------------------------------------------------------

SerialThread::SerialThread(wxFrame *frame, wxString portName)
    : wxThread(wxTHREAD_JOINABLE),
      m_frame(frame),
      m_buttons(0),
      m_serialPort(portName)
{
}

void *SerialThread::Entry()
{
    if (!m_serialPort.Open()) {
        wxLogError("Failed to open serial port");
        return NULL;
    }
    while (!TestDestroy() && !m_terminate) {
        uint8_t data[1];
        size_t read = m_serialPort.Read(data, sizeof(data));
        if (read == 0) {
            continue;
        }
        uint8_t changed_buttons = data[0] ^ m_buttons;
        m_buttons = data[0];
        if (changed_buttons == 0) {
            continue;
        }
        for(int i=0; i < NUM_BUTTONS; i++) {
            if (changed_buttons & (1 << i)) {
                wxLogMessage("Button %d: %s", i+1, (data[0] & (1 << i)) ? "pressed" : "released");
#ifdef _WIN32
                INPUT input;
                memset(&input, 0, sizeof(input));
                input.type = INPUT_KEYBOARD;
                input.ki.wVk = button_keys[i];
                input.ki.dwFlags = (data[0] & (1 << i)) ? 0 : KEYEVENTF_KEYUP;
                SendInput(1, &input, sizeof(input));
#endif
            }
        }
    }
    return NULL;
}

void SerialThread::OnExit()
{
    m_serialPort.Close();
}

void SerialThread::Terminate()
{
    m_terminate = true;
}