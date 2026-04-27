// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison GLR parsers in C

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// C++ GLR parser skeleton written by Valentin Tolmer.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "glr2.cc"







# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "idyl.tab.hh"

namespace
{
  /* Default (constant) value used for initialization for null
     right-hand sides.  Unlike the standard yacc.c template, here we set
     the default value of $$ to a zeroed-out value.  Since the default
     value is undefined, this behavior is technically correct.  */
  yy::parser::value_type yyval_default;
}


// Unqualified %code blocks.
#line 26 "/home/johann/Documents/git/idyl/src/parser/idyl.y"

    #include <iostream>
    #include <sstream>
    //#include "idyl_lexer.hpp"
    
    namespace yy {
        parser::symbol_type yylex(lexer& driver);
    }
    
    std::shared_ptr<idyl::parser::program> g_program = nullptr;

#line 92 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"


#include <cstdio>
#include <cstdlib>

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#ifndef YYFREE
# define YYFREE free
#endif
#ifndef YYMALLOC
# define YYMALLOC malloc
#endif

#ifndef YYSETJMP
# include <setjmp.h>
# define YYJMP_BUF jmp_buf
# define YYSETJMP(Env) setjmp (Env)
/* Pacify Clang and ICC.  */
# define YYLONGJMP(Env, Val)                    \
 do {                                           \
   longjmp (Env, Val);                          \
   YYASSERT (0);                                \
 } while (false)
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* The _Noreturn keyword of C11.  */
#ifndef _Noreturn
# if (defined __cplusplus \
      && ((201103 <= __cplusplus && !(__GNUC__ == 4 && __GNUC_MINOR__ == 7)) \
          || (defined _MSC_VER && 1900 <= _MSC_VER)))
#  define _Noreturn [[noreturn]]
# elif ((!defined __cplusplus || defined __clang__) \
        && (201112 <= (defined __STDC_VERSION__ ? __STDC_VERSION__ : 0) \
            || (!defined __STRICT_ANSI__ \
                && (4 < __GNUC__ + (7 <= __GNUC_MINOR__) \
                    || (defined __apple_build_version__ \
                        ? 6000000 <= __apple_build_version__ \
                        : 3 < __clang_major__ + (5 <= __clang_minor__))))))
   /* _Noreturn works as-is.  */
# elif (2 < __GNUC__ + (8 <= __GNUC_MINOR__) || defined __clang__ \
        || 0x5110 <= __SUNPRO_C)
#  define _Noreturn __attribute__ ((__noreturn__))
# elif 1200 <= (defined _MSC_VER ? _MSC_VER : 0)
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#if defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_NULL_DEREFERENCE_BEGIN                               \
  _Pragma ("GCC diagnostic push")                                       \
  _Pragma ("GCC diagnostic ignored \"-Wnull-dereference\"")
# define YY_IGNORE_NULL_DEREFERENCE_END         \
  _Pragma ("GCC diagnostic pop")
#else
# define YY_IGNORE_NULL_DEREFERENCE_BEGIN
# define YY_IGNORE_NULL_DEREFERENCE_END
#endif

# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif
# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif

// FIXME: Use the same conventions as lalr1.cc.

#ifndef YYASSERT
# define YYASSERT(Condition) ((void) ((Condition) || (abort (), 0)))
#endif

#ifdef YYDEBUG
# define YYDASSERT(Condition) YYASSERT(Condition)
#else
# define YYDASSERT(Condition)
#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  20
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1274

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  58
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  44
/* YYNRULES -- Number of rules.  */
#define YYNRULES  137
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  302
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 10
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

namespace
{
#if YYDEBUG
  /* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
  const short yyrline[] =
  {
       0,   115,   115,   122,   131,   136,   144,   145,   146,   154,
     163,   171,   183,   184,   188,   198,   209,   220,   232,   242,
     256,   271,   286,   296,   309,   319,   330,   340,   354,   366,
     370,   375,   392,   401,   405,   423,   434,   446,   460,   465,
     473,   522,   532,   542,   546,   550,   554,   558,   562,   573,
     582,   594,   603,   615,   626,   637,   647,   660,   669,   678,
     689,   700,   705,   713,   752,   762,   772,   782,   793,   798,
     802,   811,   820,   829,   842,   846,   850,   851,   879,   884,
     891,   892,   909,   910,   927,   928,   945,   946,   963,   964,
     981,   982,   996,  1013,  1014,  1028,  1042,  1056,  1073,  1074,
    1088,  1105,  1106,  1120,  1137,  1138,  1152,  1166,  1183,  1184,
    1197,  1210,  1222,  1238,  1239,  1252,  1265,  1278,  1292,  1306,
    1320,  1336,  1348,  1360,  1389,  1400,  1411,  1423,  1435,  1447,
    1459,  1467,  1468,  1478,  1483,  1487,  1496,  1505
  };
#endif

#define YYPACT_NINF -202
#define YYTABLE_NINF -1

  // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
// STATE-NUM.
  const short yypact[] =
  {
     136,   -30,     9,    10,   -17,   -14,   129,   136,  -202,  -202,
    -202,  -202,  -202,   923,     8,     3,   -26,    43,   100,   125,
    -202,  -202,  -202,  -202,  -202,  -202,   119,   126,  -202,  -202,
    -202,  -202,   132,  -202,  1134,  1134,  -202,  1134,  1163,  -202,
    -202,   137,  -202,  -202,    -3,   149,   113,   127,   154,    31,
      38,    66,    91,   111,  -202,    -2,  -202,    36,   161,   162,
      69,  -202,    32,    11,   153,   193,   725,   159,   160,   207,
     209,  1134,  -202,  -202,   165,   174,    28,  -202,   167,  -202,
    1134,  1134,  1134,  1134,  1134,  1134,  1134,  1134,  1134,  1134,
    1134,  1134,  1134,  1134,  1134,  1134,  1134,  1134,  1134,   217,
     221,   995,  1134,  1134,  -202,  1134,  1134,    11,   183,   226,
    -202,   101,   725,   185,   186,   229,    97,   230,   231,   187,
    1134,    27,  -202,  -202,  -202,  -202,  -202,  -202,  -202,    46,
    -202,  -202,   184,   188,   103,  -202,  1134,  1034,  -202,   966,
     149,   190,   200,  -202,   127,  -202,    31,    38,    38,    66,
      66,    66,    66,   214,   214,   111,   111,  -202,  -202,  -202,
     191,  -202,   198,   199,  -202,  -202,   106,  -202,   192,  -202,
    -202,   137,  -202,  1134,   -29,     1,   202,   285,  1134,  1134,
     203,    84,  -202,  -202,  1134,   204,  -202,  -202,  1134,  -202,
    -202,  1134,  -202,   225,  -202,  1192,  -202,   208,   210,   242,
     213,   768,   810,  -202,  -202,    83,  1134,  1063,  1134,  1134,
    1221,  -202,  -202,  -202,   137,   201,   252,   -28,  -202,    41,
    -202,   215,  -202,  1134,    24,   212,   340,  -202,   216,  1134,
     205,  1134,   219,   220,  1134,  -202,   852,  -202,  -202,  1134,
     200,  -202,   116,  -202,  -202,  -202,  -202,  -202,   223,   201,
     257,   226,  -202,   227,  -202,    79,   224,   232,   725,  -202,
    -202,   234,  -202,  -202,  1105,  1134,  -202,  -202,  -202,  -202,
     201,  -202,   235,     6,   725,   228,   237,   395,   450,  1134,
     894,  -202,  -202,   201,  -202,   505,   725,   725,   725,  -202,
    -202,   218,  -202,  -202,  -202,   560,   615,   670,  -202,  -202,
    -202,  -202
  };

  // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
// Performed when YYTABLE does not specify something else to do.  Zero
// means the default is an error.
  const unsigned char yydefact[] =
  {
       3,     0,     0,     0,     0,     0,     0,     2,     5,     6,
      12,    13,     7,     0,     0,     0,     0,     0,     0,     0,
       1,     4,   121,   122,   123,   129,     0,     0,   126,   127,
     132,   128,     0,   125,     0,     0,   124,     0,     0,   131,
      29,    18,    74,    75,    76,    80,    82,    84,    86,    88,
      90,    93,    98,   101,   104,   108,   113,    70,     0,     0,
       0,    69,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   110,   109,     0,   121,     0,    33,     0,    19,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    71,     0,     0,     0,     0,     0,
      20,     0,     0,     0,   121,     0,     0,   132,     0,     0,
       0,     0,    39,    46,    47,    43,    44,    45,    48,   108,
       8,    10,     0,     0,     0,   130,     0,     0,    28,     0,
      81,    77,    79,    83,    85,    87,    89,    91,    92,    94,
      95,    96,    97,    99,   100,   102,   103,   105,   106,   107,
     117,   116,   121,   126,   115,   135,     0,   134,     0,    72,
      73,    15,    68,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    53,    54,     0,     0,    35,    38,     0,     9,
      11,     0,   111,     0,    32,     0,    30,   121,     0,     0,
     126,     0,     0,    62,    67,   108,     0,     0,     0,     0,
       0,   114,   120,    17,    14,     0,     0,     0,    22,     0,
      37,     0,    41,     0,     0,     0,     0,    40,     0,     0,
     122,     0,     0,     0,     0,    58,     0,    59,    61,     0,
      78,   119,     0,   136,   137,   133,    16,    26,     0,     0,
       0,     0,    21,     0,    42,   117,     0,     0,     0,    52,
     112,     0,    31,    64,     0,     0,    66,    57,    63,   118,
       0,    24,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    65,    27,     0,    23,     0,     0,     0,     0,    50,
      51,     0,    60,    25,    36,     0,     0,     0,    34,    55,
      56,    49
  };

  // YYPGOTO[NTERM-NUM].
  const short yypgoto[] =
  {
    -202,  -202,  -202,   255,  -202,  -202,  -202,    20,   -56,  -202,
    -202,  -202,  -102,  -116,  -202,  -202,  -202,  -202,  -202,  -155,
    -202,  -198,  -201,   236,   168,   -13,  -202,  -202,  -202,   -79,
     238,   211,   222,   233,   239,    87,   114,    98,   104,   -27,
      -1,  -202,    99,   102
  };

  // YYDEFGOTO[NTERM-NUM].
  const unsigned char yydefgoto[] =
  {
       0,     6,     7,     8,     9,    10,    11,   175,    39,    76,
      40,    12,   121,   122,   123,   124,   125,   126,   127,    79,
     201,   202,   203,    60,    61,   128,    42,    43,   141,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,   166,   167
  };

  // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
// positive, shift that token.  If negative, reduce the rule whose
// number is the opposite.  If YYTABLE_NINF, syntax error.
  const short yytable[] =
  {
      41,   238,   142,   236,   217,   187,   110,    72,    73,   217,
     177,    57,    15,    16,    57,    13,   213,   215,   249,    99,
      64,    14,    65,    58,    74,    77,    58,   255,   216,   250,
     114,    23,    24,    25,    18,   238,    80,    19,   256,   115,
     116,    81,    28,    29,   117,   118,    31,   100,    62,   101,
      32,   102,    33,   119,    63,    34,    17,   218,   134,   246,
      59,   187,   284,    86,    87,   129,   280,    99,    35,   157,
     158,   159,    88,    89,    90,    91,   137,    36,    37,   238,
      38,   103,   138,   186,   120,    38,   104,   109,   165,   168,
     169,   188,   170,   171,    38,   100,   251,   101,    66,   102,
      22,    23,    24,    25,    99,   224,    67,   185,    92,    93,
     259,   129,    28,    29,    30,   181,    31,   107,    94,    95,
     129,   108,    33,   193,   196,   275,   204,   240,   239,    20,
     207,    68,   100,   100,   101,   101,   102,   102,   205,     1,
      96,    97,    98,     2,     3,     4,     5,    36,    37,   107,
      38,   191,    83,   176,   210,   192,   278,    78,   211,   247,
     214,   289,   187,   252,   210,   221,   222,    84,   269,   187,
      69,   225,   285,   147,   148,   227,   129,    70,   228,   187,
     187,   187,    77,    71,   295,   296,   297,    82,   204,   204,
     153,   154,    85,   271,   165,   243,   244,   165,   155,   156,
     205,   205,   149,   150,   151,   152,   105,   106,   112,   113,
     254,   130,   131,   132,   282,   133,   261,   135,   263,   136,
     160,   266,   139,   204,   161,   129,   268,   293,   173,   174,
     178,   179,   180,   182,   183,   205,   189,   206,   184,    80,
     190,    94,   207,   208,   209,   233,   212,   219,   223,   229,
     226,   204,   281,   231,    38,   248,   232,   129,   234,   262,
     272,   253,    21,   205,   257,   265,   291,   204,   260,   270,
     276,   273,   298,   129,   264,   172,   129,   129,   277,   205,
     279,   283,   274,   286,   129,   129,   129,   129,   114,    23,
      24,    25,   287,   143,   129,   129,   129,   115,   116,   111,
      28,    29,   117,   118,    31,   144,   242,     0,    32,     0,
      33,   119,   245,    34,     0,     0,     0,   145,   140,     0,
       0,     0,     0,     0,   146,     0,    35,     0,     0,     0,
       0,     0,     0,     0,     0,    36,    37,     0,    38,     0,
       0,   220,   120,   114,    23,    24,    25,     0,     0,     0,
       0,     0,   115,   116,     0,    28,    29,   117,   118,    31,
       0,     0,     0,    32,     0,    33,   119,     0,    34,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    35,     0,     0,     0,     0,     0,     0,     0,     0,
      36,    37,     0,    38,     0,   258,     0,   120,   114,    23,
      24,    25,     0,     0,     0,     0,     0,   115,   116,     0,
      28,    29,   117,   118,    31,     0,     0,     0,    32,     0,
      33,   119,     0,    34,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    35,     0,     0,     0,
       0,     0,     0,     0,     0,    36,    37,     0,    38,     0,
     288,     0,   120,   114,    23,    24,    25,     0,     0,     0,
       0,     0,   115,   116,     0,    28,    29,   117,   118,    31,
       0,     0,     0,    32,     0,    33,   119,     0,    34,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    35,     0,     0,     0,     0,     0,     0,     0,     0,
      36,    37,     0,    38,     0,     0,   290,   120,   114,    23,
      24,    25,     0,     0,     0,     0,     0,   115,   116,     0,
      28,    29,   117,   118,    31,     0,     0,     0,    32,     0,
      33,   119,     0,    34,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    35,     0,     0,     0,
       0,     0,     0,     0,     0,    36,    37,     0,    38,     0,
       0,   294,   120,   114,    23,    24,    25,     0,     0,     0,
       0,     0,   115,   116,     0,    28,    29,   117,   118,    31,
       0,     0,     0,    32,     0,    33,   119,     0,    34,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    35,     0,     0,     0,     0,     0,     0,     0,     0,
      36,    37,     0,    38,     0,     0,   299,   120,   114,    23,
      24,    25,     0,     0,     0,     0,     0,   115,   116,     0,
      28,    29,   117,   118,    31,     0,     0,     0,    32,     0,
      33,   119,     0,    34,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    35,     0,     0,     0,
       0,     0,     0,     0,     0,    36,    37,     0,    38,     0,
       0,   300,   120,   114,    23,    24,    25,     0,     0,     0,
       0,     0,   115,   116,     0,    28,    29,   117,   118,    31,
       0,     0,     0,    32,     0,    33,   119,     0,    34,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    35,     0,     0,     0,     0,     0,     0,     0,     0,
      36,    37,     0,    38,     0,     0,   301,   120,   114,    23,
      24,    25,     0,     0,     0,     0,     0,   115,   116,     0,
      28,    29,   117,   118,    31,     0,     0,     0,    32,     0,
      33,   119,     0,    34,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    35,     0,     0,     0,
       0,   197,    23,    24,    25,    36,    37,     0,    38,     0,
     199,     0,   120,   200,    29,    30,     0,    31,     0,     0,
       0,    32,     0,    33,     0,     0,    34,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    35,
       0,     0,     0,   197,    23,    24,    25,     0,    36,    37,
       0,    38,   199,     0,   235,   200,    29,    30,     0,    31,
       0,     0,     0,    32,     0,    33,     0,     0,    34,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    35,     0,     0,     0,   197,    23,    24,    25,     0,
      36,    37,     0,    38,   199,     0,   237,   200,    29,    30,
       0,    31,     0,     0,     0,    32,     0,    33,     0,     0,
      34,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    35,     0,     0,     0,   197,    23,    24,
      25,     0,    36,    37,     0,    38,   199,     0,   267,   200,
      29,    30,     0,    31,     0,     0,     0,    32,     0,    33,
       0,     0,    34,     0,     0,     0,    22,    23,    24,    25,
       0,     0,    26,    27,     0,    35,     0,     0,    28,    29,
      30,     0,    31,     0,    36,    37,    32,    38,    33,     0,
     292,    34,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    35,     0,     0,     0,     0,   197,
      23,    24,    25,    36,    37,     0,    38,   198,   199,     0,
       0,   200,    29,    30,     0,    31,     0,     0,     0,    32,
       0,    33,     0,     0,    34,     0,     0,     0,   162,    23,
      24,    25,     0,     0,     0,     0,     0,    35,     0,     0,
     163,    29,    30,     0,    31,     0,    36,    37,    32,    38,
      33,     0,     0,    34,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    35,    22,    23,    24,
      25,     0,     0,     0,     0,    36,    37,   164,    38,    28,
      29,    30,     0,    31,     0,     0,   194,    32,     0,    33,
       0,     0,    34,     0,     0,     0,   162,    23,    24,    25,
       0,     0,     0,     0,     0,    35,     0,     0,   163,    29,
      30,     0,    31,     0,    36,    37,    32,   195,    33,     0,
       0,    34,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    35,     0,     0,     0,   197,    23,
      24,    25,     0,    36,    37,   241,    38,   199,     0,     0,
     200,    29,    30,     0,    31,     0,     0,     0,    32,     0,
      33,     0,     0,    34,     0,     0,     0,    22,    23,    24,
      25,     0,     0,     0,     0,     0,    35,     0,     0,    28,
      29,    30,     0,    31,     0,    36,    37,    32,    38,    33,
       0,     0,    34,     0,     0,     0,    75,    23,    24,    25,
       0,     0,     0,     0,     0,    35,     0,     0,    28,    29,
      30,     0,    31,     0,    36,    37,    32,    38,    33,     0,
       0,    34,     0,     0,     0,    75,   230,    24,    25,     0,
       0,     0,     0,     0,    35,     0,     0,    28,    29,    30,
       0,    31,     0,    36,    37,    32,    38,    33,     0,     0,
      34,     0,     0,     0,   162,    23,    24,    25,     0,     0,
       0,     0,     0,    35,     0,     0,   163,    29,    30,     0,
      31,     0,    36,    37,    32,    38,    33,     0,     0,    34,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    35,     0,     0,     0,     0,     0,     0,     0,
       0,    36,    37,     0,    38
  };

  const short yycheck[] =
  {
      13,   202,    81,   201,     3,   121,    62,    34,    35,     3,
     112,     3,     3,     3,     3,    45,   171,    46,    46,    21,
      46,    51,    48,    15,    37,    38,    15,     3,    57,    57,
       3,     4,     5,     6,    51,   236,    39,    51,    14,    12,
      13,    44,    15,    16,    17,    18,    19,    49,    45,    51,
      23,    53,    25,    26,    51,    28,    46,    56,    71,   214,
      52,   177,    56,    32,    33,    66,   264,    21,    41,    96,
      97,    98,    34,    35,    36,    37,    48,    50,    51,   280,
      53,    45,    54,    56,    57,    53,    50,    55,   101,   102,
     103,    45,   105,   106,    53,    49,    55,    51,    55,    53,
       3,     4,     5,     6,    21,    21,     6,   120,    42,    43,
     226,   112,    15,    16,    17,   116,    19,    48,    27,    28,
     121,    52,    25,   136,   137,    46,   139,   206,    45,     0,
      51,     6,    49,    49,    51,    51,    53,    53,   139,     3,
      29,    30,    31,     7,     8,     9,    10,    50,    51,    48,
      53,    48,    39,    52,    48,    52,   258,    20,    52,   215,
     173,   277,   278,   219,    48,   178,   179,    40,    52,   285,
      51,   184,   274,    86,    87,   188,   177,    51,   191,   295,
     296,   297,   195,    51,   286,   287,   288,    38,   201,   202,
      92,    93,    38,   249,   207,   208,   209,   210,    94,    95,
     201,   202,    88,    89,    90,    91,    45,    45,    55,    16,
     223,    52,    52,     6,   270,     6,   229,    52,   231,    45,
       3,   234,    55,   236,     3,   226,   239,   283,    45,     3,
      45,    45,     3,     3,     3,   236,    52,    47,    51,    39,
      52,    27,    51,    45,    45,     3,    54,    45,    45,    24,
      46,   264,   265,    45,    53,     3,    46,   258,    45,    54,
       3,    46,     7,   264,    52,    45,   279,   280,    52,    46,
      46,   251,    54,   274,    55,   107,   277,   278,    46,   280,
      46,    46,    55,    55,   285,   286,   287,   288,     3,     4,
       5,     6,    55,    82,   295,   296,   297,    12,    13,    63,
      15,    16,    17,    18,    19,    83,   207,    -1,    23,    -1,
      25,    26,   210,    28,    -1,    -1,    -1,    84,    80,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    41,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    51,    -1,    53,    -1,
      -1,    56,    57,     3,     4,     5,     6,    -1,    -1,    -1,
      -1,    -1,    12,    13,    -1,    15,    16,    17,    18,    19,
      -1,    -1,    -1,    23,    -1,    25,    26,    -1,    28,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      50,    51,    -1,    53,    -1,    55,    -1,    57,     3,     4,
       5,     6,    -1,    -1,    -1,    -1,    -1,    12,    13,    -1,
      15,    16,    17,    18,    19,    -1,    -1,    -1,    23,    -1,
      25,    26,    -1,    28,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    41,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    51,    -1,    53,    -1,
      55,    -1,    57,     3,     4,     5,     6,    -1,    -1,    -1,
      -1,    -1,    12,    13,    -1,    15,    16,    17,    18,    19,
      -1,    -1,    -1,    23,    -1,    25,    26,    -1,    28,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      50,    51,    -1,    53,    -1,    -1,    56,    57,     3,     4,
       5,     6,    -1,    -1,    -1,    -1,    -1,    12,    13,    -1,
      15,    16,    17,    18,    19,    -1,    -1,    -1,    23,    -1,
      25,    26,    -1,    28,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    41,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    51,    -1,    53,    -1,
      -1,    56,    57,     3,     4,     5,     6,    -1,    -1,    -1,
      -1,    -1,    12,    13,    -1,    15,    16,    17,    18,    19,
      -1,    -1,    -1,    23,    -1,    25,    26,    -1,    28,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      50,    51,    -1,    53,    -1,    -1,    56,    57,     3,     4,
       5,     6,    -1,    -1,    -1,    -1,    -1,    12,    13,    -1,
      15,    16,    17,    18,    19,    -1,    -1,    -1,    23,    -1,
      25,    26,    -1,    28,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    41,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    51,    -1,    53,    -1,
      -1,    56,    57,     3,     4,     5,     6,    -1,    -1,    -1,
      -1,    -1,    12,    13,    -1,    15,    16,    17,    18,    19,
      -1,    -1,    -1,    23,    -1,    25,    26,    -1,    28,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      50,    51,    -1,    53,    -1,    -1,    56,    57,     3,     4,
       5,     6,    -1,    -1,    -1,    -1,    -1,    12,    13,    -1,
      15,    16,    17,    18,    19,    -1,    -1,    -1,    23,    -1,
      25,    26,    -1,    28,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    41,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,    50,    51,    -1,    53,    -1,
      12,    -1,    57,    15,    16,    17,    -1,    19,    -1,    -1,
      -1,    23,    -1,    25,    -1,    -1,    28,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,
      -1,    -1,    -1,     3,     4,     5,     6,    -1,    50,    51,
      -1,    53,    12,    -1,    56,    15,    16,    17,    -1,    19,
      -1,    -1,    -1,    23,    -1,    25,    -1,    -1,    28,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    41,    -1,    -1,    -1,     3,     4,     5,     6,    -1,
      50,    51,    -1,    53,    12,    -1,    56,    15,    16,    17,
      -1,    19,    -1,    -1,    -1,    23,    -1,    25,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    41,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,    50,    51,    -1,    53,    12,    -1,    56,    15,
      16,    17,    -1,    19,    -1,    -1,    -1,    23,    -1,    25,
      -1,    -1,    28,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    -1,     9,    10,    -1,    41,    -1,    -1,    15,    16,
      17,    -1,    19,    -1,    50,    51,    23,    53,    25,    -1,
      56,    28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    41,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,    50,    51,    -1,    53,    11,    12,    -1,
      -1,    15,    16,    17,    -1,    19,    -1,    -1,    -1,    23,
      -1,    25,    -1,    -1,    28,    -1,    -1,    -1,     3,     4,
       5,     6,    -1,    -1,    -1,    -1,    -1,    41,    -1,    -1,
      15,    16,    17,    -1,    19,    -1,    50,    51,    23,    53,
      25,    -1,    -1,    28,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    41,     3,     4,     5,
       6,    -1,    -1,    -1,    -1,    50,    51,    52,    53,    15,
      16,    17,    -1,    19,    -1,    -1,    22,    23,    -1,    25,
      -1,    -1,    28,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    -1,    -1,    41,    -1,    -1,    15,    16,
      17,    -1,    19,    -1,    50,    51,    23,    53,    25,    -1,
      -1,    28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    41,    -1,    -1,    -1,     3,     4,
       5,     6,    -1,    50,    51,    52,    53,    12,    -1,    -1,
      15,    16,    17,    -1,    19,    -1,    -1,    -1,    23,    -1,
      25,    -1,    -1,    28,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    -1,    -1,    41,    -1,    -1,    15,
      16,    17,    -1,    19,    -1,    50,    51,    23,    53,    25,
      -1,    -1,    28,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    -1,    -1,    41,    -1,    -1,    15,    16,
      17,    -1,    19,    -1,    50,    51,    23,    53,    25,    -1,
      -1,    28,    -1,    -1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    -1,    -1,    41,    -1,    -1,    15,    16,    17,
      -1,    19,    -1,    50,    51,    23,    53,    25,    -1,    -1,
      28,    -1,    -1,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    -1,    -1,    41,    -1,    -1,    15,    16,    17,    -1,
      19,    -1,    50,    51,    23,    53,    25,    -1,    -1,    28,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    50,    51,    -1,    53
  };

  // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
// state STATE-NUM.
  const signed char yystos[] =
  {
       0,     3,     7,     8,     9,    10,    59,    60,    61,    62,
      63,    64,    69,    45,    51,     3,     3,    46,    51,    51,
       0,    61,     3,     4,     5,     6,     9,    10,    15,    16,
      17,    19,    23,    25,    28,    41,    50,    51,    53,    66,
      68,    83,    84,    85,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,     3,    15,    52,
      81,    82,    45,    51,    46,    48,    55,     6,     6,    51,
      51,    51,    97,    97,    83,     3,    67,    83,    20,    77,
      39,    44,    38,    39,    40,    38,    32,    33,    34,    35,
      36,    37,    42,    43,    27,    28,    29,    30,    31,    21,
      49,    51,    53,    45,    50,    45,    45,    48,    52,    55,
      66,    81,    55,    16,     3,    12,    13,    17,    18,    26,
      57,    70,    71,    72,    73,    74,    75,    76,    83,    98,
      52,    52,     6,     6,    83,    52,    45,    48,    54,    55,
      88,    86,    87,    89,    90,    91,    92,    93,    93,    94,
      94,    94,    94,    95,    95,    96,    96,    97,    97,    97,
       3,     3,     3,    15,    52,    83,   100,   101,    83,    83,
      83,    83,    82,    45,     3,    65,    52,    70,    45,    45,
       3,    98,     3,     3,    51,    83,    56,    71,    45,    52,
      52,    48,    52,    83,    22,    53,    83,     3,    11,    12,
      15,    78,    79,    80,    83,    98,    47,    51,    45,    45,
      48,    52,    54,    77,    83,    46,    57,     3,    56,    45,
      56,    83,    83,    45,    21,    83,    46,    83,    83,    24,
       4,    45,    46,     3,    45,    56,    79,    56,    80,    45,
      87,    52,   100,    83,    83,   101,    77,    66,     3,    46,
      57,    55,    66,    46,    83,     3,    14,    52,    55,    71,
      52,    83,    54,    83,    55,    45,    83,    56,    83,    52,
      46,    66,     3,    65,    55,    46,    46,    46,    70,    46,
      79,    83,    66,    46,    56,    70,    55,    55,    55,    71,
      56,    83,    56,    66,    56,    70,    70,    70,    54,    56,
      56,    56
  };

  // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
  const signed char yyr1[] =
  {
       0,    58,    59,    59,    60,    60,    61,    61,    61,    61,
      61,    61,    62,    62,    63,    63,    63,    63,    63,    63,
      64,    64,    64,    64,    65,    65,    65,    65,    66,    66,
      67,    67,    67,    67,    68,    69,    69,    69,    70,    70,
      71,    71,    71,    71,    71,    71,    71,    71,    71,    72,
      72,    73,    73,    74,    75,    76,    76,    77,    77,    77,
      78,    79,    79,    80,    80,    80,    80,    80,    81,    81,
      82,    82,    82,    82,    83,    84,    85,    85,    86,    86,
      87,    87,    88,    88,    89,    89,    90,    90,    91,    91,
      92,    92,    92,    93,    93,    93,    93,    93,    94,    94,
      94,    95,    95,    95,    96,    96,    96,    96,    97,    97,
      97,    97,    97,    98,    98,    98,    98,    98,    98,    98,
      98,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,   100,   100,   101,   101,   101
  };

  // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
  const signed char yyr2[] =
  {
       0,     2,     1,     0,     2,     1,     1,     1,     4,     6,
       4,     6,     1,     1,     6,     5,     7,     6,     3,     4,
       4,     7,     6,     9,     4,     6,     3,     5,     3,     1,
       3,     5,     3,     1,     9,     5,    10,     6,     2,     1,
       3,     3,     4,     1,     1,     1,     1,     1,     1,     8,
       6,     6,     4,     2,     2,     8,     8,     5,     4,     4,
       5,     2,     1,     3,     3,     4,     3,     1,     3,     1,
       1,     2,     3,     3,     1,     1,     1,     3,     3,     1,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     3,     1,     3,     3,     3,     3,     1,     3,
       3,     1,     3,     3,     1,     3,     3,     3,     1,     2,
       2,     4,     6,     1,     4,     3,     3,     3,     6,     5,
       4,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     3,     1,     1,     3,     3
  };


  /* YYDPREC[RULE-NUM] -- Dynamic precedence of rule #RULE-NUM (0 if none).  */
  const signed char yydprec[] =
  {
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0
  };

