#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <string_view>
#include <cctype>

// ── token kinds ───────────────────────────────────────────────────────────────
enum class token_kind : uint8_t {
    keyword,      // process, flow, metro, sync, on, each …
    number,       // integer or float literal
    symbol,       // operator / punctuation  ( ) [ ] { } | → = …
    identifier,   // user-defined name
    comment,      // // line comment  or  /* block comment */ (whole text)
    whitespace,   // spaces / newlines (usually skipped for display)
    unknown,
};

struct token {
    std::string  text;
    token_kind   kind = token_kind::unknown;

    token() = default;
    token(std::string t, token_kind k) : text(std::move(t)), kind(k) {}

    bool operator==(const token& o) const noexcept {
        return kind == o.kind && text == o.text;
    }
};

// ── tokeniser ─────────────────────────────────────────────────────────────────
// Very lightweight — just enough to categorise Idyl source visually.
// Does NOT need to parse the language correctly; it only needs to produce
// visually distinct token categories for the animator.

static const std::vector<std::string_view> idyl_keywords = {
    // structure
    "process", "flow", "import", "module",
    // temporal primitives
    "metro", "sync", "phasor", "free_phasor", "euclid", "dust", "coin",
    "counter", "clock", "tempo",
    // control / lifecycle
    "on", "each", "in", "init", "emit", "catch",
    "stop", "start", "pause", "resume",
    // misc
    "trigger", "true", "false",
};

[[nodiscard]] inline bool is_idyl_keyword(std::string_view s) noexcept {
    for (auto& kw : idyl_keywords)
        if (kw == s) return true;
    return false;
}

// Collapse runs of whitespace to single spaces and trim the ends.  Comment
// bodies are normalised this way so the animator can split them on ' ' alone.
[[nodiscard]] inline std::string normalise_ws(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    bool in_space = true;   // start true so leading space is dropped
    for (char c : s) {
        bool sp = std::isspace(static_cast<unsigned char>(c));
        if (sp) {
            if (!in_space) out.push_back(' ');
            in_space = true;
        } else {
            out.push_back(c);
            in_space = false;
        }
    }
    while (!out.empty() && out.back() == ' ') out.pop_back();
    return out;
}

[[nodiscard]] inline std::vector<token> tokenise(std::string_view src) {
    std::vector<token> out;
    size_t i = 0;
    const size_t n = src.size();

    while (i < n) {
        // whitespace
        if (std::isspace(static_cast<unsigned char>(src[i]))) {
            size_t start = i;
            while (i < n && std::isspace(static_cast<unsigned char>(src[i]))) ++i;
            out.push_back(token(std::string(src.substr(start, i - start)), token_kind::whitespace));
            continue;
        }

        // line comment: //... to end of line
        if (src[i] == '/' && i + 1 < n && src[i + 1] == '/') {
            i += 2;
            size_t start = i;
            while (i < n && src[i] != '\n') ++i;
            std::string text = normalise_ws(src.substr(start, i - start));
            if (!text.empty())
                out.push_back(token(std::move(text), token_kind::comment));
            continue;
        }

        // block comment: /* ... */  (may span lines; collapsed to one phrase)
        if (src[i] == '/' && i + 1 < n && src[i + 1] == '*') {
            i += 2;
            size_t start = i;
            while (i + 1 < n && !(src[i] == '*' && src[i + 1] == '/')) ++i;
            size_t end = (i + 1 < n) ? i : n;   // text up to the closing */
            std::string text = normalise_ws(src.substr(start, end - start));
            i = (i + 1 < n) ? i + 2 : n;        // consume the closing */
            if (!text.empty())
                out.push_back(token(std::move(text), token_kind::comment));
            continue;
        }

        // number
        if (std::isdigit(static_cast<unsigned char>(src[i]))
            || (src[i] == '-' && i + 1 < n && std::isdigit(static_cast<unsigned char>(src[i+1])))) {
            size_t start = i++;
            while (i < n && (std::isdigit(static_cast<unsigned char>(src[i])) || src[i] == '.' || src[i] == '_'))
                ++i;
            out.push_back(token(std::string(src.substr(start, i - start)), token_kind::number));
            continue;
        }

        // identifier or keyword
        if (std::isalpha(static_cast<unsigned char>(src[i])) || src[i] == '_') {
            size_t start = i++;
            while (i < n && (std::isalnum(static_cast<unsigned char>(src[i])) || src[i] == '_' || src[i] == '\''))
                ++i;
            std::string text(src.substr(start, i - start));
            token_kind kind = is_idyl_keyword(text) ? token_kind::keyword : token_kind::identifier;
            out.push_back(token(std::move(text), kind));
            continue;
        }

        // symbol / operator (everything else, one char at a time)
        out.push_back(token(std::string(1, src[i]), token_kind::symbol));
        ++i;
    }

    return out;
}

// Returns only the visually interesting tokens (no whitespace).
[[nodiscard]] inline std::vector<token> interesting_tokens(std::string_view src) {
    auto all = tokenise(src);
    std::vector<token> out;
    out.reserve(all.size());
    for (auto& t : all)
        if (t.kind != token_kind::whitespace)
            out.push_back(std::move(t));
    return out;
}
