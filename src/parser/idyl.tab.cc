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

      case symbol_kind::S_at_block: // at_block
        value.YY_MOVE_OR_COPY< std::shared_ptr<idyl::parser::at_block> > (YY_MOVE (that.value));
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

      case symbol_kind::S_on_block: // on_block
        value.YY_MOVE_OR_COPY< std::shared_ptr<idyl::parser::on_block> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_process_block: // process_block
        value.YY_MOVE_OR_COPY< std::shared_ptr<idyl::parser::process_block> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_start_statement: // start_statement
        value.YY_MOVE_OR_COPY< std::shared_ptr<idyl::parser::start_statement> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        value.YY_MOVE_OR_COPY< std::shared_ptr<idyl::parser::stop_statement> > (YY_MOVE (that.value));
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

      case symbol_kind::S_at_block: // at_block
        value.move< std::shared_ptr<idyl::parser::at_block> > (YY_MOVE (that.value));
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

      case symbol_kind::S_on_block: // on_block
        value.move< std::shared_ptr<idyl::parser::on_block> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_process_block: // process_block
        value.move< std::shared_ptr<idyl::parser::process_block> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_start_statement: // start_statement
        value.move< std::shared_ptr<idyl::parser::start_statement> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        value.move< std::shared_ptr<idyl::parser::stop_statement> > (YY_MOVE (that.value));
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

      case symbol_kind::S_at_block: // at_block
        value.copy< std::shared_ptr<idyl::parser::at_block> > (that.value);
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

      case symbol_kind::S_on_block: // on_block
        value.copy< std::shared_ptr<idyl::parser::on_block> > (that.value);
        break;

      case symbol_kind::S_process_block: // process_block
        value.copy< std::shared_ptr<idyl::parser::process_block> > (that.value);
        break;

      case symbol_kind::S_start_statement: // start_statement
        value.copy< std::shared_ptr<idyl::parser::start_statement> > (that.value);
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        value.copy< std::shared_ptr<idyl::parser::stop_statement> > (that.value);
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

      case symbol_kind::S_at_block: // at_block
        value.move< std::shared_ptr<idyl::parser::at_block> > (that.value);
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

      case symbol_kind::S_on_block: // on_block
        value.move< std::shared_ptr<idyl::parser::on_block> > (that.value);
        break;

      case symbol_kind::S_process_block: // process_block
        value.move< std::shared_ptr<idyl::parser::process_block> > (that.value);
        break;

      case symbol_kind::S_start_statement: // start_statement
        value.move< std::shared_ptr<idyl::parser::start_statement> > (that.value);
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        value.move< std::shared_ptr<idyl::parser::stop_statement> > (that.value);
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

      case symbol_kind::S_at_block: // at_block
        yylhs.value.emplace< std::shared_ptr<idyl::parser::at_block> > ();
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

      case symbol_kind::S_on_block: // on_block
        yylhs.value.emplace< std::shared_ptr<idyl::parser::on_block> > ();
        break;

      case symbol_kind::S_process_block: // process_block
        yylhs.value.emplace< std::shared_ptr<idyl::parser::process_block> > ();
        break;

      case symbol_kind::S_start_statement: // start_statement
        yylhs.value.emplace< std::shared_ptr<idyl::parser::start_statement> > ();
        break;

      case symbol_kind::S_stop_statement: // stop_statement
        yylhs.value.emplace< std::shared_ptr<idyl::parser::stop_statement> > ();
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
#line 114 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto prog = std::make_shared<idyl::parser::program>();
        prog->statements_ = yystack_[0].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        yylhs.value.as < idyl::parser::node_ptr > () = prog;
        g_program = prog;
    }
#line 1099 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 3: // program: %empty
#line 121 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto prog = std::make_shared<idyl::parser::program>();
        yylhs.value.as < idyl::parser::node_ptr > () = prog;
        g_program = prog;
    }
#line 1109 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 4: // top_level_statements: top_level_statements top_level_statement
#line 130 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1118 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 5: // top_level_statements: top_level_statement
#line 135 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = {};
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1127 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 6: // top_level_statement: function_or_flow_definition
#line 142 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                  { yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < idyl::parser::stmt_ptr > (); }
#line 1133 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 7: // top_level_statement: process_block
#line 143 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                    { yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::process_block> > (); }
#line 1139 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 8: // top_level_statement: IMPORT LPAREN STRING_LITERAL RPAREN
#line 145 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lib_import = std::make_shared<idyl::parser::library_import>();
        lib_import->path_ = yystack_[1].value.as < std::string > ();
        lib_import->line_ = yystack_[3].location.begin.line;
        lib_import->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = lib_import;
    }
#line 1151 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 9: // top_level_statement: IDENTIFIER ASSIGN IMPORT LPAREN STRING_LITERAL RPAREN
#line 153 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lib_import = std::make_shared<idyl::parser::library_import>();
        lib_import->namespace_ = yystack_[5].value.as < std::string > ();
        lib_import->path_ = yystack_[1].value.as < std::string > ();
        lib_import->line_ = yystack_[5].location.begin.line;
        lib_import->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = lib_import;
    }
#line 1164 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 10: // top_level_statement: MODULE LPAREN STRING_LITERAL RPAREN
#line 162 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto mod_import = std::make_shared<idyl::parser::module_import>();
        mod_import->path_ = yystack_[1].value.as < std::string > ();
        mod_import->line_ = yystack_[3].location.begin.line;
        mod_import->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = mod_import;
    }
#line 1176 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 11: // top_level_statement: IDENTIFIER ASSIGN MODULE LPAREN STRING_LITERAL RPAREN
#line 170 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto mod_import = std::make_shared<idyl::parser::module_import>();
        mod_import->namespace_ = yystack_[5].value.as < std::string > ();
        mod_import->path_ = yystack_[1].value.as < std::string > ();
        mod_import->line_ = yystack_[5].location.begin.line;
        mod_import->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = mod_import;
    }
#line 1189 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 12: // function_or_flow_definition: function_definition
#line 181 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                          { yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::function_definition> > (); }
#line 1195 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 13: // function_or_flow_definition: flow_definition
#line 182 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                      { yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::flow_definition> > (); }
#line 1201 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 14: // function_definition: IDENTIFIER LPAREN parameter_list RPAREN ASSIGN expression
#line 187 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = yystack_[5].value.as < std::string > ();
        func->parameters_ = yystack_[3].value.as < std::vector<idyl::parser::param_ptr> > ();
        func->body_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        func->line_ = yystack_[5].location.begin.line;
        func->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::function_definition> > () = func;
    }
#line 1215 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 15: // function_definition: IDENTIFIER LPAREN RPAREN ASSIGN expression
#line 197 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = yystack_[4].value.as < std::string > ();
        func->parameters_ = {};
        func->body_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        func->line_ = yystack_[4].location.begin.line;
        func->column_ = yystack_[4].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::function_definition> > () = func;
    }
#line 1229 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 16: // function_definition: IDENTIFIER LPAREN parameter_list RPAREN ASSIGN expression lambda_block
#line 207 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1244 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 17: // function_definition: IDENTIFIER LPAREN RPAREN ASSIGN expression lambda_block
#line 218 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1259 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 18: // function_definition: IDENTIFIER ASSIGN expression
#line 229 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = yystack_[2].value.as < std::string > ();
        func->parameters_ = {};
        func->body_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        func->line_ = yystack_[2].location.begin.line;
        func->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::function_definition> > () = func;
    }
