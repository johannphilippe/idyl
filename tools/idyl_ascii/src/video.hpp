#pragma once
// video.hpp — glitchy ASCII background video for idyl_ascii
// ───────────────────────────────────────────────────────────────────────────
// Decodes a video file with ffmpeg (piped raw gray frames), then plays it back
// as a faint ASCII layer *behind* the live-coding text — deliberately as a
// "distorted memory": mostly short loops, sudden jumps, freezes/stutters,
// dropped frames and degrading glitches, so the source is only ever partially
// recognisable.
//
// Cross-platform note: this leans on ffmpeg (ubiquitous, simplest robust path)
// invoked through popen(). No libav linkage, no extra build deps. If ffmpeg is
// absent or the file can't be opened, start() returns false and the visualiser
// simply carries on without a background.
//
// The decoder runs on its own thread and owns the pipe end-to-end (only the
// worker touches the FILE*), so teardown is race-free: stop() flips a flag and
// joins; the worker notices, closes the pipe (terminating ffmpeg) and exits.

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>

class video_player {
public:
    video_player() = default;
    ~video_player() { stop(); }

    video_player(const video_player&)            = delete;
    video_player& operator=(const video_player&) = delete;

    // Spawn ffmpeg and begin decoding `path` scaled to cols×rows at `fps`.
    // Returns false if the pipe can't be opened (e.g. ffmpeg missing).
    bool start(const std::string& path, int cols, int rows, double fps) {
        stop();
        if (cols <= 0 || rows <= 0) return false;
        cols_ = cols; rows_ = rows;
        fps_  = (fps > 0.0) ? fps : 12.0;

        const int ifps = std::max(1, static_cast<int>(std::lround(fps_)));
        // -stream_loop -1 : loop the file forever (the memory never ends)
        // gray + rawvideo  : one byte per pixel, cols*rows bytes per frame
        std::string cmd =
            "ffmpeg -hide_banner -loglevel error -nostdin -stream_loop -1 -i "
            + shell_quote(path) +
            " -an -vf \"fps=" + std::to_string(ifps) +
            ",scale=" + std::to_string(cols_) + ":" + std::to_string(rows_) +
            ":flags=fast_bilinear\" -pix_fmt gray -f rawvideo - 2>/dev/null";

        pipe_ = ::popen(cmd.c_str(), "r");
        if (!pipe_) return false;

        const size_t n = static_cast<size_t>(cols_) * rows_;
        out_ch_.assign(n, ' ');
        out_fg_.assign(n, -1);
        have_.store(false);
        running_.store(true);
        worker_ = std::thread([this] { run(); });
        return true;
    }

    void stop() {
        running_.store(false);
        if (worker_.joinable()) worker_.join();   // worker closes the pipe itself
    }

    [[nodiscard]] bool active() const noexcept { return running_.load(); }
    [[nodiscard]] int  cols()   const noexcept { return cols_; }
    [[nodiscard]] int  rows()   const noexcept { return rows_; }

    // Copy the latest glitched ASCII frame out. Returns false until the first
    // frame is ready (or after the stream has ended).
    bool sample(std::vector<char>& ch, std::vector<int>& fg) {
        if (!have_.load()) return false;
        std::lock_guard<std::mutex> lk(mtx_);
        ch = out_ch_;
        fg = out_fg_;
        return true;
    }

private:
    // ── decoder + glitch-playback thread ──────────────────────────────────
    void run() {
        const size_t fsz = static_cast<size_t>(cols_) * rows_;
        const int    RING = std::max(48, static_cast<int>(std::lround(fps_ * 4.0)));
        std::vector<std::vector<unsigned char>> ring;
        ring.reserve(static_cast<size_t>(RING));

        std::vector<unsigned char> frame(fsz);
        std::mt19937 rng(std::random_device{}());
        const auto interval = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            std::chrono::duration<double>(1.0 / fps_));

        int writecount = 0;
        bool started = false;

