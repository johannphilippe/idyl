/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 11 "/home/johann/Documents/git/idyl/src/parser/idyl.y"

#include <iostream>
#include <string>
#include<cstdio>
#include<cstdlib>
#include "ast.hpp"
using namespace std;
using namespace idyl::parser;

int yylex(void);
void yyerror(const char *s);
node_ptr root;

#line 85 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
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

#include "idyl_parser.tab.hh"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDENT = 3,                      /* IDENT  */
  YYSYMBOL_IDENT_ASSIGN = 4,               /* IDENT_ASSIGN  */
  YYSYMBOL_NUMBER = 5,                     /* NUMBER  */
  YYSYMBOL_TIME = 6,                       /* TIME  */
  YYSYMBOL_TRIGGER = 7,                    /* TRIGGER  */
  YYSYMBOL_MODULE = 8,                     /* MODULE  */
  YYSYMBOL_ASSIGN = 9,                     /* ASSIGN  */
  YYSYMBOL_COLON = 10,                     /* COLON  */
  YYSYMBOL_PARALLEL = 11,                  /* PARALLEL  */
  YYSYMBOL_ARROW = 12,                     /* ARROW  */
  YYSYMBOL_UPDATE_ARROW = 13,              /* UPDATE_ARROW  */
  YYSYMBOL_WHERE = 14,                     /* WHERE  */
  YYSYMBOL_DUP = 15,                       /* DUP  */
  YYSYMBOL_STOP = 16,                      /* STOP  */
  YYSYMBOL_AFTER = 17,                     /* AFTER  */
  YYSYMBOL_DOUBLEQUOTE = 18,               /* DOUBLEQUOTE  */
  YYSYMBOL_QUOTE = 19,                     /* QUOTE  */
  YYSYMBOL_PLUS = 20,                      /* PLUS  */
  YYSYMBOL_MINUS = 21,                     /* MINUS  */
  YYSYMBOL_MUL = 22,                       /* MUL  */
  YYSYMBOL_DIV = 23,                       /* DIV  */
  YYSYMBOL_MOD = 24,                       /* MOD  */
  YYSYMBOL_POW = 25,                       /* POW  */
  YYSYMBOL_LT = 26,                        /* LT  */
  YYSYMBOL_GT = 27,                        /* GT  */
  YYSYMBOL_LE = 28,                        /* LE  */
  YYSYMBOL_GE = 29,                        /* GE  */
  YYSYMBOL_EQ = 30,                        /* EQ  */
  YYSYMBOL_NEQ = 31,                       /* NEQ  */
  YYSYMBOL_AND = 32,                       /* AND  */
  YYSYMBOL_OR = 33,                        /* OR  */
  YYSYMBOL_LPAREN = 34,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 35,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 36,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 37,                    /* RBRACE  */
  YYSYMBOL_LBRACKET = 38,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 39,                  /* RBRACKET  */
  YYSYMBOL_COMMA = 40,                     /* COMMA  */
  YYSYMBOL_SEMI = 41,                      /* SEMI  */
  YYSYMBOL_EMIT = 42,                      /* EMIT  */
  YYSYMBOL_DOT = 43,                       /* DOT  */
  YYSYMBOL_TILDE = 44,                     /* TILDE  */
  YYSYMBOL_QUESTION = 45,                  /* QUESTION  */
  YYSYMBOL_AT = 46,                        /* AT  */
  YYSYMBOL_SPLIT = 47,                     /* SPLIT  */
  YYSYMBOL_MERGE = 48,                     /* MERGE  */
  YYSYMBOL_DOLLAR = 49,                    /* DOLLAR  */
  YYSYMBOL_YYACCEPT = 50,                  /* $accept  */
  YYSYMBOL_program = 51,                   /* program  */
  YYSYMBOL_statements = 52,                /* statements  */
  YYSYMBOL_statement = 53,                 /* statement  */
  YYSYMBOL_ident_assign = 54,              /* ident_assign  */
  YYSYMBOL_funcdef = 55,                   /* funcdef  */
  YYSYMBOL_parallel = 56,                  /* parallel  */
  YYSYMBOL_feedback = 57,                  /* feedback  */
  YYSYMBOL_lifecycle = 58,                 /* lifecycle  */
  YYSYMBOL_where_opt = 59,                 /* where_opt  */
  YYSYMBOL_typeparams = 60,                /* typeparams  */
  YYSYMBOL_typeparams_opt = 61,            /* typeparams_opt  */
  YYSYMBOL_paramlist_opt = 62,             /* paramlist_opt  */
  YYSYMBOL_paramlist = 63,                 /* paramlist  */
  YYSYMBOL_param = 64,                     /* param  */
  YYSYMBOL_stateupdate = 65,               /* stateupdate  */
  YYSYMBOL_stateupdate_opt = 66,           /* stateupdate_opt  */
  YYSYMBOL_block = 67,                     /* block  */
  YYSYMBOL_block_opt = 68,                 /* block_opt  */
  YYSYMBOL_stmtlist = 69,                  /* stmtlist  */
  YYSYMBOL_stmt = 70,                      /* stmt  */
  YYSYMBOL_updatestmt = 71,                /* updatestmt  */
  YYSYMBOL_emitstmt = 72,                  /* emitstmt  */
  YYSYMBOL_lambda_head = 73,               /* lambda_head  */
  YYSYMBOL_lambdaexpr = 74,                /* lambdaexpr  */
  YYSYMBOL_paramnames = 75,                /* paramnames  */
  YYSYMBOL_ternary_op = 76,                /* ternary_op  */
  YYSYMBOL_expr_opt = 77,                  /* expr_opt  */
  YYSYMBOL_expr = 78,                      /* expr  */
  YYSYMBOL_base_expr = 79,                 /* base_expr  */
  YYSYMBOL_ident_or_primary = 80,          /* ident_or_primary  */
  YYSYMBOL_operation = 81,                 /* operation  */
  YYSYMBOL_route = 82,                     /* route  */
  YYSYMBOL_module = 83,                    /* module  */
  YYSYMBOL_arglist_opt = 84,               /* arglist_opt  */
  YYSYMBOL_arglist = 85,                   /* arglist  */
  YYSYMBOL_arg = 86,                       /* arg  */
  YYSYMBOL_binop = 87,                     /* binop  */
  YYSYMBOL_string = 88,                    /* string  */
  YYSYMBOL_bracketupdate_opt = 89,         /* bracketupdate_opt  */
  YYSYMBOL_kvlist = 90,                    /* kvlist  */
  YYSYMBOL_kvpair = 91                     /* kvpair  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  48
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   615

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  50
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  42
/* YYNRULES -- Number of rules.  */
#define YYNRULES  104
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  161

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   304


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    64,    64,    73,    78,    85,    90,    95,   103,   121,
     131,   148,   154,   161,   169,   175,   184,   185,   193,   201,
     202,   208,   211,   215,   220,   227,   232,   240,   249,   250,
     254,   262,   263,   267,   268,   273,   280,   281,   282,   283,
     284,   288,   297,   301,   308,   312,   322,   327,   335,   346,
     347,   351,   357,   365,   373,   381,   392,   393,   394,   395,
     396,   397,   398,   399,   400,   401,   402,   403,   404,   405,
     410,   415,   425,   439,   452,   462,   469,   479,   489,   490,
     494,   500,   508,   514,   523,   524,   525,   526,   527,   528,
     529,   530,   531,   532,   533,   534,   535,   536,   540,   541,
     545,   546,   554,   559,   566
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IDENT",
  "IDENT_ASSIGN", "NUMBER", "TIME", "TRIGGER", "MODULE", "ASSIGN", "COLON",
  "PARALLEL", "ARROW", "UPDATE_ARROW", "WHERE", "DUP", "STOP", "AFTER",
  "DOUBLEQUOTE", "QUOTE", "PLUS", "MINUS", "MUL", "DIV", "MOD", "POW",
  "LT", "GT", "LE", "GE", "EQ", "NEQ", "AND", "OR", "LPAREN", "RPAREN",
  "LBRACE", "RBRACE", "LBRACKET", "RBRACKET", "COMMA", "SEMI", "EMIT",
  "DOT", "TILDE", "QUESTION", "AT", "SPLIT", "MERGE", "DOLLAR", "$accept",
  "program", "statements", "statement", "ident_assign", "funcdef",
  "parallel", "feedback", "lifecycle", "where_opt", "typeparams",
  "typeparams_opt", "paramlist_opt", "paramlist", "param", "stateupdate",
  "stateupdate_opt", "block", "block_opt", "stmtlist", "stmt",
  "updatestmt", "emitstmt", "lambda_head", "lambdaexpr", "paramnames",
  "ternary_op", "expr_opt", "expr", "base_expr", "ident_or_primary",
  "operation", "route", "module", "arglist_opt", "arglist", "arg", "binop",
  "string", "bracketupdate_opt", "kvlist", "kvpair", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-109)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-54)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     188,   -27,   542,  -109,  -109,  -109,   -13,   542,   522,   522,
      26,    38,   562,    52,   188,  -109,    44,  -109,    -6,  -109,
    -109,   542,  -109,  -109,   352,  -109,   -17,  -109,  -109,   502,
    -109,     7,  -109,   352,  -109,    54,   381,  -109,   352,  -109,
      27,  -109,    27,    61,    62,   -12,    -8,   207,  -109,  -109,
      46,  -109,    66,   542,   542,    47,   352,   542,   542,    46,
    -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,  -109,
    -109,  -109,  -109,  -109,    80,   542,   542,   542,    82,    83,
      53,    55,  -109,  -109,    56,    50,  -109,    57,    80,   522,
    -109,  -109,    81,    91,    59,   182,  -109,    46,  -109,   466,
       2,   182,  -109,   437,   466,  -109,  -109,    12,  -109,   577,
     577,   352,    70,  -109,   522,    89,     7,  -109,    21,  -109,
    -109,  -109,  -109,     3,   415,    58,   102,  -109,  -109,  -109,
     236,  -109,   142,  -109,    80,  -109,    65,   542,  -109,    30,
     542,  -109,  -109,   265,  -109,  -109,  -109,  -109,  -109,  -109,
      73,   294,   323,  -109,    80,  -109,    66,  -109,    22,  -109,
    -109
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    70,     0,    58,    59,    60,     0,     0,    78,     0,
       0,     0,     0,     0,     2,     3,    28,     5,    66,    61,
      68,    49,    65,    64,     7,    51,    56,    62,    67,    69,
      57,    21,    70,     8,    69,     0,     0,    82,    83,    14,
      79,    80,    15,     0,     0,    70,     0,     0,     1,     4,
       0,    29,    16,     0,     0,    31,    50,     0,     0,     0,
      84,    85,    86,    87,    88,    89,    92,    93,    94,    95,
      90,    91,    96,    97,     0,     0,     0,     0,     0,     0,
      73,     0,    25,    26,     0,    22,    23,     0,     0,     0,
      98,    99,     0,     0,    63,    33,    27,     0,    10,    12,
      48,    33,    45,    55,    11,    54,   104,     0,   102,    75,
      76,    74,     0,    72,    78,     0,     0,    77,     0,    81,
      44,    47,    13,     0,     0,    68,     0,    34,    36,    37,
       0,    17,     0,    52,     0,    18,     0,     0,    24,    52,
       0,    42,    38,     0,    40,    30,    35,    39,    32,   103,
     100,    28,     0,    41,     0,    71,    16,    43,     0,     9,
     101
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -109,  -109,  -109,    90,     0,  -109,  -109,  -109,   -76,   -44,
    -109,  -109,  -109,  -109,    -3,  -109,   -37,   -55,  -109,    14,
    -108,  -109,  -109,  -109,  -109,  -109,  -109,  -109,     1,  -109,
    -109,  -109,  -109,     6,     8,   114,    35,  -109,    93,  -109,
     -86,    -5
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    13,    14,    15,    37,    17,    18,    19,    20,    98,
      80,    81,    84,    85,    86,    51,    52,    96,   102,   126,
     127,   128,   129,    21,    22,    46,    23,    55,    38,    25,
      26,    27,    28,    34,    39,    40,    41,    77,    30,   155,
     107,   108
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      16,    24,   118,    33,   105,    53,    29,    31,    36,    78,
      82,     2,   140,    47,    16,    24,    59,   -19,   146,   125,
      29,    35,    56,   -46,   146,   125,    79,    92,   -46,    43,
     -53,    83,    93,   -53,   -53,   -53,   -53,   -53,   -53,    54,
      74,    44,   131,   -53,   141,   -53,   -53,   -53,   -53,   -53,
     125,   133,    48,   134,    99,   100,   125,    50,   103,   104,
     139,   160,   134,   134,   -53,   -53,   -53,    89,   158,   -53,
     -53,   -53,    10,    11,   106,   -53,   109,   110,   111,    90,
      97,    91,    95,   101,     2,   112,   113,   -20,   106,   114,
     116,   115,   117,   120,   121,   124,   130,   135,   137,   144,
     150,   124,   130,   122,    49,    32,     2,     3,     4,     5,
       6,   154,   159,   138,   156,   132,    83,     7,     8,     9,
      10,    11,   136,    42,   119,   143,   124,   130,    87,   149,
       0,     0,   124,   130,   106,     0,    12,     0,   151,   145,
       0,   152,     0,     0,   123,    32,     2,     3,     4,     5,
       6,     0,     0,     0,   106,     0,     0,     7,     8,     9,
      10,    11,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    12,     0,     0,   148,
       0,     0,     0,     0,   123,    32,     2,     3,     4,     5,
       6,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,     0,     7,     8,     9,    10,    11,     0,     0,
       0,     0,     0,     0,     0,     0,    12,    57,    58,     0,
       0,    59,    12,     0,   123,     0,     0,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,     0,    94,     0,     0,    74,    57,    58,     0,     0,
      59,     0,     0,     0,    75,    76,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
       0,     0,     0,     0,    74,    57,    58,   147,     0,    59,
       0,     0,     0,    75,    76,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,     0,
       0,     0,     0,    74,    57,    58,   153,    50,    59,     0,
       0,     0,    75,    76,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,     0,     0,
       0,     0,    74,    57,    58,     0,     0,    59,     0,     0,
       0,    75,    76,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,     0,     0,     0,
       0,    74,    57,    58,   157,     0,    59,     0,     0,     0,
      75,    76,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,     0,     0,     0,     0,
      74,    57,    58,     0,     0,    59,     0,     0,     0,    75,
      76,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,     0,     0,     0,    32,    88,
       3,     4,     5,     6,     0,     0,     0,     0,    75,    76,
       7,     8,     9,    10,    11,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    58,    12,
       0,    59,     0,     0,     0,     0,   142,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,     0,     0,     0,     0,    74,     0,     0,     0,     0,
      59,     0,     0,     0,    75,    76,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
       0,     0,    -6,     0,    74,    -6,    -6,    -6,    -6,    -6,
      -6,     0,     0,    75,    76,     0,     0,    -6,    -6,    -6,
      -6,    -6,     0,     0,     0,    32,     2,     3,     4,     5,
       6,     0,     0,     0,     0,     0,    -6,     7,     8,     9,
      10,    11,     0,     0,     0,    32,     0,     3,     4,     5,
       6,     0,     0,     0,     0,     0,    12,     7,     8,     9,
      10,    11,     0,     0,     0,    45,     0,     3,     4,     5,
       6,     0,     0,     0,     0,     0,    12,     7,     8,     9,
      10,    11,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    59,     0,     0,     0,     0,    12,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,     0,     0,     0,     0,    74
};