  /* YYMERGER[RULE-NUM] -- Index of merging function for rule #RULE-NUM.  */
  const signed char yymerger[] =
  {
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0
  };

  /* YYIMMEDIATE[RULE-NUM] -- True iff rule #RULE-NUM is not to be deferred, as
     in the case of predicates.  */
  const bool yyimmediate[] =
  {
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0
  };

  /* YYCONFLP[YYPACT[STATE-NUM]] -- Pointer into YYCONFL of start of
     list of conflicting reductions corresponding to action entry for
     state STATE-NUM in yytable.  0 means no conflicts.  The list in
     yyconfl is terminated by a rule number of 0.  */
  const signed char yyconflp[] =
  {
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0
  };

  /* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
     0, pointed into by YYCONFLP.  */
        const short yyconfl[] =
  {
       0
  };
} // namespace


/* Error token number */
#define YYTERROR 1


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif

# define YYRHSLOC(Rhs, K) ((Rhs)[K].getState().yyloc)


enum YYRESULTTAG { yyok, yyaccept, yyabort, yyerr };

#define YYCHK(YYE)                              \
  do {                                          \
    YYRESULTTAG yychk_flag = YYE;               \
    if (yychk_flag != yyok)                     \
      return yychk_flag;                        \
  } while (false)

#if YYDEBUG

#define YYCDEBUG if (!yydebug) {} else std::cerr

# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                  \
  do {                                                                  \
    if (yydebug)                                                        \
      {                                                                 \
        std::cerr << Title << ' ';                                      \
        yyparser.yy_symbol_print_ (Kind, Value, Location); \
        std::cerr << '\n';                                              \
      }                                                                 \
  } while (false)

# define YY_REDUCE_PRINT(Args)                  \
  do {                                          \
    if (yydebug)                                \
      yystateStack.yy_reduce_print Args;        \
  } while (false)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;

namespace
{
  using glr_stack = yy::parser::glr_stack;
  using glr_state = yy::parser::glr_state;

  void yypstack (const glr_stack& yystack, size_t yyk)
    YY_ATTRIBUTE_UNUSED;
  void yypdumpstack (const glr_stack& yystack)
    YY_ATTRIBUTE_UNUSED;
}

#else /* !YYDEBUG */

# define YYCDEBUG if (true) {} else std::cerr
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location) {}
# define YY_REDUCE_PRINT(Args) {}

#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYMAXDEPTH * sizeof (GLRStackItem)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

/* Minimum number of free items on the stack allowed after an
   allocation.  This is to allow allocation and initialization
   to be completed by functions that call yyexpandGLRStack before the
   stack is expanded, thus insuring that all necessary pointers get
   properly redirected to new data.  */
#define YYHEADROOM 2

#ifndef YYSTACKEXPANDABLE
# define YYSTACKEXPANDABLE 1
#endif

namespace
{
  template <typename Parameter>
  class strong_index_alias
  {
  public:
    static strong_index_alias create (std::ptrdiff_t value)
    {
      strong_index_alias result;
      result.value_ = value;
      return result;
    }

    std::ptrdiff_t const& get () const { return value_; }

    size_t uget () const { return static_cast<size_t> (value_); }

    strong_index_alias operator+ (std::ptrdiff_t other) const
    {
      return strong_index_alias (get () + other);
    }

    void operator+= (std::ptrdiff_t other)
    {
      value_ += other;
    }

    strong_index_alias operator- (std::ptrdiff_t other)
    {
      return strong_index_alias (get () - other);
    }

    void operator-= (std::ptrdiff_t other)
    {
      value_ -= other;
    }

    size_t operator- (strong_index_alias other)
    {
      return strong_index_alias (get () - other.get ());
    }

    strong_index_alias& operator++ ()
    {
      ++value_;
      return *this;
    }

    bool isValid () const
    {
      return value_ != INVALID_INDEX;
    }

    void setInvalid()
    {
      value_ = INVALID_INDEX;
    }

    bool operator== (strong_index_alias other)
    {
      return get () == other.get ();
    }

    bool operator!= (strong_index_alias other)
    {
      return get () != other.get ();
    }

    bool operator< (strong_index_alias other)
    {
      return get () < other.get ();
    }

  private:
    static const std::ptrdiff_t INVALID_INDEX;

    // WARNING: 0-initialized.
    std::ptrdiff_t value_;
  }; // class strong_index_alias

  template<typename T>
  const std::ptrdiff_t strong_index_alias<T>::INVALID_INDEX =
    std::numeric_limits<std::ptrdiff_t>::max ();

  using state_set_index = strong_index_alias<struct glr_state_set_tag>;

  state_set_index create_state_set_index (std::ptrdiff_t value)
  {
    return state_set_index::create (value);
  }

  /** State numbers, as in LALR(1) machine */
  using state_num = int;

  /** Rule numbers, as in LALR(1) machine */
  using rule_num = int;

  using parser_type = yy::parser;
  using glr_state = parser_type::glr_state;
  using symbol_kind = parser_type::symbol_kind;
  using symbol_kind_type = parser_type::symbol_kind_type;
  using symbol_type = parser_type::symbol_type;
  using value_type = parser_type::value_type;
  using location_type = parser_type::location_type;

  // Forward declarations.
  class glr_stack_item;
  class semantic_option;
} // namespace

namespace
{
  /** Accessing symbol of state YYSTATE.  */
  inline symbol_kind_type
  yy_accessing_symbol (state_num yystate)
  {
    return YY_CAST (symbol_kind_type, yystos[yystate]);
  }

  /** Left-hand-side symbol for rule #YYRULE.  */
  inline symbol_kind_type
  yylhsNonterm (rule_num yyrule)
  {
    return static_cast<symbol_kind_type>(yyr1[yyrule]);
  }

  /** Number of symbols composing the right hand side of rule #RULE.  */
  inline int
  yyrhsLength (rule_num yyrule)
  {
    return yyr2[yyrule];
  }
}

namespace yy
{
  class parser::glr_state
  {
  public:
    glr_state ()
      : yyresolved (false)
      , yylrState (0)
      , yyposn (0)
      , yypred (0)
      , yyfirstVal (0)
      , yyloc ()
    {}

    /// Build with a semantic value.
    glr_state (state_num lrState, size_t posn, const value_type& val, const location_type& loc)
      : yyresolved (true)
      , yylrState (lrState)
      , yyposn (posn)
      , yypred (0)
      , yyval ()
      , yyloc (loc)
    {
      switch (yy_accessing_symbol (lrState))
    {
      case symbol_kind::S_flow_literal: // flow_literal
      case symbol_kind::S_generator_expression: // generator_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_ternary_expression: // ternary_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_bitwise_or_expression: // bitwise_or_expression
      case symbol_kind::S_bitwise_xor_expression: // bitwise_xor_expression
      case symbol_kind::S_bitwise_and_expression: // bitwise_and_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_primary_expression: // primary_expression
        yyval.copy< idyl::parser::expr_ptr > (val);
        break;

      case symbol_kind::S_program: // program
        yyval.copy< idyl::parser::node_ptr > (val);
        break;

      case symbol_kind::S_parameter: // parameter
        yyval.copy< idyl::parser::param_ptr > (val);
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        yyval.copy< idyl::parser::stmt_ptr > (val);
        break;

      case symbol_kind::S_argument: // argument
        yyval.copy< std::shared_ptr<idyl::parser::argument> > (val);
        break;

      case symbol_kind::S_at_block: // at_block
        yyval.copy< std::shared_ptr<idyl::parser::at_block> > (val);
        break;

      case symbol_kind::S_catch_block: // catch_block
        yyval.copy< std::shared_ptr<idyl::parser::catch_block> > (val);
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        yyval.copy< std::shared_ptr<idyl::parser::flow_definition> > (val);
        break;

      case symbol_kind::S_function_definition: // function_definition
        yyval.copy< std::shared_ptr<idyl::parser::function_definition> > (val);
        break;

      case symbol_kind::S_init_block: // init_block
        yyval.copy< std::shared_ptr<idyl::parser::init_block> > (val);
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        yyval.copy< std::shared_ptr<idyl::parser::lambda_block> > (val);
        break;

      case symbol_kind::S_on_block: // on_block
        yyval.copy< std::shared_ptr<idyl::parser::on_block> > (val);
        break;

      case symbol_kind::S_process_block: // process_block
        yyval.copy< std::shared_ptr<idyl::parser::process_block> > (val);
        break;

      case symbol_kind::S_start_statement: // start_statement
        yyval.copy< std::shared_ptr<idyl::parser::start_statement> > (val);
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        yyval.copy< std::shared_ptr<idyl::parser::stop_statement> > (val);
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        yyval.copy< std::string > (val);
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        yyval.copy< std::vector<idyl::parser::expr_ptr> > (val);
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        yyval.copy< std::vector<idyl::parser::param_ptr> > (val);
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        yyval.copy< std::vector<idyl::parser::stmt_ptr> > (val);
        break;

      case symbol_kind::S_argument_list: // argument_list
        yyval.copy< std::vector<std::shared_ptr<idyl::parser::argument>> > (val);
        break;

      case symbol_kind::S_flow_members: // flow_members
        yyval.copy< std::vector<std::shared_ptr<idyl::parser::flow_member>> > (val);
        break;

      default:
        break;
    }
}

    /// Build with a semantic option.
    glr_state (state_num lrState, size_t posn)
      : yyresolved (false)
      , yylrState (lrState)
      , yyposn (posn)
      , yypred (0)
      , yyfirstVal (0)
      , yyloc ()
    {}

    glr_state (const glr_state& other)
      : yyresolved (other.yyresolved)
      , yylrState (other.yylrState)
      , yyposn (other.yyposn)
      , yypred (0)
      , yyloc (other.yyloc)
    {
      setPred (other.pred ());
      if (other.yyresolved)
        {
          new (&yyval) value_type ();
          switch (yy_accessing_symbol (other.yylrState))
    {
      case symbol_kind::S_flow_literal: // flow_literal
      case symbol_kind::S_generator_expression: // generator_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_ternary_expression: // ternary_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_bitwise_or_expression: // bitwise_or_expression
      case symbol_kind::S_bitwise_xor_expression: // bitwise_xor_expression
      case symbol_kind::S_bitwise_and_expression: // bitwise_and_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_primary_expression: // primary_expression
        yyval.copy< idyl::parser::expr_ptr > (other.value ());
        break;

      case symbol_kind::S_program: // program
        yyval.copy< idyl::parser::node_ptr > (other.value ());
        break;

      case symbol_kind::S_parameter: // parameter
        yyval.copy< idyl::parser::param_ptr > (other.value ());
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        yyval.copy< idyl::parser::stmt_ptr > (other.value ());
        break;

      case symbol_kind::S_argument: // argument
        yyval.copy< std::shared_ptr<idyl::parser::argument> > (other.value ());
        break;

      case symbol_kind::S_at_block: // at_block
        yyval.copy< std::shared_ptr<idyl::parser::at_block> > (other.value ());
        break;

      case symbol_kind::S_catch_block: // catch_block
        yyval.copy< std::shared_ptr<idyl::parser::catch_block> > (other.value ());
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        yyval.copy< std::shared_ptr<idyl::parser::flow_definition> > (other.value ());
        break;

      case symbol_kind::S_function_definition: // function_definition
        yyval.copy< std::shared_ptr<idyl::parser::function_definition> > (other.value ());
        break;

      case symbol_kind::S_init_block: // init_block
        yyval.copy< std::shared_ptr<idyl::parser::init_block> > (other.value ());
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        yyval.copy< std::shared_ptr<idyl::parser::lambda_block> > (other.value ());
        break;

      case symbol_kind::S_on_block: // on_block
        yyval.copy< std::shared_ptr<idyl::parser::on_block> > (other.value ());
        break;

      case symbol_kind::S_process_block: // process_block
        yyval.copy< std::shared_ptr<idyl::parser::process_block> > (other.value ());
        break;

      case symbol_kind::S_start_statement: // start_statement
        yyval.copy< std::shared_ptr<idyl::parser::start_statement> > (other.value ());
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        yyval.copy< std::shared_ptr<idyl::parser::stop_statement> > (other.value ());
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        yyval.copy< std::string > (other.value ());
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        yyval.copy< std::vector<idyl::parser::expr_ptr> > (other.value ());
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        yyval.copy< std::vector<idyl::parser::param_ptr> > (other.value ());
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        yyval.copy< std::vector<idyl::parser::stmt_ptr> > (other.value ());
        break;

      case symbol_kind::S_argument_list: // argument_list
        yyval.copy< std::vector<std::shared_ptr<idyl::parser::argument>> > (other.value ());
        break;

      case symbol_kind::S_flow_members: // flow_members
        yyval.copy< std::vector<std::shared_ptr<idyl::parser::flow_member>> > (other.value ());
        break;

      default:
        break;
    }

        }
      else
        {
          yyfirstVal = 0;
          setFirstVal (other.firstVal ());
        }
    }

    ~glr_state ()
    {
      if (yyresolved)
        {
          symbol_kind_type yykind = yy_accessing_symbol (yylrState);
          // FIXME: User destructors.
          // Value type destructor.
          switch (yykind)
    {
      case symbol_kind::S_flow_literal: // flow_literal
      case symbol_kind::S_generator_expression: // generator_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_ternary_expression: // ternary_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_bitwise_or_expression: // bitwise_or_expression
      case symbol_kind::S_bitwise_xor_expression: // bitwise_xor_expression
      case symbol_kind::S_bitwise_and_expression: // bitwise_and_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_primary_expression: // primary_expression
        yyval.template destroy< idyl::parser::expr_ptr > ();
        break;

      case symbol_kind::S_program: // program
        yyval.template destroy< idyl::parser::node_ptr > ();
        break;

      case symbol_kind::S_parameter: // parameter
        yyval.template destroy< idyl::parser::param_ptr > ();
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        yyval.template destroy< idyl::parser::stmt_ptr > ();
        break;

      case symbol_kind::S_argument: // argument
        yyval.template destroy< std::shared_ptr<idyl::parser::argument> > ();
        break;

      case symbol_kind::S_at_block: // at_block
        yyval.template destroy< std::shared_ptr<idyl::parser::at_block> > ();
        break;

      case symbol_kind::S_catch_block: // catch_block
        yyval.template destroy< std::shared_ptr<idyl::parser::catch_block> > ();
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        yyval.template destroy< std::shared_ptr<idyl::parser::flow_definition> > ();
        break;

      case symbol_kind::S_function_definition: // function_definition
        yyval.template destroy< std::shared_ptr<idyl::parser::function_definition> > ();
        break;

      case symbol_kind::S_init_block: // init_block
        yyval.template destroy< std::shared_ptr<idyl::parser::init_block> > ();
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        yyval.template destroy< std::shared_ptr<idyl::parser::lambda_block> > ();
        break;

      case symbol_kind::S_on_block: // on_block
        yyval.template destroy< std::shared_ptr<idyl::parser::on_block> > ();
        break;

      case symbol_kind::S_process_block: // process_block
        yyval.template destroy< std::shared_ptr<idyl::parser::process_block> > ();
        break;

      case symbol_kind::S_start_statement: // start_statement
        yyval.template destroy< std::shared_ptr<idyl::parser::start_statement> > ();
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        yyval.template destroy< std::shared_ptr<idyl::parser::stop_statement> > ();
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        yyval.template destroy< std::string > ();
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        yyval.template destroy< std::vector<idyl::parser::expr_ptr> > ();
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        yyval.template destroy< std::vector<idyl::parser::param_ptr> > ();
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        yyval.template destroy< std::vector<idyl::parser::stmt_ptr> > ();
        break;

      case symbol_kind::S_argument_list: // argument_list
        yyval.template destroy< std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        break;

      case symbol_kind::S_flow_members: // flow_members
        yyval.template destroy< std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        break;

      default:
        break;
    }

          yyval.~value_type ();
        }
    }