        std::uniform_real_distribution<double> u01(0.0, 1.0);
        auto frames = [&](double lo_s, double hi_s) {
            return static_cast<int>((lo_s + u01(rng) * (hi_s - lo_s)) * fps_);
        };
        // Start calm so the clip is recognisable before anything corrupts it.
        intensity_ = 0.0; intensity_target_ = 0.0; phase_left_ = frames(12.0, 30.0);

        while (running_.load()) {
            const auto t0 = std::chrono::steady_clock::now();

            // ── glitch-intensity envelope ──────────────────────────────────
            // Long CALM stretches (clip plays ~normally), occasional gentle
            // DRIFT, rare brief STORMs. Everything glitchy scales with this.
            if (--phase_left_ <= 0) {
                double r = u01(rng);
                if      (r < 0.62) { intensity_target_ = 0.0;                    phase_left_ = frames(15, 40); } // CALM
                else if (r < 0.88) { intensity_target_ = 0.20 + u01(rng) * 0.12; phase_left_ = frames(6, 15);  } // DRIFT
                else               { intensity_target_ = 0.70 + u01(rng) * 0.30; phase_left_ = frames(2, 6);   } // STORM
            }
            intensity_ += (intensity_target_ - intensity_) * 0.06;   // smooth ramp

            // 1) pull one fresh source frame into the ring (paces ffmpeg)
            if (!read_full(frame.data(), fsz)) break;   // EOF / broken pipe
            if (static_cast<int>(ring.size()) < RING) ring.push_back(frame);
            else                                      ring[static_cast<size_t>(writecount % RING)] = frame;
            ++writecount;
            const int avail = std::min(writecount, RING);

            // 2) advance the distorted-memory playback head
            const int win_hi = writecount - 1;            // newest retained
            const int win_lo = writecount - avail;         // oldest retained
            if (!started) {
                head_ = win_hi; anchor_ = win_hi;
                loop_len_ = 8; loop_left_ = 2; dir_ = 1; freeze_left_ = 0;
                started = true;
            }
            advance_head(rng, win_lo, win_hi, avail);
            const auto& src = ring[static_cast<size_t>(((head_ % RING) + RING) % RING)];

            // 3) degrade + convert to ASCII
            std::vector<char> ch(fsz, ' ');
            std::vector<int>  fg(fsz, -1);
            convert(src, ch, fg, rng);

            // 4) publish
            {
                std::lock_guard<std::mutex> lk(mtx_);
                out_ch_.swap(ch);
                out_fg_.swap(fg);
            }
            have_.store(true);

            std::this_thread::sleep_until(t0 + interval);
        }

