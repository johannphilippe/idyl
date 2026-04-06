
#pragma once
 
// =============================================================================
// osc.hpp  —  OSC 1.1 message parsing & serialisation
// Spec      : Open Sound Control 1.0 / 1.1
// Standard  : C++17
// Depends   : udp_socket.hpp  (idyl::udp_sender / udp_receiver)
//
// Supported argument types
//   i  — int32          f  — float32
//   s  — string         b  — blob (raw bytes)
//   h  — int64          d  — float64
//   T  — true           F  — false
//   N  — nil            I  — impulse (infinitum)
//   t  — timetag (NTP)
// =============================================================================
 
#include "udp.hpp"
 
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
 
// Endian helpers — byte-swap to/from big-endian (network order)
#if defined(_WIN32) || defined(_WIN64)
#   include <cstdlib>
    namespace idyl::detail {
        inline uint16_t bswap16(uint16_t v) { return _byteswap_ushort(v); }
        inline uint32_t bswap32(uint32_t v) { return _byteswap_ulong(v); }
        inline uint64_t bswap64(uint64_t v) { return _byteswap_uint64(v); }
    }
#elif defined(__GNUC__) || defined(__clang__)
    namespace idyl::detail {
        inline uint16_t bswap16(uint16_t v) { return __builtin_bswap16(v); }
        inline uint32_t bswap32(uint32_t v) { return __builtin_bswap32(v); }
        inline uint64_t bswap64(uint64_t v) { return __builtin_bswap64(v); }
    }
#else
    // Portable fallback
    namespace idyl::detail {
        inline uint16_t bswap16(uint16_t v) {
            return static_cast<uint16_t>((v << 8) | (v >> 8));
        }
        inline uint32_t bswap32(uint32_t v) {
            return ((v & 0xFF000000u) >> 24) | ((v & 0x00FF0000u) >> 8)
                 | ((v & 0x0000FF00u) <<  8) | ((v & 0x000000FFu) << 24);
        }
        inline uint64_t bswap64(uint64_t v) {
            return ((uint64_t)bswap32(static_cast<uint32_t>(v))        << 32)
                 |  (uint64_t)bswap32(static_cast<uint32_t>(v >> 32));
        }
    }
#endif
 
// Detect native endianness at compile time
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#   define IDYL_BIG_ENDIAN
#endif
 
namespace idyl::detail {
 
    // Network ↔ host for each width
    inline uint32_t to_net32(uint32_t v) {
#ifdef IDYL_BIG_ENDIAN
        return v;
#else
        return bswap32(v);
#endif
    }
    inline uint64_t to_net64(uint64_t v) {
#ifdef IDYL_BIG_ENDIAN
        return v;
#else
        return bswap64(v);
#endif
    }
    inline uint32_t from_net32(uint32_t v) { return to_net32(v); }
    inline uint64_t from_net64(uint64_t v) { return to_net64(v); }
 
    // Type-punning helpers (avoids UB from direct cast)
    template<typename Dst, typename Src>
    Dst bit_cast(Src src) {
        static_assert(sizeof(Dst) == sizeof(Src));
        Dst dst;
        std::memcpy(&dst, &src, sizeof(src));
        return dst;
    }
 
    // Round up to next 4-byte OSC boundary
    inline std::size_t pad4(std::size_t n) { return (n + 3) & ~std::size_t(3); }
 
} // namespace idyl::detail
 
namespace idyl {
 
// =============================================================================
// osc_timetag
//
// 64-bit NTP timestamp: seconds since 1 Jan 1900 (upper 32 bits)
//                     + sub-second fraction   (lower 32 bits)
// Special value {0, 1} = "immediate".
// =============================================================================
 
struct osc_timetag {
    uint32_t member_seconds;    // seconds since 1 Jan 1900
    uint32_t member_fraction;   // 2^-32 seconds per unit
 
