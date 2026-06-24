#pragma once
#include "ascii_font.hpp"
#include "token_stream.hpp"

#include <string>
#include <string_view>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <random>
#include <sys/ioctl.h>
#include <unistd.h>

// ── ANSI helpers ──────────────────────────────────────────────────────────────
namespace ansi {
    inline void clear_screen()            { std::fputs("\033[2J\033[H",   stdout); }
    inline void cursor_home()             { std::fputs("\033[H",          stdout); }
    inline void hide_cursor()             { std::fputs("\033[?25l",       stdout); }
    inline void show_cursor()             { std::fputs("\033[?25h",       stdout); }
    inline void move_to(int row, int col) { std::fprintf(stdout, "\033[%d;%dH", row+1, col+1); }
    inline void fg256(int n)              { std::fprintf(stdout, "\033[38;5;%dm", n); }
    inline void reset()                   { std::fputs("\033[0m",         stdout); }
    inline void bold()                    { std::fputs("\033[1m",         stdout); }
    inline void dim()                     { std::fputs("\033[2m",         stdout); }
}

// ── colour palette ────────────────────────────────────────────────────────────
static constexpr int colour_keyword    = 214;   // orange
static constexpr int colour_identifier = 39;    // cyan-blue
static constexpr int colour_number     = 154;   // lime
static constexpr int colour_symbol     = 245;   // grey
static constexpr int colour_comment    = 108;   // muted sage-green (code-comment feel)
static constexpr int colour_fade       = 238;   // dark grey
static constexpr int colour_shadow     = 234;   // near-black shadow

[[nodiscard]] inline int token_colour(token_kind k) noexcept {
    switch (k) {
        case token_kind::keyword:    return colour_keyword;
        case token_kind::identifier: return colour_identifier;
        case token_kind::number:     return colour_number;
        case token_kind::symbol:     return colour_symbol;
        case token_kind::comment:    return colour_comment;
        default:                     return colour_symbol;
    }
}

// ── display and glitch modes ──────────────────────────────────────────────────
enum class disp_mode  : uint8_t {
    centered,      // default: big token centred on screen
    corner,        // token placed in a random screen quadrant
    offset,        // small random drift from centre
    mirror,        // horizontally mirrored
    typewriter,    // characters revealed one by one (requires visible_chars param)
    COUNT
};

enum class glitch_mode : uint8_t {
    clean,         // no effect
    scanlines,     // alternating dim rows, CRT-style
    noise,         // scattered low-level background noise chars
    drift,         // per-row horizontal shift (CRT sync loss)
    corrupt,       // some on-pixels replaced with wrong chars/colours
    static_,       // bright random foreground sparks everywhere
    COUNT
};

// ── render scale ──────────────────────────────────────────────────────────────
struct render_scale { int col_scale = 2; int row_scale = 1; };

// ── terminal size ─────────────────────────────────────────────────────────────
struct term_size { int cols = 80; int rows = 24; };

[[nodiscard]] inline term_size get_term_size() noexcept {
    struct winsize w{};
    if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
        return { w.ws_col, w.ws_row };
    return {};
}

// ── frame buffer ─────────────────────────────────────────────────────────────
struct frame_buffer {
    int cols_ = 0, rows_ = 0;
    std::vector<char> ch_;
    std::vector<int>  fg_;
    std::vector<bool> bold_;

    void resize(int cols, int rows) {
        cols_ = cols; rows_ = rows;
        size_t n = static_cast<size_t>(cols * rows);
        ch_.assign(n, ' '); fg_.assign(n, -1); bold_.assign(n, false);
    }
    void clear() {
        std::fill(ch_.begin(), ch_.end(), ' ');
        std::fill(fg_.begin(), fg_.end(), -1);
        std::fill(bold_.begin(), bold_.end(), false);
    }
    void set(int row, int col, char c, int fg, bool b = false) noexcept {
        if (row < 0 || col < 0 || row >= rows_ || col >= cols_) return;
        size_t idx = static_cast<size_t>(row * cols_ + col);
        ch_[idx] = c; fg_[idx] = fg; bold_[idx] = b;
    }
    void flush_diff(const frame_buffer& prev) const {
        int cur_fg = -2; bool cur_bold = false;
        for (int r = 0; r < rows_; ++r) {
            for (int c = 0; c < cols_; ++c) {
                size_t idx = static_cast<size_t>(r * cols_ + c);
                char nc = ch_[idx]; int nfg = fg_[idx]; bool nb = bold_[idx];
                if (nc == prev.ch_[idx] && nfg == prev.fg_[idx] && nb == prev.bold_[idx])
                    continue;
                ansi::move_to(r, c);
                if (nb != cur_bold || nfg != cur_fg) {
                    ansi::reset();
                    if (nb) ansi::bold();
                    if (nfg >= 0) ansi::fg256(nfg);
                    cur_fg = nfg; cur_bold = nb;
                }
                std::fputc(nc, stdout);
            }
        }
        ansi::reset();
        std::fflush(stdout);
    }
};

