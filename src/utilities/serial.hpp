#pragma once

// =============================================================================
// serial.hpp — cross-platform serial port utility
// Platforms : Linux, macOS, Windows (XP+)
// Standard  : C++17
//
// Provides non-blocking open/read/write/close over a serial port.
// Reads never block — they drain whatever the OS receive buffer holds at the
// instant of the call and return immediately.  This lets the idyl scheduler
// poll the port at a dt interval without stalling.
// =============================================================================

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <algorithm>

// ── Platform detection ────────────────────────────────────────────────────────

#if defined(_WIN32) || defined(_WIN64)
#   define IDYL_SERIAL_WINDOWS
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif
#   ifndef NOMINMAX
#       define NOMINMAX
#   endif
#   include <windows.h>
#else
#   define IDYL_SERIAL_POSIX
#   include <fcntl.h>
#   include <termios.h>
#   include <unistd.h>
#   include <errno.h>
#   include <sys/ioctl.h>
#endif

namespace idyl::utilities {

// =============================================================================
// serial_port
// =============================================================================

class serial_port {
public:
    serial_port() = default;
    ~serial_port() noexcept { close(); }

    // Non-copyable, movable
    serial_port(const serial_port&)            = delete;
    serial_port& operator=(const serial_port&) = delete;
    serial_port(serial_port&&)                 = default;
    serial_port& operator=(serial_port&&)      = default;

    // ── Open ──────────────────────────────────────────────────────────────────
    // Opens the named port at the given baud rate, 8N1, no flow control.
    // Returns true on success; on failure, last_error() contains a description.
    bool open(const std::string& port, int baud) noexcept {
        close();
        last_error_.clear();

#if defined(IDYL_SERIAL_POSIX)
        return posix_open(port, baud);
#else
        return win_open(port, baud);
#endif
    }

    // ── Close ─────────────────────────────────────────────────────────────────
    void close() noexcept {
#if defined(IDYL_SERIAL_POSIX)
        if (fd_ >= 0) { ::close(fd_); fd_ = -1; }
#else
        if (handle_ != INVALID_HANDLE_VALUE) {
            CloseHandle(handle_);
            handle_ = INVALID_HANDLE_VALUE;
        }
#endif
    }

    bool is_open() const noexcept {
#if defined(IDYL_SERIAL_POSIX)
        return fd_ >= 0;
#else
        return handle_ != INVALID_HANDLE_VALUE;
#endif
    }

    // ── Write ─────────────────────────────────────────────────────────────────
    // Writes all bytes in data.  Blocks only as long as the kernel TX buffer
    // has room (typically completes immediately for short messages).
    // Returns bytes written, -1 on error.
    int write(const std::string& data) noexcept {
        if (!is_open() || data.empty()) return 0;
#if defined(IDYL_SERIAL_POSIX)
        ssize_t n = ::write(fd_, data.data(), data.size());
        return static_cast<int>(n);
#else
        DWORD written = 0;
        BOOL ok = WriteFile(handle_, data.data(),
                            static_cast<DWORD>(data.size()), &written, nullptr);
        return ok ? static_cast<int>(written) : -1;
#endif
    }

    // ── Non-blocking read ─────────────────────────────────────────────────────
    // Returns whatever bytes are currently in the OS receive buffer, up to
    // max_bytes.  Never blocks.  Returns empty string when no data is available.
    std::string read_available(std::size_t max_bytes = 4096) noexcept {
        if (!is_open()) return {};

#if defined(IDYL_SERIAL_POSIX)
        // Check how many bytes are waiting
        int waiting = 0;
        if (::ioctl(fd_, TIOCINQ, &waiting) < 0 || waiting <= 0)
            return {};

        std::size_t to_read = std::min(static_cast<std::size_t>(waiting), max_bytes);
        std::string buf(to_read, '\0');
        ssize_t n = ::read(fd_, buf.data(), to_read);
        if (n <= 0) return {};
        buf.resize(static_cast<std::size_t>(n));
        return buf;

#else
        // On Windows, query bytes available via COMSTAT
        COMSTAT cs{};
        DWORD errors = 0;
        if (!ClearCommError(handle_, &errors, &cs) || cs.cbInQue == 0)
            return {};

        DWORD to_read = std::min(static_cast<DWORD>(cs.cbInQue),
                                 static_cast<DWORD>(max_bytes));
        std::string buf(to_read, '\0');
        DWORD n = 0;
        if (!ReadFile(handle_, buf.data(), to_read, &n, nullptr) || n == 0)
            return {};
        buf.resize(n);
        return buf;
#endif
    }

    // ── Diagnostics ───────────────────────────────────────────────────────────
    const std::string& last_error() const noexcept { return last_error_; }

private:
    std::string last_error_;

#if defined(IDYL_SERIAL_POSIX)
    int fd_ = -1;

    bool posix_open(const std::string& port, int baud) noexcept {
        fd_ = ::open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd_ < 0) {
            last_error_ = "open(" + port + ") failed: " + std::strerror(errno);
            return false;
        }