    static osc_timetag immediate() noexcept { return { 0, 1 }; }
    bool is_immediate() const noexcept {
        return member_seconds == 0 && member_fraction == 1;
    }
};
 
// =============================================================================
// osc_blob  —  raw byte buffer argument
// =============================================================================
 
using osc_blob = std::vector<uint8_t>;
 
// =============================================================================
// osc_value  —  variant holding any OSC argument
// =============================================================================
 
using osc_value = std::variant<
    int32_t,       // 'i'
    float,         // 'f'
    std::string,   // 's'
    osc_blob,      // 'b'
    int64_t,       // 'h'
    double,        // 'd'
    osc_timetag,   // 't'
    bool,          // 'T' or 'F'
    std::monostate // 'N' (nil) or 'I' (impulse)
>;
 
// Tag character for each type (used in type-tag string)
inline char osc_type_tag(const osc_value& v) {
    return std::visit([](const auto& x) -> char {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, int32_t>)     return 'i';
        if constexpr (std::is_same_v<T, float>)        return 'f';
        if constexpr (std::is_same_v<T, std::string>)  return 's';
        if constexpr (std::is_same_v<T, osc_blob>)     return 'b';
        if constexpr (std::is_same_v<T, int64_t>)      return 'h';
        if constexpr (std::is_same_v<T, double>)        return 'd';
        if constexpr (std::is_same_v<T, osc_timetag>)  return 't';
        if constexpr (std::is_same_v<T, bool>)         return x ? 'T' : 'F';
        if constexpr (std::is_same_v<T, std::monostate>) return 'N';
        return '?';
    }, v);
}
 
// =============================================================================
// osc_message
//
// High-level representation of a single OSC message.
//
// Usage — building:
//   osc_message msg("/synth/play");
//   msg << 440.0f << std::string("sine") << int32_t(1);
//   auto bytes = msg.serialise();
//
// Usage — parsing:
//   auto msg = osc_message::parse(bytes);
//   float freq = msg.get<float>(0);
//
// =============================================================================
 
struct osc_message {
    std::string             member_address;   // OSC address pattern, e.g. "/foo/bar"
    std::vector<osc_value>  member_args;
 
    // -------------------------------------------------------------------------
    // Construction
    // -------------------------------------------------------------------------
 
    explicit osc_message(std::string address = "/")
        : member_address(std::move(address)) {}
 
    // Builder-style argument appending via operator<<
    osc_message& operator<<(int32_t v)          { member_args.emplace_back(v); return *this; }
    osc_message& operator<<(float v)            { member_args.emplace_back(v); return *this; }
    osc_message& operator<<(std::string v)      { member_args.emplace_back(std::move(v)); return *this; }
    osc_message& operator<<(std::string_view v) { member_args.emplace_back(std::string(v)); return *this; }
    osc_message& operator<<(const char* v)      { member_args.emplace_back(std::string(v)); return *this; }
    osc_message& operator<<(osc_blob v)         { member_args.emplace_back(std::move(v)); return *this; }
    osc_message& operator<<(int64_t v)          { member_args.emplace_back(v); return *this; }
    osc_message& operator<<(double v)           { member_args.emplace_back(v); return *this; }
    osc_message& operator<<(osc_timetag v)      { member_args.emplace_back(v); return *this; }
    osc_message& operator<<(bool v)             { member_args.emplace_back(v); return *this; }
 
    // Nil / impulse sentinels
    struct nil_t      {};
    struct impulse_t  {};
    osc_message& operator<<(nil_t)     { member_args.emplace_back(std::monostate{}); return *this; }
    osc_message& operator<<(impulse_t) { member_args.emplace_back(std::monostate{}); return *this; }
 
    // -------------------------------------------------------------------------
    // Typed argument access
    // Throws std::out_of_range or std::bad_variant_access on mismatch.
    // -------------------------------------------------------------------------
 
    template <typename T>
    const T& get(std::size_t index) const {
        if (index >= member_args.size())
            throw std::out_of_range("osc_message::get — index out of range");
        return std::get<T>(member_args[index]);
    }
 
    template <typename T>
    std::optional<T> try_get(std::size_t index) const noexcept {
        if (index >= member_args.size()) return std::nullopt;
        if (const T* p = std::get_if<T>(&member_args[index])) return *p;
        return std::nullopt;
    }
 
    std::size_t arg_count() const noexcept { return member_args.size(); }
 
    // -------------------------------------------------------------------------
    // Serialisation → raw bytes (OSC wire format)
    // -------------------------------------------------------------------------
 
    std::vector<uint8_t> serialise() const {
        std::vector<uint8_t> out;
        out.reserve(128);
 
        // 1. Address string (padded to 4 bytes)
        _write_string(out, member_address);
 
        // 2. Type-tag string: comma + one char per arg + null + padding
        std::string tags = ",";
        for (const auto& v : member_args)
            tags += osc_type_tag(v);
        _write_string(out, tags);
 
        // 3. Arguments
        for (const auto& v : member_args)
            _write_value(out, v);
 
        return out;
    }
 
    // -------------------------------------------------------------------------
    // Parsing ← raw bytes
    // Throws std::runtime_error on malformed data.
    // -------------------------------------------------------------------------
 
