// idyl_ascii — OSC-driven live-coding ASCII art visualiser
// ──────────────────────────────────────────────────────────
// Listens for OSC messages on UDP port 9001 (configurable).
//
// OSC protocol
// ────────────
//   /idyl/code  s   <idyl source string>
//       → tokenise and start animating the new code
//
//   /idyl/clear
//       → blank the screen immediately
//
//   /idyl/tempo f   <bpm, optional>
//       → override the display tempo (tokens per minute).
//         If absent, the animator uses its own autonomous tempo.
//
// All other addresses are silently ignored.
//
// Animation design
// ────────────────
// The animator maintains its own internal clock, deliberately
// decoupled from the musical tempo. It cycles through the
// interesting tokens in the most recently received code snippet,
// spending a randomly varied amount of time on each (within bounds
// set by --min-ms and --max-ms). This creates a visual rhythm that
// floats alongside the music rather than being locked to it.
//
// Build
// ─────
//   cmake -B build -DCMAKE_BUILD_TYPE=Release
//   cmake --build build
//   ./build/idyl_ascii [--port 9001] [--min-ms 300] [--max-ms 1800]

#include "renderer.hpp"
#include "token_stream.hpp"

// ── Idyl headers ──────────────────────────────────────────────────────────────
// These come from the parent idyl repo's include/ directory.
// Adjust IDYL_INCLUDE_DIR in CMakeLists.txt if your layout differs.
#include "udp.hpp"
#include "osc.hpp"

using idyl::utilities::udp_receiver;
using idyl::osc_message;

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <mutex>
#include <optional>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <csignal>

// ── shared state between OSC thread and render thread ────────────────────────
struct shared_state {
    std::mutex           mtx;
    std::vector<token>   tokens;       // current token list
    bool                 clear_req = false;
    std::optional<float> tempo_bpm;    // if set, overrides autonomous tempo
};

// ── graceful shutdown ─────────────────────────────────────────────────────────
static std::atomic<bool> g_running{ true };
static bool              g_debug{ false };

static void on_signal(int) { g_running = false; }

// ── OSC listener thread ───────────────────────────────────────────────────────
static void osc_thread(shared_state& state, uint16_t port) {
    // udp_receiver binds in its constructor — no default ctor, no open/close.
    std::optional<udp_receiver> opt_recv;
    try {
        opt_recv.emplace(port);
    } catch (const std::exception& e) {
        std::cerr << "[idyl_ascii] failed to open UDP port " << port
                  << ": " << e.what() << "\n";
        g_running = false;
        return;
    }
    auto& recv = *opt_recv;
    std::cerr << "[idyl_ascii] listening on UDP " << port << "\n";

    while (g_running) {
        // recv_timeout blocks up to 100 ms so the thread can check g_running.
        auto pkt = recv.recv_timeout(4096, 100);
        if (!pkt) continue;   // timeout

        if (g_debug)
            std::cerr << "[idyl_ascii] rx " << pkt->member_data.size()
                      << " bytes from UDP\n";

        osc_message msg;
        try {
            msg = osc_message::parse(pkt->member_data.data(),
                                     pkt->member_data.size());
        } catch (const std::exception& e) {
            if (g_debug)
                std::cerr << "[idyl_ascii] parse failed: " << e.what() << "\n";
            continue;   // malformed or unsupported packet
        }

        if (g_debug)
            std::cerr << "[idyl_ascii] osc address='" << msg.member_address
                      << "' args=" << msg.arg_count() << "\n";

        std::lock_guard<std::mutex> lk(state.mtx);

        // Accept the live-coding eval address plus a couple of aliases so the
        // visualiser reacts whether vim sends /idyl/eval or /idyl/code.
        if (msg.member_address == "/idyl/eval" ||
            msg.member_address == "/idyl/code") {
            if (auto s = msg.try_get<std::string>(0)) {
                state.tokens = interesting_tokens(*s);
                state.clear_req = false;
                if (g_debug)
                    std::cerr << "[idyl_ascii] tokens=" << state.tokens.size() << "\n";
            }
        } else if (msg.member_address == "/idyl/process/stop" ||
                   msg.member_address == "/idyl/clear") {
            state.clear_req = true;
            state.tokens.clear();
        } else if (msg.member_address == "/idyl/tempo") {
            if (auto f = msg.try_get<float>(0)) {
                if (*f > 0.f) state.tempo_bpm = *f;
                else          state.tempo_bpm = std::nullopt;
            }
        }
        // opt_recv destructor closes the socket on exit.
    }
}

