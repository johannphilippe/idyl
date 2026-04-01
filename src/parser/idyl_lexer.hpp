#pragma once

#include <string>
#include <iostream>

namespace yy {
    class lexer {
    public:
        lexer(std::istream& input) : input_stream_(input) {}
        
        std::istream& get_input() { return input_stream_; }
        
    private:
        std::istream& input_stream_;
    };
}