static const yytype_int16 yycheck[] =
{
       0,     0,    88,     2,    59,    11,     0,    34,     7,    26,
       3,     4,     9,    12,    14,    14,    14,    34,   126,    95,
      14,    34,    21,    35,   132,   101,    43,    35,    40,     3,
       0,    31,    40,     3,     4,     5,     6,     7,     8,    45,
      38,     3,    97,    13,    41,    15,    16,    17,    18,    19,
     126,    39,     0,    41,    53,    54,   132,    13,    57,    58,
      39,    39,    41,    41,    34,    35,    36,    40,   154,    39,
      40,    41,    18,    19,    74,    45,    75,    76,    77,    18,
      14,    19,    36,    36,     4,     3,     3,    34,    88,    34,
      40,    35,    35,    12,     3,    95,    95,    27,     9,    41,
      35,   101,   101,    44,    14,     3,     4,     5,     6,     7,
       8,    38,   156,   116,   151,   101,   116,    15,    16,    17,
      18,    19,   114,     9,    89,   124,   126,   126,    35,   134,
      -1,    -1,   132,   132,   134,    -1,    34,    -1,   137,    37,
      -1,   140,    -1,    -1,    42,     3,     4,     5,     6,     7,
       8,    -1,    -1,    -1,   154,    -1,    -1,    15,    16,    17,
      18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    34,    -1,    -1,    37,
      -1,    -1,    -1,    -1,    42,     3,     4,     5,     6,     7,
       8,     3,     4,     5,     6,     7,     8,    15,    16,    17,
      18,    19,    -1,    15,    16,    17,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    34,    10,    11,    -1,
      -1,    14,    34,    -1,    42,    -1,    -1,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    -1,    35,    -1,    -1,    38,    10,    11,    -1,    -1,
      14,    -1,    -1,    -1,    47,    48,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      -1,    -1,    -1,    -1,    38,    10,    11,    41,    -1,    14,
      -1,    -1,    -1,    47,    48,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    -1,
      -1,    -1,    -1,    38,    10,    11,    41,    13,    14,    -1,
      -1,    -1,    47,    48,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    -1,    -1,
      -1,    -1,    38,    10,    11,    -1,    -1,    14,    -1,    -1,
      -1,    47,    48,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    -1,    -1,    -1,
      -1,    38,    10,    11,    41,    -1,    14,    -1,    -1,    -1,
      47,    48,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    -1,    -1,    -1,    -1,
      38,    10,    11,    -1,    -1,    14,    -1,    -1,    -1,    47,
      48,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    -1,    -1,    -1,     3,    38,
       5,     6,     7,     8,    -1,    -1,    -1,    -1,    47,    48,
      15,    16,    17,    18,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    11,    34,
      -1,    14,    -1,    -1,    -1,    -1,    41,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    -1,    -1,    -1,    -1,    38,    -1,    -1,    -1,    -1,
      14,    -1,    -1,    -1,    47,    48,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      -1,    -1,     0,    -1,    38,     3,     4,     5,     6,     7,
       8,    -1,    -1,    47,    48,    -1,    -1,    15,    16,    17,
      18,    19,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,    -1,    -1,    -1,    -1,    -1,    34,    15,    16,    17,
      18,    19,    -1,    -1,    -1,     3,    -1,     5,     6,     7,
       8,    -1,    -1,    -1,    -1,    -1,    34,    15,    16,    17,
      18,    19,    -1,    -1,    -1,     3,    -1,     5,     6,     7,
       8,    -1,    -1,    -1,    -1,    -1,    34,    15,    16,    17,
      18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    14,    -1,    -1,    -1,    -1,    34,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    -1,    -1,    -1,    -1,    38
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,    15,    16,    17,
      18,    19,    34,    51,    52,    53,    54,    55,    56,    57,
      58,    73,    74,    76,    78,    79,    80,    81,    82,    83,
      88,    34,     3,    78,    83,    34,    78,    54,    78,    84,
      85,    86,    85,     3,     3,     3,    75,    78,     0,    53,
      13,    65,    66,    11,    45,    77,    78,    10,    11,    14,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    38,    47,    48,    87,    26,    43,
      60,    61,     3,    54,    62,    63,    64,    88,    38,    40,
      18,    19,    35,    40,    35,    36,    67,    14,    59,    78,
      78,    36,    68,    78,    78,    67,    54,    90,    91,    78,
      78,    78,     3,     3,    34,    35,    40,    35,    90,    86,
      12,     3,    44,    42,    54,    58,    69,    70,    71,    72,
      78,    67,    69,    39,    41,    27,    84,     9,    64,    39,
       9,    41,    41,    78,    41,    37,    70,    41,    37,    91,
      35,    78,    78,    41,    38,    89,    66,    41,    90,    59,
      39
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    50,    51,    52,    52,    53,    53,    53,    54,    55,
      55,    56,    56,    57,    58,    58,    59,    59,    60,    61,
      61,    62,    62,    63,    63,    64,    64,    65,    66,    66,
      67,    68,    68,    69,    69,    69,    70,    70,    70,    70,
      70,    71,    72,    72,    73,    74,    75,    75,    76,    77,
      77,    78,    78,    78,    78,    78,    79,    79,    79,    79,
      79,    79,    79,    79,    79,    79,    79,    79,    79,    79,
      80,    80,    80,    80,    81,    82,    82,    83,    84,    84,
      85,    85,    86,    86,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    88,    88,
      89,    89,    90,    90,    91
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     2,     8,
       3,     3,     3,     4,     2,     2,     0,     2,     3,     0,
       1,     0,     1,     1,     3,     1,     1,     2,     0,     1,
       3,     0,     3,     0,     1,     2,     1,     1,     2,     2,
       2,     3,     2,     4,     4,     3,     1,     3,     3,     0,
       1,     1,     4,     5,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     6,     3,     2,     3,     3,     3,     4,     0,     1,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       0,     3,     1,     3,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: statements  */
#line 64 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                 {
        auto pgm = std::make_shared<program>();
        pgm->statements = std::static_pointer_cast<statements>(yyvsp[0])->list;
        root = pgm;
        yyval = pgm;
    }