// ── scene definitions ─────────────────────────────────────────────────────────
struct scene_def {
    const char* name;
    disp_mode   disp;
    glitch_mode glitch;
    float       dwell_mult;   // multiplied on top of base dwell
};

static constexpr std::array scenes = {
    // name          disp                    glitch                  dwell_mult
    scene_def{"calm",      disp_mode::centered,   glitch_mode::clean,     1.00f},
    scene_def{"scanlines", disp_mode::centered,   glitch_mode::scanlines, 1.10f},
    scene_def{"noise",     disp_mode::centered,   glitch_mode::noise,     1.00f},
    scene_def{"drift",     disp_mode::centered,   glitch_mode::drift,     1.20f},
    scene_def{"corrupt",   disp_mode::centered,   glitch_mode::corrupt,   0.85f},
    scene_def{"static",    disp_mode::centered,   glitch_mode::static_,   0.55f},
    scene_def{"corner",    disp_mode::corner,     glitch_mode::clean,     1.40f},
    scene_def{"wander",    disp_mode::offset,     glitch_mode::noise,     0.90f},
    //scene_def{"mirror",    disp_mode::mirror,     glitch_mode::clean,     1.50f},
    scene_def{"typewrite", disp_mode::typewriter, glitch_mode::clean,     1.00f},
    scene_def{"chaos",     disp_mode::offset,     glitch_mode::corrupt,   0.50f},
    scene_def{"blizzard",  disp_mode::centered,   glitch_mode::static_,   0.35f},
};

// Weighted probabilities — calm is most common, chaos/blizzard/mirror are rare.
static const std::initializer_list<double> scene_weights = {
    12.0,   // calm
     4.0,   // scanlines
     4.0,   // noise
     4.0,   // drift
     2.5,   // corrupt
     1.5,   // static
     3.0,   // corner
     2.0,   // wander
     0.6,   // mirror   (rare — it's a bit much)
     3.0,   // typewrite
     1.0,   // chaos
     0.5,   // blizzard
};

// Look up a scene index by name (for the --scene CLI option). -1 if not found.
[[nodiscard]] static int scene_index_by_name(std::string_view name) noexcept {
    for (size_t i = 0; i < scenes.size(); ++i)
        if (name == scenes[i].name) return static_cast<int>(i);
    return -1;
}

// Is this glitch animated frame-to-frame (uses rng each render)?  These are
// re-rendered on a fast clock so the grain "rains" independently of the token.
[[nodiscard]] static bool is_dynamic_glitch(glitch_mode g) noexcept {
    return g == glitch_mode::noise || g == glitch_mode::static_ ||
           g == glitch_mode::corrupt || g == glitch_mode::drift;
}

