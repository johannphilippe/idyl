
#pragma once
 
// =============================================================================
// udp_socket.hpp  —  cross-platform UDP sender / receiver
// Platforms : Linux, macOS, Windows (XP+)
// Standard  : C++17
// =============================================================================
 
#include <cstdint>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
 
// -----------------------------------------------------------------------------
// Platform detection & socket API normalisation
// -----------------------------------------------------------------------------
 
#if defined(_WIN32) || defined(_WIN64)
#   define IDYL_PLATFORM_WINDOWS
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif
#   ifndef NOMINMAX
#       define NOMINMAX
#   endif
#   include <winsock2.h>
#   include <ws2tcpip.h>
#   pragma comment(lib, "ws2_32.lib")    // MSVC auto-link; harmless on MinGW
 
    using socket_t    = SOCKET;
    using socklen_t   = int;
    static constexpr socket_t invalid_socket_v = INVALID_SOCKET;
 
    inline int  close_socket(socket_t s)  { return ::closesocket(s); }
    inline bool socket_valid(socket_t s)  { return s != INVALID_SOCKET; }
    inline int  last_error()              { return ::WSAGetLastError(); }
 
#else  // POSIX (Linux / macOS / BSD)
#   define IDYL_PLATFORM_POSIX
#   include <arpa/inet.h>
#   include <netinet/in.h>
#   include <sys/socket.h>
#   include <sys/types.h>
#   include <unistd.h>
#   include <fcntl.h>
#   include <cerrno>
 
    using socket_t    = int;
    static constexpr socket_t invalid_socket_v = -1;
 
    inline int  close_socket(socket_t s)  { return ::close(s); }
    inline bool socket_valid(socket_t s)  { return s >= 0; }
    inline int  last_error()              { return errno; }
#endif
 
namespace idyl::utilities {
 
// -----------------------------------------------------------------------------
// winsock_guard
// RAII wrapper for WSAStartup / WSACleanup.
// Instantiate exactly once per process (before any socket use) on Windows.
// On POSIX this is a no-op.
// -----------------------------------------------------------------------------
 
struct winsock_guard {
    winsock_guard() {
#ifdef IDYL_PLATFORM_WINDOWS
        WSADATA wd{};
        if (::WSAStartup(MAKEWORD(2, 2), &wd) != 0)
            throw std::runtime_error("WSAStartup failed: "
                                     + std::to_string(last_error()));
#endif
    }
    ~winsock_guard() {
#ifdef IDYL_PLATFORM_WINDOWS
        ::WSACleanup();
#endif
    }
    winsock_guard(const winsock_guard&)            = delete;
    winsock_guard& operator=(const winsock_guard&) = delete;
};
 
// -----------------------------------------------------------------------------
// endpoint  —  thin IPv4 address + port pair
// -----------------------------------------------------------------------------
 
struct endpoint {
    std::string member_host;   // dotted-decimal IPv4, e.g. "127.0.0.1"
    uint16_t    member_port;
 
    // Converts to a sockaddr_in for socket calls
    ::sockaddr_in to_sockaddr() const {
        ::sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port   = ::htons(member_port);
        if (::inet_pton(AF_INET, member_host.c_str(), &addr.sin_addr) != 1)
            throw std::runtime_error("Invalid IPv4 address: " + member_host);
        return addr;
    }
 
    // Construct from a sockaddr_in (e.g. from recvfrom)
    static endpoint from_sockaddr(const ::sockaddr_in& addr) {
        char buf[INET_ADDRSTRLEN]{};
        ::inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf));
        return { buf, ::ntohs(addr.sin_port) };
    }
};
 
// -----------------------------------------------------------------------------
// receive_result  —  data + sender address returned by udp_receiver::recv()
// -----------------------------------------------------------------------------
 
struct receive_result {
    std::vector<uint8_t> member_data;
    endpoint             member_sender;
};
 
// -----------------------------------------------------------------------------
// udp_sender
//
// Sends UDP datagrams to a fixed remote endpoint.
// Not bound to a local port (OS picks one automatically).
// Thread-safety: single-threaded use only; protect externally if needed.
// -----------------------------------------------------------------------------
 
struct udp_sender {
    explicit udp_sender(endpoint remote)
        : _remote(std::move(remote))
        , _sock(invalid_socket_v)
    {
        _sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (!socket_valid(_sock))
            throw std::runtime_error("udp_sender: socket() failed: "
                                     + std::to_string(last_error()));
    }
 
    ~udp_sender() { _close(); }
 
    udp_sender(const udp_sender&)            = delete;
    udp_sender& operator=(const udp_sender&) = delete;
 
    udp_sender(udp_sender&& other) noexcept
        : _remote(std::move(other._remote))
        , _sock(other._sock)
    { other._sock = invalid_socket_v; }
 
    udp_sender& operator=(udp_sender&& other) noexcept {
        if (this != &other) {
            _close();
            _remote    = std::move(other._remote);
            _sock      = other._sock;
            other._sock = invalid_socket_v;
        }
        return *this;
    }
 
    // Send raw bytes. Returns the number of bytes sent.
    int send(const void* data, std::size_t length) {
        auto addr = _remote.to_sockaddr();
        int sent = static_cast<int>(
            ::sendto(_sock,
                     static_cast<const char*>(data),
                     static_cast<int>(length),
                     0,
                     reinterpret_cast<const ::sockaddr*>(&addr),
                     sizeof(addr)));
        if (sent < 0)
            throw std::runtime_error("udp_sender: sendto() failed: "
                                     + std::to_string(last_error()));
        return sent;
    }
 
    // Convenience overload for string_view
    int send(std::string_view text) {
        return send(text.data(), text.size());
    }
 
