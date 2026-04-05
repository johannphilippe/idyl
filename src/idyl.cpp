// idyl.cpp - Idyl language parser main driver

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <memory>
#include <thread>
#include "utilities/filesystem.hpp"
#include "parser/ast.hpp"
#include "parser/parse.hpp"
#include "semantic/analyzer.hpp"
#include "core/evaluator.hpp"
#include "time/scheduler.hpp"
#include "debug.hpp"

std::string idyl::utilities::main_source_path = ""; // Initialize the main source path variable

static void print_usage(const char* prog) {
    std::cerr << "Usage: " << prog << " [file.idyl] [options]\n"
              << "  - If no file is given, reads from stdin.\n"
              << "  - Use '-' as filename to force stdin.\n"
              << "  - Options:\n"
              << "    --trace    Enable parser/lexer debug tracing\n";
}

int main(int argc, char** argv) {
    std::istream* input = nullptr;
    std::ifstream file_stream;
    bool trace = false;
    std::string source_path;

    idyl::semantic::analyzer analyzer; 

    // Parse command-line arguments
    if (argc > 1 && std::string(argv[1]) != "-") {
        // Open file
        file_stream.open(argv[1]);
        if (!file_stream.is_open()) {
            std::cerr << "Error: Could not open file '" << argv[1] << "'" << std::endl;
            print_usage(argv[0]);
            return 1;
        }
        input = &file_stream;
        source_path = argv[1];
        idyl::utilities::main_source_path = std::filesystem::absolute(source_path).parent_path().string();
    } else {
        // Read from stdin
        input = &std::cin;
    }


    // Check for --trace option
    if (argc > 2 && std::string(argv[2]) == "--trace") {
        trace = true;
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

        idyl::core::evaluator eval;
        eval.scheduler_ = &scheduler;
        eval.run(*program);

        if (!eval.warnings_.empty()) {
            eval.print_warnings();
        }

        // If there are active temporal subscriptions, keep the process
        // alive so the scheduler thread can tick them.
        if (scheduler.active_count() > 0) {
            idyl::debug("Temporal functions active — running scheduler...");
            // Run until all subscriptions finish or user interrupts (Ctrl+C)
            while (scheduler.active_count() > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }

        scheduler.stop();
        idyl::debug("Evaluation completed.");

        return 0;
    } else {
        std::cerr << "Parse failed." << std::endl;
        return 1;
    }
}

