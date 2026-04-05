// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

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

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "idyl.tab.hh"


// Unqualified %code blocks.
#line 24 "/home/johann/Documents/git/idyl/src/parser/idyl.y"

    #include <iostream>
    #include <sstream>
    //#include "idyl_lexer.hpp"
    
    namespace yy {
        parser::symbol_type yylex(lexer& driver);
    }
    
    std::shared_ptr<idyl::parser::program> g_program = nullptr;

#line 58 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
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

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
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


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {
#line 150 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"

  /// Build a parser object.
  parser::parser (yy::lexer& driver_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      driver (driver_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
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
        value.YY_MOVE_OR_COPY< idyl::parser::expr_ptr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_program: // program
        value.YY_MOVE_OR_COPY< idyl::parser::node_ptr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter: // parameter
        value.YY_MOVE_OR_COPY< idyl::parser::param_ptr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        value.YY_MOVE_OR_COPY< idyl::parser::stmt_ptr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_argument: // argument
        value.YY_MOVE_OR_COPY< std::shared_ptr<idyl::parser::argument> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_catch_block: // catch_block
        value.YY_MOVE_OR_COPY< std::shared_ptr<idyl::parser::catch_block> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        value.YY_MOVE_OR_COPY< std::shared_ptr<idyl::parser::flow_definition> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_function_definition: // function_definition
        value.YY_MOVE_OR_COPY< std::shared_ptr<idyl::parser::function_definition> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_init_block: // init_block
        value.YY_MOVE_OR_COPY< std::shared_ptr<idyl::parser::init_block> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        value.YY_MOVE_OR_COPY< std::shared_ptr<idyl::parser::lambda_block> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_process_block: // process_block
        value.YY_MOVE_OR_COPY< std::shared_ptr<idyl::parser::process_block> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        value.YY_MOVE_OR_COPY< std::vector<idyl::parser::expr_ptr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        value.YY_MOVE_OR_COPY< std::vector<idyl::parser::param_ptr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        value.YY_MOVE_OR_COPY< std::vector<idyl::parser::stmt_ptr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_argument_list: // argument_list
        value.YY_MOVE_OR_COPY< std::vector<std::shared_ptr<idyl::parser::argument>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_flow_members: // flow_members
        value.YY_MOVE_OR_COPY< std::vector<std::shared_ptr<idyl::parser::flow_member>> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
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
        value.move< idyl::parser::expr_ptr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_program: // program
        value.move< idyl::parser::node_ptr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter: // parameter
        value.move< idyl::parser::param_ptr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        value.move< idyl::parser::stmt_ptr > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_argument: // argument
        value.move< std::shared_ptr<idyl::parser::argument> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_catch_block: // catch_block
        value.move< std::shared_ptr<idyl::parser::catch_block> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        value.move< std::shared_ptr<idyl::parser::flow_definition> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_function_definition: // function_definition
        value.move< std::shared_ptr<idyl::parser::function_definition> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_init_block: // init_block
        value.move< std::shared_ptr<idyl::parser::init_block> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        value.move< std::shared_ptr<idyl::parser::lambda_block> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_process_block: // process_block
        value.move< std::shared_ptr<idyl::parser::process_block> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        value.move< std::vector<idyl::parser::expr_ptr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        value.move< std::vector<idyl::parser::param_ptr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        value.move< std::vector<idyl::parser::stmt_ptr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_argument_list: // argument_list
        value.move< std::vector<std::shared_ptr<idyl::parser::argument>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_flow_members: // flow_members
        value.move< std::vector<std::shared_ptr<idyl::parser::flow_member>> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
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
        value.copy< idyl::parser::expr_ptr > (that.value);
        break;

      case symbol_kind::S_program: // program
        value.copy< idyl::parser::node_ptr > (that.value);
        break;

      case symbol_kind::S_parameter: // parameter
        value.copy< idyl::parser::param_ptr > (that.value);
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        value.copy< idyl::parser::stmt_ptr > (that.value);
        break;

      case symbol_kind::S_argument: // argument
        value.copy< std::shared_ptr<idyl::parser::argument> > (that.value);
        break;

      case symbol_kind::S_catch_block: // catch_block
        value.copy< std::shared_ptr<idyl::parser::catch_block> > (that.value);
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        value.copy< std::shared_ptr<idyl::parser::flow_definition> > (that.value);
        break;

      case symbol_kind::S_function_definition: // function_definition
        value.copy< std::shared_ptr<idyl::parser::function_definition> > (that.value);
        break;

      case symbol_kind::S_init_block: // init_block
        value.copy< std::shared_ptr<idyl::parser::init_block> > (that.value);
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        value.copy< std::shared_ptr<idyl::parser::lambda_block> > (that.value);
        break;

      case symbol_kind::S_process_block: // process_block
        value.copy< std::shared_ptr<idyl::parser::process_block> > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        value.copy< std::string > (that.value);
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        value.copy< std::vector<idyl::parser::expr_ptr> > (that.value);
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        value.copy< std::vector<idyl::parser::param_ptr> > (that.value);
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        value.copy< std::vector<idyl::parser::stmt_ptr> > (that.value);
        break;

      case symbol_kind::S_argument_list: // argument_list
        value.copy< std::vector<std::shared_ptr<idyl::parser::argument>> > (that.value);
        break;

      case symbol_kind::S_flow_members: // flow_members
        value.copy< std::vector<std::shared_ptr<idyl::parser::flow_member>> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
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
        value.move< idyl::parser::expr_ptr > (that.value);
        break;

      case symbol_kind::S_program: // program
        value.move< idyl::parser::node_ptr > (that.value);
        break;

      case symbol_kind::S_parameter: // parameter
        value.move< idyl::parser::param_ptr > (that.value);
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        value.move< idyl::parser::stmt_ptr > (that.value);
        break;

      case symbol_kind::S_argument: // argument
        value.move< std::shared_ptr<idyl::parser::argument> > (that.value);
        break;

      case symbol_kind::S_catch_block: // catch_block
        value.move< std::shared_ptr<idyl::parser::catch_block> > (that.value);
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        value.move< std::shared_ptr<idyl::parser::flow_definition> > (that.value);
        break;

      case symbol_kind::S_function_definition: // function_definition
        value.move< std::shared_ptr<idyl::parser::function_definition> > (that.value);
        break;

      case symbol_kind::S_init_block: // init_block
        value.move< std::shared_ptr<idyl::parser::init_block> > (that.value);
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        value.move< std::shared_ptr<idyl::parser::lambda_block> > (that.value);
        break;

      case symbol_kind::S_process_block: // process_block
        value.move< std::shared_ptr<idyl::parser::process_block> > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        value.move< std::string > (that.value);
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        value.move< std::vector<idyl::parser::expr_ptr> > (that.value);
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        value.move< std::vector<idyl::parser::param_ptr> > (that.value);
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        value.move< std::vector<idyl::parser::stmt_ptr> > (that.value);
        break;

      case symbol_kind::S_argument_list: // argument_list
        value.move< std::vector<std::shared_ptr<idyl::parser::argument>> > (that.value);
        break;

      case symbol_kind::S_flow_members: // flow_members
        value.move< std::vector<std::shared_ptr<idyl::parser::flow_member>> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
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
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
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
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
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
        yylhs.value.emplace< idyl::parser::expr_ptr > ();
        break;

      case symbol_kind::S_program: // program
        yylhs.value.emplace< idyl::parser::node_ptr > ();
        break;

      case symbol_kind::S_parameter: // parameter
        yylhs.value.emplace< idyl::parser::param_ptr > ();
        break;

      case symbol_kind::S_top_level_statement: // top_level_statement
      case symbol_kind::S_function_or_flow_definition: // function_or_flow_definition
      case symbol_kind::S_process_body_statement: // process_body_statement
      case symbol_kind::S_lambda_statement: // lambda_statement
        yylhs.value.emplace< idyl::parser::stmt_ptr > ();
        break;

      case symbol_kind::S_argument: // argument
        yylhs.value.emplace< std::shared_ptr<idyl::parser::argument> > ();
        break;

      case symbol_kind::S_catch_block: // catch_block
        yylhs.value.emplace< std::shared_ptr<idyl::parser::catch_block> > ();
        break;

      case symbol_kind::S_flow_definition: // flow_definition
        yylhs.value.emplace< std::shared_ptr<idyl::parser::flow_definition> > ();
        break;

      case symbol_kind::S_function_definition: // function_definition
        yylhs.value.emplace< std::shared_ptr<idyl::parser::function_definition> > ();
        break;

      case symbol_kind::S_init_block: // init_block
        yylhs.value.emplace< std::shared_ptr<idyl::parser::init_block> > ();
        break;

      case symbol_kind::S_lambda_block: // lambda_block
        yylhs.value.emplace< std::shared_ptr<idyl::parser::lambda_block> > ();
        break;

      case symbol_kind::S_process_block: // process_block
        yylhs.value.emplace< std::shared_ptr<idyl::parser::process_block> > ();
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_TIME_LITERAL: // TIME_LITERAL
      case symbol_kind::S_STRING_LITERAL: // STRING_LITERAL
        yylhs.value.emplace< std::string > ();
        break;

      case symbol_kind::S_flow_elements: // flow_elements
      case symbol_kind::S_ternary_options: // ternary_options
        yylhs.value.emplace< std::vector<idyl::parser::expr_ptr> > ();
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        yylhs.value.emplace< std::vector<idyl::parser::param_ptr> > ();
        break;

      case symbol_kind::S_top_level_statements: // top_level_statements
      case symbol_kind::S_process_body_statements: // process_body_statements
      case symbol_kind::S_lambda_statements: // lambda_statements
        yylhs.value.emplace< std::vector<idyl::parser::stmt_ptr> > ();
        break;

      case symbol_kind::S_argument_list: // argument_list
        yylhs.value.emplace< std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        break;

      case symbol_kind::S_flow_members: // flow_members
        yylhs.value.emplace< std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // program: top_level_statements
#line 111 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto prog = std::make_shared<idyl::parser::program>();
        prog->statements_ = yystack_[0].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        yylhs.value.as < idyl::parser::node_ptr > () = prog;
        g_program = prog;
    }
#line 1019 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 3: // program: %empty
#line 118 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto prog = std::make_shared<idyl::parser::program>();
        yylhs.value.as < idyl::parser::node_ptr > () = prog;
        g_program = prog;
    }
#line 1029 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 4: // top_level_statements: top_level_statements top_level_statement
#line 127 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1038 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 5: // top_level_statements: top_level_statement
#line 132 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = {};
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1047 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 6: // top_level_statement: function_or_flow_definition
#line 139 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                  { yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < idyl::parser::stmt_ptr > (); }
#line 1053 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 7: // top_level_statement: process_block
#line 140 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                    { yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::process_block> > (); }
#line 1059 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 8: // top_level_statement: LIB LPAREN STRING_LITERAL RPAREN
#line 142 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lib_import = std::make_shared<idyl::parser::library_import>();
        lib_import->path_ = yystack_[1].value.as < std::string > ();
        lib_import->line_ = yystack_[3].location.begin.line;
        lib_import->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = lib_import;
    }
