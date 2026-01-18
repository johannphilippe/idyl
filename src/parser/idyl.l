%{
#include "idyl_parser.tab.hh"
#include "ast.hpp"
using namespace idyl::parser;
#include<string.h>
#include<string>
#include<memory>
int yycolumn;
#define RETURN(tok) do { yycolumn += yyleng; /*fprintf(stderr, "token : %d \n", tok);*/ return tok; } while(0)
%}

%option yylineno

%%

\n              { yylineno++; yycolumn = 1; }
"="             { RETURN( ASSIGN ); }
":>" { 
    auto rt = std::make_shared<route_op>();
    rt->op = yytext;
    yylval = rt;
    RETURN( MERGE ); 
}
"<:" { 
    auto rt = std::make_shared<route_op>();
    rt->op = yytext;
    yylval = rt;
    RETURN( SPLIT ); 
}
"::"             { RETURN( PARALLEL ); }       // connection operator
":"             { RETURN( COLON ); }       // connection operator
"->"            { RETURN( ARROW ); }
"~>"            { RETURN( UPDATE_ARROW ); } // recurrence update
"~"             { RETURN( TILDE ); }       // feedback marker
"where"         { RETURN( WHERE ); }
"dup"           { RETURN( DUP ); }
"stop" { 
    auto ptr = std::make_shared<identifier>();
    ptr->name = yytext;
    yylval = ptr;
    RETURN( STOP ); 
}
"after" { 
    auto ptr = std::make_shared<identifier>();
    ptr->name = yytext;
    yylval = ptr;
    RETURN( AFTER ); 
}
"module" {
    RETURN (MODULE);
};

"$"      { RETURN(DOLLAR); }
"+"      { yylval = std::make_shared<binop>("+"); RETURN(PLUS); }
"-"      { yylval = std::make_shared<binop>("-"); RETURN(MINUS); }
"*"      { yylval = std::make_shared<binop>("*"); RETURN(MUL); }
"/"      { yylval = std::make_shared<binop>("/"); RETURN(DIV); }
"%"      { yylval = std::make_shared<binop>("%"); RETURN(MOD); }
"^"      { yylval = std::make_shared<binop>("^"); RETURN(POW); }

"=="     { yylval = std::make_shared<binop>("=="); RETURN(EQ); }
"!="     { yylval = std::make_shared<binop>("!="); RETURN(NEQ); }
"<"      { yylval = std::make_shared<binop>("<"); RETURN(LT); }
">"      { yylval = std::make_shared<binop>(">"); RETURN(GT); }
"<="     { yylval = std::make_shared<binop>("<="); RETURN(LE); }
">="     { yylval = std::make_shared<binop>(">="); RETURN(GE); }

"&&"     { yylval = std::make_shared<binop>("&&"); RETURN(AND); }
"||"     { yylval = std::make_shared<binop>("||"); RETURN(OR); }

"?"             { RETURN( QUESTION ); }
"@"             { RETURN( AT ); }

";"             { RETURN( SEMI ); }
","             { 
    
    std::cerr << "LEX: COMA value='" << yytext << "'\n"; /* debug */
    RETURN( COMMA ); 
    }

"("             { RETURN( LPAREN ); }
")"             { RETURN( RPAREN ); }
"{"             { RETURN( LBRACE ); }
"}"             { RETURN( RBRACE ); }
"["             { RETURN( LBRACKET ); }
"]"             { RETURN( RBRACKET ); }
"'"             { RETURN (QUOTE); }
"\""            { RETURN( DOUBLEQUOTE ); }

[0-9]+(\.[0-9]+)?("ms"|"millis"|"s"|"sec"|"m"|"minut"|"h"|"hour"|"beats"|"b"|"hz"|"Hz") {
    //printf("TIME(%s)\n", yytext);
    auto t = std::make_shared<time_prim>(); 
    t->value = yytext;
    yylval = t; 
    std::cerr << "LEX: TIME val='" << yytext << "'\n"; /* debug */
    RETURN( TIME );
}

[0-9]+(\.[0-9]+)?("trig"|"t") {
    auto t = std::make_shared<trigger_prim>();
    t->value = yytext;
    yylval = t;
    return(TRIGGER);
}

[0-9]+(\.[0-9]+)? {
    //printf("NUMBER(%s)\n", yytext);
    auto num = std::make_shared<number>();
    num->value = yytext;
    yylval = num; 
    std::cerr << "LEX: NUMBER value='" << yytext << "'\n"; /* debug */
    RETURN( NUMBER );
}

([A-Za-z_][A-Za-z0-9_]*)([ \t\r\n]*)=  {
    /* Check if next char is '=' (which would make it ==) */
    int c = yyinput();
    if (c == '=') {
        /* It's ==, not an assignment. Put both chars back and match as IDENT */
        unput('=');
        unput('=');
        /* Extract just the identifier part */
        std::string matched(yytext);
        size_t i = 0;
        while (i < matched.size() && !isspace((unsigned char)matched[i]) && matched[i] != '=') ++i;
        std::string name = matched.substr(0, i);
        
        auto ptr = std::make_shared<identifier>();
        ptr->name = name;
        yylval = ptr;
        yycolumn += i;
        return IDENT;
    }
    
    /* Not ==, it's a real assignment */
    unput(c);  /* Put back the char we peeked at */
    
    std::string matched(yytext);
    size_t i = 0;
    while (i < matched.size() && !isspace((unsigned char)matched[i]) && matched[i] != '=') ++i;
    std::string name = matched.substr(0, i);

    auto ptr = std::make_shared<identifier>();
    ptr->name = name;
    yylval = ptr;
    std::cerr << "LEX: IDENT ASSIGN name='" << name << "'\n";
    yycolumn += yyleng;
    return IDENT_ASSIGN;
}

[A-Za-z_][A-Za-z0-9_]*  {
    /* push back the char we consumed (if not EOF) */
    auto ptr = std::make_shared<identifier>();
    std::string name(yytext);
    ptr->name = name;
    yylval = ptr;
    /* inside the IDENT rule, before return */
    std::cerr << "LEX: IDENT name='" << name << "'\n";
    RETURN(IDENT);
}


[A-Za-z_][A-Za-z0-9_]*"!" {
    //printf("EMIT(%s)\n", yytext);
    auto em = std::make_shared<emit>();
    em->name = yytext;
    yylval = em; 
    RETURN( EMIT );
}

"." {
    RETURN(DOT);
}

[ \t\r\n]+      { /* skip */ }
"//".*          { /* skip line comment */ }
"/*"([^*]|(\*+[^*/]))*"*/" { /* skip block comment */ }

.             { yycolumn++; return *yytext; }

%%

int yywrap(void) { return 1; }

void yyerror(const char *str)
{
    fprintf(stderr, "Error at line %d char %d  \n%s\n", yylineno, yycolumn, yytext);
}