// ── renderer ─────────────────────────────────────────────────────────────────
struct renderer {
    render_scale scale_;
    frame_buffer cur_, prev_;
    term_size    term_;

    explicit renderer(render_scale sc = {}) : scale_(sc) {
        term_ = get_term_size();
        cur_.resize(term_.cols, term_.rows);
        prev_.resize(term_.cols, term_.rows);
        prev_.clear();
        ansi::hide_cursor();
        ansi::clear_screen();
    }
    ~renderer() { ansi::show_cursor(); ansi::reset(); }

    term_size term() const noexcept { return term_; }

    // ── background video layer ────────────────────────────────────────────────
    // A faint ASCII frame (from video_player) drawn *under* the text on every
    // render.  Text always overwrites the background, so it stays readable.
    void set_background(std::vector<char> ch, std::vector<int> fg, int bcols, int brows) {
        bg_ch_   = std::move(ch);
        bg_fg_   = std::move(fg);
        bg_cols_ = bcols;
        bg_rows_ = brows;
        bg_on_   = true;
    }
    void clear_background() noexcept { bg_on_ = false; bg_ch_.clear(); bg_fg_.clear(); }
    [[nodiscard]] bool background_on() const noexcept { return bg_on_; }

    // ── main render entry point ──────────────────────────────────────────────
    // visible_chars: chars of tok.text to draw (-1 = all, for typewriter mode)
    // bias_col/row:  offset in terminal cells from centred position
    void render_token(const token& tok,
                      char fill_char, float alpha,
                      disp_mode dm, glitch_mode gm, std::mt19937& rng,
                      int visible_chars = -1,
                      int bias_col = 0, int bias_row = 0)
    {
        refresh_size();
        cur_.clear();
        composite_background();

        const auto& text = tok.text;
        if (text.empty()) { flush(); return; }

        char fc  = fill_char ? fill_char : kind_fill(tok.kind);
        int  fg  = (alpha < 0.5f) ? colour_fade : token_colour(tok.kind);
        bool bo  = (alpha >= 0.8f);

        std::string_view vis = (visible_chars >= 0 && visible_chars < static_cast<int>(text.size()))
                               ? std::string_view(text).substr(0, static_cast<size_t>(visible_chars))
                               : std::string_view(text);

        int pw        = ascii_font::string_pixel_width(vis);
        int total_c   = pw  * scale_.col_scale;
        int total_r   = ascii_font::glyph_h * scale_.row_scale;
        int start_col = std::clamp((term_.cols - total_c) / 2 + bias_col,
                                   0, std::max(0, term_.cols - total_c));
        int start_row = std::clamp((term_.rows - total_r) / 2 + bias_row,
                                   0, std::max(0, term_.rows - total_r));

        // Shadow (skip for mirror to avoid weird doubling)
        if (dm != disp_mode::mirror)
            draw_text_pixels(vis, start_col+1, start_row+1, kind_shadow_fill(tok.kind), colour_shadow, false);
        // Main glyph
        draw_text_pixels(vis, start_col, start_row, fc, fg, bo);

        // Display-mode post-processing
        if (dm == disp_mode::mirror)
            apply_hflip();

        // Glitch post-processing (before label so label stays readable)
        apply_glitch(gm, rng);

        // Label
        draw_label(tok, visible_chars);

        flush();
    }