#line 1071 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 9: // top_level_statement: IDENTIFIER ASSIGN LIB LPAREN STRING_LITERAL RPAREN
#line 150 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lib_import = std::make_shared<idyl::parser::library_import>();
        lib_import->namespace_ = yystack_[5].value.as < std::string > ();
        lib_import->path_ = yystack_[1].value.as < std::string > ();
        lib_import->line_ = yystack_[5].location.begin.line;
        lib_import->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = lib_import;
    }
#line 1084 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 10: // top_level_statement: MODULE LPAREN STRING_LITERAL RPAREN
#line 159 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto mod_import = std::make_shared<idyl::parser::module_import>();
        mod_import->path_ = yystack_[1].value.as < std::string > ();
        mod_import->line_ = yystack_[3].location.begin.line;
        mod_import->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = mod_import;
    }
#line 1096 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 11: // top_level_statement: IDENTIFIER ASSIGN MODULE LPAREN STRING_LITERAL RPAREN
#line 167 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto mod_import = std::make_shared<idyl::parser::module_import>();
        mod_import->namespace_ = yystack_[5].value.as < std::string > ();
        mod_import->path_ = yystack_[1].value.as < std::string > ();
        mod_import->line_ = yystack_[5].location.begin.line;
        mod_import->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = mod_import;
    }
#line 1109 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 12: // function_or_flow_definition: function_definition
#line 178 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                          { yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::function_definition> > (); }
#line 1115 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 13: // function_or_flow_definition: flow_definition
#line 179 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                      { yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::flow_definition> > (); }
#line 1121 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 14: // function_definition: IDENTIFIER LPAREN parameter_list RPAREN ASSIGN expression
#line 184 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = yystack_[5].value.as < std::string > ();
        func->parameters_ = yystack_[3].value.as < std::vector<idyl::parser::param_ptr> > ();
        func->body_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        func->line_ = yystack_[5].location.begin.line;
        func->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::function_definition> > () = func;
    }
#line 1135 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 15: // function_definition: IDENTIFIER LPAREN RPAREN ASSIGN expression
#line 194 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = yystack_[4].value.as < std::string > ();
        func->parameters_ = {};
        func->body_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        func->line_ = yystack_[4].location.begin.line;
        func->column_ = yystack_[4].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::function_definition> > () = func;
    }
#line 1149 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 16: // function_definition: IDENTIFIER LPAREN parameter_list RPAREN ASSIGN expression lambda_block
#line 204 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = yystack_[6].value.as < std::string > ();
        func->parameters_ = yystack_[4].value.as < std::vector<idyl::parser::param_ptr> > ();
        func->body_ = yystack_[1].value.as < idyl::parser::expr_ptr > ();
        func->lambda_block_ = yystack_[0].value.as < std::shared_ptr<idyl::parser::lambda_block> > ();
        func->line_ = yystack_[6].location.begin.line;
        func->column_ = yystack_[6].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::function_definition> > () = func;
    }
#line 1164 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 17: // function_definition: IDENTIFIER LPAREN RPAREN ASSIGN expression lambda_block
#line 215 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = yystack_[5].value.as < std::string > ();
        func->parameters_ = {};
        func->body_ = yystack_[1].value.as < idyl::parser::expr_ptr > ();
        func->lambda_block_ = yystack_[0].value.as < std::shared_ptr<idyl::parser::lambda_block> > ();
        func->line_ = yystack_[5].location.begin.line;
        func->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::function_definition> > () = func;
    }
#line 1179 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 18: // function_definition: IDENTIFIER ASSIGN expression
#line 226 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = yystack_[2].value.as < std::string > ();
        func->parameters_ = {};
        func->body_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        func->line_ = yystack_[2].location.begin.line;
        func->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::function_definition> > () = func;
    }
#line 1193 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 19: // function_definition: IDENTIFIER ASSIGN expression lambda_block
#line 236 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = yystack_[3].value.as < std::string > ();
        func->parameters_ = {};
        func->body_ = yystack_[1].value.as < idyl::parser::expr_ptr > ();
        func->lambda_block_ = yystack_[0].value.as < std::shared_ptr<idyl::parser::lambda_block> > ();
        func->line_ = yystack_[3].location.begin.line;
        func->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::function_definition> > () = func;
    }