        struct termios tty{};
        if (::tcgetattr(fd_, &tty) != 0) {
            last_error_ = "tcgetattr failed: " + std::string(std::strerror(errno));
            ::close(fd_); fd_ = -1;
            return false;
        }

        // Apply baud rate
        speed_t speed = baud_to_speed(baud);
        ::cfsetispeed(&tty, speed);
        ::cfsetospeed(&tty, speed);

        // 8N1, no flow control, raw mode
        tty.c_cflag &= ~static_cast<tcflag_t>(PARENB | CSTOPB | CSIZE | CRTSCTS);
        tty.c_cflag |= static_cast<tcflag_t>(CS8 | CREAD | CLOCAL);
        tty.c_lflag &= ~static_cast<tcflag_t>(ICANON | ECHO | ECHOE | ECHONL | ISIG);
        tty.c_iflag &= ~static_cast<tcflag_t>(IXON | IXOFF | IXANY
                                               | IGNBRK | BRKINT | PARMRK
                                               | ISTRIP | INLCR | IGNCR | ICRNL);
        tty.c_oflag &= ~static_cast<tcflag_t>(OPOST | ONLCR);

        // Non-blocking: return immediately with whatever is available
        tty.c_cc[VMIN]  = 0;
        tty.c_cc[VTIME] = 0;

        if (::tcsetattr(fd_, TCSANOW, &tty) != 0) {
            last_error_ = "tcsetattr failed: " + std::string(std::strerror(errno));
            ::close(fd_); fd_ = -1;
            return false;
        }
        ::tcflush(fd_, TCIOFLUSH);
        return true;
    }

    static speed_t baud_to_speed(int baud) noexcept {
        switch (baud) {
            case 50:      return B50;
            case 75:      return B75;
            case 110:     return B110;
            case 134:     return B134;
            case 150:     return B150;
            case 200:     return B200;
            case 300:     return B300;
            case 600:     return B600;
            case 1200:    return B1200;
            case 1800:    return B1800;
            case 2400:    return B2400;
            case 4800:    return B4800;
            case 9600:    return B9600;
            case 19200:   return B19200;
            case 38400:   return B38400;
            case 57600:   return B57600;
            case 115200:  return B115200;
            case 230400:  return B230400;
#ifdef B460800
            case 460800:  return B460800;
#endif
#ifdef B921600
            case 921600:  return B921600;
#endif
#ifdef B1000000
            case 1000000: return B1000000;
#endif
            default:      return B9600;  // safe fallback
        }
    }

#else // Windows

    HANDLE handle_ = INVALID_HANDLE_VALUE;

    bool win_open(const std::string& port, int baud) noexcept {
        // Prefix with \\.\ so COM10+ work correctly
        std::string path = "\\\\.\\" + port;
        handle_ = CreateFileA(path.c_str(),
                              GENERIC_READ | GENERIC_WRITE,
                              0, nullptr,
                              OPEN_EXISTING,
                              0, nullptr);
        if (handle_ == INVALID_HANDLE_VALUE) {
            last_error_ = "CreateFile(" + port + ") failed (error "
                        + std::to_string(GetLastError()) + ")";
            return false;
        }

        // Configure DCB: 8N1, no flow control
        DCB dcb{};
        dcb.DCBlength = sizeof(DCB);
        if (!GetCommState(handle_, &dcb)) {
            last_error_ = "GetCommState failed";
            CloseHandle(handle_); handle_ = INVALID_HANDLE_VALUE;
            return false;
        }
        dcb.BaudRate = static_cast<DWORD>(baud);
        dcb.ByteSize = 8;
        dcb.Parity   = NOPARITY;
        dcb.StopBits = ONESTOPBIT;
        dcb.fBinary  = TRUE;
        dcb.fParity  = FALSE;
        dcb.fOutxCtsFlow = FALSE;
        dcb.fOutxDsrFlow = FALSE;
        dcb.fDsrSensitivity = FALSE;
        dcb.fOutX    = FALSE;
        dcb.fInX     = FALSE;
        dcb.fRtsControl  = RTS_CONTROL_DISABLE;
        dcb.fDtrControl  = DTR_CONTROL_DISABLE;
        if (!SetCommState(handle_, &dcb)) {
            last_error_ = "SetCommState failed";
            CloseHandle(handle_); handle_ = INVALID_HANDLE_VALUE;
            return false;
        }

        // Non-blocking reads: return immediately with whatever is available
        COMMTIMEOUTS ct{};
        ct.ReadIntervalTimeout         = MAXDWORD;
        ct.ReadTotalTimeoutMultiplier  = 0;
        ct.ReadTotalTimeoutConstant    = 0;
        ct.WriteTotalTimeoutMultiplier = 0;
        ct.WriteTotalTimeoutConstant   = 0;
        SetCommTimeouts(handle_, &ct);

        PurgeComm(handle_, PURGE_RXCLEAR | PURGE_TXCLEAR);
        return true;
    }
#endif
};

} // namespace idyl::utilities