#line 1767 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 3: /* statements: statement  */
#line 73 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                {
        auto ptr = std::make_shared<statements>();
        ptr->list.push_back(yyvsp[0]);
        yyval = ptr;
    }
#line 1777 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 4: /* statements: statements statement  */
#line 78 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                           {
        std::static_pointer_cast<statements>(yyvsp[-1])->list.push_back( yyvsp[0] ); 
        yyval = yyvsp[-1];
    }
#line 1786 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 5: /* statement: funcdef  */
#line 85 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
              {
        auto ptr = std::make_shared<statement>();
        ptr->content = yyvsp[0];
        yyval = ptr;
    }
#line 1796 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 6: /* statement: module  */
#line 90 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
             {
        auto ptr = std::make_shared<statement>(); 
        ptr->content = yyvsp[0];
        yyval = ptr;
    }
#line 1806 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 7: /* statement: expr  */
#line 95 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
           {
        auto ptr = std::make_shared<statement>();
        ptr->content = yyvsp[0];
        yyval = ptr;
    }
#line 1816 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 8: /* ident_assign: IDENT_ASSIGN expr  */
#line 103 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                        {
        std::cerr << "DEBUG: ident_assign, expr is actually type: ";
        if (auto p = std::dynamic_pointer_cast<pipeline>(yyvsp[0])) {
            std::cerr << "PIPELINE!" << std::endl;
        } else if (auto e = std::dynamic_pointer_cast<expr>(yyvsp[0])) {
            std::cerr << "EXPR" << std::endl;
        } else {
            std::cerr << "OTHER: " << typeid(*yyvsp[0]).name() << std::endl;
        }
        auto ptr = std::make_shared<assignment>(); 
        ptr->identifier = yyvsp[-1]; 
        ptr->expr = yyvsp[0];
        yyval = ptr;
    }