#line 1273 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 19: // function_definition: IDENTIFIER ASSIGN expression lambda_block
#line 239 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1288 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 20: // flow_definition: FLOW IDENTIFIER ASSIGN flow_literal
#line 253 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1307 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 21: // flow_definition: FLOW IDENTIFIER LPAREN parameter_list RPAREN ASSIGN flow_literal
#line 268 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1326 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 22: // flow_definition: FLOW IDENTIFIER ASSIGN LBRACE flow_members RBRACE
#line 283 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto flow = std::make_shared<idyl::parser::flow_definition>();
        flow->name_ = yystack_[4].value.as < std::string > ();
        flow->parameters_ = {};
        flow->members_ = yystack_[1].value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        flow->line_ = yystack_[5].location.begin.line;
        flow->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::flow_definition> > () = flow;
    }
#line 1340 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 23: // flow_definition: FLOW IDENTIFIER LPAREN parameter_list RPAREN ASSIGN LBRACE flow_members RBRACE
#line 293 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto flow = std::make_shared<idyl::parser::flow_definition>();
        flow->name_ = yystack_[7].value.as < std::string > ();
        flow->parameters_ = yystack_[5].value.as < std::vector<idyl::parser::param_ptr> > ();
        flow->members_ = yystack_[1].value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        flow->line_ = yystack_[8].location.begin.line;
        flow->column_ = yystack_[8].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::flow_definition> > () = flow;
    }
#line 1354 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 24: // flow_members: flow_members IDENTIFIER COLON flow_literal
#line 306 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > () = yystack_[3].value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = yystack_[2].value.as < std::string > ();
        member->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        member->line_ = yystack_[2].location.begin.line;
        member->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ().push_back(member);
    }
#line 1368 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 25: // flow_members: flow_members IDENTIFIER ON IDENTIFIER COLON flow_literal
#line 316 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > () = yystack_[5].value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ();
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = yystack_[4].value.as < std::string > ();
        member->gate_name_ = yystack_[2].value.as < std::string > ();
        member->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        member->line_ = yystack_[4].location.begin.line;
        member->column_ = yystack_[4].location.begin.column;
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ().push_back(member);
    }
#line 1383 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 26: // flow_members: IDENTIFIER COLON flow_literal
#line 327 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > () = {};
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = yystack_[2].value.as < std::string > ();
        member->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        member->line_ = yystack_[2].location.begin.line;
        member->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ().push_back(member);
    }
#line 1397 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 27: // flow_members: IDENTIFIER ON IDENTIFIER COLON flow_literal
#line 337 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > () = {};
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = yystack_[4].value.as < std::string > ();
        member->gate_name_ = yystack_[2].value.as < std::string > ();
        member->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        member->line_ = yystack_[4].location.begin.line;
        member->column_ = yystack_[4].location.begin.column;
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::flow_member>> > ().push_back(member);
    }
#line 1412 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 28: // flow_literal: LBRACKET flow_elements RBRACKET
#line 351 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1428 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 29: // flow_literal: generator_expression
#line 362 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                           { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1434 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 30: // flow_elements: flow_elements COMMA expression
#line 367 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = yystack_[2].value.as < std::vector<idyl::parser::expr_ptr> > ();
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::expr_ptr > ());
    }
#line 1443 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 31: // flow_elements: flow_elements COMMA LBRACKET NUMBER RBRACKET
#line 372 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = yystack_[4].value.as < std::vector<idyl::parser::expr_ptr> > ();
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
#line 1464 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 32: // flow_elements: flow_elements COMMA RESTART_MARKER
#line 389 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = yystack_[2].value.as < std::vector<idyl::parser::expr_ptr> > ();
        auto restart = std::make_shared<idyl::parser::repetition_marker>();
        restart->repetition_count_ = nullptr;
        restart->line_ = yystack_[0].location.begin.line;
        restart->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > ().push_back(nullptr); // Placeholder for restart
    }
#line 1477 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 33: // flow_elements: expression
#line 397 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                 { yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = {yystack_[0].value.as < idyl::parser::expr_ptr > ()}; }
#line 1483 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 34: // generator_expression: LBRACKET IDENTIFIER ASSIGN expression RANGE expression COLON expression RBRACKET
#line 402 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1502 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 35: // process_block: PROCESS COLON LBRACE process_body_statements RBRACE
#line 420 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1517 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 36: // process_block: PROCESS IDENTIFIER COMMA DUR ASSIGN expression COLON LBRACE process_body_statements RBRACE
#line 431 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1533 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 37: // process_block: PROCESS IDENTIFIER COLON LBRACE process_body_statements RBRACE
#line 443 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1548 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 38: // process_body_statements: process_body_statements process_body_statement
#line 457 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1557 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 39: // process_body_statements: process_body_statement
#line 462 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = {};
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1566 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 40: // process_body_statement: postfix_expression ASSIGN expression
#line 470 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        // Interpret postfix_expression ASSIGN expression as a local function definition.
        // postfix_expression is either:
        //   - identifier_expr → 0-param binding: "name = expr" (same as assignment)
        //   - function_call_expr → local function: "name(params) = expr"
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->body_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        func->line_ = yystack_[2].location.begin.line;
        func->column_ = yystack_[2].location.begin.column;
        auto& lhs = yystack_[2].value.as < idyl::parser::expr_ptr > ();
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
        yylhs.value.as < idyl::parser::stmt_ptr > () = func;
    }
#line 1619 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 41: // process_body_statement: IDENTIFIER ASSIGN expression
#line 519 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = yystack_[2].value.as < std::string > ();
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = false;
        assign->line_ = yystack_[2].location.begin.line;
        assign->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1633 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 42: // process_body_statement: EMIT IDENTIFIER ASSIGN expression
#line 529 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = yystack_[2].value.as < std::string > ();
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = true;
        assign->line_ = yystack_[3].location.begin.line;
        assign->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1647 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 43: // process_body_statement: stop_statement
#line 539 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::stop_statement> > ();
    }
#line 1655 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 44: // process_body_statement: start_statement
#line 543 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::start_statement> > ();
    }
#line 1663 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 45: // process_body_statement: catch_block
#line 547 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::catch_block> > ();
    }
#line 1671 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 46: // process_body_statement: at_block
#line 551 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::at_block> > ();
    }
#line 1679 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 47: // process_body_statement: on_block
#line 555 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::on_block> > ();
    }
#line 1687 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 48: // process_body_statement: expression
#line 559 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto es = std::make_shared<idyl::parser::expression_stmt>();
        es->expression_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        es->line_ = yystack_[0].location.begin.line;
        es->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = es;
    }
#line 1699 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 49: // at_block: AT_OP LPAREN expression RPAREN COLON LBRACE process_body_statements RBRACE
#line 570 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto at_stmt = std::make_shared<idyl::parser::at_block>();
        at_stmt->time_expr_ = yystack_[5].value.as < idyl::parser::expr_ptr > ();
        at_stmt->handler_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        at_stmt->line_ = yystack_[7].location.begin.line;
        at_stmt->column_ = yystack_[7].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::at_block> > () = at_stmt;
    }
