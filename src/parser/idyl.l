%{
#include <string>
#include <iostream>
#include "ast.hpp"
#include "idyl.tab.hh"
#include "idyl_lexer.hpp"

// Global storage for semantic values and location
static std::string yylval_string;
static yy::location yylloc;  // Declare yylloc globally
static yy::lexer* lexer_instance = nullptr;
static bool location_initialized = false;

#define YY_INPUT(buf, result, max_size) \
    { \
        if (lexer_instance) { \
            std::istream& input = lexer_instance->get_input(); \
            input.read(buf, max_size); \
            result = input.gcount(); \
        } else { \
            result = 0; \
        } \
    }

// Macro to track line numbers
#define TRACK_LOCATION() \
    do { \
        yylloc.step(); \
        for (int i = 0; i < yyleng; i++) { \
            if (yytext[i] == '\n') { \
                yylloc.lines(1); \
            } else { \
                yylloc.columns(1); \
            } \
        } \
    } while (0)

// Macro for returning tokens with location tracking
#define RETURN_TOKEN(token_type) \
    do { \
        TRACK_LOCATION(); \
        return yy::parser::token::token_type; \
    } while (0)

%}

%option noyywrap
%option never-interactive

DIGIT       [0-9]
DIGITS      {DIGIT}+
FLOAT       {DIGITS}\.{DIGITS}
NUMBER      ({DIGITS}|{FLOAT})
IDENTIFIER  [a-zA-Z_][a-zA-Z0-9_]*
WHITESPACE  [ \t\r]
NEWLINE     \n

SINGLE_COMMENT  \/\/[^\n]*
MULTI_COMMENT   \/\*([^*]|\*[^/])*\*\/

%%

{WHITESPACE}    { TRACK_LOCATION(); }
{NEWLINE}       { TRACK_LOCATION(); }

{MULTI_COMMENT} { TRACK_LOCATION(); }
{SINGLE_COMMENT} { TRACK_LOCATION(); }

"flow"          { TRACK_LOCATION(); return yy::parser::token::FLOW; }
"process"       { TRACK_LOCATION(); return yy::parser::token::PROCESS; }
"lib"           { TRACK_LOCATION(); return yy::parser::token::LIB; }
"module"        { TRACK_LOCATION(); return yy::parser::token::MODULE; }
"init"          { TRACK_LOCATION(); return yy::parser::token::INIT; }
"emit"          { TRACK_LOCATION(); return yy::parser::token::EMIT; }
"catch"         { TRACK_LOCATION(); return yy::parser::token::CATCH; }
"end"           { TRACK_LOCATION(); return yy::parser::token::END; }
"dt"            { TRACK_LOCATION(); return yy::parser::token::DT; }
"dur"           { TRACK_LOCATION(); return yy::parser::token::DUR; }

{NUMBER}"ms"    { yylval_string = yytext; TRACK_LOCATION(); return yy::parser::token::TIME_LITERAL; }
{NUMBER}"s"     { yylval_string = yytext; TRACK_LOCATION(); return yy::parser::token::TIME_LITERAL; }
{NUMBER}"hz"    { yylval_string = yytext; TRACK_LOCATION(); return yy::parser::token::TIME_LITERAL; }
{NUMBER}"b"     { yylval_string = yytext; TRACK_LOCATION(); return yy::parser::token::TIME_LITERAL; }

