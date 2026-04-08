// idyl.cpp - Idyl language parser main driver

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <memory>
#include <thread>
#include <csignal>
#include <atomic>
#include <mutex>
#include <deque>
#include "utilities/filesystem.hpp"
#include "parser/ast.hpp"
#include "parser/parse.hpp"
#include "semantic/analyzer.hpp"
#include "core/evaluator.hpp"
#include "time/scheduler.hpp"
#include "include/module.hpp"
#ifdef IDYL_MODULE_OSC
#include "modules/osc_module.hpp"
#endif
#include "utilities/osc.hpp"
#include "utilities/udp.hpp"
#include "debug.hpp"

std::string idyl::utilities::main_source_path = ""; // Initialize the main source path variable

// ── Signal handling for clean shutdown ──────────────────────────────────────
static std::atomic<bool> g_running{true};

static void signal_handler(int /*sig*/) {
    g_running.store(false, std::memory_order_relaxed);
}

static void print_usage(const char* prog) {
    std::cerr << "Usage: " << prog << " [file.idyl] [options]\n"
              << "  - If no file is given, reads from stdin.\n"
              << "  - Use '-' as filename to force stdin.\n"
              << "  - Options:\n"
              << "    --trace            Enable parser/lexer debug tracing\n"
              << "    --process <name>   Run only the named process block\n"
              << "    -p <name>          Short form of --process\n"
              << "    --listen [port]    Listen mode: receive OSC on port (default 9000)\n"
              << "    -l [port]          Short form of --listen\n";
}