#line 1712 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 50: // at_block: AT_OP LPAREN expression RPAREN COLON process_body_statement
#line 579 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto at_stmt = std::make_shared<idyl::parser::at_block>();
        at_stmt->time_expr_ = yystack_[3].value.as < idyl::parser::expr_ptr > ();
        at_stmt->handler_ = {yystack_[0].value.as < idyl::parser::stmt_ptr > ()};
        at_stmt->line_ = yystack_[5].location.begin.line;
        at_stmt->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::at_block> > () = at_stmt;
    }
#line 1725 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 51: // on_block: ON expression COLON LBRACE process_body_statements RBRACE
#line 591 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto on_stmt = std::make_shared<idyl::parser::on_block>();
        on_stmt->trigger_expr_ = yystack_[4].value.as < idyl::parser::expr_ptr > ();
        on_stmt->handler_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        on_stmt->line_ = yystack_[5].location.begin.line;
        on_stmt->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::on_block> > () = on_stmt;
    }
#line 1738 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 52: // on_block: ON expression COLON process_body_statement
#line 600 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto on_stmt = std::make_shared<idyl::parser::on_block>();
        on_stmt->trigger_expr_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        on_stmt->handler_ = {yystack_[0].value.as < idyl::parser::stmt_ptr > ()};
        on_stmt->line_ = yystack_[3].location.begin.line;
        on_stmt->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::on_block> > () = on_stmt;
    }
#line 1751 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 53: // stop_statement: STOP IDENTIFIER
#line 612 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto stop_stmt = std::make_shared<idyl::parser::stop_statement>();
        stop_stmt->target_ = yystack_[0].value.as < std::string > ();
        stop_stmt->line_ = yystack_[1].location.begin.line;
        stop_stmt->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::stop_statement> > () = stop_stmt;
    }
#line 1763 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 54: // stop_statement: STOP
#line 620 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto stop_stmt = std::make_shared<idyl::parser::stop_statement>();
        stop_stmt->target_ = "";
        stop_stmt->line_ = yystack_[0].location.begin.line;
        stop_stmt->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::stop_statement> > () = stop_stmt;
    }
#line 1775 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 55: // start_statement: START IDENTIFIER
#line 631 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto start_stmt = std::make_shared<idyl::parser::start_statement>();
        start_stmt->target_ = yystack_[0].value.as < std::string > ();
        start_stmt->line_ = yystack_[1].location.begin.line;
        start_stmt->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::start_statement> > () = start_stmt;
    }
#line 1787 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 56: // catch_block: CATCH postfix_expression NAMESPACE_DOT IDENTIFIER COLON LBRACE process_body_statements RBRACE
#line 642 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto catch_b = std::make_shared<idyl::parser::catch_block>();
        catch_b->instance_expr_ = yystack_[6].value.as < idyl::parser::expr_ptr > ();
        catch_b->signal_name_   = yystack_[4].value.as < std::string > ();
        catch_b->handler_       = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        catch_b->line_          = yystack_[7].location.begin.line;
        catch_b->column_        = yystack_[7].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::catch_block> > () = catch_b;
    }
#line 1801 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 57: // catch_block: CATCH postfix_expression NAMESPACE_DOT END COLON LBRACE process_body_statements RBRACE
#line 652 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto catch_b = std::make_shared<idyl::parser::catch_block>();
        catch_b->instance_expr_ = yystack_[6].value.as < idyl::parser::expr_ptr > ();
        catch_b->signal_name_   = "end";
        catch_b->handler_       = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        catch_b->line_          = yystack_[7].location.begin.line;
        catch_b->column_        = yystack_[7].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::catch_block> > () = catch_b;
    }
#line 1815 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 58: // lambda_block: LAMBDA_BLOCK LBRACE init_block lambda_statements RBRACE
#line 665 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->init_ = yystack_[2].value.as < std::shared_ptr<idyl::parser::init_block> > ();
        lambda->update_statements_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        lambda->line_ = yystack_[4].location.begin.line;
        lambda->column_ = yystack_[4].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::lambda_block> > () = lambda;
    }
#line 1828 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 59: // lambda_block: LAMBDA_BLOCK LBRACE init_block RBRACE
#line 674 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->init_ = yystack_[1].value.as < std::shared_ptr<idyl::parser::init_block> > ();
        lambda->update_statements_ = {};
        lambda->line_ = yystack_[3].location.begin.line;
        lambda->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::lambda_block> > () = lambda;
    }
#line 1841 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 60: // lambda_block: LAMBDA_BLOCK LBRACE lambda_statements RBRACE
#line 683 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->update_statements_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        lambda->line_ = yystack_[3].location.begin.line;
        lambda->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::lambda_block> > () = lambda;
    }
#line 1853 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 61: // init_block: INIT COLON LBRACE lambda_statements RBRACE
#line 694 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto init = std::make_shared<idyl::parser::init_block>();
        init->statements_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        init->line_ = yystack_[4].location.begin.line;
        init->column_ = yystack_[4].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::init_block> > () = init;
    }
#line 1865 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 62: // lambda_statements: lambda_statements lambda_statement
#line 705 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1874 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 63: // lambda_statements: lambda_statement
#line 710 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = {};
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1883 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 64: // lambda_statement: postfix_expression ASSIGN expression
#line 718 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        // Same pattern as in process_body_statement: interpret as local function definition
        // when lhs is a function call, or as an assignment when lhs is an identifier.
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->body_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        func->line_ = yystack_[2].location.begin.line;
        func->column_ = yystack_[2].location.begin.column;
        auto& lhs = yystack_[2].value.as < idyl::parser::expr_ptr > ();
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
        yylhs.value.as < idyl::parser::stmt_ptr > () = func;
    }
#line 1926 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 65: // lambda_statement: IDENTIFIER ASSIGN expression
#line 757 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = yystack_[2].value.as < std::string > ();
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = false;
        assign->line_ = yystack_[2].location.begin.line;
        assign->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1940 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 66: // lambda_statement: EMIT IDENTIFIER ASSIGN expression
#line 767 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = yystack_[2].value.as < std::string > ();
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = true;
        assign->line_ = yystack_[3].location.begin.line;
        assign->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1954 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 67: // lambda_statement: DT ASSIGN expression
#line 777 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = "dt";
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = false;
        assign->line_ = yystack_[2].location.begin.line;
        assign->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1968 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 68: // lambda_statement: expression
#line 787 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto es = std::make_shared<idyl::parser::expression_stmt>();
        es->expression_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        es->line_ = yystack_[0].location.begin.line;
        es->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = es;
    }
#line 1980 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 69: // parameter_list: parameter_list COMMA parameter
#line 798 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::param_ptr> > () = yystack_[2].value.as < std::vector<idyl::parser::param_ptr> > ();
        if (yystack_[0].value.as < idyl::parser::param_ptr > ()) yylhs.value.as < std::vector<idyl::parser::param_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::param_ptr > ());
    }
#line 1989 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 70: // parameter_list: parameter
#line 802 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                { yylhs.value.as < std::vector<idyl::parser::param_ptr> > () = {yystack_[0].value.as < idyl::parser::param_ptr > ()}; }
#line 1995 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 71: // parameter: IDENTIFIER
#line 807 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = yystack_[0].value.as < std::string > ();
        param->is_trigger_parameter_ = false;
        param->line_ = yystack_[0].location.begin.line;
        param->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 2008 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 72: // parameter: IDENTIFIER TRIGGER
