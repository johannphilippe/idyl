// main.cpp — Xrun DSL parser entry point

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

// Bison-generated header (from XrunScheduler.y via `bison -d`)
#include "idyl_parser.tab.hh"
#include "ast.hpp"

using namespace idyl::parser;

// Flex globals
extern FILE* yyin;
extern int yyparse();
extern node_ptr root;

// Optional: enable bison debug tracing if you compiled with %debug
// extern int yydebug;

static void print_usage(const char* prog) {
    std::cerr << "Usage: " << prog << " [file.dsl]\n"
              << "  - If no file is given, reads from stdin.\n"
              << "  - Use '-' as filename to force stdin.\n";
}

int main(int argc, char** argv) {
    // yydebug = 1; // uncomment if you want bison trace output (requires %debug in .y)

    FILE* in = nullptr;

    if (argc > 1 && std::string(argv[1]) != "-") {
        in = std::fopen(argv[1], "r");
        if (!in) {
            std::perror(("Failed to open " + std::string(argv[1])).c_str());
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
        yyin = in;
    } else {
        // Read from stdin if no file or "-" is provided
        yyin = stdin;
    }

    // Run the parser
    int status = yyparse();

    if (in) std::fclose(in);

    if (status == 0) {
        std::cout << "Parse succeeded." << std::endl;
        printAST(root);


        // If you build an AST root in your grammar, you can print it here:
        // extern ASTNode* g_root;
        // if (g_root) dump_json(g_root);

        return EXIT_SUCCESS;
    } else {
        std::cerr << "Parse failed." << std::endl;
        return EXIT_FAILURE;
    }
}