    glr_state& operator= (const glr_state& other)
    {
      if (!yyresolved && other.yyresolved)
        new (&yyval) value_type;
      yyresolved = other.yyresolved;
      yylrState = other.yylrState;
      yyposn = other.yyposn;
      setPred (other.pred ());
      if (other.yyresolved)
        switch (yy_accessing_symbol (other.yylrState))
    {
      case symbol_kind::S_flow_literal: // flow_literal
      case symbol_kind::S_generator_expression: // generator_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_ternary_expression: // ternary_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_bitwise_or_expression: // bitwise_or_expression
      case symbol_kind::S_bitwise_xor_expression: // bitwise_xor_expression
      case symbol_kind::S_bitwise_and_expression: // bitwise_and_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_primary_expression: // primary_expression
        yyval.copy< idyl::parser::expr_ptr > (other.value ());
        break;

      case symbol_kind::S_program: // program
        yyval.copy< idyl::parser::node_ptr > (other.value ());
        break;

      case symbol_kind::S_parameter: // parameter
        yyval.copy< idyl::parser::param_ptr > (other.value ());
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        yyval.copy< idyl::parser::stmt_ptr > (other.value ());
        break;

      case symbol_kind::S_argument: // argument
        yyval.copy< std::shared_ptr<idyl::parser::argument> > (other.value ());
        break;

      case symbol_kind::S_at_block: // at_block
        yyval.copy< std::shared_ptr<idyl::parser::at_block> > (other.value ());
        break;

      case symbol_kind::S_catch_block: // catch_block
        yyval.copy< std::shared_ptr<idyl::parser::catch_block> > (other.value ());
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        yyval.copy< std::shared_ptr<idyl::parser::flow_definition> > (other.value ());
        break;

      case symbol_kind::S_function_definition: // function_definition
        yyval.copy< std::shared_ptr<idyl::parser::function_definition> > (other.value ());
        break;

      case symbol_kind::S_init_block: // init_block
        yyval.copy< std::shared_ptr<idyl::parser::init_block> > (other.value ());
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        yyval.copy< std::shared_ptr<idyl::parser::lambda_block> > (other.value ());
        break;

      case symbol_kind::S_on_block: // on_block
        yyval.copy< std::shared_ptr<idyl::parser::on_block> > (other.value ());
        break;

      case symbol_kind::S_process_block: // process_block
        yyval.copy< std::shared_ptr<idyl::parser::process_block> > (other.value ());
        break;

      case symbol_kind::S_start_statement: // start_statement
        yyval.copy< std::shared_ptr<idyl::parser::start_statement> > (other.value ());
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        yyval.copy< std::shared_ptr<idyl::parser::stop_statement> > (other.value ());
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        yyval.copy< std::string > (other.value ());
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        yyval.copy< std::vector<idyl::parser::expr_ptr> > (other.value ());
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        yyval.copy< std::vector<idyl::parser::param_ptr> > (other.value ());
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        yyval.copy< std::vector<idyl::parser::stmt_ptr> > (other.value ());
        break;

      case symbol_kind::S_argument_list: // argument_list
        yyval.copy< std::vector<std::shared_ptr<idyl::parser::argument>> > (other.value ());
        break;

      case symbol_kind::S_flow_members: // flow_members
        yyval.copy< std::vector<std::shared_ptr<idyl::parser::flow_member>> > (other.value ());
        break;

      default:
        break;
    }

      else
        setFirstVal (other.firstVal ());
      yyloc = other.yyloc;
      return *this;
    }

    /** Type tag for the semantic value.  If true, yyval applies, otherwise
     *  yyfirstVal applies.  */
    bool yyresolved;
    /** Number of corresponding LALR(1) machine state.  */
    state_num yylrState;
    /** Source position of the last token produced by my symbol */
    size_t yyposn;

    /// Only call pred() and setPred() on objects in yyitems, not temporaries.
    glr_state* pred ();
    const glr_state* pred () const;
    void setPred (const glr_state* state);

    /// Only call firstVal() and setFirstVal() on objects in yyitems, not
    /// temporaries.
    semantic_option* firstVal ();
    const semantic_option* firstVal () const;
    void setFirstVal (const semantic_option* option);

    value_type& value ()
    {
      return yyval;
    }

    const value_type& value () const
    {
      return yyval;
    }

    void
    destroy (char const *yymsg, yy::parser& yyparser);

    /* DEBUGGING ONLY */
  #if YYDEBUG
    void yy_yypstack () const
    {
      if (pred () != YY_NULLPTR)
        {
          pred ()->yy_yypstack ();
          std::cerr << " -> ";
        }
      std::cerr << yylrState << "@" << yyposn;
    }
  #endif

    std::ptrdiff_t indexIn (const glr_stack_item* array) const YY_ATTRIBUTE_UNUSED;

    glr_stack_item* asItem ()
    {
      return asItem(this);
    }

    const glr_stack_item* asItem () const
    {
      return asItem (this);
    }

  private:
    template <typename T>
    static const glr_stack_item* asItem (const T* state)
    {
      return reinterpret_cast<const glr_stack_item*>(state);
    }
    template <typename T>
    static glr_stack_item* asItem (T* state)
    {
      return reinterpret_cast<glr_stack_item*> (state);
    }
    static const char *as_pointer_ (const glr_state *state)
    {
      return reinterpret_cast<const char *> (state);
    }
    static char *as_pointer_ (glr_state *state)
    {
      return reinterpret_cast<char *> (state);
    }
    /** Preceding state in this stack */
    std::ptrdiff_t yypred;
    union {
      /** First in a chain of alternative reductions producing the
       *  nonterminal corresponding to this state, threaded through
       *  yyfirstVal.  Value "0" means empty.  */
      std::ptrdiff_t yyfirstVal;
      /** Semantic value for this state.  */
      value_type yyval;
    };
   // FIXME: Why public?
   public:
    /** Source location for this state.  */
    location_type yyloc;


  }; // class parser::glr_state
} // namespace yy


namespace
{
  /** A stack of GLRState representing the different heads during
    * nondeterministic evaluation. */
  class glr_state_set
  {
  public:
    /** Initialize YYSET to a singleton set containing an empty stack.  */
    glr_state_set ()
      : yylastDeleted (YY_NULLPTR)
    {
      yystates.push_back (YY_NULLPTR);
      yylookaheadNeeds.push_back (false);
    }

    // Behave like a vector of states.
    glr_state*& operator[] (state_set_index index)
    {
      return yystates[index.uget()];
    }

    glr_state* operator[] (state_set_index index) const
    {
      return yystates[index.uget()];
    }

    size_t size () const
    {
      return yystates.size ();
    }

    std::vector<glr_state*>::iterator begin ()
    {
      return yystates.begin ();
    }

    std::vector<glr_state*>::iterator end ()
    {
      return yystates.end ();
    }

    bool lookaheadNeeds (state_set_index index) const
    {
      return yylookaheadNeeds[index.uget ()];
    }

    bool setLookaheadNeeds (state_set_index index, bool value)
    {
      return yylookaheadNeeds[index.uget ()] = value;
    }

    /** Invalidate stack #YYK.  */
    void
    yymarkStackDeleted (state_set_index yyk)
    {
      size_t k = yyk.uget ();
      if (yystates[k] != YY_NULLPTR)
        yylastDeleted = yystates[k];
      yystates[k] = YY_NULLPTR;
    }

    /** Undelete the last stack in *this that was marked as deleted.  Can
        only be done once after a deletion, and only when all other stacks have
        been deleted.  */
    void
    yyundeleteLastStack ()
    {
      if (yylastDeleted == YY_NULLPTR || !yystates.empty ())
        return;
      yystates.push_back (yylastDeleted);
      YYCDEBUG << "Restoring last deleted stack as stack #0.\n";
      clearLastDeleted ();
    }

    /** Remove the dead stacks (yystates[i] == YY_NULLPTR) and shift the later
     * ones.  */
    void
    yyremoveDeletes ()
    {
      size_t newsize = yystates.size ();
      /* j is the number of live stacks we have seen.  */
      for (size_t i = 0, j = 0; j < newsize; ++i)
        {
          if (yystates[i] == YY_NULLPTR)
            {
              if (i == j)
                {
                  YYCDEBUG << "Removing dead stacks.\n";
                }
              newsize -= 1;
            }
          else
            {
              yystates[j] = yystates[i];
              /* In the current implementation, it's unnecessary to copy
                 yylookaheadNeeds[i] since, after
                 yyremoveDeletes returns, the parser immediately either enters
                 deterministic operation or shifts a token.  However, it doesn't
                 hurt, and the code might evolve to need it.  */
              yylookaheadNeeds[j] = yylookaheadNeeds[i];
              if (j != i)
                {
                  YYCDEBUG << "Rename stack " << i << " -> " << j << ".\n";
                }
              j += 1;
            }
        }
      yystates.resize (newsize);
      yylookaheadNeeds.resize (newsize);
    }


    state_set_index
    yysplitStack (state_set_index yyk)
    {
      const size_t k = yyk.uget ();
      yystates.push_back (yystates[k]);
      yylookaheadNeeds.push_back (yylookaheadNeeds[k]);
      return create_state_set_index (static_cast<std::ptrdiff_t> (yystates.size () - 1));
    }

    void clearLastDeleted ()
    {
      yylastDeleted = YY_NULLPTR;
    }

  private:

    std::vector<glr_state*> yystates;
    /** During nondeterministic operation, yylookaheadNeeds tracks which
     *  stacks have actually needed the current lookahead.  During deterministic
     *  operation, yylookaheadNeeds[0] is not maintained since it would merely
     *  duplicate !yyla.empty ().  */
    std::vector<bool> yylookaheadNeeds;

    /** The last stack we invalidated.  */
    glr_state* yylastDeleted;
  }; // class glr_state_set
} // namespace

namespace
{
  class semantic_option
  {
  public:
    semantic_option ()
      : yyrule (0)
      , yystate (0)
      , yynext (0)
      , yyla ()
    {}

    semantic_option (rule_num rule)
      : yyrule (rule)
      , yystate (0)
      , yynext (0)
      , yyla ()
    {}

    semantic_option (const semantic_option& that)
      : yyrule (that.yyrule)
      , yystate (that.yystate)
      , yynext (that.yynext)
      , yyla (that.yyla)
    {
    }

    // Needed for the assignment in yynewSemanticOption.
    semantic_option& operator= (const semantic_option& that)
    {
      yyrule = that.yyrule;
      yystate = that.yystate;
      yynext = that.yynext;
      yyla = that.yyla;
      return *this;
    }

    /// Only call state() and setState() on objects in yyitems, not temporaries.
    glr_state* state();
    const glr_state* state() const;
    void setState(const glr_state* s);

    const semantic_option* next () const YY_ATTRIBUTE_UNUSED;
    semantic_option* next ();
    void setNext (const semantic_option* s);

    std::ptrdiff_t indexIn (const glr_stack_item* array) const YY_ATTRIBUTE_UNUSED;

    /** True iff YYY0 and YYY1 represent identical options at the top level.
     *  That is, they represent the same rule applied to RHS symbols
     *  that produce the same terminal symbols.  */
    bool
    isIdenticalTo (const semantic_option& yyy1) const
    {
      if (this->yyrule == yyy1.yyrule)
        {
          const glr_state *yys0, *yys1;
          int yyn;
          for (yys0 = this->state(),
               yys1 = yyy1.state(),
               yyn = yyrhsLength (this->yyrule);
               yyn > 0;
               yys0 = yys0->pred(),
               yys1 = yys1->pred(), yyn -= 1)
            if (yys0->yyposn != yys1->yyposn)
              return false;
          return true;
        }
      else
        return false;
    }

    /** Assuming identicalOptions (YYY0,YYY1), destructively merge the
     *  alternative semantic values for the RHS-symbols of YYY1 and YYY0.  */
    void
    mergeWith (semantic_option& yyy1)
    {
      glr_state *yys0 = this->state ();
      glr_state *yys1 = yyy1.state ();
      for (int yyn = yyrhsLength (this->yyrule);
           yyn > 0;
           yyn -= 1, yys0 = yys0->pred (), yys1 = yys1->pred ())
        {
          if (yys0 == yys1)
            break;
          else if (yys0->yyresolved)
            {
              yys1->yyresolved = true;
              YYASSERT (yys1->yylrState == yys0->yylrState);
              switch (yy_accessing_symbol (yys0->yylrState))
    {
      case symbol_kind::S_flow_literal: // flow_literal
      case symbol_kind::S_generator_expression: // generator_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_ternary_expression: // ternary_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_bitwise_or_expression: // bitwise_or_expression
      case symbol_kind::S_bitwise_xor_expression: // bitwise_xor_expression
      case symbol_kind::S_bitwise_and_expression: // bitwise_and_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_primary_expression: // primary_expression
        yys1->value ().copy< idyl::parser::expr_ptr > (yys0->value ());
        break;

      case symbol_kind::S_program: // program
        yys1->value ().copy< idyl::parser::node_ptr > (yys0->value ());
        break;

      case symbol_kind::S_parameter: // parameter
        yys1->value ().copy< idyl::parser::param_ptr > (yys0->value ());
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        yys1->value ().copy< idyl::parser::stmt_ptr > (yys0->value ());
        break;

      case symbol_kind::S_argument: // argument
        yys1->value ().copy< std::shared_ptr<idyl::parser::argument> > (yys0->value ());
        break;

      case symbol_kind::S_at_block: // at_block
        yys1->value ().copy< std::shared_ptr<idyl::parser::at_block> > (yys0->value ());
        break;

      case symbol_kind::S_catch_block: // catch_block
        yys1->value ().copy< std::shared_ptr<idyl::parser::catch_block> > (yys0->value ());
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        yys1->value ().copy< std::shared_ptr<idyl::parser::flow_definition> > (yys0->value ());
        break;

      case symbol_kind::S_function_definition: // function_definition
        yys1->value ().copy< std::shared_ptr<idyl::parser::function_definition> > (yys0->value ());
        break;

      case symbol_kind::S_init_block: // init_block
        yys1->value ().copy< std::shared_ptr<idyl::parser::init_block> > (yys0->value ());
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        yys1->value ().copy< std::shared_ptr<idyl::parser::lambda_block> > (yys0->value ());
        break;

      case symbol_kind::S_on_block: // on_block
        yys1->value ().copy< std::shared_ptr<idyl::parser::on_block> > (yys0->value ());
        break;

      case symbol_kind::S_process_block: // process_block
        yys1->value ().copy< std::shared_ptr<idyl::parser::process_block> > (yys0->value ());
        break;

      case symbol_kind::S_start_statement: // start_statement
        yys1->value ().copy< std::shared_ptr<idyl::parser::start_statement> > (yys0->value ());
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        yys1->value ().copy< std::shared_ptr<idyl::parser::stop_statement> > (yys0->value ());
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        yys1->value ().copy< std::string > (yys0->value ());
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        yys1->value ().copy< std::vector<idyl::parser::expr_ptr> > (yys0->value ());
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        yys1->value ().copy< std::vector<idyl::parser::param_ptr> > (yys0->value ());
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        yys1->value ().copy< std::vector<idyl::parser::stmt_ptr> > (yys0->value ());
        break;

      case symbol_kind::S_argument_list: // argument_list
        yys1->value ().copy< std::vector<std::shared_ptr<idyl::parser::argument>> > (yys0->value ());
        break;

      case symbol_kind::S_flow_members: // flow_members
        yys1->value ().copy< std::vector<std::shared_ptr<idyl::parser::flow_member>> > (yys0->value ());
        break;

      default:
        break;
    }

            }
          else if (yys1->yyresolved)
            {
              yys0->yyresolved = true;
              YYASSERT (yys0->yylrState == yys1->yylrState);
              switch (yy_accessing_symbol (yys1->yylrState))
    {
      case symbol_kind::S_flow_literal: // flow_literal
      case symbol_kind::S_generator_expression: // generator_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_ternary_expression: // ternary_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_bitwise_or_expression: // bitwise_or_expression
      case symbol_kind::S_bitwise_xor_expression: // bitwise_xor_expression
      case symbol_kind::S_bitwise_and_expression: // bitwise_and_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_primary_expression: // primary_expression
        yys0->value ().copy< idyl::parser::expr_ptr > (yys1->value ());
        break;

      case symbol_kind::S_program: // program
        yys0->value ().copy< idyl::parser::node_ptr > (yys1->value ());
        break;

      case symbol_kind::S_parameter: // parameter
        yys0->value ().copy< idyl::parser::param_ptr > (yys1->value ());
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        yys0->value ().copy< idyl::parser::stmt_ptr > (yys1->value ());
        break;

      case symbol_kind::S_argument: // argument
        yys0->value ().copy< std::shared_ptr<idyl::parser::argument> > (yys1->value ());
        break;

      case symbol_kind::S_at_block: // at_block
        yys0->value ().copy< std::shared_ptr<idyl::parser::at_block> > (yys1->value ());
        break;

      case symbol_kind::S_catch_block: // catch_block
        yys0->value ().copy< std::shared_ptr<idyl::parser::catch_block> > (yys1->value ());
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        yys0->value ().copy< std::shared_ptr<idyl::parser::flow_definition> > (yys1->value ());
        break;

      case symbol_kind::S_function_definition: // function_definition
        yys0->value ().copy< std::shared_ptr<idyl::parser::function_definition> > (yys1->value ());
        break;

      case symbol_kind::S_init_block: // init_block
        yys0->value ().copy< std::shared_ptr<idyl::parser::init_block> > (yys1->value ());
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        yys0->value ().copy< std::shared_ptr<idyl::parser::lambda_block> > (yys1->value ());
        break;

      case symbol_kind::S_on_block: // on_block
        yys0->value ().copy< std::shared_ptr<idyl::parser::on_block> > (yys1->value ());
        break;

      case symbol_kind::S_process_block: // process_block
        yys0->value ().copy< std::shared_ptr<idyl::parser::process_block> > (yys1->value ());
        break;

      case symbol_kind::S_start_statement: // start_statement
        yys0->value ().copy< std::shared_ptr<idyl::parser::start_statement> > (yys1->value ());
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        yys0->value ().copy< std::shared_ptr<idyl::parser::stop_statement> > (yys1->value ());
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        yys0->value ().copy< std::string > (yys1->value ());
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        yys0->value ().copy< std::vector<idyl::parser::expr_ptr> > (yys1->value ());
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        yys0->value ().copy< std::vector<idyl::parser::param_ptr> > (yys1->value ());
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        yys0->value ().copy< std::vector<idyl::parser::stmt_ptr> > (yys1->value ());
        break;

      case symbol_kind::S_argument_list: // argument_list
        yys0->value ().copy< std::vector<std::shared_ptr<idyl::parser::argument>> > (yys1->value ());
        break;

      case symbol_kind::S_flow_members: // flow_members
        yys0->value ().copy< std::vector<std::shared_ptr<idyl::parser::flow_member>> > (yys1->value ());
        break;

      default:
        break;
    }

            }
          else
            {
              semantic_option* yyz0prev = YY_NULLPTR;
              semantic_option* yyz0 = yys0->firstVal();
              semantic_option* yyz1 = yys1->firstVal();
              while (true)
                {
                  if (yyz1 == yyz0 || yyz1 == YY_NULLPTR)
                    break;
                  else if (yyz0 == YY_NULLPTR)
                    {
                      if (yyz0prev != YY_NULLPTR)
                        yyz0prev->setNext (yyz1);
                      else
                        yys0->setFirstVal (yyz1);
                      break;
                    }
                  else if (yyz0 < yyz1)
                    {
                      semantic_option* yyz = yyz0;
                      if (yyz0prev != YY_NULLPTR)
                        yyz0prev->setNext(yyz1);
                      else
                        yys0->setFirstVal(yyz1);
                      yyz1 = yyz1->next();
                      yyz0->setNext(yyz);
                    }
                  yyz0prev = yyz0;
                  yyz0 = yyz0->next();
                }
              yys1->setFirstVal(yys0->firstVal());
            }
        }
    }

#if YYDEBUG
    void yyreportTree (size_t yyindent = 2) const
    {
      int yynrhs = yyrhsLength (this->yyrule);
      const glr_state* yystates[1 + YYMAXRHS];
      glr_state yyleftmost_state;

      {
        const glr_state* yys = this->state();
        for (int yyi = yynrhs; 0 < yyi; yyi -= 1)
          {
            yystates[yyi] = yys;
            yys = yys->pred();
          }
        if (yys == YY_NULLPTR)
          {
            yyleftmost_state.yyposn = 0;
            yystates[0] = &yyleftmost_state;
          }
        else
          yystates[0] = yys;
      }

      std::string yylhs = yy::parser::symbol_name (yylhsNonterm (this->yyrule));
      YYASSERT(this->state());
      if (this->state()->yyposn < yystates[0]->yyposn + 1)
        std::cerr << std::string(yyindent, ' ') << yylhs << " -> <Rule "
                  << this->yyrule - 1 << ", empty>\n";
      else
        std::cerr << std::string(yyindent, ' ') << yylhs << " -> <Rule "
                  << this->yyrule - 1 << ", tokens "
                  << yystates[0]->yyposn + 1 << " .. "
                  << this->state()->yyposn << ">\n";
      for (int yyi = 1; yyi <= yynrhs; yyi += 1)
        {
          if (yystates[yyi]->yyresolved)
            {
              std::string yysym = yy::parser::symbol_name (yy_accessing_symbol (yystates[yyi]->yylrState));
              if (yystates[yyi-1]->yyposn+1 > yystates[yyi]->yyposn)
                std::cerr << std::string(yyindent + 2, ' ') << yysym
                          << " <empty>\n";
              else
                std::cerr << std::string(yyindent + 2, ' ') << yysym
                          << " <tokens " << yystates[yyi-1]->yyposn + 1
                          << " .. " << yystates[yyi]->yyposn << ">\n";
            }
          else
            yystates[yyi]->firstVal ()->yyreportTree (yyindent+2);
        }
    }
#endif

    /** Rule number for this reduction */
    rule_num yyrule;

  private:
    template <typename T>
    static const glr_stack_item* asItem(const T* state)
    {
      return reinterpret_cast<const glr_stack_item*>(state);
    }
    template <typename T>
    static glr_stack_item* asItem(T* state)
    {
      return reinterpret_cast<glr_stack_item*>(state);
    }
    /** The last RHS state in the list of states to be reduced.  */
    std::ptrdiff_t yystate;
    /** Next sibling in chain of options.  To facilitate merging,
     *  options are chained in decreasing order by address.  */
    std::ptrdiff_t yynext;

  public:
    /** The lookahead for this reduction.  */
    symbol_type yyla;


  }; // class semantic_option
} // namespace

namespace
{
  /** Type of the items in the GLR stack.
   *  It can be either a glr_state or a semantic_option. The is_state_ field
   *  indicates which item of the union is valid.  */
  class glr_stack_item
  {
  public:
    glr_stack_item (bool state = true)
      : is_state_ (state)
    {
      if (is_state_)
        new (&raw_) glr_state;
      else
        new (&raw_) semantic_option;
    }

    glr_stack_item (const glr_stack_item& other) YY_NOEXCEPT YY_NOTHROW
      : is_state_ (other.is_state_)
    {
      std::memcpy (raw_, other.raw_, union_size);
    }

    glr_stack_item& operator= (glr_stack_item other)
    {
      std::swap (is_state_, other.is_state_);
      std::swap (raw_, other.raw_);
      return *this;
    }

    ~glr_stack_item ()
    {
      if (is_state ())
        getState ().~glr_state ();
      else
        getOption ().~semantic_option ();
    }

    void setState (const glr_state &state)
    {
      if (this != state.asItem ())
        {
          if (is_state_)
            getState ().~glr_state ();
          else
            getOption ().~semantic_option ();
          new (&raw_) glr_state (state);
          is_state_ = true;
        }
    }

    glr_state& getState ()
    {
      YYDASSERT (is_state ());
      void *yyp = raw_;
      glr_state& res = *static_cast<glr_state*> (yyp);
      return res;
    }

    const glr_state& getState () const
    {
      YYDASSERT (is_state ());
      const void *yyp = raw_;
      const glr_state& res = *static_cast<const glr_state*> (yyp);
      return res;
    }

    semantic_option& getOption ()
    {
      YYDASSERT (!is_state ());
      void *yyp = raw_;
      return *static_cast<semantic_option*> (yyp);
    }
    const semantic_option& getOption () const
    {
      YYDASSERT (!is_state ());
      const void *yyp = raw_;
      return *static_cast<const semantic_option*> (yyp);
    }
    bool is_state () const
    {
      return is_state_;
    }

  private:
    /// The possible contents of raw_. Since they have constructors, they cannot
    /// be directly included in the union.
    union contents
    {
      char yystate[sizeof (glr_state)];
      char yyoption[sizeof (semantic_option)];
    };
    enum { union_size = sizeof (contents) };
    union {
      /// Strongest alignment constraints.
      long double yyalign_me;
      /// A buffer large enough to store the contents.
      char raw_[union_size];
    };
    /** Type tag for the union. */
    bool is_state_;

  }; // class glr_stack_item
} // namespace

glr_state* glr_state::pred ()
{
  YY_IGNORE_NULL_DEREFERENCE_BEGIN
  return yypred ? &asItem (as_pointer_ (this) - yypred)->getState () : YY_NULLPTR;
  YY_IGNORE_NULL_DEREFERENCE_END
}

