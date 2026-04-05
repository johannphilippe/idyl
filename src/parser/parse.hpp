#pragma once

#include <memory>
#include <string>
#include <iostream>
#include "ast.hpp"

namespace idyl::parser {

    // Parse an idyl source file at the given path.
    // Returns the program AST on success, nullptr on failure.
    // Errors are written to stderr.
    std::shared_ptr<program> parse_file(const std::string& path);

    // Parse idyl source from an already-opened input stream.
    // Returns the program AST on success, nullptr on failure.
    std::shared_ptr<program> parse_stream(std::istream& input, bool trace = false);

} // namespace idyl::parser