"\""[^"]*"\""   {
    std::string s = yytext;
    yylval_string = s.substr(1, s.length() - 2);
    TRACK_LOCATION();
    return yy::parser::token::STRING_LITERAL;
}

{NUMBER}        { yylval_string = yytext; TRACK_LOCATION(); return yy::parser::token::NUMBER; }

"_"             { TRACK_LOCATION(); return yy::parser::token::REST; }
{IDENTIFIER}    { yylval_string = yytext; TRACK_LOCATION(); return yy::parser::token::IDENTIFIER; }

"|>"             { TRACK_LOCATION(); return yy::parser::token::LAMBDA_BLOCK; }
"::"             { TRACK_LOCATION(); return yy::parser::token::NAMESPACE_DOT; }
"||"             { TRACK_LOCATION(); return yy::parser::token::RESTART_MARKER; }
".."             { TRACK_LOCATION(); return yy::parser::token::RANGE; }
"=="             { TRACK_LOCATION(); return yy::parser::token::EQ; }
"!="             { TRACK_LOCATION(); return yy::parser::token::NEQ; }
"<="             { TRACK_LOCATION(); return yy::parser::token::LE; }
">="             { TRACK_LOCATION(); return yy::parser::token::GE; }
"<<"             { TRACK_LOCATION(); return yy::parser::token::LSHIFT; }
">>"             { TRACK_LOCATION(); return yy::parser::token::RSHIFT; }
"@"             { TRACK_LOCATION(); return yy::parser::token::MEMORY_OP; }

"+"             { TRACK_LOCATION(); return yy::parser::token::PLUS; }
"-"             { TRACK_LOCATION(); return yy::parser::token::MINUS; }
"*"             { TRACK_LOCATION(); return yy::parser::token::MUL; }
"/"             { TRACK_LOCATION(); return yy::parser::token::DIV; }
"%"             { TRACK_LOCATION(); return yy::parser::token::MOD; }
"<"             { TRACK_LOCATION(); return yy::parser::token::LT; }
">"             { TRACK_LOCATION(); return yy::parser::token::GT; }
"&"             { TRACK_LOCATION(); return yy::parser::token::AND; }
"|"             { TRACK_LOCATION(); return yy::parser::token::OR; }
"^"             { TRACK_LOCATION(); return yy::parser::token::XOR; }
"~"             { TRACK_LOCATION(); return yy::parser::token::NOT; }
"?"             { TRACK_LOCATION(); return yy::parser::token::QUESTION; }
"="             { TRACK_LOCATION(); return yy::parser::token::ASSIGN; }
":"             { TRACK_LOCATION(); return yy::parser::token::COLON; }
";"             { TRACK_LOCATION(); return yy::parser::token::SEMICOLON; }
","             { TRACK_LOCATION(); return yy::parser::token::COMMA; }
"."             { TRACK_LOCATION(); return yy::parser::token::DOT; }
"!"             { TRACK_LOCATION(); return yy::parser::token::TRIGGER; }
"("             { TRACK_LOCATION(); return yy::parser::token::LPAREN; }
")"             { TRACK_LOCATION(); return yy::parser::token::RPAREN; }
"["             { TRACK_LOCATION(); return yy::parser::token::LBRACKET; }
"]"             { TRACK_LOCATION(); return yy::parser::token::RBRACKET; }
"{"             { TRACK_LOCATION(); return yy::parser::token::LBRACE; }
"}"             { TRACK_LOCATION(); return yy::parser::token::RBRACE; }

<<EOF>>        { TRACK_LOCATION(); return yy::parser::token::YYEOF; }

.               {
    std::cerr << "Unexpected character: " << yytext[0] << std::endl;
}

%%

yy::parser::symbol_type yylex_impl(yy::lexer& driver) {
    lexer_instance = &driver;
    
    // Initialize location on first call
    if (!location_initialized) {
        yylloc.initialize();
        location_initialized = true;
    }
    
    // Call yylex once - Flex internally handles non-returning rules (whitespace, comments)
    // Note: YYEOF == 0, so a while(token == 0) loop would infinite-loop on EOF
    int token = yylex();
    
    // Convert C lexer token to Bison symbol based on semantic value requirements
    switch(token) {
        case yy::parser::token::IDENTIFIER: 
            return yy::parser::make_IDENTIFIER(yylval_string, yylloc);
        case yy::parser::token::NUMBER: 
            return yy::parser::make_NUMBER(yylval_string, yylloc);
        case yy::parser::token::TIME_LITERAL: 
            return yy::parser::make_TIME_LITERAL(yylval_string, yylloc);
        case yy::parser::token::STRING_LITERAL: 
            return yy::parser::make_STRING_LITERAL(yylval_string, yylloc);
        case yy::parser::token::FLOW: 
            return yy::parser::make_FLOW(yylloc);
        case yy::parser::token::PROCESS: 
            return yy::parser::make_PROCESS(yylloc);
        case yy::parser::token::LIB: 
            return yy::parser::make_LIB(yylloc);
        case yy::parser::token::MODULE: 
            return yy::parser::make_MODULE(yylloc);
        case yy::parser::token::INIT: 
            return yy::parser::make_INIT(yylloc);
        case yy::parser::token::EMIT: 
            return yy::parser::make_EMIT(yylloc);
        case yy::parser::token::CATCH: 
            return yy::parser::make_CATCH(yylloc);
        case yy::parser::token::END: 
            return yy::parser::make_END(yylloc);
        case yy::parser::token::DT: 
            return yy::parser::make_DT(yylloc);
        case yy::parser::token::DUR: 
            return yy::parser::make_DUR(yylloc);
        case yy::parser::token::LAMBDA_BLOCK: 
            return yy::parser::make_LAMBDA_BLOCK(yylloc);
        case yy::parser::token::NAMESPACE_DOT: 
            return yy::parser::make_NAMESPACE_DOT(yylloc);
        case yy::parser::token::RANGE: 
            return yy::parser::make_RANGE(yylloc);
        case yy::parser::token::REST: 
            return yy::parser::make_REST(yylloc);
        case yy::parser::token::RESTART_MARKER: 
            return yy::parser::make_RESTART_MARKER(yylloc);
        case yy::parser::token::EQ: 
            return yy::parser::make_EQ(yylloc);
        case yy::parser::token::NEQ: 
            return yy::parser::make_NEQ(yylloc);
        case yy::parser::token::LE: 
            return yy::parser::make_LE(yylloc);
        case yy::parser::token::GE: 
            return yy::parser::make_GE(yylloc);
        case yy::parser::token::LSHIFT: 
            return yy::parser::make_LSHIFT(yylloc);
        case yy::parser::token::RSHIFT: 
            return yy::parser::make_RSHIFT(yylloc);
        case yy::parser::token::MEMORY_OP: 
            return yy::parser::make_MEMORY_OP(yylloc);
        case yy::parser::token::PLUS: 
            return yy::parser::make_PLUS(yylloc);
        case yy::parser::token::MINUS: 
            return yy::parser::make_MINUS(yylloc);
        case yy::parser::token::MUL: 
            return yy::parser::make_MUL(yylloc);
        case yy::parser::token::DIV: 
            return yy::parser::make_DIV(yylloc);
        case yy::parser::token::MOD: 
            return yy::parser::make_MOD(yylloc);
        case yy::parser::token::LT: 
            return yy::parser::make_LT(yylloc);
        case yy::parser::token::GT: 
            return yy::parser::make_GT(yylloc);
        case yy::parser::token::AND: 
            return yy::parser::make_AND(yylloc);
        case yy::parser::token::OR: 
            return yy::parser::make_OR(yylloc);
        case yy::parser::token::XOR: 
            return yy::parser::make_XOR(yylloc);
        case yy::parser::token::NOT: 
            return yy::parser::make_NOT(yylloc);
        case yy::parser::token::QUESTION: 
            return yy::parser::make_QUESTION(yylloc);
        case yy::parser::token::ASSIGN: 
            return yy::parser::make_ASSIGN(yylloc);
        case yy::parser::token::COLON: 
            return yy::parser::make_COLON(yylloc);
        case yy::parser::token::SEMICOLON: 
            return yy::parser::make_SEMICOLON(yylloc);
        case yy::parser::token::COMMA: 
            return yy::parser::make_COMMA(yylloc);
        case yy::parser::token::DOT: 
            return yy::parser::make_DOT(yylloc);
        case yy::parser::token::TRIGGER: 
            return yy::parser::make_TRIGGER(yylloc);
        case yy::parser::token::LPAREN: 
            return yy::parser::make_LPAREN(yylloc);
        case yy::parser::token::RPAREN: 
            return yy::parser::make_RPAREN(yylloc);
        case yy::parser::token::LBRACKET: 
            return yy::parser::make_LBRACKET(yylloc);
        case yy::parser::token::RBRACKET: 
            return yy::parser::make_RBRACKET(yylloc);
        case yy::parser::token::LBRACE: 
            return yy::parser::make_LBRACE(yylloc);
        case yy::parser::token::RBRACE: 
            return yy::parser::make_RBRACE(yylloc);
        case yy::parser::token::YYEOF: 
            return yy::parser::make_YYEOF(yylloc);
        default: 
            return yy::parser::make_YYEOF(yylloc);
    }
}

namespace yy {
    parser::symbol_type yylex(lexer& driver) {
        return yylex_impl(driver);
    }
}

// Reset the lexer state so the scanner can be reused for a new input stream.
// Must be called before parsing a different file (e.g. for library imports).
void reset_lexer_state() {
    location_initialized = false;
    lexer_instance = nullptr;
    // Flush the current Flex buffer so the next yylex() starts fresh
    YY_FLUSH_BUFFER;
}