const glr_state* glr_state::pred () const
{
  YY_IGNORE_NULL_DEREFERENCE_BEGIN
  return yypred ? &asItem (as_pointer_ (this) - yypred)->getState () : YY_NULLPTR;
  YY_IGNORE_NULL_DEREFERENCE_END
}

void glr_state::setPred (const glr_state* state)
{
  yypred = state ? as_pointer_ (this) - as_pointer_ (state) : 0;
}

semantic_option* glr_state::firstVal ()
{
  return yyfirstVal ? &(asItem(this) - yyfirstVal)->getOption() : YY_NULLPTR;
}

const semantic_option* glr_state::firstVal () const
{
  return yyfirstVal ? &(asItem(this) - yyfirstVal)->getOption() : YY_NULLPTR;
}

void glr_state::setFirstVal (const semantic_option* option)
{
  yyfirstVal = option ? asItem(this) - asItem(option) : 0;
}

std::ptrdiff_t glr_state::indexIn (const glr_stack_item* array) const
{
  return asItem(this) - array;
}

std::ptrdiff_t semantic_option::indexIn (const glr_stack_item* array) const
{
  return asItem(this) - array;
}

glr_state* semantic_option::state ()
{
  YY_IGNORE_NULL_DEREFERENCE_BEGIN
  return yystate ? &(asItem(this) - yystate)->getState() : YY_NULLPTR;
  YY_IGNORE_NULL_DEREFERENCE_END
}

const glr_state* semantic_option::state () const
{
  return yystate ? &(asItem(this) - yystate)->getState() : YY_NULLPTR;
}

void semantic_option::setState (const glr_state* s)
{
  yystate = s ? asItem(this) - asItem(s) : 0;
}

const semantic_option* semantic_option::next () const
{
  return yynext ? &(asItem(this) - yynext)->getOption() : YY_NULLPTR;
}

semantic_option* semantic_option::next ()
{
  return yynext ? &(asItem(this) - yynext)->getOption() : YY_NULLPTR;
}

void semantic_option::setNext (const semantic_option* s)
{
  yynext = s ? asItem(this) - asItem(s) : 0;
}

void glr_state::destroy (char const* yymsg, yy::parser& yyparser)
{
  if (yyresolved)
    yyparser.yy_destroy_ (yymsg, yy_accessing_symbol(yylrState),
                          value (), yyloc);
  else
    {
#if YYDEBUG
      YYCDEBUG << yymsg
               << (firstVal() ? " unresolved " : " incomplete ")
               << (yy_accessing_symbol (yylrState) < YYNTOKENS ? "token" : "nterm")
               << ' ' << yyparser.symbol_name (yy_accessing_symbol (yylrState))
               << " ("
               << yyloc << ": "
               << ")\n";
#endif
      if (firstVal() != YY_NULLPTR)
        {
          semantic_option& yyoption = *firstVal ();
          glr_state *yyrh = yyoption.state ();
          for (int yyn = yyrhsLength (yyoption.yyrule); yyn > 0; yyn -= 1)
            {
              yyrh->destroy (yymsg, yyparser);
              yyrh = yyrh->pred();
            }
        }
    }
}


#undef YYFILL
#define YYFILL(N) yyfill (yyvsp, yylow, (N), yynormal)

namespace
{
  class state_stack
  {
  public:
    using parser_type = yy::parser;
    using symbol_kind = parser_type::symbol_kind;
    using value_type = parser_type::value_type;
    using location_type = parser_type::location_type;

    /** Initialize to a single empty stack, with total maximum
     *  capacity for all stacks of YYSIZE.  */
    state_stack (size_t yysize)
      : yysplitPoint (YY_NULLPTR)
    {
      yyitems.reserve (yysize);
    }

#if YYSTACKEXPANDABLE
    /** Returns false if it tried to expand but could not. */
    bool
    yyexpandGLRStackIfNeeded ()
    {
      return YYHEADROOM <= spaceLeft () || yyexpandGLRStack ();
    }

  private:
    /** If *this is expandable, extend it.  WARNING: Pointers into the
        stack from outside should be considered invalid after this call.
        We always expand when there are 1 or fewer items left AFTER an
        allocation, so that we can avoid having external pointers exist
        across an allocation.  */
    bool
    yyexpandGLRStack ()
    {
      const size_t oldsize = yyitems.size();
      if (YYMAXDEPTH - YYHEADROOM < oldsize)
        return false;
      const size_t yynewSize = YYMAXDEPTH < 2 * oldsize ? YYMAXDEPTH : 2 * oldsize;
      const glr_stack_item *oldbase = &yyitems[0];

      yyitems.reserve (yynewSize);
      const glr_stack_item *newbase = &yyitems[0];

      // Adjust the pointers.  Perform raw pointer arithmetic, as there
      // is no reason for objects to be aligned on their size.
      const ptrdiff_t disp
        = reinterpret_cast<const char*> (newbase) - reinterpret_cast<const char*> (oldbase);
      if (yysplitPoint)
        const_cast<glr_state*&> (yysplitPoint)
          = reinterpret_cast<glr_state*> (reinterpret_cast<char*> (const_cast<glr_state*> (yysplitPoint)) + disp);

      for (std::vector<glr_state*>::iterator
             i = yytops.begin (),
             yyend = yytops.end ();
           i != yyend; ++i)
        if (glr_state_not_null (*i))
          *i = reinterpret_cast<glr_state*>(reinterpret_cast<char*>(*i) + disp);

      return true;
    }

  public:
#else
    bool yyexpandGLRStackIfNeeded ()
    {
      return YYHEADROOM <= spaceLeft ();
    }
#endif
#undef YYSTACKEXPANDABLE

    static bool glr_state_not_null (glr_state* s)
    {
      return s != YY_NULLPTR;
    }

    bool
    reduceToOneStack ()
    {
      using iterator = std::vector<glr_state*>::iterator;
      const iterator yybegin = yytops.begin();
      const iterator yyend = yytops.end();
      const iterator yyit = std::find_if(yybegin, yyend, glr_state_not_null);
      if (yyit == yyend)
        return false;
      for (state_set_index yyk = create_state_set_index(yyit + 1 - yybegin);
           yyk.uget() != numTops(); ++yyk)
        yytops.yymarkStackDeleted (yyk);
      yytops.yyremoveDeletes ();
      yycompressStack ();
      return true;
    }

    /** Called when returning to deterministic operation to clean up the extra
     * stacks. */
    void
    yycompressStack ()
    {
      if (yytops.size() != 1 || !isSplit())
        return;

      // yyr is the state after the split point.
      glr_state* yyr = YY_NULLPTR;
      for (glr_state *yyp = firstTop(), *yyq = yyp->pred();
           yyp != yysplitPoint;
           yyr = yyp, yyp = yyq, yyq = yyp->pred())
        yyp->setPred(yyr);

      // This const_cast is okay, since anyway we have access to the mutable
      // yyitems into which yysplitPoint points.
      glr_stack_item* nextFreeItem
        = const_cast<glr_state*> (yysplitPoint)->asItem () + 1;
      yysplitPoint = YY_NULLPTR;
      yytops.clearLastDeleted ();

      while (yyr != YY_NULLPTR)
        {
          nextFreeItem->setState (*yyr);
          glr_state& nextFreeState = nextFreeItem->getState();
          yyr = yyr->pred();
          nextFreeState.setPred(&(nextFreeItem - 1)->getState());
          setFirstTop (&nextFreeState);
          ++nextFreeItem;
        }
      yyitems.resize(static_cast<size_t>(nextFreeItem - yyitems.data()));
    }

    bool isSplit() const {
      return yysplitPoint != YY_NULLPTR;
    }

    // Present the interface of a vector of glr_stack_item.
    std::vector<glr_stack_item>::const_iterator begin () const
    {
      return yyitems.begin ();
    }

    std::vector<glr_stack_item>::const_iterator end () const
    {
      return yyitems.end ();
    }

    size_t size() const
    {
      return yyitems.size ();
    }

    glr_stack_item& operator[] (size_t i)
    {
      return yyitems[i];
    }

    glr_stack_item& stackItemAt (size_t index)
    {
      return yyitems[index];
    }

    size_t numTops () const
    {
      return yytops.size ();
    }

    glr_state* firstTop () const
    {
      return yytops[create_state_set_index (0)];
    }

    glr_state* topAt (state_set_index i) const
    {
      return yytops[i];
    }

    void setFirstTop (glr_state* value)
    {
      yytops[create_state_set_index (0)] = value;
    }

    void setTopAt (state_set_index i, glr_state* value)
    {
      yytops[i] = value;
    }

    void pop_back ()
    {
      yyitems.pop_back ();
    }

    void pop_back (size_t n)
    {
      yyitems.resize (yyitems.size () - n);
    }

    state_set_index
    yysplitStack (state_set_index yyk)
    {
      if (!isSplit ())
        {
          YYASSERT (yyk.get () == 0);
          yysplitPoint = topAt (yyk);
        }
      return yytops.yysplitStack (yyk);
    }

    /** Assuming that YYS is a GLRState somewhere on *this, update the
     *  splitpoint of *this, if needed, so that it is at least as deep as
     *  YYS.  */
    void
    yyupdateSplit (glr_state& yys)
    {
      if (isSplit() && &yys < yysplitPoint)
        yysplitPoint = &yys;
    }

    /** Return a fresh GLRState.
     * Callers should call yyreserveStack afterwards to make sure there is
     * sufficient headroom.  */
    glr_state& yynewGLRState (const glr_state& newState)
    {
      glr_state& state = yyitems[yynewGLRStackItem (true)].getState ();
#if false && 201103L <= YY_CPLUSPLUS
      state = std::move (newState);
#else
      state = newState;
#endif
      return state;
    }

    /** Return a fresh SemanticOption.
     * Callers should call yyreserveStack afterwards to make sure there is
     * sufficient headroom.  */
    semantic_option& yynewSemanticOption (semantic_option newOption)
    {
      semantic_option& option = yyitems[yynewGLRStackItem (false)].getOption ();
      option = std::move (newOption);
      return option;
    }

    /* Do nothing if YYNORMAL or if *YYLOW <= YYLOW1.  Otherwise, fill in
     * YYVSP[YYLOW1 .. *YYLOW-1] as in yyfillin and set *YYLOW = YYLOW1.
     * For convenience, always return YYLOW1.  */
    int
    yyfill (glr_stack_item *yyvsp, int &yylow, int yylow1, bool yynormal)
    {
      if (!yynormal && yylow1 < yylow)
        {
          yyfillin (yyvsp, yylow, yylow1);
          yylow = yylow1;
        }
      return yylow1;
    }

    /** Fill in YYVSP[YYLOW1 .. YYLOW0-1] from the chain of states starting
     *  at YYVSP[YYLOW0].getState().pred().  Leaves YYVSP[YYLOW1].getState().pred()
     *  containing the pointer to the next state in the chain.  */
    void
    yyfillin (glr_stack_item *yyvsp, int yylow0, int yylow1)
    {
      glr_state* s = yyvsp[yylow0].getState().pred();
      YYASSERT(s != YY_NULLPTR);
      for (int i = yylow0-1; i >= yylow1; i -= 1, s = s->pred())
        {
          glr_state& yys = yyvsp[i].getState();
#if YYDEBUG
          yys.yylrState = s->yylrState;
#endif
          yys.yyresolved = s->yyresolved;
          if (s->yyresolved)
            {
              new (&yys.value ()) value_type ();
              switch (yy_accessing_symbol (s->yylrState))
    {
      case symbol_kind::S_flow_literal: // flow_literal
      case symbol_kind::S_generator_expression: // generator_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_ternary_expression: // ternary_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_bitwise_or_expression: // bitwise_or_expression
      case symbol_kind::S_bitwise_xor_expression: // bitwise_xor_expression
      case symbol_kind::S_bitwise_and_expression: // bitwise_and_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_primary_expression: // primary_expression
        yys.value ().copy< idyl::parser::expr_ptr > (s->value ());
        break;

      case symbol_kind::S_program: // program
        yys.value ().copy< idyl::parser::node_ptr > (s->value ());
        break;

      case symbol_kind::S_parameter: // parameter
        yys.value ().copy< idyl::parser::param_ptr > (s->value ());
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        yys.value ().copy< idyl::parser::stmt_ptr > (s->value ());
        break;

      case symbol_kind::S_argument: // argument
        yys.value ().copy< std::shared_ptr<idyl::parser::argument> > (s->value ());
        break;

      case symbol_kind::S_at_block: // at_block
        yys.value ().copy< std::shared_ptr<idyl::parser::at_block> > (s->value ());
        break;

      case symbol_kind::S_catch_block: // catch_block
        yys.value ().copy< std::shared_ptr<idyl::parser::catch_block> > (s->value ());
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        yys.value ().copy< std::shared_ptr<idyl::parser::flow_definition> > (s->value ());
        break;

      case symbol_kind::S_function_definition: // function_definition
        yys.value ().copy< std::shared_ptr<idyl::parser::function_definition> > (s->value ());
        break;

      case symbol_kind::S_init_block: // init_block
        yys.value ().copy< std::shared_ptr<idyl::parser::init_block> > (s->value ());
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        yys.value ().copy< std::shared_ptr<idyl::parser::lambda_block> > (s->value ());
        break;

      case symbol_kind::S_on_block: // on_block
        yys.value ().copy< std::shared_ptr<idyl::parser::on_block> > (s->value ());
        break;

      case symbol_kind::S_process_block: // process_block
        yys.value ().copy< std::shared_ptr<idyl::parser::process_block> > (s->value ());
        break;

      case symbol_kind::S_start_statement: // start_statement
        yys.value ().copy< std::shared_ptr<idyl::parser::start_statement> > (s->value ());
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        yys.value ().copy< std::shared_ptr<idyl::parser::stop_statement> > (s->value ());
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        yys.value ().copy< std::string > (s->value ());
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        yys.value ().copy< std::vector<idyl::parser::expr_ptr> > (s->value ());
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        yys.value ().copy< std::vector<idyl::parser::param_ptr> > (s->value ());
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        yys.value ().copy< std::vector<idyl::parser::stmt_ptr> > (s->value ());
        break;

      case symbol_kind::S_argument_list: // argument_list
        yys.value ().copy< std::vector<std::shared_ptr<idyl::parser::argument>> > (s->value ());
        break;

      case symbol_kind::S_flow_members: // flow_members
        yys.value ().copy< std::vector<std::shared_ptr<idyl::parser::flow_member>> > (s->value ());
        break;

      default:
        break;
    }

            }
          else
            /* The effect of using yyval or yyloc (in an immediate
             * rule) is undefined.  */
            yys.setFirstVal (YY_NULLPTR);
          yys.yyloc = s->yyloc;
          yys.setPred(s->pred());
        }
    }

#if YYDEBUG

    /*----------------------------------------------------------------------.
    | Report that stack #YYK of *YYSTACKP is going to be reduced by YYRULE. |
    `----------------------------------------------------------------------*/

    void
    yy_reduce_print (bool yynormal, glr_stack_item* yyvsp, state_set_index yyk,
                     rule_num yyrule, parser_type& yyparser)
    {
      int yynrhs = yyrhsLength (yyrule);
      int yylow = 1;
      int yyi;
      std::cerr << "Reducing stack " << yyk.get() << " by rule " << yyrule - 1
                << " (line " << int (yyrline[yyrule]) << "):\n";
      if (! yynormal)
        yyfillin (yyvsp, 1, -yynrhs);
      /* The symbols being reduced.  */
      for (yyi = 0; yyi < yynrhs; yyi++)
        {
          std::cerr << "   $" << yyi + 1 << " = ";
          yyparser.yy_symbol_print_
            (yy_accessing_symbol (yyvsp[yyi - yynrhs + 1].getState().yylrState),
             yyvsp[yyi - yynrhs + 1].getState().value (),
             ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL ((yyi + 1) - (yynrhs))].getState().yyloc));
          if (!yyvsp[yyi - yynrhs + 1].getState().yyresolved)
            std::cerr << " (unresolved)";
          std::cerr << '\n';
        }
    }


#define YYINDEX(YYX)                                                         \
      ((YYX) == YY_NULLPTR ? -1 : (YYX)->indexIn (yyitems.data ()))

    void
    dumpStack () const
    {
      for (size_t yyi = 0; yyi < size(); ++yyi)
        {
          const glr_stack_item& item = yyitems[yyi];
          std::cerr << std::setw(3) << yyi << ". ";
          if (item.is_state())
            {
              std::cerr << "Res: " << item.getState().yyresolved
                        << ", LR State: " << item.getState().yylrState
                        << ", posn: " << item.getState().yyposn
                        << ", pred: " << YYINDEX(item.getState().pred());
              if (! item.getState().yyresolved)
                std::cerr << ", firstVal: "
                          << YYINDEX(item.getState().firstVal());
            }
          else
            {
              std::cerr << "Option. rule: " << item.getOption().yyrule - 1
                        << ", state: " << YYINDEX(item.getOption().state())
                        << ", next: " << YYINDEX(item.getOption().next());
            }
          std::cerr << '\n';
        }
      std::cerr << "Tops:";
      for (state_set_index yyi = create_state_set_index(0); yyi.uget() < numTops(); ++yyi) {
        std::cerr << yyi.get() << ": " << YYINDEX(topAt(yyi)) << "; ";
      }
      std::cerr << '\n';
    }

#undef YYINDEX
#endif

    YYRESULTTAG
    yyreportAmbiguity (const semantic_option& yyx0,
                       const semantic_option& yyx1, parser_type& yyparser, const location_type& yyloc)
    {
      YY_USE (yyx0);
      YY_USE (yyx1);

#if YYDEBUG
      std::cerr << "Ambiguity detected.\n"
        "Option 1,\n";
      yyx0.yyreportTree ();
      std::cerr << "\nOption 2,\n";
      yyx1.yyreportTree ();
      std::cerr << '\n';
#endif

      yyparser.error (yyloc, YY_("syntax is ambiguous"));
      return yyabort;
    }

#if YYDEBUG
    /* Print YYS (possibly NULL) and its predecessors. */
    void
    yypstates (const glr_state* yys) const
    {
      if (yys != YY_NULLPTR)
        yys->yy_yypstack();
      else
        std::cerr << "<null>";
      std::cerr << '\n';
    }
#endif

  private:
    size_t spaceLeft() const
    {
      return yyitems.capacity() - yyitems.size();
    }

    /** Return a fresh GLRStackItem in this.  The item is an LR state
     *  if YYIS_STATE, and otherwise a semantic option.  Callers should call
     *  yyreserveStack afterwards to make sure there is sufficient
     *  headroom.  */
    size_t
    yynewGLRStackItem (bool yyis_state)
    {
      YYDASSERT(yyitems.size() < yyitems.capacity());
      yyitems.push_back(glr_stack_item(yyis_state));
      return yyitems.size() - 1;
    }


  public:
    std::vector<glr_stack_item> yyitems;
    // Where the stack splits. Anything below this address is deterministic.
    const glr_state* yysplitPoint;
    glr_state_set yytops;
  }; // class state_stack
} // namespace

#undef YYFILL
#define YYFILL(N) yystateStack.yyfill (yyvsp, yylow, (N), yynormal)

namespace yy
{
  class parser::glr_stack
  {
  public:

    // Needs access to yypact_value_is_default, etc.
    friend context;


    glr_stack (size_t yysize, parser_type& yyparser_yyarg, yy::lexer& driver_yyarg)
      : yyerrState (0)
      , yystateStack (yysize)
      , yyerrcnt (0)
      , yyla ()
      , yyparser (yyparser_yyarg),
      driver (driver_yyarg)
    {}

    ~glr_stack ()
    {
      if (!this->yyla.empty ())
        yyparser.yy_destroy_ ("Cleanup: discarding lookahead",
                              this->yyla.kind (), this->yyla.value, this->yyla.location);
      popall_ ();
    }

    int yyerrState;
  /* To compute the location of the error token.  */
    glr_stack_item yyerror_range[3];
    state_stack yystateStack;
    int yyerrcnt;
    symbol_type yyla;
    YYJMP_BUF yyexception_buffer;
    parser_type& yyparser;

  #define YYCHK1(YYE)                                                          \
    do {                                                                       \
      switch (YYE) {                                                           \
      case yyok:                                                               \
        break;                                                                 \
      case yyabort:                                                            \
        goto yyabortlab;                                                       \
      case yyaccept:                                                           \
        goto yyacceptlab;                                                      \
      case yyerr:                                                              \
        goto yyuser_error;                                                     \
      default:                                                                 \
        goto yybuglab;                                                         \
      }                                                                        \
    } while (false)