        running_.store(false);
        have_.store(false);
        if (pipe_) { ::pclose(pipe_); pipe_ = nullptr; }   // terminates ffmpeg
    }

    // Read exactly n bytes from the pipe; false on EOF/error.
    bool read_full(unsigned char* buf, size_t n) {
        size_t got = 0;
        while (got < n) {
            if (!running_.load()) return false;
            size_t r = std::fread(buf + got, 1, n - got, pipe_);
            if (r == 0) return false;   // EOF or error
            got += r;
        }
        return true;
    }

    // ── distorted-memory head motion ──────────────────────────────────────
    // Mostly short forward loops repeated a few times, punctuated by freezes,
    // dropped frames, sudden jumps to another point in the retained window,
    // and the occasional snap back to the newest frame ("catching up").
    void advance_head(std::mt19937& rng, int win_lo, int win_hi, int avail) {
        std::uniform_real_distribution<double> u(0.0, 1.0);
        const auto ri = [&](int lo, int hi) {
            return lo + static_cast<int>(u(rng) * (hi - lo + 1));
        };
        const double I = intensity_;

        // CALM: lock to the freshest frame → the clip simply plays forward,
        // fully recognisable, with no jumps/loops/freezes.
        if (I < 0.06) {
            head_ = win_hi; anchor_ = head_; dir_ = 1; freeze_left_ = 0;
        } else if (freeze_left_ > 0) {
            --freeze_left_;                                   // hold (stutter)
        } else {
            double r = u(rng);
            if (r < 0.04 * I) {
                freeze_left_ = ri(1, 2 + static_cast<int>(6 * I));  // brief freeze
            } else if (r < 0.14 * I) {
                head_   = win_lo + static_cast<int>(u(rng) * std::max(1, avail));
                anchor_ = head_;                              // jump + new loop
                loop_len_  = ri(3, 26);
                loop_left_ = ri(1, 4);
                dir_       = (u(rng) < 0.18) ? -1 : 1;
            } else {
                head_ += dir_;
                if (u(rng) < 0.07 * I) head_ += dir_;         // drop a frame
                if (dir_ > 0 && head_ >= anchor_ + loop_len_) {
                    if (loop_left_-- > 0) head_ = anchor_;     // repeat loop
                    else { anchor_ = head_; loop_len_ = ri(4, 26); loop_left_ = ri(1, 3); }
                } else if (dir_ < 0 && head_ <= anchor_ - loop_len_) {
                    if (loop_left_-- > 0) head_ = anchor_;
                    else { dir_ = 1; anchor_ = head_; loop_len_ = ri(4, 26); loop_left_ = ri(1, 3); }
                }
                // pull back toward live — stronger as intensity eases off.
                if (u(rng) < 0.02 + 0.05 * (1.0 - I)) { head_ = win_hi; anchor_ = head_; }
            }
        }
        // keep the head inside the retained window (circular memory)
        if (avail > 0) {
            if (head_ > win_hi) head_ = win_lo + (head_ - win_hi - 1) % avail;
            if (head_ < win_lo) head_ = win_hi - (win_lo - head_ - 1) % avail;
        }
    }

    // ── gray frame → faint ASCII (with degradation) ───────────────────────
    void convert(const std::vector<unsigned char>& src,
                 std::vector<char>& ch, std::vector<int>& fg,
                 std::mt19937& rng) {
        std::uniform_real_distribution<double> u(0.0, 1.0);

        // Degradation params persist for a handful of frames, then re-roll —
        // so the picture "settles" and "slips" rather than shimmering every frame.
        // All of it scales with the glitch intensity: at rest the picture is
        // clean (gain 1, full ramp, no invert).
        const double I = intensity_;
        if (deg_left_-- <= 0) {
            deg_gain_   = 1.0 + (u(rng) - 0.5) * 0.70 * I;                       // ±0.35·I
            deg_levels_ = (u(rng) < 0.25 * I) ? (2 + static_cast<int>(u(rng) * 3)) : 0;
            deg_invert_ = (u(rng) < 0.05 * I);
            deg_left_   = 4 + static_cast<int>(u(rng) * 20);
        }

        static const char ramp[] = " .`:,;-+=*oO#%@";   // dark → light
        const int rn = static_cast<int>(sizeof(ramp)) - 2;  // last printable index

        for (int r = 0; r < rows_; ++r) {
            for (int c = 0; c < cols_; ++c) {
                const size_t i = static_cast<size_t>(r) * cols_ + c;
                double L = src[i] * deg_gain_;
                if (deg_invert_) L = 255.0 - L;
                if (deg_levels_ > 1) {
                    const double q = 255.0 / (deg_levels_ - 1);
                    L = std::round(L / q) * q;
                }
                if (L < 0) L = 0; else if (L > 255) L = 255;

                if (L < 26.0) { ch[i] = ' '; fg[i] = -1; }      // dark → transparent
                else {
                    int lvl = static_cast<int>(L * rn / 255.0);
                    if (lvl < 0) lvl = 0; else if (lvl > rn) lvl = rn;
                    ch[i] = ramp[lvl];
                    fg[i] = 234 + static_cast<int>(L * 11.0 / 255.0);  // dim grey 234–245
                }
            }
        }

        if (u(rng) < 0.55 * I) {                  // block dropouts
            int blocks = 1 + static_cast<int>(u(rng) * 4);
            for (int b = 0; b < blocks; ++b) glitch_block(ch, fg, rng);
        }
        if (u(rng) < 0.45 * I) tear_rows(ch, fg, rng); // horizontal sync tears
    }

    void glitch_block(std::vector<char>& ch, std::vector<int>& fg, std::mt19937& rng) {
        if (cols_ < 2 || rows_ < 2) return;
        std::uniform_real_distribution<double> u(0.0, 1.0);
        static const char nc[] = { ' ', '.', ':', '#', '%', '=', '/', '\\', '|', '~' };
        const int bw = 2 + static_cast<int>(u(rng) * (cols_ / 4));
        const int bh = 1 + static_cast<int>(u(rng) * (rows_ / 6));
        const int c0 = static_cast<int>(u(rng) * std::max(1, cols_ - bw));
        const int r0 = static_cast<int>(u(rng) * std::max(1, rows_ - bh));
        const bool bright = (u(rng) < 0.15);      // rare brighter "signal" block
        for (int r = r0; r < r0 + bh && r < rows_; ++r)
            for (int c = c0; c < c0 + bw && c < cols_; ++c) {
                const size_t i = static_cast<size_t>(r) * cols_ + c;
                ch[i] = nc[static_cast<size_t>(u(rng) * (sizeof(nc) - 1))];
                fg[i] = bright ? 248 : (234 + static_cast<int>(u(rng) * 8));
            }
    }

    void tear_rows(std::vector<char>& ch, std::vector<int>& fg, std::mt19937& rng) {
        std::uniform_real_distribution<double> u(0.0, 1.0);
        const int nrows = 1 + static_cast<int>(u(rng) * 4);
        for (int k = 0; k < nrows; ++k) {
            const int r = static_cast<int>(u(rng) * rows_);
            int shift = static_cast<int>(u(rng) * 13) - 6;   // −6..+6
            if (shift == 0) continue;
            const int base = r * cols_;
            std::vector<char> tch(static_cast<size_t>(cols_));
            std::vector<int>  tfg(static_cast<size_t>(cols_));
            for (int c = 0; c < cols_; ++c) {
                int sc = c - shift;
                if (sc < 0 || sc >= cols_) { tch[static_cast<size_t>(c)] = ' '; tfg[static_cast<size_t>(c)] = -1; }
                else { tch[static_cast<size_t>(c)] = ch[static_cast<size_t>(base + sc)];
                       tfg[static_cast<size_t>(c)] = fg[static_cast<size_t>(base + sc)]; }
            }
            for (int c = 0; c < cols_; ++c) {
                ch[static_cast<size_t>(base + c)] = tch[static_cast<size_t>(c)];
                fg[static_cast<size_t>(base + c)] = tfg[static_cast<size_t>(c)];
            }
        }
    }

    // POSIX single-quote the path for /bin/sh.
    static std::string shell_quote(const std::string& s) {
        std::string out = "'";
        for (char c : s) { if (c == '\'') out += "'\\''"; else out += c; }
        out += "'";
        return out;
    }

    // ── state ─────────────────────────────────────────────────────────────
    std::FILE*        pipe_   = nullptr;
    std::thread       worker_;
    std::atomic<bool> running_{ false };
    std::atomic<bool> have_{ false };
    std::mutex        mtx_;
    std::vector<char> out_ch_;
    std::vector<int>  out_fg_;

    int    cols_ = 0, rows_ = 0;
    double fps_  = 12.0;

    // playback-head state
    int head_ = 0, anchor_ = 0, loop_len_ = 8, loop_left_ = 2, dir_ = 1, freeze_left_ = 0;
    // degradation state
    double deg_gain_ = 1.0; int deg_levels_ = 0; bool deg_invert_ = false; int deg_left_ = 0;
    // glitch-intensity envelope (0 = calm/normal playback, 1 = full storm)
    double intensity_ = 0.0, intensity_target_ = 0.0; int phase_left_ = 0;
};