    static osc_message parse(const uint8_t* data, std::size_t length) {
        if (length < 8)
            throw std::runtime_error("osc_message::parse — packet too short");
        if (data[0] != '/')
            throw std::runtime_error("osc_message::parse — missing leading '/'");
 
        std::size_t pos = 0;
 
        // 1. Address
        osc_message msg;
        msg.member_address = _read_string(data, length, pos);
 
        // 2. Type-tag string
        if (pos >= length || data[pos] != ',')
            throw std::runtime_error("osc_message::parse — missing type-tag string");
        std::string tags = _read_string(data, length, pos);
        // tags[0] == ','
 
        // 3. Arguments
        for (std::size_t i = 1; i < tags.size(); ++i) {
            char tag = tags[i];
            msg.member_args.push_back(_read_value(tag, data, length, pos));
        }
 
        return msg;
    }
 
    // Convenience: parse from a vector (from udp_receiver)
    static osc_message parse(const std::vector<uint8_t>& bytes) {
        return parse(bytes.data(), bytes.size());
    }
 
    // -------------------------------------------------------------------------
    // Direct UDP integration
    // -------------------------------------------------------------------------
 
    // Send this message via an existing udp_sender
    void send(utilities::udp_sender& sender) const {
        auto bytes = serialise();
        sender.send(bytes);
    }
 
    // Receive one message from an existing udp_receiver (blocking)
    static osc_message receive(utilities::udp_receiver& receiver, std::size_t max_size = 65507) {
        auto result = receiver.recv_blocking(max_size);
        return parse(result.member_data);
    }
 
    // Receive with timeout. Returns nullopt on timeout.
    static std::optional<osc_message> receive_timeout(utilities::udp_receiver& receiver,
                                                       int timeout_ms,
                                                       std::size_t max_size = 65507) {
        auto result = receiver.recv_timeout(max_size, timeout_ms);
        if (!result) return std::nullopt;
        return parse(result->member_data);
    }
 
private:
    // -------------------------------------------------------------------------
    // Write helpers
    // -------------------------------------------------------------------------
 
    static void _write_string(std::vector<uint8_t>& out, std::string_view s) {
        for (char c : s) out.push_back(static_cast<uint8_t>(c));
        out.push_back(0); // null terminator
        // Pad to 4-byte boundary
        std::size_t padded = detail::pad4(s.size() + 1);
        while (out.size() % 4 != 0) out.push_back(0);
        (void)padded;
    }
 