    int
    parse ()
    {
      int yyresult;
      size_t yyposn;

      YYCDEBUG << "Starting parse\n";

      this->yyla.clear ();

      switch (YYSETJMP (this->yyexception_buffer))
        {
        case 0: break;
        case 1: goto yyabortlab;
        case 2: goto yyexhaustedlab;
        default: goto yybuglab;
        }
      this->yyglrShift (create_state_set_index(0), 0, 0, this->yyla.value, this->yyla.location);
      yyposn = 0;

      while (true)
        {
          /* For efficiency, we have two loops, the first of which is
             specialized to deterministic operation (single stack, no
             potential ambiguity).  */
          /* Standard mode */
          while (true)
            {
              const state_num yystate = this->firstTopState()->yylrState;
              YYCDEBUG << "Entering state " << yystate << '\n';
              if (yystate == YYFINAL)
                goto yyacceptlab;
              if (yy_is_defaulted_state (yystate))
                {
                  const rule_num yyrule = yy_default_action (yystate);
                  if (yyrule == 0)
                    {
                      this->yyerror_range[1].getState().yyloc = this->yyla.location;
                      this->yyreportSyntaxError ();
                      goto yyuser_error;
                    }
                  YYCHK1 (this->yyglrReduce (create_state_set_index(0), yyrule, true));
                }
              else
                {
                  yyget_token ();
                  const short* yyconflicts;
                  const int yyaction = yygetLRActions (yystate, this->yyla.kind (), yyconflicts);
                  if (*yyconflicts != 0)
                    break;
                  if (yy_is_shift_action (yyaction))
                    {
                      YY_SYMBOL_PRINT ("Shifting", this->yyla.kind (), this->yyla.value, this->yyla.location);
                      yyposn += 1;
                      // FIXME: we should move yylval.
                      this->yyglrShift (create_state_set_index(0), yyaction, yyposn, this->yyla.value, this->yyla.location);
                      yyla.clear ();
                      if (0 < this->yyerrState)
                        this->yyerrState -= 1;
                    }
                  else if (yy_is_error_action (yyaction))
                    {
                      this->yyerror_range[1].getState().yyloc = this->yyla.location;
                      /* Don't issue an error message again for exceptions
                         thrown from the scanner.  */
                      if (this->yyla.kind () != symbol_kind::S_YYerror)
                        this->yyreportSyntaxError ();
                      goto yyuser_error;
                    }
                  else
                    YYCHK1 (this->yyglrReduce (create_state_set_index(0), -yyaction, true));
                }
            }

          while (true)
            {
              for (state_set_index yys = create_state_set_index(0); yys.uget() < this->yystateStack.numTops(); ++yys)
                this->yystateStack.yytops.setLookaheadNeeds(yys, !this->yyla.empty ());

              /* yyprocessOneStack returns one of three things:

                  - An error flag.  If the caller is yyprocessOneStack, it
                    immediately returns as well.  When the caller is finally
                    yyparse, it jumps to an error label via YYCHK1.

                  - yyok, but yyprocessOneStack has invoked yymarkStackDeleted
                    (yys), which sets the top state of yys to NULL.  Thus,
                    yyparse's following invocation of yyremoveDeletes will remove
                    the stack.

                  - yyok, when ready to shift a token.

                 Except in the first case, yyparse will invoke yyremoveDeletes and
                 then shift the next token onto all remaining stacks.  This
                 synchronization of the shift (that is, after all preceding
                 reductions on all stacks) helps prevent double destructor calls
                 on yylval in the event of memory exhaustion.  */

              for (state_set_index yys = create_state_set_index (0); yys.uget () < this->yystateStack.numTops (); ++yys)
                YYCHK1 (this->yyprocessOneStack (yys, yyposn, &this->yyla.location));
              this->yystateStack.yytops.yyremoveDeletes ();
              if (this->yystateStack.yytops.size() == 0)
                {
                  this->yystateStack.yytops.yyundeleteLastStack ();
                  if (this->yystateStack.yytops.size() == 0)
                    this->yyFail (&this->yyla.location, YY_("syntax error"));
                  YYCHK1 (this->yyresolveStack ());
                  YYCDEBUG << "Returning to deterministic operation.\n";
                  this->yyerror_range[1].getState ().yyloc = this->yyla.location;
                  this->yyreportSyntaxError ();
                  goto yyuser_error;
                }

              /* If any yyglrShift call fails, it will fail after shifting.  Thus,
                 a copy of yylval will already be on stack 0 in the event of a
                 failure in the following loop.  Thus, yyla is emptied
                 before the loop to make sure the user destructor for yylval isn't
                 called twice.  */
              symbol_kind_type yytoken_to_shift = this->yyla.kind ();
              this->yyla.kind_ = symbol_kind::S_YYEMPTY;
              yyposn += 1;
              for (state_set_index yys = create_state_set_index (0); yys.uget () < this->yystateStack.numTops (); ++yys)
                {
                  const state_num yystate = this->topState (yys)->yylrState;
                  const short* yyconflicts;
                  const int yyaction = yygetLRActions (yystate, yytoken_to_shift, yyconflicts);
                  /* Note that yyconflicts were handled by yyprocessOneStack.  */
                  YYCDEBUG << "On stack " << yys.get() << ", ";
                  YY_SYMBOL_PRINT ("shifting", yytoken_to_shift, this->yyla.value, this->yyla.location);
                  this->yyglrShift (yys, yyaction, yyposn, this->yyla.value, this->yyla.location);
                  YYCDEBUG << "Stack " << yys.get() << " now in state "
                           << this->topState(yys)->yylrState << '\n';
                }

                // FIXME: User destructors.
                // Value type destructor.
                switch (yytoken_to_shift)
    {
      case symbol_kind::S_flow_literal: // flow_literal
      case symbol_kind::S_generator_expression: // generator_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_ternary_expression: // ternary_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_bitwise_or_expression: // bitwise_or_expression
      case symbol_kind::S_bitwise_xor_expression: // bitwise_xor_expression
      case symbol_kind::S_bitwise_and_expression: // bitwise_and_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_primary_expression: // primary_expression
        this->yyla.value.template destroy< idyl::parser::expr_ptr > ();
        break;

      case symbol_kind::S_program: // program
        this->yyla.value.template destroy< idyl::parser::node_ptr > ();
        break;

      case symbol_kind::S_parameter: // parameter
        this->yyla.value.template destroy< idyl::parser::param_ptr > ();
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        this->yyla.value.template destroy< idyl::parser::stmt_ptr > ();
        break;

      case symbol_kind::S_argument: // argument
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::argument> > ();
        break;

      case symbol_kind::S_at_block: // at_block
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::at_block> > ();
        break;

      case symbol_kind::S_catch_block: // catch_block
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::catch_block> > ();
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::flow_definition> > ();
        break;

      case symbol_kind::S_function_definition: // function_definition
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::function_definition> > ();
        break;

      case symbol_kind::S_init_block: // init_block
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::init_block> > ();
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::lambda_block> > ();
        break;

      case symbol_kind::S_on_block: // on_block
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::on_block> > ();
        break;

      case symbol_kind::S_process_block: // process_block
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::process_block> > ();
        break;

      case symbol_kind::S_start_statement: // start_statement
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::start_statement> > ();
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::stop_statement> > ();
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        this->yyla.value.template destroy< std::string > ();
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        this->yyla.value.template destroy< std::vector<idyl::parser::expr_ptr> > ();
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        this->yyla.value.template destroy< std::vector<idyl::parser::param_ptr> > ();
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        this->yyla.value.template destroy< std::vector<idyl::parser::stmt_ptr> > ();
        break;

      case symbol_kind::S_argument_list: // argument_list
        this->yyla.value.template destroy< std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        break;

      case symbol_kind::S_flow_members: // flow_members
        this->yyla.value.template destroy< std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        break;

      default:
        break;
    }


              if (this->yystateStack.yytops.size () == 1)
                {
                  YYCHK1 (this->yyresolveStack ());
                  YYCDEBUG << "Returning to deterministic operation.\n";
                  this->yystateStack.yycompressStack ();
                  break;
                }
            }
          continue;
        yyuser_error:
          this->yyrecoverSyntaxError (&this->yyla.location);
          yyposn = this->firstTopState()->yyposn;
        }

    yyacceptlab:
      yyresult = 0;
      goto yyreturn;

    yybuglab:
      YYASSERT (false);
      goto yyabortlab;

    yyabortlab:
      yyresult = 1;
      goto yyreturn;

    yyexhaustedlab:
      yyparser.error (this->yyla.location, YY_("memory exhausted"));
      yyresult = 2;
      goto yyreturn;

    yyreturn:
      return yyresult;
    }
  #undef YYCHK1

    void yyreserveGlrStack ()
    {
      if (!yystateStack.yyexpandGLRStackIfNeeded ())
        yyMemoryExhausted ();
    }

    _Noreturn void
    yyMemoryExhausted ()
    {
      YYLONGJMP (yyexception_buffer, 2);
    }

    _Noreturn void
    yyFail (location_type* yylocp, const char* yymsg)
    {
      if (yymsg != YY_NULLPTR)
        yyparser.error (*yylocp, yymsg);
      YYLONGJMP (yyexception_buffer, 1);
    }

                                  /* GLRStates */


    /** Add a new semantic action that will execute the action for rule
     *  YYRULE on the semantic values in YYRHS to the list of
     *  alternative actions for YYSTATE.  Assumes that YYRHS comes from
     *  stack #YYK of *this. */
    void
    yyaddDeferredAction (state_set_index yyk, glr_state* yystate,
                         glr_state* yyrhs, rule_num yyrule)
    {
      semantic_option& yyopt = yystateStack.yynewSemanticOption (semantic_option (yyrule));
      yyopt.setState (yyrhs);
      yyopt.setNext (yystate->firstVal ());
      if (yystateStack.yytops.lookaheadNeeds (yyk))
        yyopt.yyla = this->yyla;
      yystate->setFirstVal (&yyopt);

      yyreserveGlrStack ();
    }

  #if YYDEBUG
    void yypdumpstack () const
    {
      yystateStack.dumpStack();
    }
  #endif

    void
    yyreportSyntaxError ()
    {
      if (yyerrState != 0)
        return;

      context yyctx (*this, yyla);
      std::string msg = yyparser.yysyntax_error_ (yyctx);
      yyparser.error (yyla.location, YY_MOVE (msg));
      yyerrcnt += 1;
    }

    /* Recover from a syntax error on this, assuming that yytoken,
       yylval, and yylloc are the syntactic category, semantic value, and location
       of the lookahead.  */
    void
    yyrecoverSyntaxError (location_type* yylocp)
    {
      if (yyerrState == 3)
        /* We just shifted the error token and (perhaps) took some
           reductions.  Skip tokens until we can proceed.  */
        while (true)
          {
            if (this->yyla.kind () == symbol_kind::S_YYEOF)
              yyFail (yylocp, YY_NULLPTR);
            if (this->yyla.kind () != symbol_kind::S_YYEMPTY)
              {
                /* We throw away the lookahead, but the error range
                   of the shifted error token must take it into account.  */
                glr_state *yys = firstTopState();
                yyerror_range[1].getState().yyloc = yys->yyloc;
                yyerror_range[2].getState().yyloc = this->yyla.location;
                YYLLOC_DEFAULT ((yys->yyloc), yyerror_range, 2);
                yyparser.yy_destroy_ ("Error: discarding",
                                      this->yyla.kind (), this->yyla.value, this->yyla.location);
                // Value type destructor.
                switch (this->yyla.kind ())
    {
      case symbol_kind::S_flow_literal: // flow_literal
      case symbol_kind::S_generator_expression: // generator_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_ternary_expression: // ternary_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_bitwise_or_expression: // bitwise_or_expression
      case symbol_kind::S_bitwise_xor_expression: // bitwise_xor_expression
      case symbol_kind::S_bitwise_and_expression: // bitwise_and_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_primary_expression: // primary_expression
        this->yyla.value.template destroy< idyl::parser::expr_ptr > ();
        break;

      case symbol_kind::S_program: // program
        this->yyla.value.template destroy< idyl::parser::node_ptr > ();
        break;

      case symbol_kind::S_parameter: // parameter
        this->yyla.value.template destroy< idyl::parser::param_ptr > ();
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        this->yyla.value.template destroy< idyl::parser::stmt_ptr > ();
        break;

      case symbol_kind::S_argument: // argument
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::argument> > ();
        break;

      case symbol_kind::S_at_block: // at_block
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::at_block> > ();
        break;

      case symbol_kind::S_catch_block: // catch_block
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::catch_block> > ();
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::flow_definition> > ();
        break;

      case symbol_kind::S_function_definition: // function_definition
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::function_definition> > ();
        break;

      case symbol_kind::S_init_block: // init_block
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::init_block> > ();
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::lambda_block> > ();
        break;

      case symbol_kind::S_on_block: // on_block
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::on_block> > ();
        break;

      case symbol_kind::S_process_block: // process_block
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::process_block> > ();
        break;

      case symbol_kind::S_start_statement: // start_statement
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::start_statement> > ();
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        this->yyla.value.template destroy< std::shared_ptr<idyl::parser::stop_statement> > ();
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        this->yyla.value.template destroy< std::string > ();
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        this->yyla.value.template destroy< std::vector<idyl::parser::expr_ptr> > ();
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        this->yyla.value.template destroy< std::vector<idyl::parser::param_ptr> > ();
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        this->yyla.value.template destroy< std::vector<idyl::parser::stmt_ptr> > ();
        break;

      case symbol_kind::S_argument_list: // argument_list
        this->yyla.value.template destroy< std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        break;

      case symbol_kind::S_flow_members: // flow_members
        this->yyla.value.template destroy< std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        break;

      default:
        break;
    }

                this->yyla.kind_ = symbol_kind::S_YYEMPTY;
              }
            yyget_token ();
            int yyj = yypact[firstTopState()->yylrState];
            if (yypact_value_is_default (yyj))
              return;
            yyj += this->yyla.kind ();
            if (yyj < 0 || YYLAST < yyj || yycheck[yyj] != this->yyla.kind ())
              {
                if (yydefact[firstTopState()->yylrState] != 0)
                  return;
              }
            else if (! yytable_value_is_error (yytable[yyj]))
              return;
          }

      if (!yystateStack.reduceToOneStack())
        yyFail (yylocp, YY_NULLPTR);

      /* Now pop stack until we find a state that shifts the error token.  */
      yyerrState = 3;
      while (firstTopState () != YY_NULLPTR)
        {
          glr_state *yys = firstTopState ();
          int yyj = yypact[yys->yylrState];
          if (! yypact_value_is_default (yyj))
            {
              yyj += YYTERROR;
              if (0 <= yyj && yyj <= YYLAST && yycheck[yyj] == YYTERROR
                  && yy_is_shift_action (yytable[yyj]))
                {
                  /* Shift the error token.  */
                  /* First adjust its location.*/
                  location_type yyerrloc;
                  yyerror_range[2].getState().yyloc = this->yyla.location;
                  YYLLOC_DEFAULT (yyerrloc, (yyerror_range), 2);
                  YY_SYMBOL_PRINT ("Shifting", yy_accessing_symbol (yytable[yyj]),
                                   this->yyla.value, yyerrloc);
                  yyglrShift (create_state_set_index(0), yytable[yyj],
                              yys->yyposn, yyla.value, yyerrloc);
                  yys = firstTopState();
                  break;
                }
            }
          yyerror_range[1].getState().yyloc = yys->yyloc;
          if (yys->pred() != YY_NULLPTR)
            yys->destroy ("Error: popping", yyparser);
          yystateStack.setFirstTop(yys->pred());
          yystateStack.pop_back();
        }
      if (firstTopState() == YY_NULLPTR)
        yyFail (yylocp, YY_NULLPTR);
    }

    YYRESULTTAG
    yyprocessOneStack (state_set_index yyk,
                       size_t yyposn, location_type* yylocp)
    {
      while (yystateStack.topAt(yyk) != YY_NULLPTR)
        {
          const state_num yystate = topState(yyk)->yylrState;
          YYCDEBUG << "Stack " << yyk.get()
                   << " Entering state " << yystate << '\n';

          YYASSERT (yystate != YYFINAL);

          if (yy_is_defaulted_state (yystate))
            {
              const rule_num yyrule = yy_default_action (yystate);
              if (yyrule == 0)
                {
                  YYCDEBUG << "Stack " << yyk.get() << " dies.\n";
                  yystateStack.yytops.yymarkStackDeleted (yyk);
                  return yyok;
                }
              const YYRESULTTAG yyflag
                = yyglrReduce (yyk, yyrule, yyimmediate[yyrule]);
              if (yyflag == yyerr)
                {
                  YYCDEBUG << "Stack " << yyk.get() << " dies"
                    " (predicate failure or explicit user error).\n";
                  yystateStack.yytops.yymarkStackDeleted (yyk);
                  return yyok;
                }
              if (yyflag != yyok)
                return yyflag;
            }
          else
            {
              yystateStack.yytops.setLookaheadNeeds(yyk, true);
              yyget_token ();
              const short* yyconflicts;
              const int yyaction = yygetLRActions (yystate, this->yyla.kind (), yyconflicts);

              for (; *yyconflicts != 0; ++yyconflicts)
                {
                  state_set_index yynewStack = yystateStack.yysplitStack (yyk);
                  YYCDEBUG << "Splitting off stack " << yynewStack.get()
                           << " from " << yyk.get() << ".\n";
                  YYRESULTTAG yyflag =
                    yyglrReduce (yynewStack, *yyconflicts, yyimmediate[*yyconflicts]);
                  if (yyflag == yyok)
                    YYCHK (yyprocessOneStack (yynewStack,
                                              yyposn, yylocp));
                  else if (yyflag == yyerr)
                    {
                      YYCDEBUG << "Stack " << yynewStack.get() << " dies.\n";
                      yystateStack.yytops.yymarkStackDeleted (yynewStack);
                    }
                  else
                    return yyflag;
                }

              if (yy_is_shift_action (yyaction))
                break;
              else if (yy_is_error_action (yyaction))
                {
                  YYCDEBUG << "Stack " << yyk.get() << " dies.\n";
                  yystateStack.yytops.yymarkStackDeleted (yyk);
                  break;
                }
              else
                {
                  YYRESULTTAG yyflag
                    = yyglrReduce (yyk, -yyaction, yyimmediate[-yyaction]);
                  if (yyflag == yyerr)
                    {
                      YYCDEBUG << "Stack " << yyk.get() << " dies"
                        " (predicate failure or explicit user error).\n";
                      yystateStack.yytops.yymarkStackDeleted (yyk);
                      break;
                    }
                  else if (yyflag != yyok)
                    return yyflag;
                }
            }
        }
      return yyok;
    }

    /** Perform user action for rule number YYN, with RHS length YYRHSLEN,
     *  and top stack item YYVSP.  YYVALP points to place to put semantic
     *  value ($$), and yylocp points to place for location information
     *  (@$).  Returns yyok for normal return, yyaccept for YYACCEPT,
     *  yyerr for YYERROR, yyabort for YYABORT.  */
    YYRESULTTAG
    yyuserAction (rule_num yyrule, int yyrhslen, glr_stack_item* yyvsp, state_set_index yyk,
                  value_type* yyvalp, location_type* yylocp)
    {
      bool yynormal YY_ATTRIBUTE_UNUSED = !yystateStack.isSplit();
      int yylow = 1;
  YY_USE (yyvalp);
  YY_USE (yylocp);
  YY_USE (driver);
      YY_USE (yyk);
      YY_USE (yyrhslen);
    # undef yyerrok
    # define yyerrok (yyerrState = 0)
    # undef YYACCEPT
    # define YYACCEPT return yyaccept
    # undef YYABORT
    # define YYABORT return yyabort
    # undef YYERROR
    # define YYERROR return yyerrok, yyerr
    # undef YYRECOVERING
    # define YYRECOVERING() (yyerrState != 0)
    # undef yytoken
    # define yytoken this->yyla.kind_
    # undef yyclearin
    # define yyclearin (yytoken = symbol_kind::S_YYEMPTY)
    # undef YYBACKUP
    # define YYBACKUP(Token, Value)                                              \
      return yyparser.error (*yylocp, YY_("syntax error: cannot back up")),     \
             yyerrok, yyerr


      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      // However we really need to prepare yyvsp now if we want to get
      // correct locations, so invoke YYFILL for $1 anyway.
      (void) YYFILL (1-yyrhslen);
      switch (yylhsNonterm (yyrule))
    {
      case symbol_kind::S_flow_literal: // flow_literal
      case symbol_kind::S_generator_expression: // generator_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_ternary_expression: // ternary_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_bitwise_or_expression: // bitwise_or_expression
      case symbol_kind::S_bitwise_xor_expression: // bitwise_xor_expression
      case symbol_kind::S_bitwise_and_expression: // bitwise_and_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_primary_expression: // primary_expression
        (*yyvalp).emplace< idyl::parser::expr_ptr > ();
        break;

      case symbol_kind::S_program: // program
        (*yyvalp).emplace< idyl::parser::node_ptr > ();
        break;

      case symbol_kind::S_parameter: // parameter
        (*yyvalp).emplace< idyl::parser::param_ptr > ();
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        (*yyvalp).emplace< idyl::parser::stmt_ptr > ();
        break;

      case symbol_kind::S_argument: // argument
        (*yyvalp).emplace< std::shared_ptr<idyl::parser::argument> > ();
        break;

      case symbol_kind::S_at_block: // at_block
        (*yyvalp).emplace< std::shared_ptr<idyl::parser::at_block> > ();
        break;

      case symbol_kind::S_catch_block: // catch_block
        (*yyvalp).emplace< std::shared_ptr<idyl::parser::catch_block> > ();
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        (*yyvalp).emplace< std::shared_ptr<idyl::parser::flow_definition> > ();
        break;

      case symbol_kind::S_function_definition: // function_definition
        (*yyvalp).emplace< std::shared_ptr<idyl::parser::function_definition> > ();
        break;

      case symbol_kind::S_init_block: // init_block
        (*yyvalp).emplace< std::shared_ptr<idyl::parser::init_block> > ();
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        (*yyvalp).emplace< std::shared_ptr<idyl::parser::lambda_block> > ();
        break;

      case symbol_kind::S_on_block: // on_block
        (*yyvalp).emplace< std::shared_ptr<idyl::parser::on_block> > ();
        break;

      case symbol_kind::S_process_block: // process_block
        (*yyvalp).emplace< std::shared_ptr<idyl::parser::process_block> > ();
        break;

      case symbol_kind::S_start_statement: // start_statement
        (*yyvalp).emplace< std::shared_ptr<idyl::parser::start_statement> > ();
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        (*yyvalp).emplace< std::shared_ptr<idyl::parser::stop_statement> > ();
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        (*yyvalp).emplace< std::string > ();
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        (*yyvalp).emplace< std::vector<idyl::parser::expr_ptr> > ();
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        (*yyvalp).emplace< std::vector<idyl::parser::param_ptr> > ();
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        (*yyvalp).emplace< std::vector<idyl::parser::stmt_ptr> > ();
        break;

      case symbol_kind::S_argument_list: // argument_list
        (*yyvalp).emplace< std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        break;

      case symbol_kind::S_flow_members: // flow_members
        (*yyvalp).emplace< std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        break;

      default:
        break;
    }

      /* Default location. */
      YYLLOC_DEFAULT ((*yylocp), (yyvsp - yyrhslen), yyrhslen);
      yyerror_range[1].getState().yyloc = *yylocp;

    /* If yyk == -1, we are running a deferred action on a temporary
       stack.  In that case, YY_REDUCE_PRINT must not play with YYFILL,
       so pretend the stack is "normal". */
    YY_REDUCE_PRINT ((yynormal || yyk == create_state_set_index (-1), yyvsp, yyk, yyrule, yyparser));
    #if YY_EXCEPTIONS
      try
      {
    #endif // YY_EXCEPTIONS
      switch (yyrule)
        {
      case 2: // program: top_level_statements
#line 116 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto prog = std::make_shared<idyl::parser::program>();
        prog->statements_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::vector<idyl::parser::stmt_ptr> > ();
        (*yyvalp).as < idyl::parser::node_ptr > () = prog;
        g_program = prog;
    }
#line 4016 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 3: // program: %empty
#line 123 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto prog = std::make_shared<idyl::parser::program>();
        (*yyvalp).as < idyl::parser::node_ptr > () = prog;
        g_program = prog;
    }
#line 4026 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 4: // top_level_statements: top_level_statements top_level_statement
#line 132 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<idyl::parser::stmt_ptr> > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<idyl::parser::stmt_ptr> > ();
        if ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::stmt_ptr > ()) (*yyvalp).as < std::vector<idyl::parser::stmt_ptr> > ().push_back((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::stmt_ptr > ());
    }
#line 4035 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 5: // top_level_statements: top_level_statement
#line 137 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<idyl::parser::stmt_ptr> > () = {};
        if ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::stmt_ptr > ()) (*yyvalp).as < std::vector<idyl::parser::stmt_ptr> > ().push_back((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::stmt_ptr > ());
    }
#line 4044 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 6: // top_level_statement: function_or_flow_definition
#line 144 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                  { (*yyvalp).as < idyl::parser::stmt_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::stmt_ptr > (); }
#line 4050 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 7: // top_level_statement: process_block
#line 145 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                    { (*yyvalp).as < idyl::parser::stmt_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::shared_ptr<idyl::parser::process_block> > (); }
#line 4056 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 8: // top_level_statement: IMPORT LPAREN STRING_LITERAL RPAREN
#line 147 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lib_import = std::make_shared<idyl::parser::library_import>();
        lib_import->path_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::string > ();
        lib_import->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.line;
        lib_import->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::stmt_ptr > () = lib_import;
    }
#line 4068 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 9: // top_level_statement: IDENTIFIER ASSIGN IMPORT LPAREN STRING_LITERAL RPAREN
#line 155 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lib_import = std::make_shared<idyl::parser::library_import>();
        lib_import->namespace_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().value ().as < std::string > ();
        lib_import->path_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::string > ();
        lib_import->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.line;
        lib_import->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::stmt_ptr > () = lib_import;
    }
#line 4081 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 10: // top_level_statement: MODULE LPAREN STRING_LITERAL RPAREN
#line 164 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto mod_import = std::make_shared<idyl::parser::module_import>();
        mod_import->path_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::string > ();
        mod_import->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.line;
        mod_import->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::stmt_ptr > () = mod_import;
    }
#line 4093 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 11: // top_level_statement: IDENTIFIER ASSIGN MODULE LPAREN STRING_LITERAL RPAREN
#line 172 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto mod_import = std::make_shared<idyl::parser::module_import>();
        mod_import->namespace_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().value ().as < std::string > ();
        mod_import->path_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::string > ();
        mod_import->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.line;
        mod_import->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::stmt_ptr > () = mod_import;
    }
#line 4106 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 12: // function_or_flow_definition: function_definition
#line 183 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                          { (*yyvalp).as < idyl::parser::stmt_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::shared_ptr<idyl::parser::function_definition> > (); }
#line 4112 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 13: // function_or_flow_definition: flow_definition
#line 184 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                      { (*yyvalp).as < idyl::parser::stmt_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::shared_ptr<idyl::parser::flow_definition> > (); }
#line 4118 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 14: // function_definition: IDENTIFIER LPAREN parameter_list RPAREN ASSIGN expression
#line 189 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().value ().as < std::string > ();
        func->parameters_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().value ().as < std::vector<idyl::parser::param_ptr> > ();
        func->body_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        func->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.line;
        func->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::function_definition> > () = func;
    }