#line 1208 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 20: // flow_definition: FLOW IDENTIFIER ASSIGN flow_literal
#line 250 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto flow = std::make_shared<idyl::parser::flow_definition>();
        flow->name_ = yystack_[2].value.as < std::string > ();
        flow->parameters_ = {};
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = "";
        member->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        member->line_ = yystack_[1].location.begin.line;
        member->column_ = yystack_[1].location.begin.column;
        flow->members_.push_back(member);
        flow->line_ = yystack_[3].location.begin.line;
        flow->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::flow_definition> > () = flow;
    }
#line 1227 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 21: // flow_definition: FLOW IDENTIFIER LPAREN parameter_list RPAREN ASSIGN flow_literal
#line 265 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto flow = std::make_shared<idyl::parser::flow_definition>();
        flow->name_ = yystack_[5].value.as < std::string > ();
        flow->parameters_ = yystack_[3].value.as < std::vector<idyl::parser::param_ptr> > ();
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = "";
        member->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        member->line_ = yystack_[1].location.begin.line;
        member->column_ = yystack_[1].location.begin.column;
        flow->members_.push_back(member);
        flow->line_ = yystack_[6].location.begin.line;
        flow->column_ = yystack_[6].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::flow_definition> > () = flow;
    }
#line 1246 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 22: // flow_definition: FLOW IDENTIFIER ASSIGN LBRACE flow_members RBRACE
#line 280 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto flow = std::make_shared<idyl::parser::flow_definition>();
        flow->name_ = yystack_[4].value.as < std::string > ();
        flow->parameters_ = {};
        flow->members_ = yystack_[1].value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        flow->line_ = yystack_[5].location.begin.line;
        flow->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::flow_definition> > () = flow;
    }
#line 1260 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 23: // flow_definition: FLOW IDENTIFIER LPAREN parameter_list RPAREN ASSIGN LBRACE flow_members RBRACE
#line 290 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto flow = std::make_shared<idyl::parser::flow_definition>();
        flow->name_ = yystack_[7].value.as < std::string > ();
        flow->parameters_ = yystack_[5].value.as < std::vector<idyl::parser::param_ptr> > ();
        flow->members_ = yystack_[1].value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        flow->line_ = yystack_[8].location.begin.line;
        flow->column_ = yystack_[8].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::flow_definition> > () = flow;
    }
#line 1274 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 24: // flow_members: flow_members IDENTIFIER COLON flow_literal
#line 303 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > () = yystack_[3].value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = yystack_[2].value.as < std::string > ();
        member->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        member->line_ = yystack_[2].location.begin.line;
        member->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ().push_back(member);
    }
#line 1288 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 25: // flow_members: IDENTIFIER COLON flow_literal
#line 313 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > () = {};
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = yystack_[2].value.as < std::string > ();
        member->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        member->line_ = yystack_[2].location.begin.line;
        member->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ().push_back(member);
    }
#line 1302 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 26: // flow_literal: LBRACKET flow_elements RBRACKET
#line 326 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto flow_lit = std::make_shared<idyl::parser::flow_literal>();
        flow_lit->elements_ = yystack_[1].value.as < std::vector<idyl::parser::expr_ptr> > ();
        flow_lit->line_ = yystack_[2].location.begin.line;
        flow_lit->column_ = yystack_[2].location.begin.column;
        auto expr = std::make_shared<idyl::parser::flow_literal_expr>();
        expr->flow_ = flow_lit;
        expr->line_ = yystack_[2].location.begin.line;
        expr->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 1318 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 27: // flow_literal: generator_expression
#line 337 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                           { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1324 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 28: // flow_elements: flow_elements expression
#line 342 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = yystack_[1].value.as < std::vector<idyl::parser::expr_ptr> > ();
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::expr_ptr > ());
    }
#line 1333 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 29: // flow_elements: flow_elements LBRACKET NUMBER RBRACKET
#line 347 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = yystack_[3].value.as < std::vector<idyl::parser::expr_ptr> > ();
        auto rep = std::make_shared<idyl::parser::repetition_marker>();
        auto num = std::make_shared<idyl::parser::number_literal>();
        num->value_ = yystack_[1].value.as < std::string > ();
        num->line_ = yystack_[1].location.begin.line;
        num->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = num;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        rep->repetition_count_ = expr;
        rep->line_ = yystack_[2].location.begin.line;
        rep->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > ().push_back(expr);
    }
#line 1354 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 30: // flow_elements: flow_elements RESTART_MARKER
#line 364 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = yystack_[1].value.as < std::vector<idyl::parser::expr_ptr> > ();
        auto restart = std::make_shared<idyl::parser::repetition_marker>();
        restart->repetition_count_ = nullptr;
        restart->line_ = yystack_[0].location.begin.line;
        restart->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > ().push_back(nullptr); // Placeholder for restart
    }
#line 1367 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 31: // flow_elements: expression
#line 372 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                 { yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = {yystack_[0].value.as < idyl::parser::expr_ptr > ()}; }
#line 1373 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 32: // generator_expression: LBRACKET IDENTIFIER ASSIGN expression RANGE expression COLON expression RBRACKET
#line 377 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto gen = std::make_shared<idyl::parser::generator_expr>();
        gen->variable_ = yystack_[7].value.as < std::string > ();
        gen->range_start_ = yystack_[5].value.as < idyl::parser::expr_ptr > ();
        gen->range_end_ = yystack_[3].value.as < idyl::parser::expr_ptr > ();
        gen->body_ = yystack_[1].value.as < idyl::parser::expr_ptr > ();
        gen->line_ = yystack_[8].location.begin.line;
        gen->column_ = yystack_[8].location.begin.column;
        auto expr = std::make_shared<idyl::parser::generator_expr_node>();
        expr->generator_ = gen;
        expr->line_ = yystack_[8].location.begin.line;
        expr->column_ = yystack_[8].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 1392 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 33: // process_block: PROCESS COLON LBRACE process_body_statements RBRACE
#line 395 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto proc = std::make_shared<idyl::parser::process_block>();
        proc->name_ = "main";
        auto body = std::make_shared<idyl::parser::process_block_body>();
        body->statements_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        proc->body_ = body;
        proc->line_ = yystack_[4].location.begin.line;
        proc->column_ = yystack_[4].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::process_block> > () = proc;
    }
#line 1407 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 34: // process_block: PROCESS IDENTIFIER COMMA DT ASSIGN expression COLON LBRACE process_body_statements RBRACE
#line 406 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto proc = std::make_shared<idyl::parser::process_block>();
        proc->name_ = yystack_[8].value.as < std::string > ();
        proc->duration_ = yystack_[4].value.as < idyl::parser::expr_ptr > ();
        auto body = std::make_shared<idyl::parser::process_block_body>();
        body->statements_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        proc->body_ = body;
        proc->line_ = yystack_[9].location.begin.line;
        proc->column_ = yystack_[9].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::process_block> > () = proc;
    }
#line 1423 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 35: // process_block: PROCESS IDENTIFIER COLON LBRACE process_body_statements RBRACE
#line 418 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto proc = std::make_shared<idyl::parser::process_block>();
        proc->name_ = yystack_[4].value.as < std::string > ();
        auto body = std::make_shared<idyl::parser::process_block_body>();
        body->statements_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        proc->body_ = body;
        proc->line_ = yystack_[5].location.begin.line;
        proc->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::process_block> > () = proc;
    }
#line 1438 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 36: // process_body_statements: process_body_statements process_body_statement
#line 432 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1447 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 37: // process_body_statements: process_body_statement
#line 437 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = {};
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1456 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 38: // process_body_statement: IDENTIFIER ASSIGN expression
#line 445 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = yystack_[2].value.as < std::string > ();
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = false;
        assign->line_ = yystack_[2].location.begin.line;
        assign->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1470 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 39: // process_body_statement: EMIT IDENTIFIER ASSIGN expression