    // Convenience overload for byte vector
    int send(const std::vector<uint8_t>& bytes) {
        return send(bytes.data(), bytes.size());
    }
 
    const endpoint& remote() const noexcept { return _remote; }
 
private:
    void _close() {
        if (socket_valid(_sock)) {
            close_socket(_sock);
            _sock = invalid_socket_v;
        }
    }
 
    endpoint _remote;
    socket_t _sock;
};
 
// -----------------------------------------------------------------------------
// udp_receiver
//
// Binds to a local port and receives UDP datagrams.
// Supports blocking and non-blocking (with optional timeout) receive.
// Thread-safety: single-threaded use only.
// -----------------------------------------------------------------------------
 
struct udp_receiver {
    // bind_host: "0.0.0.0" to listen on all interfaces, or a specific IP.
    explicit udp_receiver(uint16_t port, std::string_view bind_host = "0.0.0.0")
        : _sock(invalid_socket_v)
    {
        _sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (!socket_valid(_sock))
            throw std::runtime_error("udp_receiver: socket() failed: "
                                     + std::to_string(last_error()));
 
        // SO_REUSEADDR: allows quick restart without "address already in use"
        int yes = 1;
        ::setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR,
                     reinterpret_cast<const char*>(&yes), sizeof(yes));
 
        ::sockaddr_in local{};
        local.sin_family = AF_INET;
        local.sin_port   = ::htons(port);
        if (::inet_pton(AF_INET, std::string(bind_host).c_str(),
                        &local.sin_addr) != 1)
            throw std::runtime_error("udp_receiver: invalid bind address");
 
        if (::bind(_sock,
                   reinterpret_cast<const ::sockaddr*>(&local),
                   sizeof(local)) < 0)
            throw std::runtime_error("udp_receiver: bind() failed: "
                                     + std::to_string(last_error()));
    }
 
    ~udp_receiver() { _close(); }
 
    udp_receiver(const udp_receiver&)            = delete;
    udp_receiver& operator=(const udp_receiver&) = delete;
 
    udp_receiver(udp_receiver&& other) noexcept : _sock(other._sock) {
        other._sock = invalid_socket_v;
    }
    udp_receiver& operator=(udp_receiver&& other) noexcept {
        if (this != &other) {
            _close();
            _sock       = other._sock;
            other._sock = invalid_socket_v;
        }
        return *this;
    }
 
    // -------------------------------------------------------------------------
    // recv_blocking
    // Blocks indefinitely until a datagram arrives.
    // max_size: maximum datagram size to accept (excess bytes are silently
    //           discarded by the OS — standard UDP behaviour).
    // -------------------------------------------------------------------------
    receive_result recv_blocking(std::size_t max_size = 65507) {
        return _recv(max_size, -1);
    }
 
    // -------------------------------------------------------------------------
    // recv_timeout
    // Blocks up to timeout_ms milliseconds.
    // Returns nullopt on timeout.
    // -------------------------------------------------------------------------
    std::optional<receive_result> recv_timeout(std::size_t max_size,
                                               int timeout_ms) {
        // Use select() to wait with a timeout — identical API on all platforms.
        ::fd_set fds;
        FD_ZERO(&fds);
        FD_SET(_sock, &fds);
 
        ::timeval tv{};
        tv.tv_sec  = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
 
        int ready = ::select(
#ifdef IDYL_PLATFORM_WINDOWS
            0,          // ignored on Windows
#else
            static_cast<int>(_sock) + 1,
#endif
            &fds, nullptr, nullptr, &tv);
 
        if (ready < 0)
            throw std::runtime_error("udp_receiver: select() failed: "
                                     + std::to_string(last_error()));
        if (ready == 0)
            return std::nullopt; // timeout
 
        return _recv(max_size, -1);
    }
 
    // -------------------------------------------------------------------------
    // set_non_blocking
    // After this call, recv_blocking() returns immediately if no data is
    // available — check the returned vector's size or use recv_timeout.
    // -------------------------------------------------------------------------
    void set_non_blocking(bool enable) {
#ifdef IDYL_PLATFORM_WINDOWS
        u_long mode = enable ? 1u : 0u;
        if (::ioctlsocket(_sock, FIONBIO, &mode) != 0)
            throw std::runtime_error("udp_receiver: ioctlsocket() failed: "
                                     + std::to_string(last_error()));
#else
        int flags = ::fcntl(_sock, F_GETFL, 0);
        if (flags < 0)
            throw std::runtime_error("udp_receiver: fcntl(F_GETFL) failed");
        flags = enable ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
        if (::fcntl(_sock, F_SETFL, flags) < 0)
            throw std::runtime_error("udp_receiver: fcntl(F_SETFL) failed");
#endif
    }
 
private:
    void _close() {
        if (socket_valid(_sock)) {
            close_socket(_sock);
            _sock = invalid_socket_v;
        }
    }
 
    receive_result _recv(std::size_t max_size, int /*flags*/) {
        std::vector<uint8_t> buf(max_size);
        ::sockaddr_in sender{};
        socklen_t sender_len = sizeof(sender);
 
        int received = static_cast<int>(
            ::recvfrom(_sock,
                       reinterpret_cast<char*>(buf.data()),
                       static_cast<int>(max_size),
                       0,
                       reinterpret_cast<::sockaddr*>(&sender),
                       &sender_len));
 
        if (received < 0)
            throw std::runtime_error("udp_receiver: recvfrom() failed: "
                                     + std::to_string(last_error()));
 
        buf.resize(static_cast<std::size_t>(received));
        return { std::move(buf), endpoint::from_sockaddr(sender) };
    }
 
    socket_t _sock;
};
 
} // --- idyl::utilities ---