#line 816 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = yystack_[1].value.as < std::string > ();
        param->is_trigger_parameter_ = true;
        param->line_ = yystack_[1].location.begin.line;
        param->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 2021 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 73: // parameter: IDENTIFIER ASSIGN expression
#line 825 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = yystack_[2].value.as < std::string > ();
        param->default_value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        param->line_ = yystack_[2].location.begin.line;
        param->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 2034 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 74: // parameter: DT ASSIGN expression
#line 834 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = "dt";
        param->default_value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        param->has_default_time_ = true;
        param->line_ = yystack_[2].location.begin.line;
        param->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 2048 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 75: // expression: assignment_expression
#line 846 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2054 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 76: // assignment_expression: ternary_expression
#line 850 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2060 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 77: // ternary_expression: logical_or_expression
#line 854 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2066 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 78: // ternary_expression: logical_or_expression QUESTION ternary_options
#line 856 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto ternary = std::make_shared<idyl::parser::ternary_op>();
        ternary->options_ = yystack_[0].value.as < std::vector<idyl::parser::expr_ptr> > ();
        // Single-option form: cond ? expr  →  cond ? _ ; expr
        // options_[0] = false/rest branch, options_[1] = true branch
        if (ternary->options_.size() == 1) {
            auto rest = std::make_shared<idyl::parser::rest_literal>();
            rest->line_ = yystack_[1].location.begin.line;
            rest->column_ = yystack_[1].location.begin.column;
            auto rest_expr = std::make_shared<idyl::parser::literal_expr>();
            rest_expr->literal_ = rest;
            rest_expr->line_ = yystack_[1].location.begin.line;
            rest_expr->column_ = yystack_[1].location.begin.column;
            ternary->options_.insert(ternary->options_.begin(), rest_expr);
        }
        ternary->condition_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        ternary->line_ = yystack_[1].location.begin.line;
        ternary->column_ = yystack_[1].location.begin.column;
        auto expr = std::make_shared<idyl::parser::ternary_op_expr>();
        expr->op_ = ternary;
        expr->line_ = yystack_[1].location.begin.line;
        expr->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2095 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 79: // ternary_options: ternary_options SEMICOLON logical_or_expression
#line 884 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = yystack_[2].value.as < std::vector<idyl::parser::expr_ptr> > ();
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::expr_ptr > ());
    }
#line 2104 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 80: // ternary_options: logical_or_expression
#line 889 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = {yystack_[0].value.as < idyl::parser::expr_ptr > ()};
    }
#line 2112 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 81: // logical_or_expression: logical_and_expression
#line 895 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2118 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 82: // logical_or_expression: logical_or_expression OR logical_and_expression
#line 897 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2136 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 83: // logical_and_expression: bitwise_or_expression
#line 913 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2142 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 84: // logical_and_expression: logical_and_expression AND bitwise_or_expression
#line 915 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2160 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 85: // bitwise_or_expression: bitwise_xor_expression
#line 931 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2166 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 86: // bitwise_or_expression: bitwise_or_expression OR bitwise_xor_expression
#line 933 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2184 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 87: // bitwise_xor_expression: bitwise_and_expression
#line 949 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2190 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 88: // bitwise_xor_expression: bitwise_xor_expression XOR bitwise_and_expression
#line 951 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2208 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 89: // bitwise_and_expression: equality_expression
#line 967 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                          { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2214 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 90: // bitwise_and_expression: bitwise_and_expression AND equality_expression
#line 969 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2232 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 91: // equality_expression: relational_expression
#line 985 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2238 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 92: // equality_expression: equality_expression EQ relational_expression
#line 987 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2256 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 93: // equality_expression: equality_expression NEQ relational_expression
#line 1001 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2274 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 94: // relational_expression: shift_expression
#line 1017 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                       { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2280 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 95: // relational_expression: relational_expression LT shift_expression
#line 1019 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2298 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 96: // relational_expression: relational_expression GT shift_expression
#line 1033 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2316 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 97: // relational_expression: relational_expression LE shift_expression
#line 1047 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2334 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 98: // relational_expression: relational_expression GE shift_expression
#line 1061 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2352 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 99: // shift_expression: additive_expression
#line 1077 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                          { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2358 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 100: // shift_expression: shift_expression LSHIFT additive_expression
#line 1079 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2376 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 101: // shift_expression: shift_expression RSHIFT additive_expression
#line 1093 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2394 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 102: // additive_expression: multiplicative_expression
#line 1109 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2400 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 103: // additive_expression: additive_expression PLUS multiplicative_expression
#line 1111 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2418 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 104: // additive_expression: additive_expression MINUS multiplicative_expression
#line 1125 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2436 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 105: // multiplicative_expression: unary_expression
#line 1141 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                       { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2442 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 106: // multiplicative_expression: multiplicative_expression MUL unary_expression
#line 1143 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2460 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 107: // multiplicative_expression: multiplicative_expression DIV unary_expression
#line 1157 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2478 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 108: // multiplicative_expression: multiplicative_expression MOD unary_expression
#line 1171 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2496 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 109: // unary_expression: postfix_expression
#line 1187 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2502 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 110: // unary_expression: NOT unary_expression
#line 1189 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2519 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 111: // unary_expression: MINUS unary_expression
#line 1202 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2536 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 112: // unary_expression: MEMORY_OP LPAREN expression RPAREN
#line 1215 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2552 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 113: // unary_expression: MEMORY_OP LPAREN expression COMMA expression RPAREN
#line 1227 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2569 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 114: // postfix_expression: primary_expression
#line 1242 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2575 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 115: // postfix_expression: postfix_expression LPAREN argument_list RPAREN
#line 1244 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2592 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 116: // postfix_expression: postfix_expression LPAREN RPAREN
#line 1257 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2609 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 117: // postfix_expression: postfix_expression DOT IDENTIFIER
#line 1270 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2626 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 118: // postfix_expression: postfix_expression NAMESPACE_DOT IDENTIFIER
#line 1283 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2644 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 119: // postfix_expression: postfix_expression NAMESPACE_DOT IDENTIFIER LPAREN argument_list RPAREN
#line 1297 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2662 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 120: // postfix_expression: postfix_expression NAMESPACE_DOT IDENTIFIER LPAREN RPAREN
#line 1311 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2680 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 121: // postfix_expression: postfix_expression LBRACKET expression RBRACKET
#line 1325 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2697 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 122: // primary_expression: IDENTIFIER
#line 1341 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2713 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 123: // primary_expression: NUMBER
#line 1353 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2729 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 124: // primary_expression: TIME_LITERAL
#line 1365 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
        time->line_ = yystack_[0].location.begin.line;
        time->column_ = yystack_[0].location.begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = time;
        expr->line_ = yystack_[0].location.begin.line;
        expr->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2762 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 125: // primary_expression: TRIGGER