#line 4132 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 15: // function_definition: IDENTIFIER LPAREN RPAREN ASSIGN expression
#line 199 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().value ().as < std::string > ();
        func->parameters_ = {};
        func->has_parens_ = true;
        func->body_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        func->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.line;
        func->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::function_definition> > () = func;
    }
#line 4147 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 16: // function_definition: IDENTIFIER LPAREN parameter_list RPAREN ASSIGN expression lambda_block
#line 210 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-6)].getState().value ().as < std::string > ();
        func->parameters_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().value ().as < std::vector<idyl::parser::param_ptr> > ();
        func->body_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < idyl::parser::expr_ptr > ();
        func->lambda_block_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::shared_ptr<idyl::parser::lambda_block> > ();
        func->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-6)].getState().yyloc).begin.line;
        func->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-6)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::function_definition> > () = func;
    }
#line 4162 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 17: // function_definition: IDENTIFIER LPAREN RPAREN ASSIGN expression lambda_block
#line 221 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().value ().as < std::string > ();
        func->parameters_ = {};
        func->has_parens_ = true;
        func->body_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < idyl::parser::expr_ptr > ();
        func->lambda_block_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::shared_ptr<idyl::parser::lambda_block> > ();
        func->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.line;
        func->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::function_definition> > () = func;
    }
#line 4178 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 18: // function_definition: IDENTIFIER ASSIGN expression
#line 233 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::string > ();
        func->parameters_ = {};
        func->body_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        func->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        func->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::function_definition> > () = func;
    }
#line 4192 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 19: // function_definition: IDENTIFIER ASSIGN expression lambda_block
#line 243 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().value ().as < std::string > ();
        func->parameters_ = {};
        func->body_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < idyl::parser::expr_ptr > ();
        func->lambda_block_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::shared_ptr<idyl::parser::lambda_block> > ();
        func->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.line;
        func->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::function_definition> > () = func;
    }
#line 4207 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 20: // flow_definition: FLOW IDENTIFIER ASSIGN flow_literal
#line 257 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto flow = std::make_shared<idyl::parser::flow_definition>();
        flow->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::string > ();
        flow->parameters_ = {};
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = "";
        member->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        member->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        member->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        flow->members_.push_back(member);
        flow->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.line;
        flow->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::flow_definition> > () = flow;
    }
#line 4226 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 21: // flow_definition: FLOW IDENTIFIER LPAREN parameter_list RPAREN ASSIGN flow_literal
#line 272 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto flow = std::make_shared<idyl::parser::flow_definition>();
        flow->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().value ().as < std::string > ();
        flow->parameters_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().value ().as < std::vector<idyl::parser::param_ptr> > ();
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = "";
        member->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        member->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        member->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        flow->members_.push_back(member);
        flow->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-6)].getState().yyloc).begin.line;
        flow->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-6)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::flow_definition> > () = flow;
    }
#line 4245 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 22: // flow_definition: FLOW IDENTIFIER ASSIGN LBRACE flow_members RBRACE
#line 287 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto flow = std::make_shared<idyl::parser::flow_definition>();
        flow->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().value ().as < std::string > ();
        flow->parameters_ = {};
        flow->members_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        flow->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.line;
        flow->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::flow_definition> > () = flow;
    }
#line 4259 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 23: // flow_definition: FLOW IDENTIFIER LPAREN parameter_list RPAREN ASSIGN LBRACE flow_members RBRACE
#line 297 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto flow = std::make_shared<idyl::parser::flow_definition>();
        flow->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-7)].getState().value ().as < std::string > ();
        flow->parameters_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().value ().as < std::vector<idyl::parser::param_ptr> > ();
        flow->members_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        flow->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-8)].getState().yyloc).begin.line;
        flow->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-8)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::flow_definition> > () = flow;
    }
#line 4273 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 24: // flow_members: flow_members IDENTIFIER COLON flow_literal
#line 310 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().value ().as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::string > ();
        member->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        member->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        member->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        (*yyvalp).as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ().push_back(member);
    }
#line 4287 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 25: // flow_members: flow_members IDENTIFIER ON IDENTIFIER COLON flow_literal
#line 320 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().value ().as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().value ().as < std::string > ();
        member->gate_name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::string > ();
        member->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        member->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.line;
        member->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.column;
        (*yyvalp).as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ().push_back(member);
    }
#line 4302 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 26: // flow_members: IDENTIFIER COLON flow_literal
#line 331 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > () = {};
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::string > ();
        member->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        member->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        member->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        (*yyvalp).as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ().push_back(member);
    }
#line 4316 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 27: // flow_members: IDENTIFIER ON IDENTIFIER COLON flow_literal
#line 341 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > () = {};
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().value ().as < std::string > ();
        member->gate_name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::string > ();
        member->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        member->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.line;
        member->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.column;
        (*yyvalp).as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ().push_back(member);
    }
#line 4331 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 28: // flow_literal: LBRACKET flow_elements RBRACKET
#line 355 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto flow_lit = std::make_shared<idyl::parser::flow_literal>();
        flow_lit->elements_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<idyl::parser::expr_ptr> > ();
        flow_lit->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        flow_lit->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::flow_literal_expr>();
        expr->flow_ = flow_lit;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 4347 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 29: // flow_literal: generator_expression
#line 366 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                           { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 4353 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 30: // flow_elements: flow_elements COMMA expression
#line 371 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<idyl::parser::expr_ptr> > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::vector<idyl::parser::expr_ptr> > ();
        (*yyvalp).as < std::vector<idyl::parser::expr_ptr> > ().push_back((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ());
    }
#line 4362 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 31: // flow_elements: flow_elements COMMA LBRACKET NUMBER RBRACKET
#line 376 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<idyl::parser::expr_ptr> > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().value ().as < std::vector<idyl::parser::expr_ptr> > ();
        auto rep = std::make_shared<idyl::parser::repetition_marker>();
        auto num = std::make_shared<idyl::parser::number_literal>();
        num->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::string > ();
        num->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        num->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = num;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        rep->repetition_count_ = expr;
        rep->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        rep->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        (*yyvalp).as < std::vector<idyl::parser::expr_ptr> > ().push_back(expr);
    }
#line 4383 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 32: // flow_elements: flow_elements COMMA RESTART_MARKER
#line 393 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<idyl::parser::expr_ptr> > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::vector<idyl::parser::expr_ptr> > ();
        auto restart = std::make_shared<idyl::parser::repetition_marker>();
        restart->repetition_count_ = nullptr;
        restart->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        restart->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        (*yyvalp).as < std::vector<idyl::parser::expr_ptr> > ().push_back(nullptr); // Placeholder for restart
    }
#line 4396 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 33: // flow_elements: expression
#line 401 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                 { (*yyvalp).as < std::vector<idyl::parser::expr_ptr> > () = {(static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ()}; }
#line 4402 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 34: // generator_expression: LBRACKET IDENTIFIER ASSIGN expression RANGE expression COLON expression RBRACKET
#line 406 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto gen = std::make_shared<idyl::parser::generator_expr>();
        gen->variable_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-7)].getState().value ().as < std::string > ();
        gen->range_start_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().value ().as < idyl::parser::expr_ptr > ();
        gen->range_end_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().value ().as < idyl::parser::expr_ptr > ();
        gen->body_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < idyl::parser::expr_ptr > ();
        gen->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-8)].getState().yyloc).begin.line;
        gen->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-8)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::generator_expr_node>();
        expr->generator_ = gen;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-8)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-8)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 4421 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 35: // process_block: PROCESS COLON LBRACE process_body_statements RBRACE
#line 424 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto proc = std::make_shared<idyl::parser::process_block>();
        proc->name_ = "main";
        auto body = std::make_shared<idyl::parser::process_block_body>();
        body->statements_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<idyl::parser::stmt_ptr> > ();
        proc->body_ = body;
        proc->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.line;
        proc->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::process_block> > () = proc;
    }
#line 4436 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 36: // process_block: PROCESS IDENTIFIER COMMA DUR ASSIGN expression COLON LBRACE process_body_statements RBRACE
#line 435 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto proc = std::make_shared<idyl::parser::process_block>();
        proc->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-8)].getState().value ().as < std::string > ();
        proc->duration_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().value ().as < idyl::parser::expr_ptr > ();
        auto body = std::make_shared<idyl::parser::process_block_body>();
        body->statements_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<idyl::parser::stmt_ptr> > ();
        proc->body_ = body;
        proc->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-9)].getState().yyloc).begin.line;
        proc->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-9)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::process_block> > () = proc;
    }
#line 4452 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 37: // process_block: PROCESS IDENTIFIER COLON LBRACE process_body_statements RBRACE
#line 447 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto proc = std::make_shared<idyl::parser::process_block>();
        proc->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().value ().as < std::string > ();
        auto body = std::make_shared<idyl::parser::process_block_body>();
        body->statements_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<idyl::parser::stmt_ptr> > ();
        proc->body_ = body;
        proc->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.line;
        proc->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::process_block> > () = proc;
    }
#line 4467 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 38: // process_body_statements: process_body_statements process_body_statement
#line 461 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<idyl::parser::stmt_ptr> > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<idyl::parser::stmt_ptr> > ();
        if ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::stmt_ptr > ()) (*yyvalp).as < std::vector<idyl::parser::stmt_ptr> > ().push_back((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::stmt_ptr > ());
    }
#line 4476 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 39: // process_body_statements: process_body_statement
#line 466 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<idyl::parser::stmt_ptr> > () = {};
        if ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::stmt_ptr > ()) (*yyvalp).as < std::vector<idyl::parser::stmt_ptr> > ().push_back((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::stmt_ptr > ());
    }
#line 4485 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 40: // process_body_statement: postfix_expression ASSIGN expression
#line 474 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        // Interpret postfix_expression ASSIGN expression as a local function definition.
        // postfix_expression is either:
        //   - identifier_expr → 0-param binding: "name = expr" (same as assignment)
        //   - function_call_expr → local function: "name(params) = expr"
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->body_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        func->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        func->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        auto& lhs = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        if (lhs && lhs->type_ == idyl::parser::node_t::function_call_expr) {
            // Extract function name from the call's function_ expression
            auto& call_expr = static_cast<idyl::parser::function_call_expr&>(*lhs);
            if (call_expr.call_ && call_expr.call_->function_) {
                auto& fn_expr = *call_expr.call_->function_;
                if (fn_expr.type_ == idyl::parser::node_t::identifier_expr) {
                    auto& id_expr = static_cast<idyl::parser::identifier_expr&>(fn_expr);
                    if (id_expr.identifier_) func->name_ = id_expr.identifier_->name_;
                }
            }
            // Convert arguments to parameters
            if (call_expr.call_) {
                for (const auto& arg : call_expr.call_->arguments_) {
                    if (!arg) continue;
                    auto param = std::make_shared<idyl::parser::parameter>();
                    if (!arg->name_.empty()) {
                        // Named arg: "dt=10ms" → parameter with default
                        param->name_ = arg->name_;
                        param->default_value_ = arg->value_;
                        param->has_default_time_ = (arg->name_ == "dt");
                    } else if (arg->value_ && arg->value_->type_ == idyl::parser::node_t::identifier_expr) {
                        auto& id = static_cast<idyl::parser::identifier_expr&>(*arg->value_);
                        if (id.identifier_) param->name_ = id.identifier_->name_;
                    } else if (arg->value_ && arg->value_->type_ == idyl::parser::node_t::unary_op_expr) {
                        // Could be trigger param: !name
                        param->name_ = "__trigger__";
                        param->is_trigger_parameter_ = true;
                    }
                    func->parameters_.push_back(param);
                }
            }
        } else if (lhs && lhs->type_ == idyl::parser::node_t::identifier_expr) {
            // Simple binding: "name = expr"
            auto& id_expr = static_cast<idyl::parser::identifier_expr&>(*lhs);
            if (id_expr.identifier_) func->name_ = id_expr.identifier_->name_;
        }
        (*yyvalp).as < idyl::parser::stmt_ptr > () = func;
    }
#line 4538 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 41: // process_body_statement: IDENTIFIER ASSIGN expression
#line 523 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::string > ();
        assign->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = false;
        assign->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        assign->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::stmt_ptr > () = assign;
    }
#line 4552 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 42: // process_body_statement: EMIT IDENTIFIER ASSIGN expression
#line 533 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::string > ();
        assign->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = true;
        assign->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.line;
        assign->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::stmt_ptr > () = assign;
    }
#line 4566 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 43: // process_body_statement: stop_statement
#line 543 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < idyl::parser::stmt_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::shared_ptr<idyl::parser::stop_statement> > ();
    }
#line 4574 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 44: // process_body_statement: start_statement
#line 547 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < idyl::parser::stmt_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::shared_ptr<idyl::parser::start_statement> > ();
    }
#line 4582 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 45: // process_body_statement: catch_block
#line 551 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < idyl::parser::stmt_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::shared_ptr<idyl::parser::catch_block> > ();
    }
#line 4590 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 46: // process_body_statement: at_block
#line 555 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < idyl::parser::stmt_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::shared_ptr<idyl::parser::at_block> > ();
    }
#line 4598 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 47: // process_body_statement: on_block
#line 559 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < idyl::parser::stmt_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::shared_ptr<idyl::parser::on_block> > ();
    }
#line 4606 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 48: // process_body_statement: expression
#line 563 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto es = std::make_shared<idyl::parser::expression_stmt>();
        es->expression_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        es->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        es->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::stmt_ptr > () = es;
    }
#line 4618 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 49: // at_block: AT_OP LPAREN expression RPAREN COLON LBRACE process_body_statements RBRACE
#line 574 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto at_stmt = std::make_shared<idyl::parser::at_block>();
        at_stmt->time_expr_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().value ().as < idyl::parser::expr_ptr > ();
        at_stmt->handler_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<idyl::parser::stmt_ptr> > ();
        at_stmt->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-7)].getState().yyloc).begin.line;
        at_stmt->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-7)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::at_block> > () = at_stmt;
    }
#line 4631 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 50: // at_block: AT_OP LPAREN expression RPAREN COLON process_body_statement
#line 583 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto at_stmt = std::make_shared<idyl::parser::at_block>();
        at_stmt->time_expr_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().value ().as < idyl::parser::expr_ptr > ();
        at_stmt->handler_ = {(static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::stmt_ptr > ()};
        at_stmt->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.line;
        at_stmt->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::at_block> > () = at_stmt;
    }
#line 4644 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 51: // on_block: ON expression COLON LBRACE process_body_statements RBRACE
#line 595 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto on_stmt = std::make_shared<idyl::parser::on_block>();
        on_stmt->trigger_expr_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().value ().as < idyl::parser::expr_ptr > ();
        on_stmt->handler_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<idyl::parser::stmt_ptr> > ();
        on_stmt->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.line;
        on_stmt->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::on_block> > () = on_stmt;
    }
#line 4657 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 52: // on_block: ON expression COLON process_body_statement
#line 604 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto on_stmt = std::make_shared<idyl::parser::on_block>();
        on_stmt->trigger_expr_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        on_stmt->handler_ = {(static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::stmt_ptr > ()};
        on_stmt->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.line;
        on_stmt->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::on_block> > () = on_stmt;
    }
#line 4670 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 53: // stop_statement: STOP IDENTIFIER
#line 616 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto stop_stmt = std::make_shared<idyl::parser::stop_statement>();
        stop_stmt->target_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::string > ();
        stop_stmt->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        stop_stmt->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::stop_statement> > () = stop_stmt;
    }
#line 4682 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 54: // start_statement: START IDENTIFIER
#line 627 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto start_stmt = std::make_shared<idyl::parser::start_statement>();
        start_stmt->target_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::string > ();
        start_stmt->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        start_stmt->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::start_statement> > () = start_stmt;
    }
#line 4694 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 55: // catch_block: CATCH postfix_expression NAMESPACE_DOT IDENTIFIER COLON LBRACE process_body_statements RBRACE
#line 638 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto catch_b = std::make_shared<idyl::parser::catch_block>();
        catch_b->instance_expr_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-6)].getState().value ().as < idyl::parser::expr_ptr > ();
        catch_b->signal_name_   = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().value ().as < std::string > ();
        catch_b->handler_       = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<idyl::parser::stmt_ptr> > ();
        catch_b->line_          = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-7)].getState().yyloc).begin.line;
        catch_b->column_        = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-7)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::catch_block> > () = catch_b;
    }
#line 4708 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 56: // catch_block: CATCH postfix_expression NAMESPACE_DOT END COLON LBRACE process_body_statements RBRACE
#line 648 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto catch_b = std::make_shared<idyl::parser::catch_block>();
        catch_b->instance_expr_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-6)].getState().value ().as < idyl::parser::expr_ptr > ();
        catch_b->signal_name_   = "end";
        catch_b->handler_       = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<idyl::parser::stmt_ptr> > ();
        catch_b->line_          = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-7)].getState().yyloc).begin.line;
        catch_b->column_        = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-7)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::catch_block> > () = catch_b;
    }
#line 4722 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 57: // lambda_block: LAMBDA_BLOCK LBRACE init_block lambda_statements RBRACE
#line 661 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->init_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::shared_ptr<idyl::parser::init_block> > ();
        lambda->update_statements_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<idyl::parser::stmt_ptr> > ();
        lambda->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.line;
        lambda->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::lambda_block> > () = lambda;
    }
#line 4735 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 58: // lambda_block: LAMBDA_BLOCK LBRACE init_block RBRACE
#line 670 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->init_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::shared_ptr<idyl::parser::init_block> > ();
        lambda->update_statements_ = {};
        lambda->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.line;
        lambda->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::lambda_block> > () = lambda;
    }
#line 4748 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 59: // lambda_block: LAMBDA_BLOCK LBRACE lambda_statements RBRACE
#line 679 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->update_statements_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<idyl::parser::stmt_ptr> > ();
        lambda->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.line;
        lambda->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::lambda_block> > () = lambda;
    }
#line 4760 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 60: // init_block: INIT COLON LBRACE lambda_statements RBRACE
#line 690 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto init = std::make_shared<idyl::parser::init_block>();
        init->statements_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<idyl::parser::stmt_ptr> > ();
        init->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.line;
        init->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::init_block> > () = init;
    }
#line 4772 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 61: // lambda_statements: lambda_statements lambda_statement
#line 701 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<idyl::parser::stmt_ptr> > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<idyl::parser::stmt_ptr> > ();
        if ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::stmt_ptr > ()) (*yyvalp).as < std::vector<idyl::parser::stmt_ptr> > ().push_back((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::stmt_ptr > ());
    }
#line 4781 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 62: // lambda_statements: lambda_statement
#line 706 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<idyl::parser::stmt_ptr> > () = {};
        if ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::stmt_ptr > ()) (*yyvalp).as < std::vector<idyl::parser::stmt_ptr> > ().push_back((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::stmt_ptr > ());
    }
#line 4790 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 63: // lambda_statement: postfix_expression ASSIGN expression
#line 714 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        // Same pattern as in process_body_statement: interpret as local function definition
        // when lhs is a function call, or as an assignment when lhs is an identifier.
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->body_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        func->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        func->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        auto& lhs = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        if (lhs && lhs->type_ == idyl::parser::node_t::function_call_expr) {
            auto& call_expr = static_cast<idyl::parser::function_call_expr&>(*lhs);
            if (call_expr.call_ && call_expr.call_->function_) {
                auto& fn_expr = *call_expr.call_->function_;
                if (fn_expr.type_ == idyl::parser::node_t::identifier_expr) {
                    auto& id_expr = static_cast<idyl::parser::identifier_expr&>(fn_expr);
                    if (id_expr.identifier_) func->name_ = id_expr.identifier_->name_;
                }
            }
            if (call_expr.call_) {
                for (const auto& arg : call_expr.call_->arguments_) {
                    if (!arg) continue;
                    auto param = std::make_shared<idyl::parser::parameter>();
                    if (!arg->name_.empty()) {
                        param->name_ = arg->name_;
                        param->default_value_ = arg->value_;
                        param->has_default_time_ = (arg->name_ == "dt");
                    } else if (arg->value_ && arg->value_->type_ == idyl::parser::node_t::identifier_expr) {
                        auto& id = static_cast<idyl::parser::identifier_expr&>(*arg->value_);
                        if (id.identifier_) param->name_ = id.identifier_->name_;
                    }
                    func->parameters_.push_back(param);
                }
            }
        } else if (lhs && lhs->type_ == idyl::parser::node_t::identifier_expr) {
            auto& id_expr = static_cast<idyl::parser::identifier_expr&>(*lhs);
            if (id_expr.identifier_) func->name_ = id_expr.identifier_->name_;
        }
        (*yyvalp).as < idyl::parser::stmt_ptr > () = func;
    }
#line 4833 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 64: // lambda_statement: IDENTIFIER ASSIGN expression
#line 753 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::string > ();
        assign->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = false;
        assign->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        assign->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::stmt_ptr > () = assign;
    }
#line 4847 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 65: // lambda_statement: EMIT IDENTIFIER ASSIGN expression
#line 763 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::string > ();
        assign->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = true;
        assign->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.line;
        assign->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::stmt_ptr > () = assign;
    }
#line 4861 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 66: // lambda_statement: DT ASSIGN expression
#line 773 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = "dt";
        assign->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = false;
        assign->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        assign->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::stmt_ptr > () = assign;
    }
#line 4875 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 67: // lambda_statement: expression
#line 783 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto es = std::make_shared<idyl::parser::expression_stmt>();
        es->expression_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        es->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        es->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::stmt_ptr > () = es;
    }
#line 4887 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 68: // parameter_list: parameter_list COMMA parameter
#line 794 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<idyl::parser::param_ptr> > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::vector<idyl::parser::param_ptr> > ();
        if ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::param_ptr > ()) (*yyvalp).as < std::vector<idyl::parser::param_ptr> > ().push_back((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::param_ptr > ());
    }
#line 4896 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 69: // parameter_list: parameter
#line 798 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                { (*yyvalp).as < std::vector<idyl::parser::param_ptr> > () = {(static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::param_ptr > ()}; }
#line 4902 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 70: // parameter: IDENTIFIER
#line 803 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::string > ();
        param->is_trigger_parameter_ = false;
        param->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        param->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::param_ptr > () = param;
    }
#line 4915 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 71: // parameter: IDENTIFIER TRIGGER
#line 812 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::string > ();
        param->is_trigger_parameter_ = true;
        param->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        param->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::param_ptr > () = param;
    }
#line 4928 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 72: // parameter: IDENTIFIER ASSIGN expression
#line 821 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::string > ();
        param->default_value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        param->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        param->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::param_ptr > () = param;
    }
#line 4941 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 73: // parameter: DT ASSIGN expression
#line 830 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = "dt";
        param->default_value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        param->has_default_time_ = true;
        param->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        param->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::param_ptr > () = param;
    }
#line 4955 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 74: // expression: assignment_expression
#line 842 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 4961 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 75: // assignment_expression: ternary_expression
#line 846 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 4967 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 76: // ternary_expression: logical_or_expression
#line 850 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 4973 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 77: // ternary_expression: logical_or_expression QUESTION ternary_options
#line 852 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto ternary = std::make_shared<idyl::parser::ternary_op>();
        ternary->options_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::vector<idyl::parser::expr_ptr> > ();
        // Single-option form: cond ? expr  →  cond ? _ ; expr
        // options_[0] = false/rest branch, options_[1] = true branch
        if (ternary->options_.size() == 1) {
            auto rest = std::make_shared<idyl::parser::rest_literal>();
            rest->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
            rest->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
            auto rest_expr = std::make_shared<idyl::parser::literal_expr>();
            rest_expr->literal_ = rest;
            rest_expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
            rest_expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
            ternary->options_.insert(ternary->options_.begin(), rest_expr);
        }
        ternary->condition_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        ternary->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        ternary->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::ternary_op_expr>();
        expr->op_ = ternary;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5002 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 78: // ternary_options: ternary_options SEMICOLON logical_or_expression
