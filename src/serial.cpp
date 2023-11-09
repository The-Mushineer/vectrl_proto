#include "serial.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

#define NUM_BUTTONS 4
#define NUM_ENCODERS 1
#ifdef _WIN32
const WORD button_keys[NUM_BUTTONS] = { /*';'*/VK_OEM_2, VK_SPACE, /*'\''*/VK_OEM_3, VK_SPACE };
const WORD encoder_keys[NUM_ENCODERS][2] = { { VK_RIGHT, VK_LEFT } };
#endif

//----------------------------------------------------------------------

static void issueKey(int button_number, bool pressed) {
#ifdef _WIN32
    INPUT input;
    memset(&input, 0, sizeof(input));
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = button_keys[button_number];
    input.ki.dwFlags = pressed ? 0 : KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(input));
#endif
}

static void issueEncoder(int encoder_number, int8_t count) {
#ifdef _WIN32
    INPUT input[2];
    WORD key = count > 0 ? encoder_keys[encoder_number][0] : encoder_keys[encoder_number][1];
    uint8_t abs_count = count > 0 ? count : -count;
    memset(input, 0, sizeof(input));
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = key;
    input[0].ki.dwFlags = 0;
    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wVk = key;
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;
    for (uint8_t i = 0; i < abs_count; i++) {
        wxLogMessage("Sending input %d", key);
        SendInput(1, input, sizeof(input[0]));
        SendInput(1, input + 1, sizeof(input[1]));
    }
#endif
}

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
    DWORD read = 0;
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

const uint8_t START_PACKET = 0xE9;
enum MessageType {
  MSG_BTN_DOWN = 1,
  MSG_BTN_UP = 2,
  MSG_TURN = 3,
};

void *SerialThread::Entry()
{
    if (!m_serialPort.Open()) {
        wxLogError("Failed to open serial port");
        return NULL;
    }
    while (!TestDestroy() && !m_terminate) {
        uint8_t data[3];
        uint8_t checksum = 0;
        // Checks if a packet is available
        size_t read = m_serialPort.Read(data, 1);
        if (read != 1 || data[0] != START_PACKET) {
            continue;
        }
        checksum += data[0];
        // Reads the message type
        read = m_serialPort.Read(data, 1);
        if (read != 1) {
            continue;
        }
        uint8_t messageType = data[0];
        checksum += data[0];
        switch (messageType) {
            case MSG_BTN_DOWN: {
                // Button was pressed, reads the rest of the message
                read = m_serialPort.Read(data, 2);
                if (read != 2) {
                    continue;
                }
                checksum += data[0];
                // Checks the checksum
                if (checksum != data[1]) {
                    continue;
                }
                // Issues the key
                issueKey(data[0], true);
                m_buttons |= (1 << data[0]);
                wxLogMessage("Button %d: pressed", data[0]);
                break;
            }
            case MSG_BTN_UP: {
                // Button was released, reads the rest of the message
                read = m_serialPort.Read(data, 2);
                if (read != 2) {
                    continue;
                }
                checksum += data[0];
                // Checks the checksum
                if (checksum != data[1]) {
                    continue;
                }
                // Issues the key
                issueKey(data[0], false);
                m_buttons &= ~(1 << data[0]);
                wxLogMessage("Button %d: released", data[0]);
                break;
            }
            case MSG_TURN: {
                // Button was released, reads the rest of the message
                read = m_serialPort.Read(data, 3);
                if (read != 3) {
                    continue;
                }
                checksum += data[0] + data[1];
                // Checks the checksum
                if (checksum != data[2]) {
                    continue;
                }
                // Issues the key
                int8_t count = data[1];
                issueEncoder(data[0], count);
                wxLogMessage("Dial %d: %d", data[0], count);
                break;
            }
            default:
                wxLogError("Unknown message type: %d", messageType);
                break;
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