    static void _write_u32(std::vector<uint8_t>& out, uint32_t v) {
        v = detail::to_net32(v);
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&v);
        out.insert(out.end(), p, p + 4);
    }
 
    static void _write_u64(std::vector<uint8_t>& out, uint64_t v) {
        v = detail::to_net64(v);
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&v);
        out.insert(out.end(), p, p + 8);
    }
 
    static void _write_value(std::vector<uint8_t>& out, const osc_value& v) {
        std::visit([&](const auto& x) {
            using T = std::decay_t<decltype(x)>;
            if constexpr (std::is_same_v<T, int32_t>) {
                _write_u32(out, detail::bit_cast<uint32_t>(x));
            } else if constexpr (std::is_same_v<T, float>) {
                _write_u32(out, detail::bit_cast<uint32_t>(x));
            } else if constexpr (std::is_same_v<T, std::string>) {
                _write_string(out, x);
            } else if constexpr (std::is_same_v<T, osc_blob>) {
                // blob: int32 size, then data, then padding
                _write_u32(out, static_cast<uint32_t>(x.size()));
                out.insert(out.end(), x.begin(), x.end());
                while (out.size() % 4 != 0) out.push_back(0);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                _write_u64(out, detail::bit_cast<uint64_t>(x));
            } else if constexpr (std::is_same_v<T, double>) {
                _write_u64(out, detail::bit_cast<uint64_t>(x));
            } else if constexpr (std::is_same_v<T, osc_timetag>) {
                _write_u32(out, x.member_seconds);
                _write_u32(out, x.member_fraction);
            } else if constexpr (std::is_same_v<T, bool>) {
                // T/F have no data bytes — encoded purely in the type-tag
            } else if constexpr (std::is_same_v<T, std::monostate>) {
                // N/I have no data bytes
            }
        }, v);
    }
 
    // -------------------------------------------------------------------------
    // Read helpers
    // -------------------------------------------------------------------------
 
    static std::string _read_string(const uint8_t* data, std::size_t length,
                                    std::size_t& pos) {
        std::size_t start = pos;
        while (pos < length && data[pos] != 0) ++pos;
        if (pos >= length)
            throw std::runtime_error("osc_message::parse — unterminated string");
        std::string s(reinterpret_cast<const char*>(data + start), pos - start);
        ++pos; // consume null
        // Advance to next 4-byte boundary
        pos = detail::pad4(pos);
        return s;
    }
 
    static uint32_t _read_u32(const uint8_t* data, std::size_t length,
                               std::size_t& pos) {
        if (pos + 4 > length)
            throw std::runtime_error("osc_message::parse — unexpected end of data");
        uint32_t v;
        std::memcpy(&v, data + pos, 4);
        pos += 4;
        return detail::from_net32(v);
    }
 
    static uint64_t _read_u64(const uint8_t* data, std::size_t length,
                               std::size_t& pos) {
        if (pos + 8 > length)
            throw std::runtime_error("osc_message::parse — unexpected end of data");
        uint64_t v;
        std::memcpy(&v, data + pos, 8);
        pos += 8;
        return detail::from_net64(v);
    }
 
    static osc_value _read_value(char tag, const uint8_t* data,
                                  std::size_t length, std::size_t& pos) {
        switch (tag) {
            case 'i': return detail::bit_cast<int32_t>(_read_u32(data, length, pos));
            case 'f': return detail::bit_cast<float>  (_read_u32(data, length, pos));
            case 's': return _read_string(data, length, pos);
            case 'b': {
                uint32_t sz = _read_u32(data, length, pos);
                if (pos + sz > length)
                    throw std::runtime_error("osc_message::parse — blob overflows packet");
                osc_blob blob(data + pos, data + pos + sz);
                pos += sz;
                pos = detail::pad4(pos);
                return blob;
            }
            case 'h': return detail::bit_cast<int64_t>(_read_u64(data, length, pos));
            case 'd': return detail::bit_cast<double>  (_read_u64(data, length, pos));
            case 't': {
                uint32_t sec  = _read_u32(data, length, pos);
                uint32_t frac = _read_u32(data, length, pos);
                return osc_timetag{ sec, frac };
            }
            case 'T': return osc_value{ true  };
            case 'F': return osc_value{ false };
            case 'N': return osc_value{ std::monostate{} };
            case 'I': return osc_value{ std::monostate{} };
            default:
                throw std::runtime_error(
                    std::string("osc_message::parse — unknown type tag: ") + tag);
        }
    }
};
 
// =============================================================================
// osc_bundle
//
// Groups multiple osc_message objects under a single timetag.
// Wire format: "#bundle\0" + timetag (8 bytes) + [ size(4) + message ]...
// =============================================================================
 
struct osc_bundle {
    osc_timetag              member_timetag = osc_timetag::immediate();
    std::vector<osc_message> member_messages;
 
    explicit osc_bundle(osc_timetag tt = osc_timetag::immediate())
        : member_timetag(tt) {}
 
    osc_bundle& add(osc_message msg) {
        member_messages.push_back(std::move(msg));
        return *this;
    }
 
    // Serialise to wire bytes
    std::vector<uint8_t> serialise() const {
        std::vector<uint8_t> out;
        out.reserve(256);
 
        // "#bundle\0" — exactly 8 bytes, already 4-byte aligned
        const char* header = "#bundle";
        for (int i = 0; i < 8; ++i)
            out.push_back(static_cast<uint8_t>(header[i])); // includes '\0'
 
        // Timetag (8 bytes)
        _write_u32(out, member_timetag.member_seconds);
        _write_u32(out, member_timetag.member_fraction);
 
        // Each element: int32 size + message bytes
        for (const auto& msg : member_messages) {
            auto bytes = msg.serialise();
            _write_u32(out, static_cast<uint32_t>(bytes.size()));
            out.insert(out.end(), bytes.begin(), bytes.end());
        }
 
        return out;
    }
 
    // Parse from raw bytes
    static osc_bundle parse(const uint8_t* data, std::size_t length) {
        if (length < 16)
            throw std::runtime_error("osc_bundle::parse — packet too short");
        if (std::memcmp(data, "#bundle\0", 8) != 0)
            throw std::runtime_error("osc_bundle::parse — missing #bundle header");
 
        std::size_t pos = 8;
 
        uint32_t sec  = _read_u32(data, length, pos);
        uint32_t frac = _read_u32(data, length, pos);
        osc_bundle bundle(osc_timetag{ sec, frac });
 
        while (pos < length) {
            uint32_t elem_size = _read_u32(data, length, pos);
            if (pos + elem_size > length)
                throw std::runtime_error("osc_bundle::parse — element size overflows");
            bundle.member_messages.push_back(
                osc_message::parse(data + pos, elem_size));
            pos += elem_size;
        }
 
        return bundle;
    }
 
