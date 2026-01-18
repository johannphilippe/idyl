/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_HOME_JOHANN_DOCUMENTS_GIT_IDYL_SRC_PARSER_IDYL_PARSER_TAB_HH_INCLUDED
# define YY_YY_HOME_JOHANN_DOCUMENTS_GIT_IDYL_SRC_PARSER_IDYL_PARSER_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 7 "/home/johann/Documents/git/idyl/src/parser/idyl.y"

    #include "ast.hpp"

#line 53 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.hh"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IDENT = 258,                   /* IDENT  */
    NUMBER = 259,                  /* NUMBER  */
    TIME = 260,                    /* TIME  */
    TRIGGER = 261,                 /* TRIGGER  */
    STRING = 262,                  /* STRING  */
    MODULE = 263,                  /* MODULE  */
    ASSIGN = 264,                  /* ASSIGN  */
    COLON = 265,                   /* COLON  */
    ARROW = 266,                   /* ARROW  */
    UPDATE_ARROW = 267,            /* UPDATE_ARROW  */
    WHERE = 268,                   /* WHERE  */
    DUP = 269,                     /* DUP  */
    STOP = 270,                    /* STOP  */
    AFTER = 271,                   /* AFTER  */
    DOUBLEQUOTE = 272,             /* DOUBLEQUOTE  */
    QUOTE = 273,                   /* QUOTE  */
    PLUS = 274,                    /* PLUS  */
    MINUS = 275,                   /* MINUS  */
    MUL = 276,                     /* MUL  */
    DIV = 277,                     /* DIV  */
    MOD = 278,                     /* MOD  */
    POW = 279,                     /* POW  */
    LT = 280,                      /* LT  */
    GT = 281,                      /* GT  */
    LE = 282,                      /* LE  */
    GE = 283,                      /* GE  */
    EQ = 284,                      /* EQ  */
    NEQ = 285,                     /* NEQ  */
    AND = 286,                     /* AND  */
    OR = 287,                      /* OR  */
    LPAREN = 288,                  /* LPAREN  */
    RPAREN = 289,                  /* RPAREN  */
    LBRACE = 290,                  /* LBRACE  */
    RBRACE = 291,                  /* RBRACE  */
    LBRACKET = 292,                /* LBRACKET  */
    RBRACKET = 293,                /* RBRACKET  */
    COMMA = 294,                   /* COMMA  */
    SEMI = 295,                    /* SEMI  */
    EMIT = 296,                    /* EMIT  */
    DOT = 297,                     /* DOT  */
    TILDE = 298,                   /* TILDE  */
    QUESTION = 299,                /* QUESTION  */
    AT = 300,                      /* AT  */
    SPLIT = 301,                   /* SPLIT  */
    MERGE = 302,                   /* MERGE  */
    DOLLAR = 303                   /* DOLLAR  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef idyl::parser::node_ptr YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (void);


#endif /* !YY_YY_HOME_JOHANN_DOCUMENTS_GIT_IDYL_SRC_PARSER_IDYL_PARSER_TAB_HH_INCLUDED  */