#line 455 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = yystack_[2].value.as < std::string > ();
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = true;
        assign->line_ = yystack_[3].location.begin.line;
        assign->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1484 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 40: // process_body_statement: expression catch_block
#line 465 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::catch_block> > ();
    }
#line 1492 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 41: // process_body_statement: expression
#line 469 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto es = std::make_shared<idyl::parser::expression_stmt>();
        es->expression_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        es->line_ = yystack_[0].location.begin.line;
        es->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = es;
    }
#line 1504 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 42: // catch_block: CATCH END COLON LBRACE process_body_statements RBRACE
#line 480 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto catch_b = std::make_shared<idyl::parser::catch_block>();
        catch_b->event_type_ = "end";
        catch_b->handler_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        catch_b->line_ = yystack_[5].location.begin.line;
        catch_b->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::catch_block> > () = catch_b;
    }
#line 1517 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 43: // lambda_block: LAMBDA_BLOCK LBRACE init_block lambda_statements RBRACE
#line 492 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->init_ = yystack_[2].value.as < std::shared_ptr<idyl::parser::init_block> > ();
        lambda->update_statements_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        lambda->line_ = yystack_[4].location.begin.line;
        lambda->column_ = yystack_[4].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::lambda_block> > () = lambda;
    }
#line 1530 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 44: // lambda_block: LAMBDA_BLOCK LBRACE init_block RBRACE
#line 501 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->init_ = yystack_[1].value.as < std::shared_ptr<idyl::parser::init_block> > ();
        lambda->update_statements_ = {};
        lambda->line_ = yystack_[3].location.begin.line;
        lambda->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::lambda_block> > () = lambda;
    }
#line 1543 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 45: // lambda_block: LAMBDA_BLOCK LBRACE lambda_statements RBRACE
#line 510 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->update_statements_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        lambda->line_ = yystack_[3].location.begin.line;
        lambda->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::lambda_block> > () = lambda;
    }
#line 1555 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 46: // init_block: INIT COLON LBRACE lambda_statements RBRACE
#line 521 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto init = std::make_shared<idyl::parser::init_block>();
        init->statements_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        init->line_ = yystack_[4].location.begin.line;
        init->column_ = yystack_[4].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::init_block> > () = init;
    }
#line 1567 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 47: // lambda_statements: lambda_statements lambda_statement
#line 532 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1576 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 48: // lambda_statements: lambda_statement
#line 537 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = {};
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1585 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 49: // lambda_statement: IDENTIFIER ASSIGN expression
#line 545 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = yystack_[2].value.as < std::string > ();
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = false;
        assign->line_ = yystack_[2].location.begin.line;
        assign->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1599 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 50: // lambda_statement: EMIT IDENTIFIER ASSIGN expression
#line 555 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = yystack_[2].value.as < std::string > ();
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = true;
        assign->line_ = yystack_[3].location.begin.line;
        assign->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1613 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 51: // lambda_statement: IDENTIFIER LPAREN parameter_list RPAREN ASSIGN expression
#line 565 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = yystack_[5].value.as < std::string > ();
        func->parameters_ = yystack_[3].value.as < std::vector<idyl::parser::param_ptr> > ();
        func->body_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        func->line_ = yystack_[5].location.begin.line;
        func->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = func;
    }
#line 1627 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 52: // lambda_statement: IDENTIFIER LPAREN RPAREN ASSIGN expression
#line 575 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = yystack_[4].value.as < std::string > ();
        func->parameters_ = {};
        func->body_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        func->line_ = yystack_[4].location.begin.line;
        func->column_ = yystack_[4].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = func;
    }
#line 1641 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 53: // parameter_list: parameter_list COMMA parameter
#line 588 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::param_ptr> > () = yystack_[2].value.as < std::vector<idyl::parser::param_ptr> > ();
        if (yystack_[0].value.as < idyl::parser::param_ptr > ()) yylhs.value.as < std::vector<idyl::parser::param_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::param_ptr > ());
    }
#line 1650 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 54: // parameter_list: parameter
#line 592 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                { yylhs.value.as < std::vector<idyl::parser::param_ptr> > () = {yystack_[0].value.as < idyl::parser::param_ptr > ()}; }
#line 1656 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 55: // parameter: IDENTIFIER
#line 597 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = yystack_[0].value.as < std::string > ();
        param->is_trigger_parameter_ = false;
        param->line_ = yystack_[0].location.begin.line;
        param->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 1669 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 56: // parameter: IDENTIFIER TRIGGER
#line 606 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = yystack_[1].value.as < std::string > ();
        param->is_trigger_parameter_ = true;
        param->line_ = yystack_[1].location.begin.line;
        param->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 1682 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 57: // parameter: IDENTIFIER ASSIGN expression
#line 615 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = yystack_[2].value.as < std::string > ();
        param->default_value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        param->line_ = yystack_[2].location.begin.line;
        param->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 1695 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 58: // parameter: DT ASSIGN expression
#line 624 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = "dt";
        param->default_value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        param->has_default_time_ = true;
        param->line_ = yystack_[2].location.begin.line;
        param->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 1709 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 59: // expression: assignment_expression
#line 636 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1715 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 60: // assignment_expression: ternary_expression
#line 640 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1721 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 61: // ternary_expression: logical_or_expression
#line 644 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1727 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 62: // ternary_expression: ternary_options QUESTION logical_or_expression
#line 646 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto ternary = std::make_shared<idyl::parser::ternary_op>();
        ternary->options_ = yystack_[2].value.as < std::vector<idyl::parser::expr_ptr> > ();
        ternary->condition_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        ternary->line_ = yystack_[1].location.begin.line;
        ternary->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::ternary_op_expr>();
        expr->op_ = ternary;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 1744 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 63: // ternary_options: ternary_options SEMICOLON logical_or_expression
#line 662 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = yystack_[2].value.as < std::vector<idyl::parser::expr_ptr> > ();
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::expr_ptr > ());
    }
#line 1753 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 64: // ternary_options: logical_or_expression
#line 667 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = {yystack_[0].value.as < idyl::parser::expr_ptr > ()};
    }