// ── render / animation thread (main thread) ───────────────────────────────────
// forced_scene: index into scenes[] to lock (no auto-switching), or -1 for auto.
static void run_animation(shared_state& state,
                          int min_ms, int max_ms,
                          render_scale scale,
                          int forced_scene = -1) {
    renderer rend(scale);
    std::mt19937 rng(std::random_device{}());

    const bool scene_locked = (forced_scene >= 0);

    // ── distributions ─────────────────────────────────────────────────────
    std::uniform_int_distribution<int> dwell_dist(min_ms, max_ms);
    std::bernoulli_distribution        skip_dist(0.12);
    std::bernoulli_distribution        repeat_dist(0.08);
    std::discrete_distribution<int>    scene_dist(scene_weights);

    // Occasional spontaneous scene-change independent of mode timer
    std::bernoulli_distribution spontaneous_scene(0.008);   // ~0.8 % per token
    // Occasional spontaneous tempo-multiplier change
    std::bernoulli_distribution spontaneous_tempo(0.004);   // ~0.4 % per token

    // ── mode state ────────────────────────────────────────────────────────
    using clock    = std::chrono::steady_clock;
    using seconds  = std::chrono::seconds;
    using ms       = std::chrono::milliseconds;

    int cur_scene  = scene_locked ? forced_scene : 0;   // start calm (or locked)
    auto next_scene_time = clock::now() + seconds(
        std::uniform_int_distribution<int>(25, 55)(rng));

    // Tempo multiplier — allows autonomous speed bursts or slowdowns.
    static constexpr float tempo_table[] = {
        0.40f, 0.60f, 0.80f, 1.00f, 1.00f, 1.00f, 1.30f, 1.80f, 2.50f };
    float tempo_mult = 1.0f;
    auto  next_tempo_time = clock::now() + seconds(
        std::uniform_int_distribution<int>(40, 90)(rng));

    // ── token state ───────────────────────────────────────────────────────
    static constexpr std::array fill_chars = {
        '#', '@', '%', '=', '+', '*', '&', 'X', 'O', '8' };
    std::vector<token> local_tokens;
    size_t idx = 0, fill_idx = 0;
    bool   showing_blank = true;

    // ── helpers ───────────────────────────────────────────────────────────
    auto base_dwell = [&](const std::optional<float>& bpm) -> int {
        int ms_val;
        if (bpm.has_value()) {
            float beat_ms = 60000.f / bpm.value();
            std::uniform_real_distribution<float> jitter(-0.15f, 0.15f);
            ms_val = static_cast<int>(beat_ms * (1.f + jitter(rng)));
        } else {
            ms_val = dwell_dist(rng);
        }
        float mult = scenes[cur_scene].dwell_mult * tempo_mult;
        return std::max(40, static_cast<int>(ms_val * mult));
    };

    auto switch_scene = [&](bool log) {
        int ns = scene_dist(rng);
        if (ns == cur_scene) ns = scene_dist(rng);  // avoid same twice
        cur_scene = ns;
        std::uniform_int_distribution<int> dur(20, 50);
        next_scene_time = clock::now() + seconds(dur(rng));
        //if (log)
        //    std::cerr << "[idyl_ascii] scene → " << scenes[cur_scene].name << "\n";
    };

    auto switch_tempo = [&](bool log) {
        std::uniform_int_distribution<int> ti(
            0, static_cast<int>(sizeof tempo_table / sizeof *tempo_table) - 1);
        tempo_mult = tempo_table[ti(rng)];
        std::uniform_int_distribution<int> dur(35, 80);
        next_tempo_time = clock::now() + seconds(dur(rng));
        //if (log)
        //    std::cerr << "[idyl_ascii] tempo ×" << tempo_mult << "\n";
    };

    // Render `tok` for `total_ms`.  For dynamic glitches (noise/static/corrupt/
    // drift) the frame is re-rendered with fresh grain every glitch_frame_ms so
    // the noise rains asynchronously from the token's own (slower) dwell rate.
    // Clean/scanline glitches are deterministic → rendered once, then held.
    auto animate_token = [&](const token& tok, char fc, float alpha,
                             const scene_def& sc, int visible,
                             int bc, int br, int total_ms) {
        if (total_ms <= 0) return;
        if (!is_dynamic_glitch(sc.glitch)) {
            rend.render_token(tok, fc, alpha, sc.disp, sc.glitch, rng, visible, bc, br);
            std::this_thread::sleep_for(ms(total_ms));
            return;
        }
        constexpr int glitch_frame_ms = 65;   // ~15 fps grain
        auto end = clock::now() + ms(total_ms);
        while (g_running) {
            rend.render_token(tok, fc, alpha, sc.disp, sc.glitch, rng, visible, bc, br);
            auto now2 = clock::now();
            if (now2 >= end) break;
            auto remain = std::chrono::duration_cast<ms>(end - now2);
            std::this_thread::sleep_for(std::min(ms(glitch_frame_ms), remain));
        }
    };

    // Split a normalised (single-space) comment body into words.
    auto split_words = [](const std::string& s) {
        std::vector<std::string> w;
        size_t i = 0, n = s.size();
        while (i < n) {
            while (i < n && s[i] == ' ') ++i;
            size_t st = i;
            while (i < n && s[i] != ' ') ++i;
            if (i > st) w.push_back(s.substr(st, i - st));
        }
        return w;
    };

    // New code arrived (or a clear) while we were mid-display?  Used to make the
    // long comment routines interruptible so a fresh OSC eval takes effect at
    // once instead of waiting for the whole comment to finish playing.
    auto incoming_changed = [&]() -> bool {
        std::lock_guard<std::mutex> lk(state.mtx);
        return state.clear_req || state.tokens != local_tokens;
    };

    // Comments get a calm, readable treatment that breaks from the glitchy code
    // display — the "human voice" in the source.  Two presentations:
    //   • subtitle    — a few words at a time as readable text, typed in, held
    //                    long enough to read (favoured for longer comments)
    //   • word-by-word — each word as a big clean glyph with extra dwell
    // Both poll incoming_changed() so a new eval interrupts the comment promptly.
    auto display_comment = [&](const std::string& text) {
        auto words = split_words(text);
        if (words.empty()) return;

        bool subtitle = (words.size() >= 4)
            ? std::bernoulli_distribution(0.65)(rng)
            : std::bernoulli_distribution(0.25)(rng);

        if (g_debug)
            std::cerr << "[idyl_ascii] comment ["
                      << (subtitle ? "subtitle" : "word-by-word") << "]: "
                      << text << "\n";

        // Sleep in small slices so a new eval interrupts within ~40 ms.
        auto interruptible_sleep = [&](int total_ms) -> bool {
            constexpr int slice = 40;
            for (int e = 0; e < total_ms && g_running; e += slice) {
                if (incoming_changed()) return true;
                std::this_thread::sleep_for(ms(std::min(slice, total_ms - e)));
            }
            return incoming_changed();
        };

        if (subtitle) {
            std::uniform_int_distribution<int> chunk_dist(3, 5);
            size_t w = 0;
            while (w < words.size() && g_running) {
                size_t cn = static_cast<size_t>(chunk_dist(rng));
                std::string phrase;
                size_t shown = 0;
                for (size_t k = 0; k < cn && w + k < words.size(); ++k, ++shown) {
                    if (k) phrase += " ";
                    phrase += words[w + k];
                }
                w += shown;
                // Typewriter reveal of the chunk.
                int len = static_cast<int>(phrase.size());
                for (int v = 1; v <= len && g_running; ++v) {
                    if (incoming_changed()) return;
                    rend.render_phrase(phrase, 1.0f, v);
                    std::this_thread::sleep_for(ms(18));
                }
                // Generous reading time: ~320 ms per word + base.
                if (interruptible_sleep(500 + static_cast<int>(shown) * 320)) return;
                rend.render_phrase(phrase, 0.3f);   // fade
                if (interruptible_sleep(150)) return;
            }
        } else {
            std::uniform_int_distribution<int> hold(480, 760);
            for (size_t k = 0; k < words.size() && g_running; ++k) {
                token wt{ words[k], token_kind::comment };
                rend.render_token(wt, '#', 1.0f,
                                  disp_mode::centered, glitch_mode::clean, rng);
                if (interruptible_sleep(hold(rng))) return;
                rend.render_token(wt, '.', 0.3f,
                                  disp_mode::centered, glitch_mode::clean, rng);
                if (interruptible_sleep(130)) return;
            }
        }
    };

    // ── main loop ─────────────────────────────────────────────────────────
    while (g_running) {

        // ── timed mode switches ───────────────────────────────────────────
        auto now = clock::now();
        if (!scene_locked && now >= next_scene_time) switch_scene(true);
        if (now >= next_tempo_time)                  switch_tempo(true);

        // ── spontaneous switches (rare, adds unpredictability) ────────────
        if (!scene_locked && spontaneous_scene(rng)) switch_scene(false);
        if (spontaneous_tempo(rng))                  switch_tempo(false);

        // ── snapshot shared state ─────────────────────────────────────────
        std::vector<token>   snap_tokens;
        bool                 snap_clear = false;
        std::optional<float> snap_bpm;
        {
            std::lock_guard<std::mutex> lk(state.mtx);
            snap_tokens = state.tokens;
            snap_clear  = state.clear_req;
            snap_bpm    = state.tempo_bpm;
            state.clear_req = false;
        }

        if (snap_tokens != local_tokens) {
            local_tokens = snap_tokens;
            idx = 0; fill_idx = 0;
        }

        if (snap_clear || local_tokens.empty()) {
            if (!showing_blank) { rend.blank(); showing_blank = true; }
            std::this_thread::sleep_for(ms(80));
            continue;
        }
        showing_blank = false;

        // ── token selection ───────────────────────────────────────────────
        if (skip_dist(rng) && local_tokens.size() > 1)
            idx = (idx + 1) % local_tokens.size();

        const auto& tok = local_tokens[idx % local_tokens.size()];

        // ── comments get their own calm, readable presentation ────────────
        if (tok.kind == token_kind::comment) {
            display_comment(tok.text);
            if (!repeat_dist(rng)) idx = (idx + 1) % local_tokens.size();
            ++fill_idx;
            continue;
        }

        const auto& sc  = scenes[cur_scene];
        char fc  = fill_chars[fill_idx % fill_chars.size()];
        int  dw  = base_dwell(snap_bpm);

        // ── positional bias for corner / offset modes ─────────────────────
        int bc = 0, br = 0;
        if (sc.disp == disp_mode::corner) {
            auto t = rend.term();
            std::uniform_int_distribution<int> qd(0, 3);
            int q = qd(rng);
            bc = (q & 1) ?  t.cols / 5 : -t.cols / 5;
            br = (q & 2) ?  t.rows / 5 : -t.rows / 5;
        } else if (sc.disp == disp_mode::offset) {
            auto t = rend.term();
            std::uniform_int_distribution<int> odc(-t.cols / 6, t.cols / 6);
            std::uniform_int_distribution<int> odr(-t.rows / 6, t.rows / 6);
            bc = odc(rng); br = odr(rng);
        }

        // ── render ────────────────────────────────────────────────────────
        if (sc.disp == disp_mode::typewriter && !tok.text.empty()) {
            // Reveal characters one by one, then hold.
            int n = static_cast<int>(tok.text.size());
            std::uniform_int_distribution<int> char_delay(45, 95);
            for (int k = 1; k <= n && g_running; ++k) {
                rend.render_token(tok, fc, 1.0f, sc.disp, sc.glitch, rng, k, 0, 0);
                std::this_thread::sleep_for(ms(char_delay(rng)));
            }
            // Hold fully-revealed for a moment, then fade.
            animate_token(tok, fc,  1.0f, sc, -1, 0, 0, std::max(120, dw - n * 65));
            animate_token(tok, '.', 0.3f, sc, -1, 0, 0, 180);
        } else {
            int bright_ms = dw * 7 / 10;
            animate_token(tok, fc,  1.0f, sc, -1, bc, br, bright_ms);
            animate_token(tok, '.', 0.3f, sc, -1, bc, br, dw - bright_ms);
        }

        if (!repeat_dist(rng)) idx = (idx + 1) % local_tokens.size();
        ++fill_idx;
    }
}