#line 1394 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2777 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 126: // primary_expression: REST
#line 1405 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2792 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 127: // primary_expression: DT
#line 1416 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2808 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 128: // primary_expression: DUR
#line 1428 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto id = std::make_shared<idyl::parser::identifier>();
        id->name_ = "dur";
        id->line_ = yystack_[0].location.begin.line;
        id->column_ = yystack_[0].location.begin.column;
        auto expr = std::make_shared<idyl::parser::identifier_expr>();
        expr->identifier_ = id;
        expr->line_ = yystack_[0].location.begin.line;
        expr->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = expr;
    }
#line 2824 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 129: // primary_expression: STRING_LITERAL
#line 1440 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2840 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 130: // primary_expression: LPAREN expression RPAREN
#line 1452 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto paren = std::make_shared<idyl::parser::parenthesized_expr>();
        paren->expr_ = yystack_[1].value.as < idyl::parser::expr_ptr > ();
        paren->line_ = yystack_[2].location.begin.line;
        paren->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = paren;
    }
#line 2852 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 131: // primary_expression: flow_literal
#line 1459 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                   { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2858 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 132: // primary_expression: generator_expression
#line 1460 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                           { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2864 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 133: // argument_list: argument_list COMMA argument
#line 1465 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > () = yystack_[2].value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        if (yystack_[0].value.as < std::shared_ptr<idyl::parser::argument> > ()) yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > ().push_back(yystack_[0].value.as < std::shared_ptr<idyl::parser::argument> > ());
    }
#line 2873 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 134: // argument_list: argument
#line 1469 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
               { yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > () = {yystack_[0].value.as < std::shared_ptr<idyl::parser::argument> > ()}; }
#line 2879 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 135: // argument: expression
#line 1474 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = "";
        arg->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        arg->line_ = yystack_[0].location.begin.line;
        arg->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::argument> > () = arg;
    }
#line 2892 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 136: // argument: IDENTIFIER ASSIGN expression
#line 1483 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = yystack_[2].value.as < std::string > ();
        arg->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        arg->line_ = yystack_[2].location.begin.line;
        arg->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::argument> > () = arg;
    }
#line 2905 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 137: // argument: DT ASSIGN expression
#line 1492 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = "dt";
        arg->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        arg->line_ = yystack_[2].location.begin.line;
        arg->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::argument> > () = arg;
    }
#line 2918 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;


