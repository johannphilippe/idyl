#include "parser/parse.hpp"
#include "parser/idyl_lexer.hpp"
#include "parser/idyl.tab.hh"
#include <fstream>

// Defined in the generated scanner (idyl.l)
extern void reset_lexer_state();

// Defined in the generated parser (idyl.y)
extern std::shared_ptr<idyl::parser::program> g_program;

namespace idyl::parser {

    std::shared_ptr<program> parse_stream(std::istream& input, bool trace) {
        // Save + reset the global program pointer so nested parses don't clobber the caller's result
        auto saved_program = g_program;
        g_program = nullptr;

        // Reset the Flex scanner so it starts fresh on the new stream
        reset_lexer_state();

        yy::lexer lex(input);
        yy::parser parser_obj(lex);

        if (trace) {
            parser_obj.set_debug_level(1);
            parser_obj.set_debug_stream(std::cerr);
        }

        int result = parser_obj.parse();

        std::shared_ptr<program> parsed;
        if (result == 0 && g_program) {
            parsed = g_program;
        }

        // Restore the caller's g_program (important when parsing libraries during semantic analysis)
        g_program = saved_program;

        return parsed;
    }

    std::shared_ptr<program> parse_file(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file '" << path << "'\n";
            return nullptr;
        }
        return parse_stream(file);
    }

} // namespace idyl::parser