#line 1835 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 9: /* funcdef: IDENT LPAREN paramlist_opt RPAREN ASSIGN expr stateupdate_opt where_opt  */
#line 121 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                                                               {
        std::cerr << "DEBUG: funcdef rule 1 (with params)" << std::endl;
        auto ptr = std::make_shared<function_def>();
        ptr->name = yyvsp[-7];
        ptr->paramlist_opt = yyvsp[-5]; 
        ptr->expr = yyvsp[-2];
        ptr->stateupdate_opt = yyvsp[-1];
        ptr->where_opt = yyvsp[0];
        yyval = ptr;
    }
#line 1850 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 10: /* funcdef: ident_assign stateupdate_opt where_opt  */
#line 132 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        std::cerr << "DEBUG: funcdef rule 2 (ident_assign)" << std::endl;
        auto ptr = std::make_shared<function_def>(); 
        auto assign = std::static_pointer_cast<assignment>(yyvsp[-2]);
        ptr->name = yyvsp[-2];
        ptr->expr = assign->expr; 
        ptr->stateupdate_opt = yyvsp[-1]; 
        ptr->where_opt = yyvsp[0];
        yyval = ptr;
    }
#line 1865 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 11: /* parallel: expr PARALLEL expr  */
#line 148 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                          {
        auto ptr = std::make_shared<parallel>();
        ptr->exprs.push_back(yyvsp[-2]);
        ptr->exprs.push_back(yyvsp[0]);
        yyval = ptr;
    }