#line 1761 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 65: // logical_or_expression: logical_and_expression
#line 673 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1767 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 66: // logical_or_expression: logical_or_expression OR logical_and_expression
#line 675 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "|";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 1785 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 67: // logical_and_expression: bitwise_or_expression
#line 691 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1791 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 68: // logical_and_expression: logical_and_expression AND bitwise_or_expression
#line 693 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "&";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 1809 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 69: // bitwise_or_expression: bitwise_xor_expression
#line 709 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1815 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 70: // bitwise_or_expression: bitwise_or_expression OR bitwise_xor_expression
#line 711 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "|";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 1833 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 71: // bitwise_xor_expression: bitwise_and_expression
#line 727 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1839 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 72: // bitwise_xor_expression: bitwise_xor_expression XOR bitwise_and_expression
#line 729 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "^";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 1857 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 73: // bitwise_and_expression: equality_expression
#line 745 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                          { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1863 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 74: // bitwise_and_expression: bitwise_and_expression AND equality_expression
#line 747 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "&";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 1881 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 75: // equality_expression: relational_expression
#line 763 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1887 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 76: // equality_expression: equality_expression EQ relational_expression
#line 765 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "==";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 1905 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 77: // equality_expression: equality_expression NEQ relational_expression
#line 779 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "!=";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 1923 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 78: // relational_expression: shift_expression
#line 795 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                       { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1929 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 79: // relational_expression: relational_expression LT shift_expression
#line 797 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "<";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 1947 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 80: // relational_expression: relational_expression GT shift_expression
#line 811 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = ">";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 1965 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 81: // relational_expression: relational_expression LE shift_expression
#line 825 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "<=";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 1983 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 82: // relational_expression: relational_expression GE shift_expression
#line 839 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = ">=";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2001 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 83: // shift_expression: additive_expression
#line 855 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                          { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2007 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 84: // shift_expression: shift_expression LSHIFT additive_expression
#line 857 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "<<";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2025 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 85: // shift_expression: shift_expression RSHIFT additive_expression
#line 871 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = ">>";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2043 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 86: // additive_expression: multiplicative_expression
#line 887 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2049 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 87: // additive_expression: additive_expression PLUS multiplicative_expression
#line 889 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "+";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2067 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 88: // additive_expression: additive_expression MINUS multiplicative_expression
#line 903 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "-";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2085 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 89: // multiplicative_expression: unary_expression
#line 919 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                       { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2091 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 90: // multiplicative_expression: multiplicative_expression MUL unary_expression
#line 921 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "*";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2109 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 91: // multiplicative_expression: multiplicative_expression DIV unary_expression
#line 935 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "/";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2127 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 92: // multiplicative_expression: multiplicative_expression MOD unary_expression
#line 949 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "%";
        binop->left_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        binop->right_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        binop->line_ = yystack_[1].location.begin.line;
        binop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2145 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 93: // unary_expression: postfix_expression
#line 965 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2151 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 94: // unary_expression: NOT unary_expression
#line 967 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto unop = std::make_shared<idyl::parser::unary_op>();
        unop->op_ = "~";
        unop->operand_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        unop->line_ = yystack_[1].location.begin.line;
        unop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::unary_op_expr>();
        expr->op_ = unop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2168 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 95: // unary_expression: MINUS unary_expression
#line 980 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto unop = std::make_shared<idyl::parser::unary_op>();
        unop->op_ = "-";
        unop->operand_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        unop->line_ = yystack_[1].location.begin.line;
        unop->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::unary_op_expr>();
        expr->op_ = unop;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2185 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 96: // unary_expression: MEMORY_OP LPAREN expression RPAREN
#line 993 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto memop = std::make_shared<idyl::parser::memory_op>();
        memop->expr_ = yystack_[1].value.as < idyl::parser::expr_ptr > ();
        memop->line_ = yystack_[3].location.begin.line;
        memop->column_ = yystack_[3].location.begin.column;
        auto expr = std::make_shared<idyl::parser::memory_op_expr>();
        expr->op_ = memop;
        expr->line_ = yystack_[3].location.begin.line;
        expr->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2201 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 97: // unary_expression: MEMORY_OP LPAREN expression COMMA expression RPAREN
#line 1005 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto memop = std::make_shared<idyl::parser::memory_op>();
        memop->expr_ = yystack_[3].value.as < idyl::parser::expr_ptr > ();
        memop->delay_count_ = yystack_[1].value.as < idyl::parser::expr_ptr > ();
        memop->line_ = yystack_[5].location.begin.line;
        memop->column_ = yystack_[5].location.begin.column;
        auto expr = std::make_shared<idyl::parser::memory_op_expr>();
        expr->op_ = memop;
        expr->line_ = yystack_[5].location.begin.line;
        expr->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2218 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 98: // postfix_expression: primary_expression
#line 1020 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2224 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 99: // postfix_expression: postfix_expression LPAREN argument_list RPAREN
#line 1022 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto call = std::make_shared<idyl::parser::function_call>();
        call->function_ = yystack_[3].value.as < idyl::parser::expr_ptr > ();
        call->arguments_ = yystack_[1].value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        call->line_ = yystack_[2].location.begin.line;
        call->column_ = yystack_[2].location.begin.column;
        auto expr = std::make_shared<idyl::parser::function_call_expr>();
        expr->call_ = call;
        expr->line_ = yystack_[2].location.begin.line;
        expr->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2241 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 100: // postfix_expression: postfix_expression LPAREN RPAREN
#line 1035 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto call = std::make_shared<idyl::parser::function_call>();
        call->function_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        call->arguments_ = {};
        call->line_ = yystack_[1].location.begin.line;
        call->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::function_call_expr>();
        expr->call_ = call;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2258 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 101: // postfix_expression: postfix_expression DOT IDENTIFIER
#line 1048 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto access = std::make_shared<idyl::parser::flow_access>();
        access->flow_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        access->member_ = yystack_[0].value.as < std::string > ();
        access->line_ = yystack_[1].location.begin.line;
        access->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::flow_access_expr>();
        expr->access_ = access;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2275 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 102: // postfix_expression: postfix_expression NAMESPACE_DOT IDENTIFIER
#line 1061 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto access = std::make_shared<idyl::parser::module_access>();
        access->module_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        access->function_ = yystack_[0].value.as < std::string > ();
        access->arguments_ = {};
        access->line_ = yystack_[1].location.begin.line;
        access->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::module_access_expr>();
        expr->access_ = access;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2293 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 103: // postfix_expression: postfix_expression NAMESPACE_DOT IDENTIFIER LPAREN argument_list RPAREN
#line 1075 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto access = std::make_shared<idyl::parser::module_access>();
        access->module_ = yystack_[5].value.as < idyl::parser::expr_ptr > ();
        access->function_ = yystack_[3].value.as < std::string > ();
        access->arguments_ = yystack_[1].value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        access->line_ = yystack_[4].location.begin.line;
        access->column_ = yystack_[4].location.begin.column;
        auto expr = std::make_shared<idyl::parser::module_access_expr>();
        expr->access_ = access;
        expr->line_ = yystack_[4].location.begin.line;
        expr->column_ = yystack_[4].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2311 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 104: // postfix_expression: postfix_expression NAMESPACE_DOT IDENTIFIER LPAREN RPAREN
#line 1089 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto access = std::make_shared<idyl::parser::module_access>();
        access->module_ = yystack_[4].value.as < idyl::parser::expr_ptr > ();
        access->function_ = yystack_[2].value.as < std::string > ();
        access->arguments_ = {};
        access->line_ = yystack_[3].location.begin.line;
        access->column_ = yystack_[3].location.begin.column;
        auto expr = std::make_shared<idyl::parser::module_access_expr>();
        expr->access_ = access;
        expr->line_ = yystack_[3].location.begin.line;
        expr->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2329 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 105: // postfix_expression: postfix_expression LBRACKET expression RBRACKET
#line 1103 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto access = std::make_shared<idyl::parser::flow_access>();
        access->flow_ = yystack_[3].value.as < idyl::parser::expr_ptr > ();
        access->index_ = yystack_[1].value.as < idyl::parser::expr_ptr > ();
        access->line_ = yystack_[2].location.begin.line;
        access->column_ = yystack_[2].location.begin.column;
        auto expr = std::make_shared<idyl::parser::flow_access_expr>();
        expr->access_ = access;
        expr->line_ = yystack_[2].location.begin.line;
        expr->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2346 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 106: // primary_expression: IDENTIFIER
#line 1119 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto id = std::make_shared<idyl::parser::identifier>();
        id->name_ = yystack_[0].value.as < std::string > ();
        id->line_ = yystack_[0].location.begin.line;
        id->column_ = yystack_[0].location.begin.column;
        auto expr = std::make_shared<idyl::parser::identifier_expr>();
        expr->identifier_ = id;
        expr->line_ = yystack_[0].location.begin.line;
        expr->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2362 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 107: // primary_expression: NUMBER