    // Backward-compatible overload (centred, clean, no rng needed)
    void render_token(const token& tok, char fill_char = 0, float alpha = 1.0f) {
        static std::mt19937 dummy(42);
        render_token(tok, fill_char, alpha,
                     disp_mode::centered, glitch_mode::clean, dummy, -1, 0, 0);
    }

    void blank() { cur_.clear(); composite_background(); flush(); }

    // ── readable phrase (for comments) ───────────────────────────────────────
    // Draws word-wrapped, literal (non-bitmap) text centred on screen in the
    // comment colour — a calm, readable "subtitle" look, distinct from the big
    // glitchy code glyphs.  visible_chars < 0 draws the whole phrase; otherwise
    // only the first N chars are shown (typewriter reveal) plus a block cursor.
    void render_phrase(std::string_view text, float alpha = 1.0f,
                       int visible_chars = -1) {
        refresh_size();
        cur_.clear();
        composite_background();

        std::string_view vis = (visible_chars >= 0 &&
                                visible_chars < static_cast<int>(text.size()))
                               ? text.substr(0, static_cast<size_t>(visible_chars))
                               : text;
        bool typing = (visible_chars >= 0 &&
                       visible_chars < static_cast<int>(text.size()));

        // Word-wrap to ~60 % of the terminal width.
        int maxw = std::max(12, term_.cols * 3 / 5);
        std::vector<std::string> lines;
        {
            std::string line;
            size_t i = 0, n = vis.size();
            while (i < n) {
                while (i < n && vis[i] == ' ') ++i;
                size_t s = i;
                while (i < n && vis[i] != ' ') ++i;
                if (i == s) continue;
                std::string word(vis.substr(s, i - s));
                if (line.empty())
                    line = word;
                else if (static_cast<int>(line.size() + 1 + word.size()) <= maxw)
                    line += " " + word;
                else { lines.push_back(line); line = word; }
            }
            if (!line.empty()) lines.push_back(line);
        }
        if (lines.empty()) { flush(); return; }
        if (typing && !lines.empty()) lines.back() += "\xe2\x96\x88";   // block cursor

        int fg = (alpha < 0.5f) ? colour_fade : colour_comment;
        bool bo = (alpha >= 0.8f);
        int total = static_cast<int>(lines.size());
        int start_row = std::max(0, (term_.rows - total) / 2);

        for (int li = 0; li < total; ++li) {
            const std::string& ln = lines[li];
            int w = static_cast<int>(ln.size());
            int start_col = std::max(0, (term_.cols - w) / 2);
            for (int c = 0; c < w; ++c)
                cur_.set(start_row + li, start_col + c, ln[c], fg, bo);
        }

        // Bottom-left label so it's clear this is a comment.
        std::string label = "[ // comment ]";
        int lr = term_.rows - 2, lc = 2;
        for (char c : label) {
            cur_.set(lr, lc++, c, colour_fade, false);
            if (lc >= term_.cols - 2) break;
        }
        flush();
    }

private:
    // ── background layer state ─────────────────────────────────────────────────
    std::vector<char> bg_ch_;
    std::vector<int>  bg_fg_;
    int  bg_cols_ = 0, bg_rows_ = 0;
    bool bg_on_   = false;

    // Paint the faint video frame into the freshly-cleared buffer. Only cells
    // within both the bg frame and the current terminal are touched, and only
    // where the bg pixel is non-blank — text drawn afterwards overwrites it.
    void composite_background() {
        if (!bg_on_ || bg_cols_ <= 0 || bg_rows_ <= 0) return;
        const int rmax = std::min(bg_rows_, cur_.rows_);
        const int cmax = std::min(bg_cols_, cur_.cols_);
        for (int r = 0; r < rmax; ++r)
            for (int c = 0; c < cmax; ++c) {
                const size_t bi = static_cast<size_t>(r) * bg_cols_ + c;
                const char bc = bg_ch_[bi];
                if (bc == ' ') continue;
                cur_.set(r, c, bc, bg_fg_[bi], false);
            }
    }

    // ── size helpers ─────────────────────────────────────────────────────────
    void refresh_size() {
        term_ = get_term_size();
        if (cur_.cols_ != term_.cols || cur_.rows_ != term_.rows) {
            cur_.resize(term_.cols, term_.rows);
            prev_.resize(term_.cols, term_.rows);
            prev_.clear();
        }
    }

