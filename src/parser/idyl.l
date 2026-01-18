%{
#include "idyl_parser.tab.hh"
#include "ast.hpp"
using namespace idyl::parser;
#include<string.h>
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
"+"      { yylval = std::make_shared<binop>("+"); return PLUS; }
"-"      { yylval = std::make_shared<binop>("-"); return MINUS; }
"*"      { yylval = std::make_shared<binop>("*"); return MUL; }
"/"      { yylval = std::make_shared<binop>("/"); return DIV; }
"%"      { yylval = std::make_shared<binop>("%"); return MOD; }
"^"      { yylval = std::make_shared<binop>("^"); return POW; }

"=="     { yylval = std::make_shared<binop>("=="); return EQ; }
"!="     { yylval = std::make_shared<binop>("!="); return NEQ; }
"<"      { yylval = std::make_shared<binop>("<"); return LT; }
">"      { yylval = std::make_shared<binop>(">"); return GT; }
"<="     { yylval = std::make_shared<binop>("<="); return LE; }
">="     { yylval = std::make_shared<binop>(">="); return GE; }

"&&"     { yylval = std::make_shared<binop>("&&"); return AND; }
"||"     { yylval = std::make_shared<binop>("||"); return OR; }


"?"             { RETURN( QUESTION ); }
"@"             { RETURN( AT ); }

"&&"            { RETURN( AND ); }
"||"            { RETURN( OR ); }

";"             { RETURN( SEMI ); }
","             { RETURN( COMMA ); }

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
    RETURN( NUMBER );
}

[A-Za-z_][A-Za-z0-9_]* {
    //printf("IDENT(%s)\n", yytext);
    auto id = std::make_shared<identifier>();
    id->name = yytext;
    yylval = id;
    RETURN( IDENT );
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