#line 1876 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 12: /* parallel: parallel PARALLEL expr  */
#line 154 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             {
        std::static_pointer_cast<parallel>(yyvsp[-2])->exprs.push_back(yyvsp[0]);
        yyval = yyvsp[-2];
    }
#line 1885 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 13: /* feedback: LPAREN expr RPAREN TILDE  */
#line 161 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                               {
        auto ptr = std::make_shared<feedback>(); 
        ptr->expr = yyvsp[-2]; 
        yyval = ptr;
    }
#line 1895 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 14: /* lifecycle: STOP arglist_opt  */
#line 169 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                       {
        auto ptr = std::make_shared<lifecycle>();
        ptr->name = yyvsp[-1]; 
        ptr->args = yyvsp[0];
        yyval = ptr;
    }
#line 1906 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 15: /* lifecycle: AFTER arglist  */
#line 175 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                    {
        auto ptr = std::make_shared<lifecycle>();
        ptr->name = yyvsp[-1]; 
        ptr->args = yyvsp[0];
        yyval = ptr;
    }
#line 1917 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 16: /* where_opt: %empty  */
#line 184 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                  {yyval = std::shared_ptr<where>(nullptr); }
#line 1923 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 17: /* where_opt: WHERE block  */
#line 185 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                  {
        auto ptr = std::make_shared<where>();
        ptr->block = yyvsp[0];
        yyval = ptr;
    }
#line 1933 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 18: /* typeparams: LT IDENT GT  */
#line 193 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                  {
        auto ptr = std::make_shared<typeparams>();
        ptr->identifier = yyvsp[-1];
        yyval = ptr;
    }
#line 1943 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 19: /* typeparams_opt: %empty  */
#line 201 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                  { yyval = std::shared_ptr<typeparams>(nullptr);}
#line 1949 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 20: /* typeparams_opt: typeparams  */
#line 202 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                 {
        yyval = yyvsp[0];
    }
#line 1957 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 21: /* paramlist_opt: %empty  */
#line 208 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                  {
        yyval = std::make_shared<paramlist>();
    }