int main(int argc, char** argv) {
    std::istream* input = nullptr;
    std::ifstream file_stream;
    bool trace = false;
    std::string source_path;

    idyl::semantic::analyzer analyzer; 

    // ── Module registry ─────────────────────────────────────────────────
    // Built-in modules are registered in the catalog but NOT loaded until
    // the program explicitly calls module("osc"), module("csound"), etc.
    // This keeps the global namespace clean.
    idyl::module::registry module_registry;
    #ifdef IDYL_MODULE_OSC
    module_registry.register_builtin("osc", []() {
        return std::make_unique<idyl::modules::osc_module>();
    });
    #endif
    #ifdef IDYL_MODULE_CSOUND
    module_registry.register_builtin("csound", []() {
        return std::make_unique<idyl::module::csound_module>();
    });
    #endif

    std::string process_filter;
    bool listen_mode = false;
    int  listen_port = 9000;

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--trace") {
            trace = true;
        } else if (arg == "--process" || arg == "-p") {
            if (i + 1 < argc) {
                process_filter = argv[++i];
            } else {
                std::cerr << "Error: " << arg << " requires a process name\n";
                print_usage(argv[0]);
                return 1;
            }
        } else if (arg == "--listen" || arg == "-l") {
            listen_mode = true;
            // Optional port argument (next arg must be a number)
            if (i + 1 < argc) {
                try {
                    int p = std::stoi(argv[i + 1]);
                    if (p > 0 && p <= 65535) {
                        listen_port = p;
                        ++i;
                    }
                } catch (...) {
                    // Not a number — leave default port, don't consume arg
                }
            }
        } else if (arg == "-" ) {
            // stdin marker — handled below
        } else if (arg[0] == '-') {
            std::cerr << "Error: Unknown option '" << arg << "'\n";
            print_usage(argv[0]);
            return 1;
        } else if (source_path.empty()) {
            // First non-option argument is the source file
            source_path = arg;
        } else {
            std::cerr << "Error: Unexpected argument '" << arg << "'\n";
            print_usage(argv[0]);
            return 1;
        }
    }

    if (!source_path.empty()) {
        file_stream.open(source_path);
        if (!file_stream.is_open()) {
            std::cerr << "Error: Could not open file '" << source_path << "'" << std::endl;
            print_usage(argv[0]);
            return 1;
        }
        input = &file_stream;
        idyl::utilities::main_source_path = std::filesystem::absolute(source_path).parent_path().string();
    } else {
        input = &std::cin;
    }

    // Parse the main source
    auto program = idyl::parser::parse_stream(*input, trace);

    if (program) {
        idyl::debug("Parse successful!");
        idyl::debug("Generated AST:");
        #ifdef IDYL_VERBOSE
        program->print();
        #endif

        // Provide source path context to the analyzer so library paths can be resolved relative to it
        if (!source_path.empty()) {
            analyzer.source_path_ = source_path;
        }

        // Wire module registry into semantic analyzer
        analyzer.scope_stack_.module_registry_ = &module_registry;

        // Perform semantic analysis
        analyzer.analyze(*program);
        analyzer.print_analysis_results();
        if(analyzer.has_errors()) {
            std::cerr << "Semantic analysis failed with errors.\n";
            return 1;
        } else {
            idyl::debug("Semantic analysis completed successfully with no errors.");
        }

        // ── Evaluate ────────────────────────────────────────────────────────
        idyl::time::sys_clock_scheduler scheduler;
        scheduler.start();

        // Install signal handlers for clean shutdown (Ctrl+C)
        std::signal(SIGINT,  signal_handler);
        std::signal(SIGTERM, signal_handler);

        // Provide the scheduler to modules that need timing (e.g. osc dt sends)
        module_registry.provide_scheduler(&scheduler);

        idyl::core::evaluator eval;
        eval.scheduler_ = &scheduler;
        eval.env_.module_registry_ = &module_registry;
        eval.process_filter_ = process_filter;
        eval.listen_mode_ = listen_mode;
        eval.run(*program);

        if (!eval.warnings_.empty()) {
            eval.print_warnings();
        }

        // ── Listen mode: print available processes & start OSC receiver ─────
        // Commands are queued from the listener thread and dispatched on the
        // main thread to avoid concurrent evaluator access.
        struct listen_command {
            enum class type { start, stop, list } type_;
            std::string name_;
        };
        std::mutex cmd_mutex;
        std::deque<listen_command> cmd_queue;

        std::thread listener_thread;
        if (listen_mode) {
            // Print discovered process blocks
            auto procs = eval.list_stored_processes();
            std::cerr << "listen: port " << listen_port << ", "
                      << procs.size() << " process(es) available";
            if (!procs.empty()) {
                std::cerr << ":";
                for (const auto& p : procs) std::cerr << " " << p;
            }
            std::cerr << "\n";

            if (!process_filter.empty())
                std::cerr << "listen: pre-started '" << process_filter << "'\n";

            // Spawn OSC listener thread
            listener_thread = std::thread([&cmd_mutex, &cmd_queue, listen_port]() {
                try {
                    idyl::utilities::udp_receiver receiver(listen_port);
                    receiver.set_non_blocking(true);

                    while (g_running.load(std::memory_order_relaxed)) {
                        auto result = receiver.recv_timeout(65507, 100);
                        if (!result) continue;

                        try {
                            auto msg = idyl::osc_message::parse(result->member_data);
                            const auto& addr = msg.member_address;

                            if (addr == "/idyl/process/start") {
                                auto name = msg.try_get<std::string>(0);
                                if (name) {
                                    std::lock_guard<std::mutex> lk(cmd_mutex);
                                    cmd_queue.push_back(listen_command{listen_command::type::start, *name});
                                }
                            } else if (addr == "/idyl/process/stop") {
                                auto name = msg.try_get<std::string>(0);
                                if (name) {
                                    std::lock_guard<std::mutex> lk(cmd_mutex);
                                    cmd_queue.push_back(listen_command{listen_command::type::stop, *name});
                                }
                            } else if (addr == "/idyl/process/list") {
                                std::lock_guard<std::mutex> lk(cmd_mutex);
                                cmd_queue.push_back(listen_command{listen_command::type::list, {}});
                            }
                        } catch (...) {
                            // Malformed OSC packet — skip silently
                        }
                    }
                } catch (const std::exception& e) {
                    std::cerr << "listen: error: " << e.what() << "\n";
                }
            });
        }

        // Keep-alive logic:
        //  - Listen mode: always stay alive, poll command queue
        //  - Normal mode: stay alive while temporal subscriptions exist
        if (listen_mode || (eval.process_count_ >= 1 && scheduler.active_count() > 0)) {
            idyl::debug("Temporal functions active — running scheduler...");
            while (g_running.load(std::memory_order_relaxed)) {
                // ── Dispatch queued listen commands on main thread ───────
                if (listen_mode) {
                    std::deque<listen_command> batch;
                    {
                        std::lock_guard<std::mutex> lk(cmd_mutex);
                        batch.swap(cmd_queue);
                    }
                    for (const auto& cmd : batch) {
                        switch (cmd.type_) {
                        case listen_command::type::start:
                            if (eval.start_process(cmd.name_))
                                std::cerr << "listen: started '" << cmd.name_ << "'\n";
                            else
                                std::cerr << "listen: unknown process '" << cmd.name_ << "'\n";
                            break;
                        case listen_command::type::stop:
                            if (eval.stop_process(cmd.name_))
                                std::cerr << "listen: stopped '" << cmd.name_ << "'\n";
                            else
                                std::cerr << "listen: '" << cmd.name_ << "' not running\n";
                            break;
                        case listen_command::type::list: {
                            auto names = eval.list_stored_processes();
                            std::cerr << "listen: available:";
                            for (const auto& n : names) std::cerr << " " << n;
                            std::cerr << "\n";
                            break;
                        }
                        }
                    }
                }

                // In non-listen mode, exit when no more active subscriptions
                if (!listen_mode && scheduler.active_count() == 0) break;

                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }

        // Clean up listener thread
        if (listener_thread.joinable()) {
            g_running.store(false, std::memory_order_relaxed);
            listener_thread.join();
        }

        scheduler.stop();
        idyl::debug("Evaluation completed.");

        return 0;
    } else {
        std::cerr << "Parse failed." << std::endl;
        return 1;
    }
}