    // ── glyph drawing ────────────────────────────────────────────────────────
    void draw_text_pixels(std::string_view text, int start_col, int start_row,
                          char fc, int fg, bool bo) {
        int x = 0;
        for (char ch : text) {
            const auto& glyph = ascii_font::get(ch);
            for (int row = 0; row < ascii_font::glyph_h; ++row) {
                uint8_t bits = glyph[static_cast<size_t>(row)];
                for (int bit = 0; bit < ascii_font::glyph_w; ++bit) {
                    if (!((bits >> (ascii_font::glyph_w - 1 - bit)) & 1)) continue;
                    int tc = start_col + (x + bit) * scale_.col_scale;
                    int tr = start_row + row * scale_.row_scale;
                    for (int dc = 0; dc < scale_.col_scale; ++dc)
                        cur_.set(tr, tc + dc, fc, fg, bo);
                }
            }
            x += ascii_font::glyph_w + ascii_font::char_spacing;
        }
    }

    void draw_label(const token& tok, int visible_chars) {
        std::string label;
        if (visible_chars >= 0 && visible_chars < static_cast<int>(tok.text.size())) {
            // Typewriter: show partial text with block cursor
            label = "[ " + tok.text.substr(0, static_cast<size_t>(visible_chars)) + "\xe2\x96\x88 ]";
        } else {
            label = "[ ";
            switch (tok.kind) {
                case token_kind::keyword:    label += "kw:";  break;
                case token_kind::identifier: label += "id:";  break;
                case token_kind::number:     label += "num:"; break;
                case token_kind::symbol:     label += "sym:"; break;
                default:                     label += "?:";   break;
            }
            label += " " + tok.text + " ]";
        }
        int row = term_.rows - 2, col = 2;
        for (char c : label) {
            cur_.set(row, col++, c, colour_fade, false);
            if (col >= term_.cols - 2) break;
        }
    }

    // ── glitch effects ────────────────────────────────────────────────────────

    void apply_scanlines() {
        for (int r = 1; r < cur_.rows_; r += 2)
            for (int c = 0; c < cur_.cols_; ++c) {
                size_t i = static_cast<size_t>(r * cur_.cols_ + c);
                if (cur_.ch_[i] != ' ') { cur_.fg_[i] = colour_fade; cur_.bold_[i] = false; }
            }
    }

    void apply_noise(std::mt19937& rng, int count) {
        if (cur_.rows_ < 2 || cur_.cols_ < 2) return;
        static constexpr char nc[] = { '.', ',', '\'', '`', ':', ';', '~', '-', '_', '|' };
        std::uniform_int_distribution<int> rr(0, cur_.rows_ - 1);
        std::uniform_int_distribution<int> rc(0, cur_.cols_ - 1);
        std::uniform_int_distribution<int> ci(0, static_cast<int>(sizeof nc) - 1);
        for (int n = 0; n < count; ++n) {
            size_t idx = static_cast<size_t>(rr(rng) * cur_.cols_ + rc(rng));
            if (cur_.ch_[idx] == ' ') { cur_.ch_[idx] = nc[ci(rng)]; cur_.fg_[idx] = colour_fade; }
        }
    }

    // CRT horizontal sync drift: rows shift by a smoothly-varying amount.
    void apply_drift(std::mt19937& rng) {
        std::uniform_int_distribution<int> step(-1, 1);
        int cumulative = 0;
        auto& ch = cur_.ch_; auto& fg = cur_.fg_; auto& bo = cur_.bold_;
        for (int r = 0; r < cur_.rows_; ++r) {
            cumulative = std::clamp(cumulative + step(rng), -6, 6);
            int shift = cumulative;
            if (shift == 0) continue;
            int base = r * cur_.cols_;
            if (shift > 0) {
                for (int c = cur_.cols_-1; c >= shift; --c) {
                    ch[base+c] = ch[base+c-shift];
                    fg[base+c] = fg[base+c-shift];
                    bo[base+c] = bo[base+c-shift];
                }
                for (int c = 0; c < shift; ++c)
                    { ch[base+c]=' '; fg[base+c]=-1; bo[base+c]=false; }
            } else {
                int s = -shift;
                for (int c = 0; c < cur_.cols_-s; ++c) {
                    ch[base+c] = ch[base+c+s];
                    fg[base+c] = fg[base+c+s];
                    bo[base+c] = bo[base+c+s];
                }
                for (int c = cur_.cols_-s; c < cur_.cols_; ++c)
                    { ch[base+c]=' '; fg[base+c]=-1; bo[base+c]=false; }
            }
        }
    }