#line 1965 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 22: /* paramlist_opt: paramlist  */
#line 211 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                { yyval = yyvsp[0]; }
#line 1971 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 23: /* paramlist: param  */
#line 215 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
            {
        auto ptr = std::make_shared<paramlist>();
        ptr->list.push_back(yyvsp[0]);
        yyval = ptr;
    }
#line 1981 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 24: /* paramlist: paramlist COMMA param  */
#line 220 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            {
        std::static_pointer_cast<paramlist>(yyvsp[-2])->list.push_back(yyvsp[0]);
        yyval = yyvsp[-2];
    }
#line 1990 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 25: /* param: IDENT  */
#line 227 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
            {
        auto ptr = std::make_shared<param>();
        ptr->expr = yyvsp[0];
        yyval = ptr;
    }
#line 2000 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 26: /* param: ident_assign  */
#line 232 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                   {
        auto ptr = std::make_shared<param>(); 
        ptr->expr = yyvsp[0]; 
        yyval = ptr;
    }
#line 2010 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 27: /* stateupdate: UPDATE_ARROW block  */
#line 241 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto ptr = std::make_shared<stateupdate>();
        ptr->block = yyvsp[0];
        yyval = ptr;
    }
#line 2020 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 28: /* stateupdate_opt: %empty  */
#line 249 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
       {yyval = std::shared_ptr<stateupdate>(nullptr);}
#line 2026 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 29: /* stateupdate_opt: stateupdate  */
#line 250 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                  {yyval = yyvsp[0];}
#line 2032 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 30: /* block: LBRACE stmtlist RBRACE  */
#line 254 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             {
        auto ptr = std::make_shared<block>();
        ptr->stmtlist = yyvsp[-1];
        yyval = ptr;
    }
#line 2042 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 31: /* block_opt: %empty  */
#line 262 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
      {yyval = std::shared_ptr<block>(nullptr);}
#line 2048 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 32: /* block_opt: LBRACE stmtlist RBRACE  */
#line 263 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             {yyval = yyvsp[-1];}
#line 2054 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 33: /* stmtlist: %empty  */
#line 267 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                  {yyval = std::make_shared<stmtlist>();}
#line 2060 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 34: /* stmtlist: stmt  */
#line 268 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
           {
        auto ptr = std::make_shared<stmtlist>();
        ptr->list.push_back(yyvsp[0]);
        yyval = ptr;
    }
#line 2070 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 35: /* stmtlist: stmtlist stmt  */
#line 273 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                    {
        std::static_pointer_cast<stmtlist>(yyvsp[-1])->list.push_back(yyvsp[0]);
        yyval = yyvsp[-1];
    }
#line 2079 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 36: /* stmt: updatestmt  */
#line 280 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                 {yyval = std::make_shared<stmt>(yyvsp[0]);}
#line 2085 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 37: /* stmt: emitstmt  */
#line 281 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
               {yyval = std::make_shared<stmt>(yyvsp[0]);}
#line 2091 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 38: /* stmt: ident_assign SEMI  */
#line 282 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                        {yyval = std::make_shared<stmt>(yyvsp[-1]);}
#line 2097 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 39: /* stmt: expr SEMI  */
#line 283 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                {yyval = std::make_shared<stmt>(yyvsp[-1]);}
#line 2103 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 40: /* stmt: lifecycle SEMI  */
#line 284 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                     {yyval = std::make_shared<stmt>(yyvsp[-1]);}
#line 2109 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 41: /* updatestmt: ident_assign expr SEMI  */
#line 288 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             {
        auto ptr = std::make_shared<updatestmt>();
        ptr->name = yyvsp[-2]; 
        ptr->expr = yyvsp[-1];
        yyval = ptr;
    }
#line 2120 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 42: /* emitstmt: EMIT SEMI  */
#line 297 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                {
        std::static_pointer_cast<emit>(yyvsp[-1])->expr = nullnode();
        yyval = yyvsp[-1];
    }
#line 2129 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 43: /* emitstmt: EMIT ASSIGN expr SEMI  */
#line 301 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            {
        std::static_pointer_cast<emit>(yyvsp[-3])->expr = yyvsp[-1];
        yyval = yyvsp[-3];
    }
#line 2138 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 44: /* lambda_head: LPAREN paramnames RPAREN ARROW  */
#line 308 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                     {yyval = yyvsp[-2];}
#line 2144 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 45: /* lambdaexpr: lambda_head expr_opt block_opt  */
#line 312 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                     {
        auto ptr = std::make_shared<lambdaexpr>();
        ptr->paramnames = yyvsp[-2];
        ptr->expr = yyvsp[-1];
        ptr->block_opt = yyvsp[0];
        yyval = ptr;
    }
#line 2156 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 46: /* paramnames: IDENT  */
#line 322 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
            {
        auto ptr = std::make_shared<paramnames>();
        ptr->list.push_back(yyvsp[0]);
        yyval = ptr;
    }
#line 2166 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 47: /* paramnames: paramnames COMMA IDENT  */
#line 327 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             {
        std::static_pointer_cast<paramnames>(yyvsp[-2])->list.push_back(yyvsp[0]);
        yyval = yyvsp[-2];
    }
#line 2175 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 48: /* ternary_op: parallel QUESTION expr  */
#line 335 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                              {
        auto ptr = std::make_shared<ternary_op>();
        ptr->parallel = yyvsp[-2];
        ptr->condition = yyvsp[0]; 
        yyval = ptr;
    }