#line 880 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<idyl::parser::expr_ptr> > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::vector<idyl::parser::expr_ptr> > ();
        (*yyvalp).as < std::vector<idyl::parser::expr_ptr> > ().push_back((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ());
    }
#line 5011 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 79: // ternary_options: logical_or_expression
#line 885 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<idyl::parser::expr_ptr> > () = {(static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ()};
    }
#line 5019 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 80: // logical_or_expression: logical_and_expression
#line 891 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 5025 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 81: // logical_or_expression: logical_or_expression OR logical_and_expression
#line 893 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "|";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5043 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 82: // logical_and_expression: bitwise_or_expression
#line 909 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 5049 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 83: // logical_and_expression: logical_and_expression AND bitwise_or_expression
#line 911 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "&";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5067 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 84: // bitwise_or_expression: bitwise_xor_expression
#line 927 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 5073 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 85: // bitwise_or_expression: bitwise_or_expression OR bitwise_xor_expression
#line 929 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "|";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5091 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 86: // bitwise_xor_expression: bitwise_and_expression
#line 945 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 5097 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 87: // bitwise_xor_expression: bitwise_xor_expression XOR bitwise_and_expression
#line 947 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "^";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5115 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 88: // bitwise_and_expression: equality_expression
#line 963 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                          { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 5121 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 89: // bitwise_and_expression: bitwise_and_expression AND equality_expression
#line 965 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "&";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5139 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 90: // equality_expression: relational_expression
#line 981 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 5145 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 91: // equality_expression: equality_expression EQ relational_expression
#line 983 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "==";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5163 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 92: // equality_expression: equality_expression NEQ relational_expression
#line 997 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "!=";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5181 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 93: // relational_expression: shift_expression
#line 1013 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                       { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 5187 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 94: // relational_expression: relational_expression LT shift_expression
#line 1015 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "<";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5205 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 95: // relational_expression: relational_expression GT shift_expression
#line 1029 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = ">";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5223 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 96: // relational_expression: relational_expression LE shift_expression
#line 1043 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "<=";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5241 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 97: // relational_expression: relational_expression GE shift_expression
#line 1057 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = ">=";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5259 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 98: // shift_expression: additive_expression
#line 1073 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                          { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 5265 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 99: // shift_expression: shift_expression LSHIFT additive_expression
#line 1075 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "<<";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5283 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 100: // shift_expression: shift_expression RSHIFT additive_expression
#line 1089 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = ">>";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5301 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 101: // additive_expression: multiplicative_expression
#line 1105 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 5307 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 102: // additive_expression: additive_expression PLUS multiplicative_expression
#line 1107 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "+";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5325 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 103: // additive_expression: additive_expression MINUS multiplicative_expression
#line 1121 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "-";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5343 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 104: // multiplicative_expression: unary_expression
#line 1137 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                       { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 5349 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 105: // multiplicative_expression: multiplicative_expression MUL unary_expression
#line 1139 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "*";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5367 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 106: // multiplicative_expression: multiplicative_expression DIV unary_expression
#line 1153 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "/";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5385 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 107: // multiplicative_expression: multiplicative_expression MOD unary_expression
#line 1167 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "%";
        binop->left_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->right_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        binop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        binop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5403 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 108: // unary_expression: postfix_expression
#line 1183 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 5409 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 109: // unary_expression: NOT unary_expression
#line 1185 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto unop = std::make_shared<idyl::parser::unary_op>();
        unop->op_ = "~";
        unop->operand_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        unop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        unop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::unary_op_expr>();
        expr->op_ = unop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5426 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 110: // unary_expression: MINUS unary_expression
#line 1198 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto unop = std::make_shared<idyl::parser::unary_op>();
        unop->op_ = "-";
        unop->operand_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        unop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        unop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::unary_op_expr>();
        expr->op_ = unop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5443 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 111: // unary_expression: MEMORY_OP LPAREN expression RPAREN
#line 1211 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto memop = std::make_shared<idyl::parser::memory_op>();
        memop->expr_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < idyl::parser::expr_ptr > ();
        memop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.line;
        memop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::memory_op_expr>();
        expr->op_ = memop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5459 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 112: // unary_expression: MEMORY_OP LPAREN expression COMMA expression RPAREN
#line 1223 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto memop = std::make_shared<idyl::parser::memory_op>();
        memop->expr_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().value ().as < idyl::parser::expr_ptr > ();
        memop->delay_count_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < idyl::parser::expr_ptr > ();
        memop->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.line;
        memop->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::memory_op_expr>();
        expr->op_ = memop;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5476 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 113: // postfix_expression: primary_expression
#line 1238 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 5482 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 114: // postfix_expression: postfix_expression LPAREN argument_list RPAREN
#line 1240 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto call = std::make_shared<idyl::parser::function_call>();
        call->function_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().value ().as < idyl::parser::expr_ptr > ();
        call->arguments_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        call->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        call->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::function_call_expr>();
        expr->call_ = call;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5499 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 115: // postfix_expression: postfix_expression LPAREN RPAREN
#line 1253 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto call = std::make_shared<idyl::parser::function_call>();
        call->function_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        call->arguments_ = {};
        call->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        call->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::function_call_expr>();
        expr->call_ = call;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5516 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 116: // postfix_expression: postfix_expression DOT IDENTIFIER
#line 1266 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto access = std::make_shared<idyl::parser::flow_access>();
        access->flow_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        access->member_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::string > ();
        access->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        access->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::flow_access_expr>();
        expr->access_ = access;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5533 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 117: // postfix_expression: postfix_expression NAMESPACE_DOT IDENTIFIER
#line 1279 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto access = std::make_shared<idyl::parser::module_access>();
        access->module_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < idyl::parser::expr_ptr > ();
        access->function_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::string > ();
        access->arguments_ = {};
        access->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        access->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::module_access_expr>();
        expr->access_ = access;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5551 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 118: // postfix_expression: postfix_expression NAMESPACE_DOT IDENTIFIER LPAREN argument_list RPAREN
#line 1293 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto access = std::make_shared<idyl::parser::module_access>();
        access->module_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-5)].getState().value ().as < idyl::parser::expr_ptr > ();
        access->function_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().value ().as < std::string > ();
        access->arguments_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        access->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.line;
        access->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::module_access_expr>();
        expr->access_ = access;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5569 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 119: // postfix_expression: postfix_expression NAMESPACE_DOT IDENTIFIER LPAREN RPAREN
#line 1307 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto access = std::make_shared<idyl::parser::module_access>();
        access->module_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-4)].getState().value ().as < idyl::parser::expr_ptr > ();
        access->function_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::string > ();
        access->arguments_ = {};
        access->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.line;
        access->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::module_access_expr>();
        expr->access_ = access;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5587 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 120: // postfix_expression: postfix_expression LBRACKET expression RBRACKET
#line 1321 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto access = std::make_shared<idyl::parser::flow_access>();
        access->flow_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-3)].getState().value ().as < idyl::parser::expr_ptr > ();
        access->index_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < idyl::parser::expr_ptr > ();
        access->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        access->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::flow_access_expr>();
        expr->access_ = access;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5604 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 121: // primary_expression: IDENTIFIER
#line 1337 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto id = std::make_shared<idyl::parser::identifier>();
        id->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::string > ();
        id->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        id->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::identifier_expr>();
        expr->identifier_ = id;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5620 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 122: // primary_expression: NUMBER
#line 1349 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto num = std::make_shared<idyl::parser::number_literal>();
        num->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::string > ();
        num->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        num->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = num;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5636 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 123: // primary_expression: TIME_LITERAL
#line 1361 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto time = std::make_shared<idyl::parser::time_literal>();
        // Parse time literal
        std::string str = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::string > ();
        if (str.find("ms") != std::string::npos) {
            time->value_ = str.substr(0, str.length() - 2);
            time->unit_ = "ms";
        } else if (str.find("hz") != std::string::npos) {
            time->value_ = str.substr(0, str.length() - 2);
            time->unit_ = "hz";
        } else if (str.find("bpm") != std::string::npos) {
            time->value_ = str.substr(0, str.length() - 3);
            time->unit_ = "bpm";
        } else if (str.find("b") != std::string::npos) {
            time->value_ = str.substr(0, str.length() - 1);
            time->unit_ = "b";
        } else if (str.find("s") != std::string::npos) {
            time->value_ = str.substr(0, str.length() - 1);
            time->unit_ = "s";
        }
        time->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        time->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = time;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5669 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 124: // primary_expression: TRIGGER
#line 1390 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto trigger = std::make_shared<idyl::parser::trigger_literal>();
        trigger->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        trigger->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = trigger;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5684 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 125: // primary_expression: REST
#line 1401 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto rest = std::make_shared<idyl::parser::rest_literal>();
        rest->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        rest->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = rest;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5699 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 126: // primary_expression: DT
#line 1412 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto id = std::make_shared<idyl::parser::identifier>();
        id->name_ = "dt";
        id->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        id->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::identifier_expr>();
        expr->identifier_ = id;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5715 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 127: // primary_expression: DUR
#line 1424 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto id = std::make_shared<idyl::parser::identifier>();
        id->name_ = "dur";
        id->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        id->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::identifier_expr>();
        expr->identifier_ = id;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5731 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 128: // primary_expression: AGE
#line 1436 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto id = std::make_shared<idyl::parser::identifier>();
        id->name_ = "age";
        id->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        id->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::identifier_expr>();
        expr->identifier_ = id;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5747 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 129: // primary_expression: STRING_LITERAL
#line 1448 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto str = std::make_shared<idyl::parser::string_literal>();
        str->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::string > ();
        str->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        str->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = str;
        expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = expr;
    }
#line 5763 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 130: // primary_expression: LPAREN expression RPAREN
#line 1460 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto paren = std::make_shared<idyl::parser::parenthesized_expr>();
        paren->expr_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-1)].getState().value ().as < idyl::parser::expr_ptr > ();
        paren->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        paren->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = paren;
    }
#line 5775 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 131: // primary_expression: flow_literal
#line 1467 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                   { (*yyvalp).as < idyl::parser::expr_ptr > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > (); }
#line 5781 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 132: // primary_expression: STOP
#line 1469 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto stop_expr = std::make_shared<idyl::parser::self_stop_expr>();
        stop_expr->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        stop_expr->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        (*yyvalp).as < idyl::parser::expr_ptr > () = stop_expr;
    }
#line 5792 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 133: // argument_list: argument_list COMMA argument
#line 1479 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        (*yyvalp).as < std::vector<std::shared_ptr<idyl::parser::argument>> > () = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        if ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::shared_ptr<idyl::parser::argument> > ()) (*yyvalp).as < std::vector<std::shared_ptr<idyl::parser::argument>> > ().push_back((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::shared_ptr<idyl::parser::argument> > ());
    }
#line 5801 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 134: // argument_list: argument
#line 1483 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
               { (*yyvalp).as < std::vector<std::shared_ptr<idyl::parser::argument>> > () = {(static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < std::shared_ptr<idyl::parser::argument> > ()}; }
#line 5807 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 135: // argument: expression
#line 1488 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = "";
        arg->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        arg->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.line;
        arg->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::argument> > () = arg;
    }
#line 5820 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 136: // argument: IDENTIFIER ASSIGN expression
#line 1497 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().value ().as < std::string > ();
        arg->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        arg->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        arg->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::argument> > () = arg;
    }
#line 5833 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 137: // argument: DT ASSIGN expression
#line 1506 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = "dt";
        arg->value_ = (static_cast<glr_stack_item const *>(yyvsp))[YYFILL (0)].getState().value ().as < idyl::parser::expr_ptr > ();
        arg->line_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.line;
        arg->column_ = ((static_cast<glr_stack_item const *>(yyvsp))[YYFILL (-2)].getState().yyloc).begin.column;
        (*yyvalp).as < std::shared_ptr<idyl::parser::argument> > () = arg;
    }
#line 5846 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;


#line 5850 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"

          default: break;
        }
    #if YY_EXCEPTIONS
      }
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          *yylocp = yyexc.location;
          yyparser.error (*yylocp, yyexc.what ());
          YYERROR;
        }
    #endif // YY_EXCEPTIONS
    YY_SYMBOL_PRINT ("-> $$ =", yylhsNonterm (yyrule), *yyvalp, *yylocp);

      return yyok;
    # undef yyerrok
    # undef YYABORT
    # undef YYACCEPT
    # undef YYERROR
    # undef YYBACKUP
    # undef yytoken
    # undef yyclearin
    # undef YYRECOVERING
    }

    YYRESULTTAG
    yyresolveStack ()
    {
      if (yystateStack.isSplit ())
        {
          int yyn = 0;
          for (glr_state* yys = firstTopState ();
               yys != yystateStack.yysplitPoint;
               yys = yys->pred ())
            yyn += 1;
          YYCHK (yyresolveStates (*firstTopState (), yyn));
        }
      return yyok;
    }

    /** Pop the symbols consumed by reduction #YYRULE from the top of stack
     *  #YYK of *YYSTACKP, and perform the appropriate semantic action on their
     *  semantic values.  Assumes that all ambiguities in semantic values
     *  have been previously resolved.  Set *YYVALP to the resulting value,
     *  and *YYLOCP to the computed location (if any).  Return value is as
     *  for userAction.  */
    YYRESULTTAG
    yydoAction (state_set_index yyk, rule_num yyrule,
                value_type* yyvalp, location_type* yylocp)
    {
      const int yynrhs = yyrhsLength (yyrule);

      if (!yystateStack.isSplit())
        {
          /* Standard special case: single stack.  */
          YYASSERT (yyk.get() == 0);
          glr_stack_item* yyrhs = yystateStack.firstTop()->asItem();
          const YYRESULTTAG res
            = yyuserAction (yyrule, yynrhs, yyrhs, yyk, yyvalp, yylocp);
          yystateStack.pop_back(static_cast<size_t>(yynrhs));
          yystateStack.setFirstTop(&yystateStack[yystateStack.size() - 1].getState());
          return res;
        }
      else
        {
          glr_stack_item yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
          glr_state* yys = yystateStack.topAt(yyk);
          yyrhsVals[YYMAXRHS + YYMAXLEFT].getState().setPred(yys);
          if (yynrhs == 0)
            /* Set default location.  */
            yyrhsVals[YYMAXRHS + YYMAXLEFT - 1].getState().yyloc = yys->yyloc;
          for (int yyi = 0; yyi < yynrhs; yyi += 1)
            {
              yys = yys->pred();
              YYASSERT (yys != YY_NULLPTR);
            }
          yystateStack.yyupdateSplit (*yys);
          yystateStack.setTopAt(yyk, yys);
          return yyuserAction (yyrule, yynrhs, yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
                               yyk,
                               yyvalp, yylocp);
        }
    }

    /** Pop items off stack #YYK of *YYSTACKP according to grammar rule YYRULE,
     *  and push back on the resulting nonterminal symbol.  Perform the
     *  semantic action associated with YYRULE and store its value with the
     *  newly pushed state, if YYFORCEEVAL or if *YYSTACKP is currently
     *  unambiguous.  Otherwise, store the deferred semantic action with
     *  the new state.  If the new state would have an identical input
     *  position, LR state, and predecessor to an existing state on the stack,
     *  it is identified with that existing state, eliminating stack #YYK from
     *  *YYSTACKP.  In this case, the semantic value is
     *  added to the options for the existing state's semantic value.
     */
    YYRESULTTAG
    yyglrReduce (state_set_index yyk, rule_num yyrule, bool yyforceEval)
    {
      size_t yyposn = topState(yyk)->yyposn;

      if (yyforceEval || !yystateStack.isSplit())
        {
          value_type val;
          location_type loc;

          YYRESULTTAG yyflag = yydoAction (yyk, yyrule, &val, &loc);
          if (yyflag == yyerr && yystateStack.isSplit())
            {
              YYCDEBUG << "Parse on stack " << yyk.get ()
                       << " rejected by rule " << yyrule - 1
                       << " (line " << int (yyrline[yyrule]) << ").\n";
            }
          if (yyflag != yyok)
            return yyflag;
          yyglrShift (yyk,
                      yyLRgotoState (topState(yyk)->yylrState,
                                     yylhsNonterm (yyrule)),
                      yyposn, val, loc);
          // FIXME: User destructors.
          // Value type destructor.
          switch (yylhsNonterm (yyrule))
    {
      case symbol_kind::S_flow_literal: // flow_literal
      case symbol_kind::S_generator_expression: // generator_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_ternary_expression: // ternary_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_bitwise_or_expression: // bitwise_or_expression
      case symbol_kind::S_bitwise_xor_expression: // bitwise_xor_expression
      case symbol_kind::S_bitwise_and_expression: // bitwise_and_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_primary_expression: // primary_expression
        val.template destroy< idyl::parser::expr_ptr > ();
        break;

      case symbol_kind::S_program: // program
        val.template destroy< idyl::parser::node_ptr > ();
        break;

      case symbol_kind::S_parameter: // parameter
        val.template destroy< idyl::parser::param_ptr > ();
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        val.template destroy< idyl::parser::stmt_ptr > ();
        break;

      case symbol_kind::S_argument: // argument
        val.template destroy< std::shared_ptr<idyl::parser::argument> > ();
        break;

      case symbol_kind::S_at_block: // at_block
        val.template destroy< std::shared_ptr<idyl::parser::at_block> > ();
        break;

      case symbol_kind::S_catch_block: // catch_block
        val.template destroy< std::shared_ptr<idyl::parser::catch_block> > ();
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        val.template destroy< std::shared_ptr<idyl::parser::flow_definition> > ();
        break;

      case symbol_kind::S_function_definition: // function_definition
        val.template destroy< std::shared_ptr<idyl::parser::function_definition> > ();
        break;

      case symbol_kind::S_init_block: // init_block
        val.template destroy< std::shared_ptr<idyl::parser::init_block> > ();
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        val.template destroy< std::shared_ptr<idyl::parser::lambda_block> > ();
        break;

      case symbol_kind::S_on_block: // on_block
        val.template destroy< std::shared_ptr<idyl::parser::on_block> > ();
        break;

      case symbol_kind::S_process_block: // process_block
        val.template destroy< std::shared_ptr<idyl::parser::process_block> > ();
        break;

      case symbol_kind::S_start_statement: // start_statement
        val.template destroy< std::shared_ptr<idyl::parser::start_statement> > ();
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        val.template destroy< std::shared_ptr<idyl::parser::stop_statement> > ();
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        val.template destroy< std::string > ();
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        val.template destroy< std::vector<idyl::parser::expr_ptr> > ();
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        val.template destroy< std::vector<idyl::parser::param_ptr> > ();
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        val.template destroy< std::vector<idyl::parser::stmt_ptr> > ();
        break;

      case symbol_kind::S_argument_list: // argument_list
        val.template destroy< std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        break;

      case symbol_kind::S_flow_members: // flow_members
        val.template destroy< std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        break;

      default:
        break;
    }

        }
      else
        {
          glr_state *yys = yystateStack.topAt(yyk);
          glr_state *yys0 = yys;
          for (int yyn = yyrhsLength (yyrule); 0 < yyn; yyn -= 1)
            {
              yys = yys->pred();
              YYASSERT (yys != YY_NULLPTR);
            }
          yystateStack.yyupdateSplit (*yys);
          state_num yynewLRState = yyLRgotoState (yys->yylrState, yylhsNonterm (yyrule));
          YYCDEBUG << "Reduced stack " << yyk.get ()
                   << " by rule " << yyrule - 1 << " (line " << int (yyrline[yyrule])
                   << "); action deferred.  Now in state " << yynewLRState
                   << ".\n";
          for (state_set_index yyi = create_state_set_index(0); yyi.uget() < yystateStack.numTops(); ++yyi)
            if (yyi != yyk && yystateStack.topAt(yyi) != YY_NULLPTR)
              {
                const glr_state* yysplit = yystateStack.yysplitPoint;
                glr_state* yyp = yystateStack.topAt(yyi);
                while (yyp != yys && yyp != yysplit
                       && yyp->yyposn >= yyposn)
                  {
                    if (yyp->yylrState == yynewLRState
                        && yyp->pred() == yys)
                      {
                        yyaddDeferredAction (yyk, yyp, yys0, yyrule);
                        yystateStack.yytops.yymarkStackDeleted (yyk);
                        YYCDEBUG << "Merging stack " << yyk.get ()
                                 << " into stack " << yyi.get () << ".\n";
                        return yyok;
                      }
                    yyp = yyp->pred();
                  }
              }
          yystateStack.setTopAt(yyk, yys);
          yyglrShiftDefer (yyk, yynewLRState, yyposn, yys0, yyrule);
        }
      return yyok;
    }

    /** Shift stack #YYK of *YYSTACKP, to a new state corresponding to LR
     *  state YYLRSTATE, at input position YYPOSN, with the (unresolved)
     *  semantic value of YYRHS under the action for YYRULE.  */
    void
    yyglrShiftDefer (state_set_index yyk, state_num yylrState,
                     size_t yyposn, glr_state* yyrhs, rule_num yyrule)
    {
      glr_state& yynewState = yystateStack.yynewGLRState (
        glr_state (yylrState, yyposn));
      yynewState.setPred (yystateStack.topAt (yyk));
      yystateStack.setTopAt (yyk, &yynewState);

      /* Invokes yyreserveStack.  */
      yyaddDeferredAction (yyk, &yynewState, yyrhs, yyrule);
    }

    /** Shift to a new state on stack #YYK of *YYSTACKP, corresponding to LR
     * state YYLRSTATE, at input position YYPOSN, with (resolved) semantic
     * value YYVAL_ARG and source location YYLOC_ARG.  */
    void
    yyglrShift (state_set_index yyk, state_num yylrState,
                size_t yyposn,
                const value_type& yyval_arg, const location_type& yyloc_arg)
    {
      glr_state& yynewState = yystateStack.yynewGLRState (
        glr_state (yylrState, yyposn, yyval_arg, yyloc_arg));
      yynewState.setPred (yystateStack.topAt(yyk));
      yystateStack.setTopAt (yyk, &yynewState);
      yyreserveGlrStack ();
    }

#if YYDEBUG
    void
    yypstack (state_set_index yyk) const
    {
      yystateStack.yypstates (yystateStack.topAt (yyk));
    }