// ── argument parsing ──────────────────────────────────────────────────────────
static int parse_int_arg(int argc, char** argv, const char* flag, int def) {
    for (int i = 1; i + 1 < argc; ++i)
        if (std::strcmp(argv[i], flag) == 0)
            return std::atoi(argv[i + 1]);
    return def;
}

static bool has_flag(int argc, char** argv, const char* flag) {
    for (int i = 1; i < argc; ++i)
        if (std::strcmp(argv[i], flag) == 0) return true;
    return false;
}

static const char* parse_str_arg(int argc, char** argv, const char* flag, const char* def) {
    for (int i = 1; i + 1 < argc; ++i)
        if (std::strcmp(argv[i], flag) == 0)
            return argv[i + 1];
    return def;
}

// ── main ──────────────────────────────────────────────────────────────────────
int main(int argc, char** argv) {
    std::signal(SIGINT,  on_signal);
    std::signal(SIGTERM, on_signal);

    uint16_t    port   = static_cast<uint16_t>(parse_int_arg(argc, argv, "--port",   9001));
    int         min_ms = parse_int_arg(argc, argv, "--min-ms", 350);
    int         max_ms = parse_int_arg(argc, argv, "--max-ms", 1600);
    int         col_sc = parse_int_arg(argc, argv, "--hscale", 2);
    int         row_sc = parse_int_arg(argc, argv, "--vscale", 1);
    g_debug            = has_flag(argc, argv, "--debug");

    // --list-scenes: print available scene names and exit.
    if (has_flag(argc, argv, "--list-scenes")) {
        std::cout << "Available scenes:\n";
        for (const auto& s : scenes) std::cout << "  " << s.name << "\n";
        return 0;
    }

    // --scene <name>: lock a single scene (disables automatic scene switching).
    int forced_scene = -1;
    if (const char* sn = parse_str_arg(argc, argv, "--scene", nullptr)) {
        forced_scene = scene_index_by_name(sn);
        if (forced_scene < 0) {
            std::cerr << "[idyl_ascii] unknown scene '" << sn << "'. Available: ";
            for (const auto& s : scenes) std::cerr << s.name << " ";
            std::cerr << "\n";
            return 1;
        }
    }

    if (min_ms > max_ms) std::swap(min_ms, max_ms);

    render_scale scale{ col_sc, row_sc };
    shared_state state;

    // OSC listener on background thread.
    std::thread osc_th([&]{ osc_thread(state, port); });

    std::cerr << "[idyl_ascii] autonomous dwell " << min_ms << "–" << max_ms << " ms\n";
    std::cerr << "[idyl_ascii] hscale=" << col_sc << " vscale=" << row_sc << "\n";
    if (forced_scene >= 0)
        std::cerr << "[idyl_ascii] scene LOCKED to '" << scenes[forced_scene].name
                  << "' (no auto-switching)\n";
    else
        std::cerr << "[idyl_ascii] scenes auto-switch (use --scene <name> to lock)\n";
    std::cerr << "[idyl_ascii] send: /idyl/eval s \"process p: { m = metro(1b) }\"\n";
    std::cerr << "[idyl_ascii] send: /idyl/process/stop  (clears screen)\n";
    std::cerr << "[idyl_ascii] send: /idyl/tempo f 120.0\n";
    if (g_debug) std::cerr << "[idyl_ascii] debug logging ON\n";

    run_animation(state, min_ms, max_ms, scale, forced_scene);

    osc_th.join();
    return 0;
}
