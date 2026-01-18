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
    IDENT_ASSIGN = 259,            /* IDENT_ASSIGN  */
    NUMBER = 260,                  /* NUMBER  */
    TIME = 261,                    /* TIME  */
    TRIGGER = 262,                 /* TRIGGER  */
    MODULE = 263,                  /* MODULE  */
    ASSIGN = 264,                  /* ASSIGN  */
    COLON = 265,                   /* COLON  */
    PARALLEL = 266,                /* PARALLEL  */
    ARROW = 267,                   /* ARROW  */
    UPDATE_ARROW = 268,            /* UPDATE_ARROW  */
    WHERE = 269,                   /* WHERE  */
    DUP = 270,                     /* DUP  */
    STOP = 271,                    /* STOP  */
    AFTER = 272,                   /* AFTER  */
    DOUBLEQUOTE = 273,             /* DOUBLEQUOTE  */
    QUOTE = 274,                   /* QUOTE  */
    PLUS = 275,                    /* PLUS  */
    MINUS = 276,                   /* MINUS  */
    MUL = 277,                     /* MUL  */
    DIV = 278,                     /* DIV  */
    MOD = 279,                     /* MOD  */
    POW = 280,                     /* POW  */
    LT = 281,                      /* LT  */
    GT = 282,                      /* GT  */
    LE = 283,                      /* LE  */
    GE = 284,                      /* GE  */
    EQ = 285,                      /* EQ  */
    NEQ = 286,                     /* NEQ  */
    AND = 287,                     /* AND  */
    OR = 288,                      /* OR  */
    LPAREN = 289,                  /* LPAREN  */
    RPAREN = 290,                  /* RPAREN  */
    LBRACE = 291,                  /* LBRACE  */
    RBRACE = 292,                  /* RBRACE  */
    LBRACKET = 293,                /* LBRACKET  */
    RBRACKET = 294,                /* RBRACKET  */
    COMMA = 295,                   /* COMMA  */
    SEMI = 296,                    /* SEMI  */
    EMIT = 297,                    /* EMIT  */
    DOT = 298,                     /* DOT  */
    TILDE = 299,                   /* TILDE  */
    QUESTION = 300,                /* QUESTION  */
    AT = 301,                      /* AT  */
    SPLIT = 302,                   /* SPLIT  */
    MERGE = 303,                   /* MERGE  */
    DOLLAR = 304                   /* DOLLAR  */
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