#line 1131 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto num = std::make_shared<idyl::parser::number_literal>();
        num->value_ = yystack_[0].value.as < std::string > ();
        num->line_ = yystack_[0].location.begin.line;
        num->column_ = yystack_[0].location.begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = num;
        expr->line_ = yystack_[0].location.begin.line;
        expr->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2378 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 108: // primary_expression: TIME_LITERAL
#line 1143 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto time = std::make_shared<idyl::parser::time_literal>();
        // Parse time literal
        std::string str = yystack_[0].value.as < std::string > ();
        if (str.find("ms") != std::string::npos) {
            time->value_ = str.substr(0, str.length() - 2);
            time->unit_ = "ms";
        } else if (str.find("hz") != std::string::npos) {
            time->value_ = str.substr(0, str.length() - 2);
            time->unit_ = "hz";
        } else if (str.find("b") != std::string::npos) {
            time->value_ = str.substr(0, str.length() - 1);
            time->unit_ = "b";
        } else if (str.find("s") != std::string::npos) {
            time->value_ = str.substr(0, str.length() - 1);
            time->unit_ = "s";
        }
        time->line_ = yystack_[0].location.begin.line;
        time->column_ = yystack_[0].location.begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = time;
        expr->line_ = yystack_[0].location.begin.line;
        expr->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2408 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 109: // primary_expression: TRIGGER
#line 1169 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto trigger = std::make_shared<idyl::parser::trigger_literal>();
        trigger->line_ = yystack_[0].location.begin.line;
        trigger->column_ = yystack_[0].location.begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = trigger;
        expr->line_ = yystack_[0].location.begin.line;
        expr->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2423 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 110: // primary_expression: REST
#line 1180 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto rest = std::make_shared<idyl::parser::rest_literal>();
        rest->line_ = yystack_[0].location.begin.line;
        rest->column_ = yystack_[0].location.begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = rest;
        expr->line_ = yystack_[0].location.begin.line;
        expr->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2438 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 111: // primary_expression: DT
#line 1191 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto id = std::make_shared<idyl::parser::identifier>();
        id->name_ = "dt";
        id->line_ = yystack_[0].location.begin.line;
        id->column_ = yystack_[0].location.begin.column;
        auto expr = std::make_shared<idyl::parser::identifier_expr>();
        expr->identifier_ = id;
        expr->line_ = yystack_[0].location.begin.line;
        expr->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2454 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 112: // primary_expression: STRING_LITERAL
#line 1203 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto str = std::make_shared<idyl::parser::string_literal>();
        str->value_ = yystack_[0].value.as < std::string > ();
        str->line_ = yystack_[0].location.begin.line;
        str->column_ = yystack_[0].location.begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = str;
        expr->line_ = yystack_[0].location.begin.line;
        expr->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2470 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 113: // primary_expression: LPAREN expression RPAREN
#line 1215 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto paren = std::make_shared<idyl::parser::parenthesized_expr>();
        paren->expr_ = yystack_[1].value.as < idyl::parser::expr_ptr > ();
        paren->line_ = yystack_[2].location.begin.line;
        paren->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = paren;
    }
#line 2482 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 114: // primary_expression: flow_literal
#line 1222 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                   { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2488 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 115: // primary_expression: generator_expression
#line 1223 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                           { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2494 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 116: // argument_list: argument_list COMMA argument
#line 1228 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > () = yystack_[2].value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        if (yystack_[0].value.as < std::shared_ptr<idyl::parser::argument> > ()) yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > ().push_back(yystack_[0].value.as < std::shared_ptr<idyl::parser::argument> > ());
    }
#line 2503 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 117: // argument_list: argument
#line 1232 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
               { yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > () = {yystack_[0].value.as < std::shared_ptr<idyl::parser::argument> > ()}; }
#line 2509 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 118: // argument: expression
#line 1237 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = "";
        arg->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        arg->line_ = yystack_[0].location.begin.line;
        arg->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::argument> > () = arg;
    }
#line 2522 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 119: // argument: IDENTIFIER ASSIGN expression
#line 1246 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = yystack_[2].value.as < std::string > ();
        arg->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        arg->line_ = yystack_[2].location.begin.line;
        arg->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::argument> > () = arg;
    }
#line 2535 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 120: // argument: DT ASSIGN expression
#line 1255 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = "dt";
        arg->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        arg->line_ = yystack_[2].location.begin.line;
        arg->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::argument> > () = arg;
    }
#line 2548 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;