#line 2186 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 49: /* expr_opt: %empty  */
#line 346 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                 {yyval = std::make_shared<expr>();}
#line 2192 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 50: /* expr_opt: expr  */
#line 347 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
           {yyval = yyvsp[0];}
#line 2198 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 51: /* expr: base_expr  */
#line 351 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                {
        auto ptr = std::make_shared<expr>();
        ptr->base_expr = yyvsp[0];
        ptr->dup = false;
        yyval = ptr;
    }
#line 2209 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 52: /* expr: expr LBRACKET kvlist RBRACKET  */
#line 358 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto ptr = std::make_shared<expr>();
        ptr->lexpr = yyvsp[-3]; 
        ptr->dup = false;
        ptr->kvlist = yyvsp[-1]; 
        yyval = ptr;
    }
#line 2221 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 53: /* expr: DUP expr LBRACKET kvlist RBRACKET  */
#line 366 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto ptr = std::make_shared<expr>();
        ptr->dup = true;
        ptr->lexpr = yyvsp[-3]; 
        ptr->kvlist = yyvsp[-1]; 
        yyval = ptr;
    }
#line 2233 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 54: /* expr: expr WHERE block  */
#line 374 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto ptr = std::make_shared<expr>();
        ptr->lexpr = yyvsp[-2]; 
        ptr->dup = false;
        ptr->block = yyvsp[0]; 
        yyval = ptr;
    }
#line 2245 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 55: /* expr: expr COLON expr  */
#line 381 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                      { // Pipeline
        std::cerr << "DEBUG: Creating pipeline" << std::endl;
        auto ptr = std::make_shared<pipeline>();
        ptr->expr1 = yyvsp[-2];
        ptr->expr2 = yyvsp[0];
        ptr->lambdaexpr = nullptr;
        yyval = ptr;
    }
#line 2258 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 56: /* base_expr: ident_or_primary  */
#line 392 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                       {yyval = yyvsp[0];}
#line 2264 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 57: /* base_expr: string  */
#line 393 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
             {yyval = yyvsp[0];}
#line 2270 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 58: /* base_expr: NUMBER  */
#line 394 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
             {yyval = yyvsp[0];}
#line 2276 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 59: /* base_expr: TIME  */
#line 395 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
           {yyval = yyvsp[0];}
#line 2282 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 60: /* base_expr: TRIGGER  */
#line 396 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
              {yyval = yyvsp[0];}
#line 2288 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 61: /* base_expr: feedback  */
#line 397 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
               {yyval = yyvsp[0];}
#line 2294 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 62: /* base_expr: operation  */
#line 398 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                { yyval = yyvsp[0]; }
#line 2300 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 63: /* base_expr: LPAREN expr RPAREN  */
#line 399 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { yyval = yyvsp[-1]; }
#line 2306 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 64: /* base_expr: ternary_op  */
#line 400 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                 {yyval = yyvsp[0];}
#line 2312 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 65: /* base_expr: lambdaexpr  */
#line 401 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                 {yyval = yyvsp[0];}
#line 2318 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 66: /* base_expr: parallel  */
#line 402 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
               {yyval = yyvsp[0];}
#line 2324 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 67: /* base_expr: route  */
#line 403 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
             {yyval = yyvsp[0];}
#line 2330 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 68: /* base_expr: lifecycle  */
#line 404 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                 {yyval = yyvsp[0];}
#line 2336 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 69: /* base_expr: module  */
#line 405 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
             {yyval = yyvsp[0]; }
#line 2342 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 70: /* ident_or_primary: IDENT  */
#line 410 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
            {
        /* start with an identifier node */
        yyval = yyvsp[0];
    }
#line 2351 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 71: /* ident_or_primary: ident_or_primary typeparams_opt LPAREN arglist_opt RPAREN bracketupdate_opt  */
#line 415 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                                                                  {
        /* $1 is the callee (node_ptr), $2 typeparams_opt, $4 arglist_opt, $6 bracketupdate_opt */
        auto call = std::make_shared<function_call>();
        call->callee = yyvsp[-5];
        call->typeparams_opt = yyvsp[-4];
        call->args_opt = yyvsp[-2];
        call->bracketupdate_opt = yyvsp[0];
        yyval = call;
    }
#line 2365 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 72: /* ident_or_primary: ident_or_primary DOT IDENT  */
#line 425 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                 {
        /* build a module_call-like node where $1 is instance and $3 is method identifier */
        auto mc = std::make_shared<module_call>();
        mc->instance_name = yyvsp[-2];
        /* create a placeholder function_call for the method; actual call suffix may follow */
        auto method_call = std::make_shared<function_call>();
        method_call->callee = yyvsp[0]; /* method identifier node */
        method_call->typeparams_opt = std::shared_ptr<typeparams>(nullptr);
        method_call->args_opt = std::make_shared<arglist>();
        method_call->bracketupdate_opt = std::shared_ptr<bracketupdate>(nullptr);
        mc->method_call = method_call;
        yyval = mc;
    }
#line 2383 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 73: /* ident_or_primary: ident_or_primary typeparams  */
#line 439 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                  {
        /* If $1 is an identifier and $2 is typeparams, create a cast node.
           If you prefer cast only when starting with IDENT, you can keep original cast rule.
           Here we allow chaining like IDENT<...> then call suffix. */
        auto c = std::make_shared<cast>();
        c->identifier = yyvsp[-1];
        c->typeparams = yyvsp[0];
        yyval = c;
    }