    void apply_corrupt(std::mt19937& rng, float prob) {
        static constexpr char cc[] = { '/', '\\', '|', '!', '?', '^', '~', '<', '>', '%' };
        static constexpr int  cg[] = { 196, 202, 46, 226, 201, 51, 124, 208, 160, 231 };
        std::bernoulli_distribution corrupt(prob);
        std::uniform_int_distribution<int> ci(0, static_cast<int>(sizeof cc) - 1);
        std::uniform_int_distribution<int> gi(0, static_cast<int>(sizeof cg / sizeof *cg) - 1);
        for (size_t i = 0; i < cur_.ch_.size(); ++i)
            if (cur_.ch_[i] != ' ' && corrupt(rng))
                { cur_.ch_[i]=cc[ci(rng)]; cur_.fg_[i]=cg[gi(rng)]; cur_.bold_[i]=true; }
    }

    // Bright sparks scattered across the whole screen.
    void apply_static_noise(std::mt19937& rng, int count) {
        if (cur_.rows_ < 2 || cur_.cols_ < 2) return;
        static constexpr char sc[] = { '#', '@', '*', '!', '%', '&', '0', '1', '+', '=' };
        static constexpr int  sg[] = { 196, 202, 46, 226, 201, 51, 231, 39, 214, 154 };
        std::uniform_int_distribution<int> rr(0, cur_.rows_ - 1);
        std::uniform_int_distribution<int> rc(0, cur_.cols_ - 1);
        std::uniform_int_distribution<int> ci(0, static_cast<int>(sizeof sc) - 1);
        std::uniform_int_distribution<int> gi(0, static_cast<int>(sizeof sg / sizeof *sg) - 1);
        for (int n = 0; n < count; ++n) {
            size_t idx = static_cast<size_t>(rr(rng) * cur_.cols_ + rc(rng));
            cur_.ch_[idx] = sc[ci(rng)];
            cur_.fg_[idx] = sg[gi(rng)];
            cur_.bold_[idx] = (n % 4 == 0);
        }
    }

    void apply_hflip() {
        for (int r = 0; r < cur_.rows_; ++r)
            for (int c = 0; c < cur_.cols_ / 2; ++c) {
                size_t i1 = static_cast<size_t>(r * cur_.cols_ + c);
                size_t i2 = static_cast<size_t>(r * cur_.cols_ + cur_.cols_ - 1 - c);
                std::swap(cur_.ch_[i1], cur_.ch_[i2]);
                std::swap(cur_.fg_[i1], cur_.fg_[i2]);
                // vector<bool> uses proxy elements — can't std::swap, do it manually
                bool b = cur_.bold_[i1];
                cur_.bold_[i1] = cur_.bold_[i2];
                cur_.bold_[i2] = b;
            }
    }

    void apply_glitch(glitch_mode gm, std::mt19937& rng) {
        switch (gm) {
            case glitch_mode::scanlines: apply_scanlines();           break;
            case glitch_mode::noise:     apply_noise(rng, 80);        break;
            case glitch_mode::drift:     apply_drift(rng);            break;
            case glitch_mode::corrupt:   apply_corrupt(rng, 0.06f);   break;
            case glitch_mode::static_:   apply_static_noise(rng, 120); break;
            default: break;
        }
    }

    void flush() { cur_.flush_diff(prev_); std::swap(cur_, prev_); }

    static char kind_fill(token_kind k) noexcept {
        switch (k) {
            case token_kind::keyword:    return '#';
            case token_kind::identifier: return '#';
            case token_kind::number:     return '@';
            case token_kind::symbol:     return '+';
            default:                     return '*';
        }
    }
    static char kind_shadow_fill(token_kind k) noexcept {
        switch (k) {
            case token_kind::keyword:    return '%';
            case token_kind::identifier: return ':';
            case token_kind::number:     return '.';
            case token_kind::symbol:     return '-';
            default:                     return '.';
        }
    }
};