#line 2922 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"

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


  const short parser::yypact_ninf_ = -201;

  const signed char parser::yytable_ninf_ = -1;

  const short
  parser::yypact_[] =
  {
     126,   -30,     8,     9,   -29,   -14,    24,   126,  -201,  -201,
    -201,  -201,  -201,   834,    10,    14,    -8,     3,    57,    97,
    -201,  -201,  -201,  -201,  -201,  -201,    62,    68,  -201,  -201,
      80,  -201,  1015,  1015,  -201,  1015,  1044,  -201,  -201,   124,
    -201,  -201,    38,   141,   123,   145,   148,    53,    39,   113,
      66,    72,  -201,    63,  -201,    50,   154,   155,    73,  -201,
     114,    13,   135,   190,   687,   156,   159,   206,   210,  1015,
    -201,  -201,   168,   177,    12,  -201,   169,  -201,  1015,  1015,
    1015,  1015,  1015,  1015,  1015,  1015,  1015,  1015,  1015,  1015,
    1015,  1015,  1015,  1015,  1015,  1015,  1015,   226,   231,   894,
    1015,  1015,  -201,  1015,  1015,    13,   196,   239,  -201,  -201,
      98,   687,   200,   204,   246,    28,   249,   250,   205,  1015,
      92,  -201,  -201,  -201,  -201,  -201,  -201,  -201,    -3,  -201,
    -201,   203,   211,    99,  -201,  1015,   926,  -201,   865,   141,
     217,   218,  -201,   145,  -201,    53,    39,    39,   113,   113,
     113,   113,   238,   238,    72,    72,  -201,  -201,  -201,   207,
    -201,   225,   230,  -201,  -201,   106,  -201,   222,  -201,  -201,
     124,  -201,  1015,   -26,     0,   232,   255,  1015,  1015,   234,
     191,  -201,  -201,  1015,   236,  -201,  -201,  1015,  -201,  -201,
    1015,  -201,   260,  -201,  1073,  -201,   240,   241,   282,   243,
     718,   747,  -201,  -201,    -2,  1015,   955,  1015,  1015,  1102,
    -201,  -201,  -201,   124,   237,   285,   -18,  -201,   117,  -201,
     245,  -201,  1015,    37,   242,   309,  -201,   247,  1015,   244,
    1015,   248,   252,  1015,  -201,   776,  -201,  -201,  1015,   218,
    -201,   109,  -201,  -201,  -201,  -201,  -201,   254,   237,   288,
     239,  -201,   262,  -201,    76,   256,   258,   687,  -201,  -201,
     261,  -201,  -201,   986,  1015,  -201,  -201,  -201,  -201,   237,
    -201,   263,     5,   687,   264,   265,   363,   417,  1015,   805,
    -201,  -201,   237,  -201,   471,   687,   687,   687,  -201,  -201,
     267,  -201,  -201,  -201,   525,   579,   633,  -201,  -201,  -201,
    -201
  };

  const unsigned char
  parser::yydefact_[] =
  {
       3,     0,     0,     0,     0,     0,     0,     2,     5,     6,
      12,    13,     7,     0,     0,     0,     0,     0,     0,     0,
       1,     4,   122,   123,   124,   129,     0,     0,   127,   128,
       0,   126,     0,     0,   125,     0,     0,   131,    29,    18,
      75,    76,    77,    81,    83,    85,    87,    89,    91,    94,
      99,   102,   105,   109,   114,    71,     0,     0,     0,    70,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     111,   110,     0,   122,     0,    33,     0,    19,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    72,     0,     0,     0,     0,     0,    20,    29,
       0,     0,     0,   122,     0,     0,    54,     0,     0,     0,
       0,    39,    46,    47,    43,    44,    45,    48,   109,     8,
      10,     0,     0,     0,   130,     0,     0,    28,     0,    82,
      78,    80,    84,    86,    88,    90,    92,    93,    95,    96,
      97,    98,   100,   101,   103,   104,   106,   107,   108,   118,
     117,   122,   127,   116,   135,     0,   134,     0,    73,    74,
      15,    69,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    53,    55,     0,     0,    35,    38,     0,     9,    11,
       0,   112,     0,    32,     0,    30,   122,     0,     0,   127,
       0,     0,    63,    68,   109,     0,     0,     0,     0,     0,
     115,   121,    17,    14,     0,     0,     0,    22,     0,    37,
       0,    41,     0,     0,     0,     0,    40,     0,     0,   123,
       0,     0,     0,     0,    59,     0,    60,    62,     0,    79,
     120,     0,   136,   137,   133,    16,    26,     0,     0,     0,
       0,    21,     0,    42,   118,     0,     0,     0,    52,   113,
       0,    31,    65,     0,     0,    67,    58,    64,   119,     0,
      24,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      66,    27,     0,    23,     0,     0,     0,     0,    50,    51,
       0,    61,    25,    36,     0,     0,     0,    34,    56,    57,
      49
  };

  const short
  parser::yypgoto_[] =
  {
    -201,  -201,  -201,   287,  -201,  -201,  -201,    42,   -56,  -201,
     -51,  -201,   -82,   -94,  -201,  -201,  -201,  -201,  -201,  -160,
    -201,  -193,  -200,   268,   195,   -13,  -201,  -201,  -201,   -77,
     257,   229,   251,   235,   259,    88,    51,    89,   140,   -27,
     -49,  -201,   122,   121
  };

  const unsigned char
  parser::yydefgoto_[] =
  {
       0,     6,     7,     8,     9,    10,    11,   174,    37,    74,
      38,    12,   120,   121,   122,   123,   124,   125,   126,    77,
     200,   201,   202,    58,    59,   127,    40,    41,   140,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,   165,   166
  };

  const short
  parser::yytable_[] =
  {
      39,   237,   141,   216,   108,    70,    71,   235,   216,   109,
     212,    15,    16,    55,    13,   128,    55,    97,    97,   214,
      14,    18,    72,    75,    20,    56,   186,   248,    56,   176,
     215,    22,    23,    24,    25,   237,    19,    62,   249,    63,
     254,   187,   238,    28,    29,    98,    98,    99,    99,   100,
     100,   255,    31,   245,    17,   217,   133,    64,    60,   136,
     283,    57,   128,    65,    61,   137,   180,   156,   157,   158,
     279,   128,    86,    87,    88,    89,    78,    34,    35,   237,
      36,    79,   186,    97,    84,    85,   164,   167,   168,   204,
     169,   170,    92,    93,   101,   113,    23,    24,    25,   102,
      94,    95,    96,    66,   114,   115,   184,    28,    29,   116,
     117,    98,    67,    99,    30,   100,    31,   118,    68,    32,
     105,   274,   192,   195,   106,   203,   206,   128,   239,     1,
      69,   258,    33,     2,     3,     4,     5,   148,   149,   150,
     151,    34,    35,    76,    36,   105,   190,   185,   119,   175,
     191,   204,   204,   209,    90,    91,   209,   210,   246,   213,
     268,    81,   251,   109,   220,   221,    36,   109,   107,    36,
     224,   250,   146,   147,   226,   277,   128,   227,    80,   152,
     153,    75,   288,   186,    82,    83,   204,   203,   203,   111,
     186,   284,   270,   164,   242,   243,   164,   109,   103,   104,
     186,   186,   186,   294,   295,   296,   112,   129,   128,   253,
     130,   223,   131,   281,   204,   260,   132,   262,   109,   134,
     265,   135,   203,   138,   128,   267,   292,   128,   128,   159,
     204,   109,   154,   155,   160,   128,   128,   128,   128,    98,
     172,    99,   173,   100,   177,   128,   128,   128,   178,   179,
     203,   280,   181,   182,   188,   183,    78,   206,   113,    23,
      24,    25,   189,   205,    92,   290,   203,   114,   115,   207,
      28,    29,   116,   117,   208,   211,   218,    30,   222,    31,
     118,   225,    32,   228,   230,   232,   231,   233,   247,    36,
     252,   271,   272,   256,    21,    33,   264,   261,   259,   269,
     171,   275,   263,   276,    34,    35,   278,    36,   282,   142,
     219,   119,   113,    23,    24,    25,   273,   144,   285,   286,
     297,   114,   115,     0,    28,    29,   116,   117,   241,   110,
     244,    30,   143,    31,   118,   139,    32,     0,     0,     0,
       0,     0,   145,     0,     0,     0,     0,     0,     0,    33,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
       0,    36,     0,   257,     0,   119,   113,    23,    24,    25,
       0,     0,     0,     0,     0,   114,   115,     0,    28,    29,
     116,   117,     0,     0,     0,    30,     0,    31,   118,     0,
      32,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    33,     0,     0,     0,     0,     0,     0,
       0,     0,    34,    35,     0,    36,     0,   287,     0,   119,
     113,    23,    24,    25,     0,     0,     0,     0,     0,   114,
     115,     0,    28,    29,   116,   117,     0,     0,     0,    30,
       0,    31,   118,     0,    32,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    33,     0,     0,
       0,     0,     0,     0,     0,     0,    34,    35,     0,    36,
       0,     0,   289,   119,   113,    23,    24,    25,     0,     0,
       0,     0,     0,   114,   115,     0,    28,    29,   116,   117,
       0,     0,     0,    30,     0,    31,   118,     0,    32,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    33,     0,     0,     0,     0,     0,     0,     0,     0,
      34,    35,     0,    36,     0,     0,   293,   119,   113,    23,
      24,    25,     0,     0,     0,     0,     0,   114,   115,     0,
      28,    29,   116,   117,     0,     0,     0,    30,     0,    31,
     118,     0,    32,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    33,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,     0,    36,     0,     0,
     298,   119,   113,    23,    24,    25,     0,     0,     0,     0,
       0,   114,   115,     0,    28,    29,   116,   117,     0,     0,
       0,    30,     0,    31,   118,     0,    32,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    33,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
       0,    36,     0,     0,   299,   119,   113,    23,    24,    25,
       0,     0,     0,     0,     0,   114,   115,     0,    28,    29,
     116,   117,     0,     0,     0,    30,     0,    31,   118,     0,
      32,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    33,     0,     0,     0,     0,     0,     0,
       0,     0,    34,    35,     0,    36,     0,     0,   300,   119,
     113,    23,    24,    25,     0,     0,     0,     0,     0,   114,
     115,     0,    28,    29,   116,   117,     0,     0,     0,    30,
       0,    31,   118,     0,    32,     0,     0,     0,     0,     0,
       0,   196,    23,    24,    25,     0,     0,    33,     0,     0,
     198,     0,     0,   199,    29,     0,    34,    35,     0,    36,
      30,     0,    31,   119,     0,    32,     0,     0,     0,     0,
     196,    23,    24,    25,     0,     0,     0,     0,    33,   198,
       0,     0,   199,    29,     0,     0,     0,    34,    35,    30,
      36,    31,     0,   234,    32,     0,     0,     0,     0,   196,
      23,    24,    25,     0,     0,     0,     0,    33,   198,     0,
       0,   199,    29,     0,     0,     0,    34,    35,    30,    36,
      31,     0,   236,    32,     0,     0,     0,     0,   196,    23,
      24,    25,     0,     0,     0,     0,    33,   198,     0,     0,
     199,    29,     0,     0,     0,    34,    35,    30,    36,    31,
       0,   266,    32,     0,     0,     0,     0,    22,    23,    24,
      25,     0,     0,    26,    27,    33,     0,     0,     0,    28,
      29,     0,     0,     0,    34,    35,    30,    36,    31,     0,
     291,    32,     0,     0,     0,     0,     0,     0,   196,    23,
      24,    25,     0,     0,    33,     0,   197,   198,     0,     0,
     199,    29,     0,    34,    35,     0,    36,    30,     0,    31,
       0,     0,    32,     0,     0,     0,     0,   161,    23,    24,
      25,     0,     0,     0,     0,    33,     0,     0,     0,   162,
      29,     0,     0,     0,    34,    35,    30,    36,    31,     0,
       0,    32,     0,     0,     0,     0,     0,     0,     0,    22,
      23,    24,    25,     0,    33,     0,     0,     0,     0,     0,
       0,    28,    29,    34,    35,   163,    36,   193,    30,     0,
      31,     0,     0,    32,     0,     0,     0,     0,   161,    23,
      24,    25,     0,     0,     0,     0,    33,     0,     0,     0,
     162,    29,     0,     0,     0,    34,    35,    30,   194,    31,
       0,     0,    32,     0,     0,     0,     0,     0,     0,   196,
      23,    24,    25,     0,     0,    33,     0,     0,   198,     0,
       0,   199,    29,     0,    34,    35,   240,    36,    30,     0,
      31,     0,     0,    32,     0,     0,     0,     0,    22,    23,
      24,    25,     0,     0,     0,     0,    33,     0,     0,     0,
      28,    29,     0,     0,     0,    34,    35,    30,    36,    31,
       0,     0,    32,     0,     0,     0,     0,    73,    23,    24,
      25,     0,     0,     0,     0,    33,     0,     0,     0,    28,
      29,     0,     0,     0,    34,    35,    30,    36,    31,     0,
       0,    32,     0,     0,     0,     0,    73,   229,    24,    25,
       0,     0,     0,     0,    33,     0,     0,     0,    28,    29,
       0,     0,     0,    34,    35,    30,    36,    31,     0,     0,
      32,     0,     0,     0,     0,   161,    23,    24,    25,     0,
       0,     0,     0,    33,     0,     0,     0,   162,    29,     0,
       0,     0,    34,    35,    30,    36,    31,     0,     0,    32,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    33,     0,     0,     0,     0,     0,     0,     0,
       0,    34,    35,     0,    36
  };

  const short
  parser::yycheck_[] =
  {
      13,   201,    79,     3,    60,    32,    33,   200,     3,    60,
     170,     3,     3,     3,    44,    64,     3,    20,    20,    45,
      50,    50,    35,    36,     0,    15,   120,    45,    15,   111,
      56,     3,     4,     5,     6,   235,    50,    45,    56,    47,
       3,    44,    44,    15,    16,    48,    48,    50,    50,    52,
      52,    14,    24,   213,    45,    55,    69,    54,    44,    47,
      55,    51,   111,     6,    50,    53,   115,    94,    95,    96,
     263,   120,    33,    34,    35,    36,    38,    49,    50,   279,
      52,    43,   176,    20,    31,    32,    99,   100,   101,   138,
     103,   104,    26,    27,    44,     3,     4,     5,     6,    49,
      28,    29,    30,     6,    12,    13,   119,    15,    16,    17,
      18,    48,    50,    50,    22,    52,    24,    25,    50,    27,
      47,    45,   135,   136,    51,   138,    50,   176,   205,     3,
      50,   225,    40,     7,     8,     9,    10,    86,    87,    88,
      89,    49,    50,    19,    52,    47,    47,    55,    56,    51,
      51,   200,   201,    47,    41,    42,    47,    51,   214,   172,
      51,    38,   218,   214,   177,   178,    52,   218,    54,    52,
     183,    54,    84,    85,   187,   257,   225,   190,    37,    90,
      91,   194,   276,   277,    39,    37,   235,   200,   201,    54,
     284,   273,   248,   206,   207,   208,   209,   248,    44,    44,
     294,   295,   296,   285,   286,   287,    16,    51,   257,   222,
      51,    20,     6,   269,   263,   228,     6,   230,   269,    51,
     233,    44,   235,    54,   273,   238,   282,   276,   277,     3,
     279,   282,    92,    93,     3,   284,   285,   286,   287,    48,
      44,    50,     3,    52,    44,   294,   295,   296,    44,     3,
     263,   264,     3,     3,    51,    50,    38,    50,     3,     4,
       5,     6,    51,    46,    26,   278,   279,    12,    13,    44,
      15,    16,    17,    18,    44,    53,    44,    22,    44,    24,
      25,    45,    27,    23,    44,     3,    45,    44,     3,    52,
      45,     3,   250,    51,     7,    40,    44,    53,    51,    45,
     105,    45,    54,    45,    49,    50,    45,    52,    45,    80,
      55,    56,     3,     4,     5,     6,    54,    82,    54,    54,
      53,    12,    13,    -1,    15,    16,    17,    18,   206,    61,
     209,    22,    81,    24,    25,    78,    27,    -1,    -1,    -1,
      -1,    -1,    83,    -1,    -1,    -1,    -1,    -1,    -1,    40,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,
      -1,    52,    -1,    54,    -1,    56,     3,     4,     5,     6,
      -1,    -1,    -1,    -1,    -1,    12,    13,    -1,    15,    16,
      17,    18,    -1,    -1,    -1,    22,    -1,    24,    25,    -1,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    40,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    49,    50,    -1,    52,    -1,    54,    -1,    56,
       3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    12,
      13,    -1,    15,    16,    17,    18,    -1,    -1,    -1,    22,
      -1,    24,    25,    -1,    27,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    -1,    52,
      -1,    -1,    55,    56,     3,     4,     5,     6,    -1,    -1,
      -1,    -1,    -1,    12,    13,    -1,    15,    16,    17,    18,
      -1,    -1,    -1,    22,    -1,    24,    25,    -1,    27,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    50,    -1,    52,    -1,    -1,    55,    56,     3,     4,
       5,     6,    -1,    -1,    -1,    -1,    -1,    12,    13,    -1,
      15,    16,    17,    18,    -1,    -1,    -1,    22,    -1,    24,
      25,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    49,    50,    -1,    52,    -1,    -1,
      55,    56,     3,     4,     5,     6,    -1,    -1,    -1,    -1,
      -1,    12,    13,    -1,    15,    16,    17,    18,    -1,    -1,
      -1,    22,    -1,    24,    25,    -1,    27,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,
      -1,    52,    -1,    -1,    55,    56,     3,     4,     5,     6,
      -1,    -1,    -1,    -1,    -1,    12,    13,    -1,    15,    16,
      17,    18,    -1,    -1,    -1,    22,    -1,    24,    25,    -1,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    40,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    49,    50,    -1,    52,    -1,    -1,    55,    56,
       3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    12,
      13,    -1,    15,    16,    17,    18,    -1,    -1,    -1,    22,
      -1,    24,    25,    -1,    27,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,    -1,    -1,    40,    -1,    -1,
      12,    -1,    -1,    15,    16,    -1,    49,    50,    -1,    52,
      22,    -1,    24,    56,    -1,    27,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,    -1,    -1,    -1,    -1,    40,    12,
      -1,    -1,    15,    16,    -1,    -1,    -1,    49,    50,    22,
      52,    24,    -1,    55,    27,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    -1,    40,    12,    -1,
      -1,    15,    16,    -1,    -1,    -1,    49,    50,    22,    52,
      24,    -1,    55,    27,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,    -1,    -1,    -1,    -1,    40,    12,    -1,    -1,
      15,    16,    -1,    -1,    -1,    49,    50,    22,    52,    24,
      -1,    55,    27,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,    -1,     9,    10,    40,    -1,    -1,    -1,    15,
      16,    -1,    -1,    -1,    49,    50,    22,    52,    24,    -1,
      55,    27,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,    -1,    -1,    40,    -1,    11,    12,    -1,    -1,
      15,    16,    -1,    49,    50,    -1,    52,    22,    -1,    24,
      -1,    -1,    27,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    -1,    40,    -1,    -1,    -1,    15,
      16,    -1,    -1,    -1,    49,    50,    22,    52,    24,    -1,
      -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,    -1,    40,    -1,    -1,    -1,    -1,    -1,
      -1,    15,    16,    49,    50,    51,    52,    21,    22,    -1,
      24,    -1,    -1,    27,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,    -1,    -1,    -1,    -1,    40,    -1,    -1,    -1,
      15,    16,    -1,    -1,    -1,    49,    50,    22,    52,    24,
      -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,    -1,    -1,    40,    -1,    -1,    12,    -1,
      -1,    15,    16,    -1,    49,    50,    51,    52,    22,    -1,
      24,    -1,    -1,    27,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,    -1,    -1,    -1,    -1,    40,    -1,    -1,    -1,
      15,    16,    -1,    -1,    -1,    49,    50,    22,    52,    24,
      -1,    -1,    27,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    -1,    40,    -1,    -1,    -1,    15,
      16,    -1,    -1,    -1,    49,    50,    22,    52,    24,    -1,
      -1,    27,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    -1,    40,    -1,    -1,    -1,    15,    16,
      -1,    -1,    -1,    49,    50,    22,    52,    24,    -1,    -1,
      27,    -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    -1,    40,    -1,    -1,    -1,    15,    16,    -1,
      -1,    -1,    49,    50,    22,    52,    24,    -1,    -1,    27,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    50,    -1,    52
  };

  const signed char
  parser::yystos_[] =
  {
       0,     3,     7,     8,     9,    10,    58,    59,    60,    61,
      62,    63,    68,    44,    50,     3,     3,    45,    50,    50,
       0,    60,     3,     4,     5,     6,     9,    10,    15,    16,
      22,    24,    27,    40,    49,    50,    52,    65,    67,    82,
      83,    84,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,     3,    15,    51,    80,    81,
      44,    50,    45,    47,    54,     6,     6,    50,    50,    50,
      96,    96,    82,     3,    66,    82,    19,    76,    38,    43,
      37,    38,    39,    37,    31,    32,    33,    34,    35,    36,
      41,    42,    26,    27,    28,    29,    30,    20,    48,    50,
      52,    44,    49,    44,    44,    47,    51,    54,    65,    67,
      80,    54,    16,     3,    12,    13,    17,    18,    25,    56,
      69,    70,    71,    72,    73,    74,    75,    82,    97,    51,
      51,     6,     6,    82,    51,    44,    47,    53,    54,    87,
      85,    86,    88,    89,    90,    91,    92,    92,    93,    93,
      93,    93,    94,    94,    95,    95,    96,    96,    96,     3,
       3,     3,    15,    51,    82,    99,   100,    82,    82,    82,
      82,    81,    44,     3,    64,    51,    69,    44,    44,     3,
      97,     3,     3,    50,    82,    55,    70,    44,    51,    51,
      47,    51,    82,    21,    52,    82,     3,    11,    12,    15,
      77,    78,    79,    82,    97,    46,    50,    44,    44,    47,
      51,    53,    76,    82,    45,    56,     3,    55,    44,    55,
      82,    82,    44,    20,    82,    45,    82,    82,    23,     4,
      44,    45,     3,    44,    55,    78,    55,    79,    44,    86,
      51,    99,    82,    82,   100,    76,    65,     3,    45,    56,
      54,    65,    45,    82,     3,    14,    51,    54,    70,    51,
      82,    53,    82,    54,    44,    82,    55,    82,    51,    45,
      65,     3,    64,    54,    45,    45,    45,    69,    45,    78,
      82,    65,    45,    55,    69,    54,    54,    54,    70,    55,
      82,    55,    65,    55,    69,    69,    69,    53,    55,    55,
      55
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    57,    58,    58,    59,    59,    60,    60,    60,    60,
      60,    60,    61,    61,    62,    62,    62,    62,    62,    62,
      63,    63,    63,    63,    64,    64,    64,    64,    65,    65,
      66,    66,    66,    66,    67,    68,    68,    68,    69,    69,
      70,    70,    70,    70,    70,    70,    70,    70,    70,    71,
      71,    72,    72,    73,    73,    74,    75,    75,    76,    76,
      76,    77,    78,    78,    79,    79,    79,    79,    79,    80,
      80,    81,    81,    81,    81,    82,    83,    84,    84,    85,
      85,    86,    86,    87,    87,    88,    88,    89,    89,    90,
      90,    91,    91,    91,    92,    92,    92,    92,    92,    93,
      93,    93,    94,    94,    94,    95,    95,    95,    95,    96,
      96,    96,    96,    96,    97,    97,    97,    97,    97,    97,
      97,    97,    98,    98,    98,    98,    98,    98,    98,    98,
      98,    98,    98,    99,    99,   100,   100,   100
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     1,     0,     2,     1,     1,     1,     4,     6,
       4,     6,     1,     1,     6,     5,     7,     6,     3,     4,
       4,     7,     6,     9,     4,     6,     3,     5,     3,     1,
       3,     5,     3,     1,     9,     5,    10,     6,     2,     1,
       3,     3,     4,     1,     1,     1,     1,     1,     1,     8,
       6,     6,     4,     2,     1,     2,     8,     8,     5,     4,
       4,     5,     2,     1,     3,     3,     4,     3,     1,     3,
       1,     1,     2,     3,     3,     1,     1,     1,     3,     3,
       1,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     3,     1,     3,     3,     3,     3,     1,
       3,     3,     1,     3,     3,     1,     3,     3,     3,     1,
       2,     2,     4,     6,     1,     4,     3,     3,     3,     6,
       5,     4,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     3,     1,     1,     3,     3
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "YYEOF", "error", "\"invalid token\"", "IDENTIFIER", "NUMBER",
  "TIME_LITERAL", "STRING_LITERAL", "FLOW", "PROCESS", "IMPORT", "MODULE",
  "INIT", "EMIT", "CATCH", "END", "DT", "DUR", "STOP", "START",
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


#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   113,   113,   120,   129,   134,   142,   143,   144,   152,
     161,   169,   181,   182,   186,   196,   206,   217,   228,   238,
     252,   267,   282,   292,   305,   315,   326,   336,   350,   362,
     366,   371,   388,   397,   401,   419,   430,   442,   456,   461,
     469,   518,   528,   538,   542,   546,   550,   554,   558,   569,
     578,   590,   599,   611,   619,   630,   641,   651,   664,   673,
     682,   693,   704,   709,   717,   756,   766,   776,   786,   797,
     802,   806,   815,   824,   833,   846,   850,   854,   855,   883,
     888,   895,   896,   913,   914,   931,   932,   949,   950,   967,
     968,   985,   986,  1000,  1017,  1018,  1032,  1046,  1060,  1077,
    1078,  1092,  1109,  1110,  1124,  1141,  1142,  1156,  1170,  1187,
    1188,  1201,  1214,  1226,  1242,  1243,  1256,  1269,  1282,  1296,
    1310,  1324,  1340,  1352,  1364,  1393,  1404,  1415,  1427,  1439,
    1451,  1459,  1460,  1464,  1469,  1473,  1482,  1491
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
#line 3768 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"

#line 1502 "/home/johann/Documents/git/idyl/src/parser/idyl.y"


void yy::parser::error(const location_type& l, const std::string& m) {
    std::cerr << l << ": " << m << std::endl;
}