#line 2397 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 74: /* operation: expr binop expr  */
#line 452 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                       {
        auto ptr = std::make_shared<operation>();
        ptr->lexpr = yyvsp[-2];
        ptr->binop = yyvsp[-1];
        ptr->rexpr = yyvsp[0];
        yyval = ptr;
    }
#line 2409 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 75: /* route: expr SPLIT expr  */
#line 462 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                      {
        auto ptr = std::make_shared<route>();
        ptr->lexpr = yyvsp[-2];
        ptr->rexpr = yyvsp[0]; 
        ptr->type = yyvsp[-1];
        yyval = ptr;
    }
#line 2421 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 76: /* route: expr MERGE expr  */
#line 469 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                      {
        auto ptr = std::make_shared<route>();
        ptr->lexpr = yyvsp[-2];
        ptr->rexpr = yyvsp[0]; 
        ptr->type = yyvsp[-1];
        yyval = ptr;
    }
#line 2433 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 77: /* module: MODULE LPAREN string RPAREN  */
#line 479 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                   {
        auto ptr = std::make_shared<module_instance>();
        ptr->identifier = yyvsp[-3];
        ptr->module_name = yyvsp[0];
        yyval = ptr;
    }
#line 2444 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 78: /* arglist_opt: %empty  */
#line 489 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                  {yyval = std::make_shared<arglist>();}
#line 2450 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 79: /* arglist_opt: arglist  */
#line 490 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
              { std::cout << "Arglist " << std::endl; yyval = yyvsp[0]; }
#line 2456 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 80: /* arglist: arg  */
#line 494 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
          {
        auto ptr = std::make_shared<arglist>();
        ptr->list.push_back(yyvsp[0]);
        std::cout << "add arg to arglist" << std::endl;
        yyval = ptr;
    }
#line 2467 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 81: /* arglist: arglist COMMA arg  */
#line 500 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                        {
        std::static_pointer_cast<arglist>(yyvsp[-2])->list.push_back(yyvsp[0]);
        std::cout << "push arg to arglist" << std::endl;
        yyval = yyvsp[-2];
    }
#line 2477 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 82: /* arg: ident_assign  */
#line 508 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                   {
        std::cout << "arg : assign" << std::endl;
        auto ptr = std::make_shared<arg>(); 
        ptr->expr = yyvsp[0]; 
        yyval = ptr;
    }
#line 2488 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 83: /* arg: expr  */
#line 514 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
           {
        std::cout << "arg: expr" << std::endl; 
        auto ptr = std::make_shared<arg>();
        ptr->expr = yyvsp[0];
        yyval = ptr;
    }
#line 2499 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 84: /* binop: PLUS  */
#line 523 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
           {yyval = yyvsp[0];}
#line 2505 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 85: /* binop: MINUS  */
#line 524 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
            {yyval = yyvsp[0];}
#line 2511 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 86: /* binop: MUL  */
#line 525 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
          {yyval = yyvsp[0];}
#line 2517 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 87: /* binop: DIV  */
#line 526 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
          {yyval = yyvsp[0];}
#line 2523 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 88: /* binop: MOD  */
#line 527 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
          {yyval = yyvsp[0];}
#line 2529 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 89: /* binop: POW  */
#line 528 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
          {yyval = yyvsp[0];}
#line 2535 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 90: /* binop: EQ  */
#line 529 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
         {yyval = yyvsp[0];}
#line 2541 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 91: /* binop: NEQ  */
#line 530 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
          {yyval = yyvsp[0];}
#line 2547 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 92: /* binop: LT  */
#line 531 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
         {yyval = yyvsp[0];}
#line 2553 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 93: /* binop: GT  */
#line 532 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
         {yyval = yyvsp[0];}
#line 2559 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 94: /* binop: LE  */
#line 533 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
         {yyval = yyvsp[0];}
#line 2565 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 95: /* binop: GE  */
#line 534 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
         {yyval = yyvsp[0];}
#line 2571 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 96: /* binop: AND  */
#line 535 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
          {yyval = yyvsp[0];}
#line 2577 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 97: /* binop: OR  */
#line 536 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
         {yyval = yyvsp[0];}
#line 2583 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 100: /* bracketupdate_opt: %empty  */
#line 545 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                  {yyval = std::shared_ptr<bracketupdate>(nullptr);}
#line 2589 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 101: /* bracketupdate_opt: LBRACKET kvlist RBRACKET  */
#line 546 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                               {
        auto ptr = std::make_shared<bracketupdate>();
        ptr->kvlist = std::static_pointer_cast<kvlist>(yyvsp[-1])->list;
        yyval = ptr;
    }
#line 2599 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 102: /* kvlist: kvpair  */
#line 554 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
             {
        auto ptr = std::make_shared<kvlist>();
        ptr->list.push_back(yyvsp[0]);
        yyval = ptr;
    }
#line 2609 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 103: /* kvlist: kvlist SEMI kvpair  */
#line 559 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         {
        std::static_pointer_cast<kvlist>(yyvsp[-2])->list.push_back(yyvsp[0]);
        yyval = yyvsp[-2];
    }
#line 2618 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;

  case 104: /* kvpair: ident_assign  */
#line 566 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                   {
        auto ptr = std::make_shared<kvpair>();
        auto assign = std::static_pointer_cast<assignment>(yyvsp[0]);
        ptr->identifier = assign->identifier; 
        ptr->expr = assign->expr; 
        yyval = ptr;
    }
#line 2630 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"
    break;


#line 2634 "/home/johann/Documents/git/idyl/src/parser/idyl_parser.tab.cc"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 575 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