#endif

    glr_state* topState(state_set_index i) {
      return yystateStack.topAt(i);
    }

    glr_state* firstTopState() {
      return yystateStack.firstTop();
    }

  private:

    void popall_ ()
    {
      /* If the stack is well-formed, pop the stack until it is empty,
         destroying its entries as we go.  But free the stack regardless
         of whether it is well-formed.  */
      for (state_set_index k = create_state_set_index(0); k.uget() < yystateStack.numTops(); k += 1)
        if (yystateStack.topAt(k) != YY_NULLPTR)
          {
            while (yystateStack.topAt(k) != YY_NULLPTR)
              {
                glr_state* state = topState(k);
                yyerror_range[1].getState().yyloc = state->yyloc;
                if (state->pred() != YY_NULLPTR)
                  state->destroy ("Cleanup: popping", yyparser);
                yystateStack.setTopAt(k, state->pred());
                yystateStack.pop_back();
              }
              break;
          }
    }

    /** Resolve the previous YYN states starting at and including state YYS
     *  on *YYSTACKP. If result != yyok, some states may have been left
     *  unresolved possibly with empty semantic option chains.  Regardless
     *  of whether result = yyok, each state has been left with consistent
     *  data so that destroy can be invoked if necessary.  */
    YYRESULTTAG
    yyresolveStates (glr_state& yys, int yyn)
    {
      if (0 < yyn)
        {
          YYASSERT (yys.pred() != YY_NULLPTR);
          YYCHK (yyresolveStates (*yys.pred(), yyn-1));
          if (! yys.yyresolved)
            YYCHK (yyresolveValue (yys));
        }
      return yyok;
    }

    static void
    yyuserMerge (int yyn, value_type& yy0, value_type& yy1)
    {
      YY_USE (yy0);
      YY_USE (yy1);

      switch (yyn)
        {

          default: break;
        }
    }

    /** Resolve the ambiguity represented in state YYS in *YYSTACKP,
     *  perform the indicated actions, and set the semantic value of YYS.
     *  If result != yyok, the chain of semantic options in YYS has been
     *  cleared instead or it has been left unmodified except that
     *  redundant options may have been removed.  Regardless of whether
     *  result = yyok, YYS has been left with consistent data so that
     *  destroy can be invoked if necessary.  */
    YYRESULTTAG
    yyresolveValue (glr_state& yys)
    {
      semantic_option* yybest = yys.firstVal();
      YYASSERT(yybest != YY_NULLPTR);
      bool yymerge = false;
      YYRESULTTAG yyflag;
      location_type *yylocp = &yys.yyloc;

      semantic_option* yypPrev = yybest;
      for (semantic_option* yyp = yybest->next();
           yyp != YY_NULLPTR; )
        {
          if (yybest->isIdenticalTo (*yyp))
            {
              yybest->mergeWith (*yyp);
              yypPrev->setNext(yyp->next());
              yyp = yypPrev->next();
            }
          else
            {
              switch (yypreference (*yybest, *yyp))
                {
                case 0:
                  yyresolveLocations (yys, 1);
                  return yystateStack.yyreportAmbiguity (*yybest, *yyp, yyparser, *yylocp);
                  break;
                case 1:
                  yymerge = true;
                  break;
                case 2:
                  break;
                case 3:
                  yybest = yyp;
                  yymerge = false;
                  break;
                default:
                  /* This cannot happen so it is not worth a YYASSERT (false),
                     but some compilers complain if the default case is
                     omitted.  */
                  break;
                }
              yypPrev = yyp;
              yyp = yyp->next();
            }
        }

      value_type val;
      if (yymerge)
        {
          int yyprec = yydprec[yybest->yyrule];
          yyflag = yyresolveAction (*yybest, &val, yylocp);
          if (yyflag == yyok)
            for (semantic_option* yyp = yybest->next();
                 yyp != YY_NULLPTR;
                 yyp = yyp->next())
              {
                if (yyprec == yydprec[yyp->yyrule])
                  {
                    value_type yyval_other;
                    location_type yydummy;
                    yyflag = yyresolveAction (*yyp, &yyval_other, &yydummy);
                    if (yyflag != yyok)
                      {
                        yyparser.yy_destroy_ ("Cleanup: discarding incompletely merged value for",
                                              yy_accessing_symbol (yys.yylrState),
                                              this->yyla.value, *yylocp);
                        break;
                      }
                    yyuserMerge (yymerger[yyp->yyrule], val, yyval_other);
                    // FIXME: User destructors.
                    // Value type destructor.
                    switch (yy_accessing_symbol (yys.yylrState))
    {
      case symbol_kind::S_flow_literal: // flow_literal
      case symbol_kind::S_generator_expression: // generator_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_ternary_expression: // ternary_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_bitwise_or_expression: // bitwise_or_expression
      case symbol_kind::S_bitwise_xor_expression: // bitwise_xor_expression
      case symbol_kind::S_bitwise_and_expression: // bitwise_and_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_primary_expression: // primary_expression
        yyval_other.template destroy< idyl::parser::expr_ptr > ();
        break;

      case symbol_kind::S_program: // program
        yyval_other.template destroy< idyl::parser::node_ptr > ();
        break;

      case symbol_kind::S_parameter: // parameter
        yyval_other.template destroy< idyl::parser::param_ptr > ();
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        yyval_other.template destroy< idyl::parser::stmt_ptr > ();
        break;

      case symbol_kind::S_argument: // argument
        yyval_other.template destroy< std::shared_ptr<idyl::parser::argument> > ();
        break;

      case symbol_kind::S_at_block: // at_block
        yyval_other.template destroy< std::shared_ptr<idyl::parser::at_block> > ();
        break;

      case symbol_kind::S_catch_block: // catch_block
        yyval_other.template destroy< std::shared_ptr<idyl::parser::catch_block> > ();
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        yyval_other.template destroy< std::shared_ptr<idyl::parser::flow_definition> > ();
        break;

      case symbol_kind::S_function_definition: // function_definition
        yyval_other.template destroy< std::shared_ptr<idyl::parser::function_definition> > ();
        break;

      case symbol_kind::S_init_block: // init_block
        yyval_other.template destroy< std::shared_ptr<idyl::parser::init_block> > ();
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        yyval_other.template destroy< std::shared_ptr<idyl::parser::lambda_block> > ();
        break;

      case symbol_kind::S_on_block: // on_block
        yyval_other.template destroy< std::shared_ptr<idyl::parser::on_block> > ();
        break;

      case symbol_kind::S_process_block: // process_block
        yyval_other.template destroy< std::shared_ptr<idyl::parser::process_block> > ();
        break;

      case symbol_kind::S_start_statement: // start_statement
        yyval_other.template destroy< std::shared_ptr<idyl::parser::start_statement> > ();
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        yyval_other.template destroy< std::shared_ptr<idyl::parser::stop_statement> > ();
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        yyval_other.template destroy< std::string > ();
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        yyval_other.template destroy< std::vector<idyl::parser::expr_ptr> > ();
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        yyval_other.template destroy< std::vector<idyl::parser::param_ptr> > ();
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        yyval_other.template destroy< std::vector<idyl::parser::stmt_ptr> > ();
        break;

      case symbol_kind::S_argument_list: // argument_list
        yyval_other.template destroy< std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        break;

      case symbol_kind::S_flow_members: // flow_members
        yyval_other.template destroy< std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        break;

      default:
        break;
    }

                  }
              }
        }
      else
        yyflag = yyresolveAction (*yybest, &val, yylocp);

      if (yyflag == yyok)
        {
          yys.yyresolved = true;
          YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
          new (&yys.value ()) value_type ();
          switch (yy_accessing_symbol (yys.yylrState))
    {
      case symbol_kind::S_flow_literal: // flow_literal
      case symbol_kind::S_generator_expression: // generator_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_ternary_expression: // ternary_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_bitwise_or_expression: // bitwise_or_expression
      case symbol_kind::S_bitwise_xor_expression: // bitwise_xor_expression
      case symbol_kind::S_bitwise_and_expression: // bitwise_and_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_primary_expression: // primary_expression
        yys.value ().copy< idyl::parser::expr_ptr > (val);
        break;

      case symbol_kind::S_program: // program
        yys.value ().copy< idyl::parser::node_ptr > (val);
        break;

      case symbol_kind::S_parameter: // parameter
        yys.value ().copy< idyl::parser::param_ptr > (val);
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        yys.value ().copy< idyl::parser::stmt_ptr > (val);
        break;

      case symbol_kind::S_argument: // argument
        yys.value ().copy< std::shared_ptr<idyl::parser::argument> > (val);
        break;

      case symbol_kind::S_at_block: // at_block
        yys.value ().copy< std::shared_ptr<idyl::parser::at_block> > (val);
        break;

      case symbol_kind::S_catch_block: // catch_block
        yys.value ().copy< std::shared_ptr<idyl::parser::catch_block> > (val);
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        yys.value ().copy< std::shared_ptr<idyl::parser::flow_definition> > (val);
        break;

      case symbol_kind::S_function_definition: // function_definition
        yys.value ().copy< std::shared_ptr<idyl::parser::function_definition> > (val);
        break;

      case symbol_kind::S_init_block: // init_block
        yys.value ().copy< std::shared_ptr<idyl::parser::init_block> > (val);
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        yys.value ().copy< std::shared_ptr<idyl::parser::lambda_block> > (val);
        break;

      case symbol_kind::S_on_block: // on_block
        yys.value ().copy< std::shared_ptr<idyl::parser::on_block> > (val);
        break;

      case symbol_kind::S_process_block: // process_block
        yys.value ().copy< std::shared_ptr<idyl::parser::process_block> > (val);
        break;

      case symbol_kind::S_start_statement: // start_statement
        yys.value ().copy< std::shared_ptr<idyl::parser::start_statement> > (val);
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        yys.value ().copy< std::shared_ptr<idyl::parser::stop_statement> > (val);
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        yys.value ().copy< std::string > (val);
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        yys.value ().copy< std::vector<idyl::parser::expr_ptr> > (val);
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        yys.value ().copy< std::vector<idyl::parser::param_ptr> > (val);
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        yys.value ().copy< std::vector<idyl::parser::stmt_ptr> > (val);
        break;

      case symbol_kind::S_argument_list: // argument_list
        yys.value ().copy< std::vector<std::shared_ptr<idyl::parser::argument>> > (val);
        break;

      case symbol_kind::S_flow_members: // flow_members
        yys.value ().copy< std::vector<std::shared_ptr<idyl::parser::flow_member>> > (val);
        break;

      default:
        break;
    }


          YY_IGNORE_MAYBE_UNINITIALIZED_END
        }
      else
        yys.setFirstVal(YY_NULLPTR);

      // FIXME: User destructors.
      // Value type destructor.
      switch (yy_accessing_symbol (yys.yylrState))
    {
      case symbol_kind::S_flow_literal: // flow_literal
      case symbol_kind::S_generator_expression: // generator_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_ternary_expression: // ternary_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_bitwise_or_expression: // bitwise_or_expression
      case symbol_kind::S_bitwise_xor_expression: // bitwise_xor_expression
      case symbol_kind::S_bitwise_and_expression: // bitwise_and_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_primary_expression: // primary_expression
        val.template destroy< idyl::parser::expr_ptr > ();
        break;

      case symbol_kind::S_program: // program
        val.template destroy< idyl::parser::node_ptr > ();
        break;

      case symbol_kind::S_parameter: // parameter
        val.template destroy< idyl::parser::param_ptr > ();
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        val.template destroy< idyl::parser::stmt_ptr > ();
        break;

      case symbol_kind::S_argument: // argument
        val.template destroy< std::shared_ptr<idyl::parser::argument> > ();
        break;

      case symbol_kind::S_at_block: // at_block
        val.template destroy< std::shared_ptr<idyl::parser::at_block> > ();
        break;

      case symbol_kind::S_catch_block: // catch_block
        val.template destroy< std::shared_ptr<idyl::parser::catch_block> > ();
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        val.template destroy< std::shared_ptr<idyl::parser::flow_definition> > ();
        break;

      case symbol_kind::S_function_definition: // function_definition
        val.template destroy< std::shared_ptr<idyl::parser::function_definition> > ();
        break;

      case symbol_kind::S_init_block: // init_block
        val.template destroy< std::shared_ptr<idyl::parser::init_block> > ();
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        val.template destroy< std::shared_ptr<idyl::parser::lambda_block> > ();
        break;

      case symbol_kind::S_on_block: // on_block
        val.template destroy< std::shared_ptr<idyl::parser::on_block> > ();
        break;

      case symbol_kind::S_process_block: // process_block
        val.template destroy< std::shared_ptr<idyl::parser::process_block> > ();
        break;

      case symbol_kind::S_start_statement: // start_statement
        val.template destroy< std::shared_ptr<idyl::parser::start_statement> > ();
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        val.template destroy< std::shared_ptr<idyl::parser::stop_statement> > ();
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        val.template destroy< std::string > ();
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        val.template destroy< std::vector<idyl::parser::expr_ptr> > ();
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        val.template destroy< std::vector<idyl::parser::param_ptr> > ();
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        val.template destroy< std::vector<idyl::parser::stmt_ptr> > ();
        break;

      case symbol_kind::S_argument_list: // argument_list
        val.template destroy< std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        break;

      case symbol_kind::S_flow_members: // flow_members
        val.template destroy< std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        break;

      default:
        break;
    }

      return yyflag;
    }

    /** Resolve the states for the RHS of YYOPT on *YYSTACKP, perform its
     *  user action, and return the semantic value and location in *YYVALP
     *  and *YYLOCP.  Regardless of whether result = yyok, all RHS states
     *  have been destroyed (assuming the user action destroys all RHS
     *  semantic values if invoked).  */
    YYRESULTTAG
    yyresolveAction (semantic_option& yyopt, value_type* yyvalp, location_type* yylocp)
    {
      glr_state* yyoptState = yyopt.state();
      YYASSERT(yyoptState != YY_NULLPTR);
      int yynrhs = yyrhsLength (yyopt.yyrule);
      YYRESULTTAG yyflag = yyresolveStates (*yyoptState, yynrhs);
      if (yyflag != yyok)
        {
          for (glr_state *yys = yyoptState; yynrhs > 0; yys = yys->pred(), yynrhs -= 1)
            yys->destroy ("Cleanup: popping", yyparser);
          return yyflag;
        }

      glr_stack_item yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
      yyrhsVals[YYMAXRHS + YYMAXLEFT].getState().setPred(yyopt.state());
      if (yynrhs == 0)
        /* Set default location.  */
        yyrhsVals[YYMAXRHS + YYMAXLEFT - 1].getState().yyloc = yyoptState->yyloc;
      {
        symbol_type yyla_current = std::move (this->yyla);
        this->yyla = std::move (yyopt.yyla);
        yyflag = yyuserAction (yyopt.yyrule, yynrhs,
                               yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
                               create_state_set_index (-1),
                               yyvalp, yylocp);
        this->yyla = std::move (yyla_current);
      }
      return yyflag;
    }

    /** Resolve the locations for each of the YYN1 states in *YYSTACKP,
     *  ending at YYS1.  Has no effect on previously resolved states.
     *  The first semantic option of a state is always chosen.  */
    void
    yyresolveLocations (glr_state &yys1, int yyn1)
    {
      if (0 < yyn1)
        {
          yyresolveLocations (*yys1.pred(), yyn1 - 1);
          if (!yys1.yyresolved)
            {
              glr_stack_item yyrhsloc[1 + YYMAXRHS];
              YYASSERT (yys1.firstVal() != YY_NULLPTR);
              semantic_option& yyoption = *yys1.firstVal();
              const int yynrhs = yyrhsLength (yyoption.yyrule);
              if (0 < yynrhs)
                {
                  yyresolveLocations (*yyoption.state(), yynrhs);
                  const glr_state *yys = yyoption.state();
                  for (int yyn = yynrhs; yyn > 0; yyn -= 1)
                  {
                    yyrhsloc[yyn].getState().yyloc = yys->yyloc;
                    yys = yys->pred();
                  }
                }
              else
                {
                  /* Both yyresolveAction and yyresolveLocations traverse the GSS
                     in reverse rightmost order.  It is only necessary to invoke
                     yyresolveLocations on a subforest for which yyresolveAction
                     would have been invoked next had an ambiguity not been
                     detected.  Thus the location of the previous state (but not
                     necessarily the previous state itself) is guaranteed to be
                     resolved already.  */
                  YY_IGNORE_NULL_DEREFERENCE_BEGIN
                  yyrhsloc[0].getState().yyloc = yyoption.state()->yyloc;
                  YY_IGNORE_NULL_DEREFERENCE_END
                }
              YYLLOC_DEFAULT ((yys1.yyloc), yyrhsloc, yynrhs);
            }
        }
    }

    /** If yytoken is empty, fetch the next token.  */
    void
    yyget_token ()
    {
  YY_USE (driver);
      if (this->yyla.empty ())
        {
          YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
          try
#endif // YY_EXCEPTIONS
            {
              symbol_type yylookahead (yylex (driver));
              yyla.move (yylookahead);
            }
#if YY_EXCEPTIONS
          catch (const parser_type::syntax_error& yyexc)
            {
              YYCDEBUG << "Caught exception: " << yyexc.what () << '\n';
              this->yyla.location = yyexc.location;
              yyparser.error (this->yyla.location, yyexc.what ());
              // Map errors caught in the scanner to the error token, so that error
              // handling is started.
              this->yyla.kind_ = symbol_kind::S_YYerror;
            }
        }
#endif // YY_EXCEPTIONS
      if (this->yyla.kind () == symbol_kind::S_YYEOF)
        YYCDEBUG << "Now at end of input.\n";
      else
        YY_SYMBOL_PRINT ("Next token is", this->yyla.kind (), this->yyla.value, this->yyla.location);
    }


                                /* Bison grammar-table manipulation.  */

    /** The action to take in YYSTATE on seeing YYTOKEN.
     *  Result R means
     *    R < 0:  Reduce on rule -R.
     *    R = 0:  Error.
     *    R > 0:  Shift to state R.
     *  Set *YYCONFLICTS to a pointer into yyconfl to a 0-terminated list
     *  of conflicting reductions.
     */
    static int
    yygetLRActions (state_num yystate, symbol_kind_type yytoken, const short*& yyconflicts)
    {
      int yyindex = yypact[yystate] + yytoken;
      if (yytoken == symbol_kind::S_YYerror)
        {
          // This is the error token.
          yyconflicts = yyconfl;
          return 0;
        }
      else if (yy_is_defaulted_state (yystate)
               || yyindex < 0 || YYLAST < yyindex || yycheck[yyindex] != yytoken)
        {
          yyconflicts = yyconfl;
          return -yydefact[yystate];
        }
      else if (! yytable_value_is_error (yytable[yyindex]))
        {
          yyconflicts = yyconfl + yyconflp[yyindex];
          return yytable[yyindex];
        }
      else
        {
          yyconflicts = yyconfl + yyconflp[yyindex];
          return 0;
        }
    }

    /** Compute post-reduction state.
     * \param yystate   the current state
     * \param yysym     the nonterminal to push on the stack
     */
    static state_num
    yyLRgotoState (state_num yystate, symbol_kind_type yysym)
    {
      const int yyr = yypgoto[yysym - YYNTOKENS] + yystate;
      if (0 <= yyr && yyr <= YYLAST && yycheck[yyr] == yystate)
        return yytable[yyr];
      else
        return yydefgoto[yysym - YYNTOKENS];
    }

    static bool
    yypact_value_is_default (state_num yystate)
    {
      return ((yystate) == YYPACT_NINF);
    }

    static bool
    yytable_value_is_error (int yytable_value YY_ATTRIBUTE_UNUSED)
    {
      return 0;
    }

    static bool
    yy_is_shift_action (int yyaction) YY_NOEXCEPT
    {
      return 0 < yyaction;
    }

    static bool
    yy_is_error_action (int yyaction) YY_NOEXCEPT
    {
      return yyaction == 0;
    }

    /** Whether LR state YYSTATE has only a default reduction
     *  (regardless of token).  */
    static bool
    yy_is_defaulted_state (state_num yystate)
    {
      return yypact_value_is_default (yypact[yystate]);
    }

    /** The default reduction for YYSTATE, assuming it has one.  */
    static rule_num
    yy_default_action (state_num yystate)
    {
      return yydefact[yystate];
    }

                                    /* GLRStacks */

    /** Y0 and Y1 represent two possible actions to take in a given
     *  parsing state; return 0 if no combination is possible,
     *  1 if user-mergeable, 2 if Y0 is preferred, 3 if Y1 is preferred.  */
    static int
    yypreference (const semantic_option& y0, const semantic_option& y1)
    {
      const rule_num r0 = y0.yyrule, r1 = y1.yyrule;
      const int p0 = yydprec[r0], p1 = yydprec[r1];

      if (p0 == p1)
        {
          if (yymerger[r0] == 0 || yymerger[r0] != yymerger[r1])
            return 0;
          else
            return 1;
        }
      else if (p0 == 0 || p1 == 0)
        return 0;
      else if (p0 < p1)
        return 3;
      else if (p1 < p0)
        return 2;
      else
        return 0;
    }


    // User arguments.
    yy::lexer& driver;
  }; // class parser::glr_stack
} // namespace yy


#if YYDEBUG
namespace
{
  void
  yypstack (const glr_stack& yystack, size_t yyk)
  {
    yystack.yypstack (create_state_set_index (static_cast<std::ptrdiff_t> (yyk)));
  }

  void
  yypdumpstack (const glr_stack& yystack)
  {
    yystack.yypdumpstack ();
  }
}
#endif

namespace yy {
#line 6937 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"

  /// Build a parser object.
  parser::parser (yy::lexer& driver_yyarg)
    :
#if YYDEBUG
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    glr_stack yystack(YYINITDEPTH, *this, driver);
    return yystack.parse ();
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "YYEOF", "error", "\"invalid token\"", "IDENTIFIER", "NUMBER",
  "TIME_LITERAL", "STRING_LITERAL", "FLOW", "PROCESS", "IMPORT", "MODULE",
  "INIT", "EMIT", "CATCH", "END", "DT", "DUR", "STOP", "START", "AGE",
  "LAMBDA_BLOCK", "NAMESPACE_DOT", "RESTART_MARKER", "MEMORY_OP", "RANGE",
  "REST", "AT_OP", "PLUS", "MINUS", "MUL", "DIV", "MOD", "EQ", "NEQ", "LT",
  "GT", "LE", "GE", "AND", "OR", "XOR", "NOT", "LSHIFT", "RSHIFT",
  "QUESTION", "ASSIGN", "COLON", "SEMICOLON", "COMMA", "DOT", "TRIGGER",
  "LPAREN", "RPAREN", "LBRACKET", "RBRACKET", "LBRACE", "RBRACE", "ON",
  "$accept", "program", "top_level_statements", "top_level_statement",
  "function_or_flow_definition", "function_definition", "flow_definition",
  "flow_members", "flow_literal", "flow_elements", "generator_expression",
  "process_block", "process_body_statements", "process_body_statement",
  "at_block", "on_block", "stop_statement", "start_statement",
  "catch_block", "lambda_block", "init_block", "lambda_statements",
  "lambda_statement", "parameter_list", "parameter", "expression",
  "assignment_expression", "ternary_expression", "ternary_options",
  "logical_or_expression", "logical_and_expression",
  "bitwise_or_expression", "bitwise_xor_expression",
  "bitwise_and_expression", "equality_expression", "relational_expression",
  "shift_expression", "additive_expression", "multiplicative_expression",
  "unary_expression", "postfix_expression", "primary_expression",
  "argument_list", "argument", YY_NULLPTR
  };
#endif



  // parser::context.
  parser::context::context (glr_stack& yystack, const symbol_type& yyla)
    : yystack_ (yystack)
    , yyla_ (yyla)
  {}

  int
  parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;
    const int yyn = yypact[yystack_.firstTopState()->yylrState];
    if (!yystack_.yypact_value_is_default (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for this
           state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        /* Stay within bounds of both yycheck and yytname.  */
        const int yychecklim = YYLAST - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yystack_.yytable_value_is_error (yytable[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }
    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }




  int
  parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }

  void
  parser::yy_destroy_ (const char* yymsg, symbol_kind_type yykind,
                           value_type& yyval,
                           location_type& yyloc)
  {
    YY_USE (yyval);
    YY_USE (yyloc);
    if (!yymsg)
      yymsg = "Deleting";
    parser& yyparser = *this;
    YY_USE (yyparser);
    YY_SYMBOL_PRINT (yymsg, yykind, yyval, yyloc);

    YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    YY_USE (yykind);
    YY_IGNORE_MAYBE_UNINITIALIZED_END
  }

#if YYDEBUG
  /*--------------------.
  | Print this symbol.  |
  `--------------------*/

  void
  parser::yy_symbol_value_print_ (symbol_kind_type yykind,
                           const value_type& yyval,
                           const location_type& yyloc) const
  {
    YY_USE (yyloc);
    YY_USE (yyval);
    std::ostream& yyo = debug_stream ();
    YY_USE (yyo);
    YY_USE (yykind);
  }

  void
  parser::yy_symbol_print_ (symbol_kind_type yykind,
                           const value_type& yyval,
                           const location_type& yyloc) const
  {
    *yycdebug_ << (yykind < YYNTOKENS ? "token" : "nterm")
               << ' ' << symbol_name (yykind) << " ("
               << yyloc << ": ";
    yy_symbol_value_print_ (yykind, yyval, yyloc);
    *yycdebug_ << ')';
  }

  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    // Actually, it is yydebug which is really used.
    yydebug = l;
  }
#endif // YYDEBUG




} // yy
#line 7238 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
#line 1516 "/home/johann/Documents/git/idyl/src/parser/idyl.y"


void yy::parser::error(const location_type& l, const std::string& m) {
    std::cerr << l << ": " << m << std::endl;
}