    static osc_bundle parse(const std::vector<uint8_t>& bytes) {
        return parse(bytes.data(), bytes.size());
    }
 
    // Direct UDP integration
    void send(utilities::udp_sender& sender) const {
        auto bytes = serialise();
        sender.send(bytes);
    }
 
    static osc_bundle receive(utilities::udp_receiver& receiver, std::size_t max_size = 65507) {
        auto result = receiver.recv_blocking(max_size);
        return parse(result.member_data);
    }
 
    static std::optional<osc_bundle> receive_timeout(utilities::udp_receiver& receiver,
                                                      int timeout_ms,
                                                      std::size_t max_size = 65507) {
        auto result = receiver.recv_timeout(max_size, timeout_ms);
        if (!result) return std::nullopt;
        return parse(result->member_data);
    }
 
private:
    static void _write_u32(std::vector<uint8_t>& out, uint32_t v) {
        v = detail::to_net32(v);
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&v);
        out.insert(out.end(), p, p + 4);
    }
    static uint32_t _read_u32(const uint8_t* data, std::size_t length,
                               std::size_t& pos) {
        if (pos + 4 > length)
            throw std::runtime_error("osc_bundle::parse — unexpected end");
        uint32_t v;
        std::memcpy(&v, data + pos, 4);
        pos += 4;
        return detail::from_net32(v);
    }
};
 
// =============================================================================
// osc_dispatcher
//
// Maps OSC address patterns to handler callbacks.
// Call dispatch() with a received message to invoke the matching handler.
//
// Pattern matching: exact match first, then simple wildcard '*' per segment.
// =============================================================================
 
struct osc_dispatcher {
    using handler_t = std::function<void(const osc_message&)>;
 
    // Register an exact address or a glob pattern (e.g. "/synth/*")
    void on(std::string pattern, handler_t handler) {
        _routes.push_back({ std::move(pattern), std::move(handler) });
    }
 
    // Dispatch one message. Returns true if at least one handler matched.
    bool dispatch(const osc_message& msg) const {
        bool matched = false;
        for (const auto& route : _routes) {
            if (_matches(route.pattern, msg.member_address)) {
                route.handler(msg);
                matched = true;
            }
        }
        return matched;
    }
 
    // Dispatch all messages in a bundle
    bool dispatch(const osc_bundle& bundle) const {
        bool any = false;
        for (const auto& msg : bundle.member_messages)
            any |= dispatch(msg);
        return any;
    }
 
    // Convenience: receive + dispatch in one call (blocking)
    void receive_and_dispatch(utilities::udp_receiver& receiver,
                              std::size_t max_size = 65507) const {
        auto result = receiver.recv_blocking(max_size);
        _dispatch_raw(result.member_data);
    }
 
private:
    struct route_t {
        std::string pattern;
        handler_t   handler;
    };
    std::vector<route_t> _routes;
 
    // Segment-by-segment glob match ('*' matches any single segment)
    static bool _matches(std::string_view pattern, std::string_view address) {
        if (pattern == address) return true; // fast path
        // Walk both strings segment by segment
        auto pp = pattern.begin(),  pe = pattern.end();
        auto ap = address.begin(),  ae = address.end();
 
        while (pp != pe && ap != ae) {
            // Find end of current segment in both
            auto p_seg_end = std::find(pp, pe, '/');
            auto a_seg_end = std::find(ap, ae, '/');
 
            std::string_view p_seg(pp, p_seg_end - pp);
            std::string_view a_seg(ap, a_seg_end - ap);
 
            if (p_seg != "*" && p_seg != a_seg) return false;
 
            pp = (p_seg_end == pe) ? pe : std::next(p_seg_end);
            ap = (a_seg_end == ae) ? ae : std::next(a_seg_end);
        }
        return pp == pe && ap == ae;
    }
 
    void _dispatch_raw(const std::vector<uint8_t>& data) const {
        if (data.size() >= 8 && std::memcmp(data.data(), "#bundle", 7) == 0)
            dispatch(osc_bundle::parse(data));
        else
            dispatch(osc_message::parse(data));
    }
};
 
} // -- idyl::utilities --