#line 2552 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
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



  // parser::context.
  parser::context::context (const parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
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
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
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


  const short parser::yypact_ninf_ = -179;

  const signed char parser::yytable_ninf_ = -65;

  const short
  parser::yypact_[] =
  {
     100,     4,    24,    27,    40,    65,    45,   100,  -179,  -179,
    -179,  -179,  -179,   274,    10,    35,    88,    71,   130,   149,
    -179,  -179,  -179,  -179,  -179,  -179,   117,   133,  -179,   140,
    -179,   383,   383,  -179,   383,   388,  -179,  -179,   172,  -179,
    -179,    99,    62,   159,   160,   163,   166,    78,   114,   124,
     145,   134,  -179,    -7,  -179,    50,   161,   162,    -1,  -179,
       5,    13,   150,   188,   302,   158,   165,   200,   201,   383,
    -179,  -179,   171,   175,    79,  -179,   173,  -179,   383,   383,
     383,   383,   383,   383,   383,   383,   383,   383,   383,   383,
     383,   383,   383,   383,   383,   383,   383,   383,   218,   222,
     330,   383,   383,  -179,   383,   383,    13,   187,   228,  -179,
    -179,    70,   302,   192,   193,   231,    28,  -179,   223,  -179,
    -179,   190,   191,    73,  -179,   383,  -179,   434,  -179,  -179,
      66,   206,   206,   159,  -179,   163,  -179,    78,   114,   114,
     124,   124,   124,   124,   213,   213,   134,   134,  -179,  -179,
    -179,   196,  -179,   207,   208,  -179,  -179,    75,  -179,   209,
    -179,  -179,   172,  -179,   383,   216,     9,   214,   168,   383,
     383,   220,  -179,  -179,   235,  -179,  -179,  -179,   383,  -179,
     242,   215,    53,   224,   263,     8,    11,  -179,   337,   383,
     383,   439,  -179,  -179,  -179,   172,   219,   227,  -179,   104,
    -179,   229,  -179,   383,   230,   225,   383,  -179,   383,    33,
     232,   233,  -179,    12,  -179,  -179,  -179,    76,  -179,  -179,
    -179,  -179,  -179,   219,   228,  -179,   237,  -179,   244,  -179,
     234,  -179,   241,    83,    26,   383,  -179,  -179,  -179,    15,
     302,   302,   383,   383,   250,    14,  -179,  -179,   205,   240,
     243,  -179,   383,  -179,  -179,  -179,  -179,  -179
  };

  const signed char
  parser::yydefact_[] =
  {
       3,     0,     0,     0,     0,     0,     0,     2,     5,     6,
      12,    13,     7,     0,     0,     0,     0,     0,     0,     0,
       1,     4,   106,   107,   108,   112,     0,     0,   111,     0,
     110,     0,     0,   109,     0,     0,   114,    27,    18,    59,
      60,     0,    61,    65,    67,    69,    71,    73,    75,    78,
      83,    86,    89,    93,    98,    55,     0,     0,     0,    54,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      95,    94,     0,   106,     0,    31,     0,    19,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    56,     0,     0,     0,     0,     0,    20,
      27,     0,     0,     0,   106,     0,     0,    37,    41,     8,
      10,     0,     0,     0,   113,     0,    30,     0,    26,    28,
       0,    62,    63,    66,    68,    70,    72,    74,    76,    77,
      79,    80,    81,    82,    84,    85,    87,    88,    90,    91,
      92,   102,   101,   106,   111,   100,   118,     0,   117,     0,
      57,    58,    15,    53,     0,     0,     0,     0,     0,     0,
       0,     0,    33,    36,     0,    40,     9,    11,     0,    96,
       0,   107,     0,     0,     0,     0,     0,    48,     0,     0,
       0,     0,    99,   105,    17,    14,     0,     0,    22,     0,
      35,     0,    38,     0,     0,     0,     0,    29,     0,     0,
       0,     0,    44,     0,    45,    47,   104,     0,   119,   120,
     116,    16,    25,     0,     0,    21,     0,    39,     0,    97,
       0,    49,     0,     0,     0,     0,    43,   103,    24,     0,
       0,     0,     0,     0,     0,     0,    50,    23,     0,     0,
       0,    52,     0,    46,    34,    42,    32,    51
  };

  const short
  parser::yypgoto_[] =
  {
    -179,  -179,  -179,   262,  -179,  -179,  -179,    72,   -59,  -179,
     -57,  -179,  -108,  -114,  -179,  -143,  -179,  -176,  -178,   -56,
     194,   -13,  -179,  -179,  -179,    91,   221,   217,   236,   226,
     245,    96,    60,    93,   103,   -25,  -179,  -179,    86,   108
  };

  const unsigned char
  parser::yydefgoto_[] =
  {
       0,     6,     7,     8,     9,    10,    11,   166,    36,    74,
      37,    12,   116,   117,   175,    77,   185,   186,   187,    58,
      59,   118,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,   157,   158
  };

  const short
  parser::yytable_[] =
  {
      38,   109,   173,   110,   168,   111,    70,    71,   215,   213,
      98,   182,   197,    55,   182,   182,    55,   182,   197,   194,
     184,    72,    75,   184,   184,    56,   184,    15,    56,   182,
      16,   114,    23,    24,    25,   215,    55,    99,   184,   100,
     115,   101,   106,    28,    13,    20,   107,    29,    56,    30,
      14,    31,   221,    35,   173,   108,   123,    57,   245,   212,
     198,   129,   214,   236,    32,   253,   247,   215,    17,   182,
     148,   149,   150,    33,    34,    60,    35,   183,   184,   172,
     232,    61,    22,    23,    24,    25,    18,   156,   159,   160,
     102,   161,   162,   208,    28,   103,    80,   126,    29,   209,
      30,   -64,    31,     1,   -64,    85,    86,     2,     3,     4,
       5,    19,   180,   106,    75,    32,   178,   167,   191,   191,
     179,    64,   192,   237,    33,    34,   106,   127,   128,    62,
     244,    63,   248,   249,   173,   173,    65,   222,    78,   110,
     225,    79,   110,    87,    88,    89,    90,   140,   141,   142,
     143,   195,    35,   233,   224,    66,   201,   202,    95,    96,
      97,    91,    92,    67,   238,   205,   110,    93,    94,   131,
     132,   114,    23,    24,    25,   156,   218,   219,   156,    68,
     115,   138,   139,    28,   144,   145,    69,    29,    76,    30,
     227,    31,    81,   230,    82,   231,   146,   147,    83,    84,
     112,   104,   105,   113,    32,   119,   121,   122,   114,    23,
      24,    25,   120,    33,    34,   125,    35,   115,   124,   200,
      28,   151,   246,   130,    29,   152,    30,   164,    31,   250,
     251,   165,   169,   170,   171,    93,   174,   176,   177,   257,
      80,    32,   188,   114,    23,    24,    25,   189,   190,   204,
      33,    34,   115,    35,   199,    28,   254,   196,   193,    29,
     203,    30,   206,    31,   207,   210,   211,    35,   223,    21,
     226,   228,   229,   235,   217,   242,    32,    22,    23,    24,
      25,   243,   234,    26,    27,    33,    34,   240,    35,    28,
     252,   255,   256,    29,   241,    30,   239,    31,   134,   220,
     163,   133,     0,     0,     0,   114,    23,    24,    25,   136,
      32,     0,     0,     0,   115,     0,     0,    28,   135,    33,
      34,    29,    35,    30,     0,    31,     0,     0,     0,   137,
       0,     0,     0,   153,    23,    24,    25,     0,    32,     0,
     153,    23,    24,    25,     0,   154,     0,    33,    34,    29,
      35,    30,   154,    31,     0,     0,    29,     0,    30,     0,
      31,     0,     0,     0,     0,     0,    32,     0,     0,     0,
       0,     0,     0,    32,     0,    33,    34,   155,    35,     0,
       0,     0,    33,    34,   216,    35,    22,    23,    24,    25,
       0,    73,    23,    24,    25,     0,     0,     0,    28,     0,
       0,     0,    29,    28,    30,     0,    31,    29,     0,    30,
       0,    31,     0,     0,     0,     0,     0,     0,     0,    32,
       0,     0,     0,     0,    32,     0,     0,     0,    33,    34,
       0,    35,     0,    33,    34,     0,    35,    73,   181,    24,
      25,     0,   153,    23,    24,    25,     0,     0,     0,    28,
       0,     0,     0,    29,   154,    30,     0,    31,    29,     0,
      30,     0,    31,     0,     0,     0,     0,     0,     0,     0,
      32,     0,     0,     0,     0,    32,     0,     0,     0,    33,
      34,     0,    35,     0,    33,    34,     0,    35
  };

  const short
  parser::yycheck_[] =
  {
      13,    60,   116,    60,   112,    61,    31,    32,   186,   185,
      17,     3,     3,     3,     3,     3,     3,     3,     3,   162,
      12,    34,    35,    12,    12,    15,    12,     3,    15,     3,
       3,     3,     4,     5,     6,   213,     3,    44,    12,    46,
      12,    48,    43,    15,    40,     0,    47,    19,    15,    21,
      46,    23,   195,    48,   168,    50,    69,    47,   234,    51,
      51,    74,    51,    51,    36,    51,    51,   245,    41,     3,
      95,    96,    97,    45,    46,    40,    48,    11,    12,    51,
      47,    46,     3,     4,     5,     6,    46,   100,   101,   102,
      40,   104,   105,    40,    15,    45,    34,    18,    19,    46,
      21,    39,    23,     3,    42,    27,    28,     7,     8,     9,
      10,    46,   125,    43,   127,    36,    43,    47,    43,    43,
      47,    50,    47,    47,    45,    46,    43,    48,    49,    41,
      47,    43,   240,   241,   248,   249,     6,   196,    39,   196,
     199,    42,   199,    29,    30,    31,    32,    87,    88,    89,
      90,   164,    48,   209,    50,     6,   169,   170,    24,    25,
      26,    37,    38,    46,   223,   178,   223,    22,    23,    78,
      79,     3,     4,     5,     6,   188,   189,   190,   191,    46,
      12,    85,    86,    15,    91,    92,    46,    19,    16,    21,
     203,    23,    33,   206,    34,   208,    93,    94,    35,    33,
      50,    40,    40,    15,    36,    47,     6,     6,     3,     4,
       5,     6,    47,    45,    46,    40,    48,    12,    47,    51,
      15,     3,   235,    50,    19,     3,    21,    40,    23,   242,
     243,     3,    40,    40,     3,    22,    13,    47,    47,   252,
      34,    36,    46,     3,     4,     5,     6,    40,    40,    14,
      45,    46,    12,    48,    40,    15,    51,    41,    49,    19,
      40,    21,    20,    23,    49,    41,     3,    48,    41,     7,
      41,    41,    47,    40,   188,    41,    36,     3,     4,     5,
       6,    40,    50,     9,    10,    45,    46,    50,    48,    15,
      40,    51,    49,    19,    50,    21,   224,    23,    81,   191,
     106,    80,    -1,    -1,    -1,     3,     4,     5,     6,    83,
      36,    -1,    -1,    -1,    12,    -1,    -1,    15,    82,    45,
      46,    19,    48,    21,    -1,    23,    -1,    -1,    -1,    84,
      -1,    -1,    -1,     3,     4,     5,     6,    -1,    36,    -1,
       3,     4,     5,     6,    -1,    15,    -1,    45,    46,    19,
      48,    21,    15,    23,    -1,    -1,    19,    -1,    21,    -1,
      23,    -1,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,
      -1,    -1,    -1,    36,    -1,    45,    46,    47,    48,    -1,
      -1,    -1,    45,    46,    47,    48,     3,     4,     5,     6,
      -1,     3,     4,     5,     6,    -1,    -1,    -1,    15,    -1,
      -1,    -1,    19,    15,    21,    -1,    23,    19,    -1,    21,
      -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,
      -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    45,    46,
      -1,    48,    -1,    45,    46,    -1,    48,     3,     4,     5,
       6,    -1,     3,     4,     5,     6,    -1,    -1,    -1,    15,
      -1,    -1,    -1,    19,    15,    21,    -1,    23,    19,    -1,
      21,    -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      36,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    45,
      46,    -1,    48,    -1,    45,    46,    -1,    48
  };

  const signed char
  parser::yystos_[] =
  {
       0,     3,     7,     8,     9,    10,    53,    54,    55,    56,
      57,    58,    63,    40,    46,     3,     3,    41,    46,    46,
       0,    55,     3,     4,     5,     6,     9,    10,    15,    19,
      21,    23,    36,    45,    46,    48,    60,    62,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,     3,    15,    47,    71,    72,
      40,    46,    41,    43,    50,     6,     6,    46,    46,    46,
      87,    87,    73,     3,    61,    73,    16,    67,    39,    42,
      34,    33,    34,    35,    33,    27,    28,    29,    30,    31,
      32,    37,    38,    22,    23,    24,    25,    26,    17,    44,
      46,    48,    40,    45,    40,    40,    43,    47,    50,    60,
      62,    71,    50,    15,     3,    12,    64,    65,    73,    47,
      47,     6,     6,    73,    47,    40,    18,    48,    49,    73,
      50,    77,    77,    78,    79,    80,    81,    82,    83,    83,
      84,    84,    84,    84,    85,    85,    86,    86,    87,    87,
      87,     3,     3,     3,    15,    47,    73,    90,    91,    73,
      73,    73,    73,    72,    40,     3,    59,    47,    64,    40,
      40,     3,    51,    65,    13,    66,    47,    47,    43,    47,
      73,     4,     3,    11,    12,    68,    69,    70,    46,    40,
      40,    43,    47,    49,    67,    73,    41,     3,    51,    40,
      51,    73,    73,    40,    14,    73,    20,    49,    40,    46,
      41,     3,    51,    69,    51,    70,    47,    90,    73,    73,
      91,    67,    60,    41,    50,    60,    41,    73,    41,    47,
      73,    73,    47,    71,    50,    40,    51,    47,    60,    59,
      50,    50,    41,    40,    47,    69,    73,    51,    64,    64,
      73,    73,    40,    51,    51,    51,    49,    73
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    52,    53,    53,    54,    54,    55,    55,    55,    55,
      55,    55,    56,    56,    57,    57,    57,    57,    57,    57,
      58,    58,    58,    58,    59,    59,    60,    60,    61,    61,
      61,    61,    62,    63,    63,    63,    64,    64,    65,    65,
      65,    65,    66,    67,    67,    67,    68,    69,    69,    70,
      70,    70,    70,    71,    71,    72,    72,    72,    72,    73,
      74,    75,    75,    76,    76,    77,    77,    78,    78,    79,
      79,    80,    80,    81,    81,    82,    82,    82,    83,    83,
      83,    83,    83,    84,    84,    84,    85,    85,    85,    86,
      86,    86,    86,    87,    87,    87,    87,    87,    88,    88,
      88,    88,    88,    88,    88,    88,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    90,    90,    91,    91,
      91
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     1,     0,     2,     1,     1,     1,     4,     6,
       4,     6,     1,     1,     6,     5,     7,     6,     3,     4,
       4,     7,     6,     9,     4,     3,     3,     1,     2,     4,
       2,     1,     9,     5,    10,     6,     2,     1,     3,     4,
       2,     1,     6,     5,     4,     4,     5,     2,     1,     3,
       4,     6,     5,     3,     1,     1,     2,     3,     3,     1,
       1,     1,     3,     3,     1,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     3,     1,     3,
       3,     3,     3,     1,     3,     3,     1,     3,     3,     1,
       3,     3,     3,     1,     2,     2,     4,     6,     1,     4,
       3,     3,     3,     6,     5,     4,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     1,     3,     1,     1,     3,
       3
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "YYEOF", "error", "\"invalid token\"", "IDENTIFIER", "NUMBER",
  "TIME_LITERAL", "STRING_LITERAL", "FLOW", "PROCESS", "LIB", "MODULE",
  "INIT", "EMIT", "CATCH", "END", "DT", "LAMBDA_BLOCK", "NAMESPACE_DOT",
  "RESTART_MARKER", "MEMORY_OP", "RANGE", "REST", "PLUS", "MINUS", "MUL",
  "DIV", "MOD", "EQ", "NEQ", "LT", "GT", "LE", "GE", "AND", "OR", "XOR",
  "NOT", "LSHIFT", "RSHIFT", "QUESTION", "ASSIGN", "COLON", "SEMICOLON",
  "COMMA", "DOT", "TRIGGER", "LPAREN", "RPAREN", "LBRACKET", "RBRACKET",
  "LBRACE", "RBRACE", "$accept", "program", "top_level_statements",
  "top_level_statement", "function_or_flow_definition",
  "function_definition", "flow_definition", "flow_members", "flow_literal",
  "flow_elements", "generator_expression", "process_block",
  "process_body_statements", "process_body_statement", "catch_block",
  "lambda_block", "init_block", "lambda_statements", "lambda_statement",
  "parameter_list", "parameter", "expression", "assignment_expression",
  "ternary_expression", "ternary_options", "logical_or_expression",
  "logical_and_expression", "bitwise_or_expression",
  "bitwise_xor_expression", "bitwise_and_expression",
  "equality_expression", "relational_expression", "shift_expression",
  "additive_expression", "multiplicative_expression", "unary_expression",
  "postfix_expression", "primary_expression", "argument_list", "argument", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   110,   110,   117,   126,   131,   139,   140,   141,   149,
     158,   166,   178,   179,   183,   193,   203,   214,   225,   235,
     249,   264,   279,   289,   302,   312,   325,   337,   341,   346,
     363,   372,   376,   394,   405,   417,   431,   436,   444,   454,
     464,   468,   479,   491,   500,   509,   520,   531,   536,   544,
     554,   564,   574,   587,   592,   596,   605,   614,   623,   636,
     640,   644,   645,   661,   666,   673,   674,   691,   692,   709,
     710,   727,   728,   745,   746,   763,   764,   778,   795,   796,
     810,   824,   838,   855,   856,   870,   887,   888,   902,   919,
     920,   934,   948,   965,   966,   979,   992,  1004,  1020,  1021,
    1034,  1047,  1060,  1074,  1088,  1102,  1118,  1130,  1142,  1168,
    1179,  1190,  1202,  1214,  1222,  1223,  1227,  1232,  1236,  1245,
    1254
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


} // yy
#line 3242 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"

#line 1265 "/home/johann/Documents/git/idyl/src/parser/idyl.y"


void yy::parser::error(const location_type& l, const std::string& m) {
    std::cerr << l << ": " << m << std::endl;
}
