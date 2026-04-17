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
#line 115 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto prog = std::make_shared<idyl::parser::program>();
        prog->statements_ = yystack_[0].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        yylhs.value.as < idyl::parser::node_ptr > () = prog;
        g_program = prog;
    }
#line 1099 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 3: // program: %empty
#line 122 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto prog = std::make_shared<idyl::parser::program>();
        yylhs.value.as < idyl::parser::node_ptr > () = prog;
        g_program = prog;
    }
#line 1109 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 4: // top_level_statements: top_level_statements top_level_statement
#line 131 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1118 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 5: // top_level_statements: top_level_statement
#line 136 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = {};
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1127 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 6: // top_level_statement: function_or_flow_definition
#line 143 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                  { yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < idyl::parser::stmt_ptr > (); }
#line 1133 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 7: // top_level_statement: process_block
#line 144 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                    { yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::process_block> > (); }
#line 1139 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 8: // top_level_statement: IMPORT LPAREN STRING_LITERAL RPAREN
#line 146 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 154 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 163 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 171 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 182 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                          { yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::function_definition> > (); }
#line 1195 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 13: // function_or_flow_definition: flow_definition
#line 183 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                      { yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::flow_definition> > (); }
#line 1201 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 14: // function_definition: IDENTIFIER LPAREN parameter_list RPAREN ASSIGN expression
#line 188 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 198 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 208 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 219 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 230 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 240 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 254 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 269 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 284 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 294 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 307 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 317 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 328 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 338 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 352 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 363 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                           { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1434 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 30: // flow_elements: flow_elements COMMA expression
#line 368 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = yystack_[2].value.as < std::vector<idyl::parser::expr_ptr> > ();
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::expr_ptr > ());
    }
#line 1443 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 31: // flow_elements: flow_elements COMMA LBRACKET NUMBER RBRACKET
#line 373 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 390 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 398 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                 { yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = {yystack_[0].value.as < idyl::parser::expr_ptr > ()}; }
#line 1483 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 34: // generator_expression: LBRACKET IDENTIFIER ASSIGN expression RANGE expression COLON expression RBRACKET
#line 403 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 421 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 432 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 444 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 458 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1557 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 39: // process_body_statements: process_body_statement
#line 463 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = {};
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1566 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 40: // process_body_statement: IDENTIFIER ASSIGN expression
#line 471 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = yystack_[2].value.as < std::string > ();
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = false;
        assign->line_ = yystack_[2].location.begin.line;
        assign->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1580 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 41: // process_body_statement: EMIT IDENTIFIER ASSIGN expression
#line 481 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = yystack_[2].value.as < std::string > ();
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = true;
        assign->line_ = yystack_[3].location.begin.line;
        assign->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1594 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 42: // process_body_statement: stop_statement
#line 491 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::stop_statement> > ();
    }
#line 1602 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 43: // process_body_statement: start_statement
#line 495 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::start_statement> > ();
    }
#line 1610 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 44: // process_body_statement: expression catch_block
#line 499 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto catch_b = std::static_pointer_cast<idyl::parser::catch_block>(yystack_[0].value.as < std::shared_ptr<idyl::parser::catch_block> > ());
        catch_b->expression_ = yystack_[1].value.as < idyl::parser::expr_ptr > ();
        yylhs.value.as < idyl::parser::stmt_ptr > () = catch_b;
    }
#line 1620 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 45: // process_body_statement: at_block
#line 505 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::at_block> > ();
    }
#line 1628 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 46: // process_body_statement: on_block
#line 509 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < idyl::parser::stmt_ptr > () = yystack_[0].value.as < std::shared_ptr<idyl::parser::on_block> > ();
    }
#line 1636 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 47: // process_body_statement: expression
#line 513 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto es = std::make_shared<idyl::parser::expression_stmt>();
        es->expression_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        es->line_ = yystack_[0].location.begin.line;
        es->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = es;
    }
#line 1648 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 48: // at_block: AT_OP LPAREN expression RPAREN COLON LBRACE process_body_statements RBRACE
#line 524 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto at_stmt = std::make_shared<idyl::parser::at_block>();
        at_stmt->time_expr_ = yystack_[5].value.as < idyl::parser::expr_ptr > ();
        at_stmt->handler_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        at_stmt->line_ = yystack_[7].location.begin.line;
        at_stmt->column_ = yystack_[7].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::at_block> > () = at_stmt;
    }
#line 1661 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 49: // at_block: AT_OP LPAREN expression RPAREN COLON process_body_statement
#line 533 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto at_stmt = std::make_shared<idyl::parser::at_block>();
        at_stmt->time_expr_ = yystack_[3].value.as < idyl::parser::expr_ptr > ();
        at_stmt->handler_ = {yystack_[0].value.as < idyl::parser::stmt_ptr > ()};
        at_stmt->line_ = yystack_[5].location.begin.line;
        at_stmt->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::at_block> > () = at_stmt;
    }
#line 1674 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 50: // on_block: ON expression COLON LBRACE process_body_statements RBRACE
#line 545 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto on_stmt = std::make_shared<idyl::parser::on_block>();
        on_stmt->trigger_expr_ = yystack_[4].value.as < idyl::parser::expr_ptr > ();
        on_stmt->handler_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        on_stmt->line_ = yystack_[5].location.begin.line;
        on_stmt->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::on_block> > () = on_stmt;
    }
#line 1687 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 51: // on_block: ON expression COLON process_body_statement
#line 554 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto on_stmt = std::make_shared<idyl::parser::on_block>();
        on_stmt->trigger_expr_ = yystack_[2].value.as < idyl::parser::expr_ptr > ();
        on_stmt->handler_ = {yystack_[0].value.as < idyl::parser::stmt_ptr > ()};
        on_stmt->line_ = yystack_[3].location.begin.line;
        on_stmt->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::on_block> > () = on_stmt;
    }
#line 1700 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 52: // stop_statement: STOP IDENTIFIER
#line 566 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto stop_stmt = std::make_shared<idyl::parser::stop_statement>();
        stop_stmt->target_ = yystack_[0].value.as < std::string > ();
        stop_stmt->line_ = yystack_[1].location.begin.line;
        stop_stmt->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::stop_statement> > () = stop_stmt;
    }
#line 1712 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 53: // stop_statement: STOP
#line 574 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto stop_stmt = std::make_shared<idyl::parser::stop_statement>();
        stop_stmt->target_ = "";
        stop_stmt->line_ = yystack_[0].location.begin.line;
        stop_stmt->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::stop_statement> > () = stop_stmt;
    }
#line 1724 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 54: // start_statement: START IDENTIFIER
#line 585 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto start_stmt = std::make_shared<idyl::parser::start_statement>();
        start_stmt->target_ = yystack_[0].value.as < std::string > ();
        start_stmt->line_ = yystack_[1].location.begin.line;
        start_stmt->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::start_statement> > () = start_stmt;
    }
#line 1736 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 55: // catch_block: CATCH END COLON LBRACE process_body_statements RBRACE
#line 596 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto catch_b = std::make_shared<idyl::parser::catch_block>();
        catch_b->event_type_ = "end";
        catch_b->handler_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        catch_b->line_ = yystack_[5].location.begin.line;
        catch_b->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::catch_block> > () = catch_b;
    }
#line 1749 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 56: // catch_block: CATCH IDENTIFIER COLON LBRACE process_body_statements RBRACE
#line 605 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto catch_b = std::make_shared<idyl::parser::catch_block>();
        catch_b->event_type_ = yystack_[4].value.as < std::string > ();
        catch_b->handler_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        catch_b->line_ = yystack_[5].location.begin.line;
        catch_b->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::catch_block> > () = catch_b;
    }
#line 1762 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 57: // lambda_block: LAMBDA_BLOCK LBRACE init_block lambda_statements RBRACE
#line 617 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->init_ = yystack_[2].value.as < std::shared_ptr<idyl::parser::init_block> > ();
        lambda->update_statements_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        lambda->line_ = yystack_[4].location.begin.line;
        lambda->column_ = yystack_[4].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::lambda_block> > () = lambda;
    }
#line 1775 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 58: // lambda_block: LAMBDA_BLOCK LBRACE init_block RBRACE
#line 626 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->init_ = yystack_[1].value.as < std::shared_ptr<idyl::parser::init_block> > ();
        lambda->update_statements_ = {};
        lambda->line_ = yystack_[3].location.begin.line;
        lambda->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::lambda_block> > () = lambda;
    }
#line 1788 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 59: // lambda_block: LAMBDA_BLOCK LBRACE lambda_statements RBRACE
#line 635 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->update_statements_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        lambda->line_ = yystack_[3].location.begin.line;
        lambda->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::lambda_block> > () = lambda;
    }
#line 1800 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 60: // init_block: INIT COLON LBRACE lambda_statements RBRACE
#line 646 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto init = std::make_shared<idyl::parser::init_block>();
        init->statements_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        init->line_ = yystack_[4].location.begin.line;
        init->column_ = yystack_[4].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::init_block> > () = init;
    }
#line 1812 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 61: // lambda_statements: lambda_statements lambda_statement
#line 657 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1821 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 62: // lambda_statements: lambda_statement
#line 662 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = {};
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1830 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 63: // lambda_statement: IDENTIFIER ASSIGN expression
#line 670 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = yystack_[2].value.as < std::string > ();
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = false;
        assign->line_ = yystack_[2].location.begin.line;
        assign->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1844 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 64: // lambda_statement: EMIT IDENTIFIER ASSIGN expression
#line 680 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = yystack_[2].value.as < std::string > ();
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = true;
        assign->line_ = yystack_[3].location.begin.line;
        assign->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1858 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 65: // lambda_statement: DT ASSIGN expression
#line 690 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = "dt";
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = false;
        assign->line_ = yystack_[2].location.begin.line;
        assign->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1872 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 66: // lambda_statement: expression
#line 700 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto es = std::make_shared<idyl::parser::expression_stmt>();
        es->expression_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        es->line_ = yystack_[0].location.begin.line;
        es->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = es;
    }
#line 1884 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 67: // parameter_list: parameter_list COMMA parameter
#line 711 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::param_ptr> > () = yystack_[2].value.as < std::vector<idyl::parser::param_ptr> > ();
        if (yystack_[0].value.as < idyl::parser::param_ptr > ()) yylhs.value.as < std::vector<idyl::parser::param_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::param_ptr > ());
    }
#line 1893 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 68: // parameter_list: parameter
#line 715 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                { yylhs.value.as < std::vector<idyl::parser::param_ptr> > () = {yystack_[0].value.as < idyl::parser::param_ptr > ()}; }
#line 1899 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 69: // parameter: IDENTIFIER
#line 720 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = yystack_[0].value.as < std::string > ();
        param->is_trigger_parameter_ = false;
        param->line_ = yystack_[0].location.begin.line;
        param->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 1912 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 70: // parameter: IDENTIFIER TRIGGER
#line 729 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = yystack_[1].value.as < std::string > ();
        param->is_trigger_parameter_ = true;
        param->line_ = yystack_[1].location.begin.line;
        param->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 1925 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 71: // parameter: IDENTIFIER ASSIGN expression
#line 738 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = yystack_[2].value.as < std::string > ();
        param->default_value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        param->line_ = yystack_[2].location.begin.line;
        param->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 1938 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 72: // parameter: DT ASSIGN expression
#line 747 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = "dt";
        param->default_value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        param->has_default_time_ = true;
        param->line_ = yystack_[2].location.begin.line;
        param->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 1952 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 73: // expression: assignment_expression
#line 759 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1958 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 74: // assignment_expression: ternary_expression
#line 763 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1964 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 75: // ternary_expression: logical_or_expression
#line 767 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1970 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 76: // ternary_expression: ternary_options QUESTION logical_or_expression
#line 769 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1987 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 77: // ternary_options: ternary_options SEMICOLON logical_or_expression
#line 785 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = yystack_[2].value.as < std::vector<idyl::parser::expr_ptr> > ();
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::expr_ptr > ());
    }
#line 1996 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 78: // ternary_options: logical_or_expression
#line 790 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = {yystack_[0].value.as < idyl::parser::expr_ptr > ()};
    }
#line 2004 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 79: // logical_or_expression: logical_and_expression
#line 796 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2010 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 80: // logical_or_expression: logical_or_expression OR logical_and_expression
#line 798 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2028 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 81: // logical_and_expression: bitwise_or_expression
#line 814 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2034 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 82: // logical_and_expression: logical_and_expression AND bitwise_or_expression
#line 816 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2052 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 83: // bitwise_or_expression: bitwise_xor_expression
#line 832 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2058 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 84: // bitwise_or_expression: bitwise_or_expression OR bitwise_xor_expression
#line 834 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2076 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 85: // bitwise_xor_expression: bitwise_and_expression
#line 850 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2082 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 86: // bitwise_xor_expression: bitwise_xor_expression XOR bitwise_and_expression
#line 852 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2100 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 87: // bitwise_and_expression: equality_expression
#line 868 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                          { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2106 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 88: // bitwise_and_expression: bitwise_and_expression AND equality_expression
#line 870 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2124 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 89: // equality_expression: relational_expression
#line 886 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2130 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 90: // equality_expression: equality_expression EQ relational_expression
#line 888 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2148 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 91: // equality_expression: equality_expression NEQ relational_expression
#line 902 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2166 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 92: // relational_expression: shift_expression
#line 918 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                       { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2172 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 93: // relational_expression: relational_expression LT shift_expression
#line 920 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2190 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 94: // relational_expression: relational_expression GT shift_expression
#line 934 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2208 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 95: // relational_expression: relational_expression LE shift_expression
#line 948 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2226 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 96: // relational_expression: relational_expression GE shift_expression
#line 962 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2244 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 97: // shift_expression: additive_expression
#line 978 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                          { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2250 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 98: // shift_expression: shift_expression LSHIFT additive_expression
#line 980 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2268 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 99: // shift_expression: shift_expression RSHIFT additive_expression
#line 994 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2286 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 100: // additive_expression: multiplicative_expression
#line 1010 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2292 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 101: // additive_expression: additive_expression PLUS multiplicative_expression
#line 1012 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2310 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 102: // additive_expression: additive_expression MINUS multiplicative_expression
#line 1026 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2328 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 103: // multiplicative_expression: unary_expression
#line 1042 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                       { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2334 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 104: // multiplicative_expression: multiplicative_expression MUL unary_expression
#line 1044 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2352 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 105: // multiplicative_expression: multiplicative_expression DIV unary_expression
#line 1058 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2370 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 106: // multiplicative_expression: multiplicative_expression MOD unary_expression
#line 1072 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2388 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 107: // unary_expression: postfix_expression
#line 1088 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2394 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 108: // unary_expression: NOT unary_expression
#line 1090 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2411 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 109: // unary_expression: MINUS unary_expression
#line 1103 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2428 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 110: // unary_expression: MEMORY_OP LPAREN expression RPAREN
#line 1116 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2444 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 111: // unary_expression: MEMORY_OP LPAREN expression COMMA expression RPAREN
#line 1128 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2461 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 112: // postfix_expression: primary_expression
#line 1143 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2467 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 113: // postfix_expression: postfix_expression LPAREN argument_list RPAREN
#line 1145 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2484 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 114: // postfix_expression: postfix_expression LPAREN RPAREN
#line 1158 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2501 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 115: // postfix_expression: postfix_expression DOT IDENTIFIER
#line 1171 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2518 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 116: // postfix_expression: postfix_expression NAMESPACE_DOT IDENTIFIER
#line 1184 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2536 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 117: // postfix_expression: postfix_expression NAMESPACE_DOT IDENTIFIER LPAREN argument_list RPAREN
#line 1198 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2554 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 118: // postfix_expression: postfix_expression NAMESPACE_DOT IDENTIFIER LPAREN RPAREN
#line 1212 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2572 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 119: // postfix_expression: postfix_expression LBRACKET expression RBRACKET
#line 1226 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2589 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 120: // primary_expression: IDENTIFIER
#line 1242 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2605 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 121: // primary_expression: NUMBER
#line 1254 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2621 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 122: // primary_expression: TIME_LITERAL
#line 1266 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2654 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 123: // primary_expression: TRIGGER
#line 1295 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2669 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 124: // primary_expression: REST
#line 1306 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2684 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 125: // primary_expression: DT
#line 1317 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2700 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 126: // primary_expression: DUR
#line 1329 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2716 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 127: // primary_expression: STRING_LITERAL
#line 1341 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2732 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 128: // primary_expression: LPAREN expression RPAREN
#line 1353 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto paren = std::make_shared<idyl::parser::parenthesized_expr>();
        paren->expr_ = yystack_[1].value.as < idyl::parser::expr_ptr > ();
        paren->line_ = yystack_[2].location.begin.line;
        paren->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = paren;
    }
#line 2744 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 129: // primary_expression: flow_literal
#line 1360 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                   { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2750 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 130: // primary_expression: generator_expression
#line 1361 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                           { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2756 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 131: // argument_list: argument_list COMMA argument
#line 1366 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > () = yystack_[2].value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        if (yystack_[0].value.as < std::shared_ptr<idyl::parser::argument> > ()) yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > ().push_back(yystack_[0].value.as < std::shared_ptr<idyl::parser::argument> > ());
    }
#line 2765 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 132: // argument_list: argument
#line 1370 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
               { yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > () = {yystack_[0].value.as < std::shared_ptr<idyl::parser::argument> > ()}; }
#line 2771 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 133: // argument: expression
#line 1375 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = "";
        arg->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        arg->line_ = yystack_[0].location.begin.line;
        arg->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::argument> > () = arg;
    }
#line 2784 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 134: // argument: IDENTIFIER ASSIGN expression
#line 1384 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = yystack_[2].value.as < std::string > ();
        arg->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        arg->line_ = yystack_[2].location.begin.line;
        arg->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::argument> > () = arg;
    }
#line 2797 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 135: // argument: DT ASSIGN expression
#line 1393 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = "dt";
        arg->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        arg->line_ = yystack_[2].location.begin.line;
        arg->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::argument> > () = arg;
    }
#line 2810 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;


#line 2814 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"

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


  const short parser::yypact_ninf_ = -199;

  const signed char parser::yytable_ninf_ = -79;

  const short
  parser::yypact_[] =
  {
      77,    -4,    14,     8,   -31,    41,    55,    77,  -199,  -199,
    -199,  -199,  -199,   682,    10,    12,    69,    40,    97,   100,
    -199,  -199,  -199,  -199,  -199,  -199,    63,    82,  -199,  -199,
      91,  -199,   863,   863,  -199,   863,   892,  -199,  -199,    39,
    -199,  -199,    28,    35,   117,    90,   108,   119,    16,     2,
      22,   135,   110,  -199,    52,  -199,   -29,   123,   128,   -23,
    -199,    92,    11,   121,   161,   535,   133,   143,   190,   191,
     863,  -199,  -199,   149,   157,    48,  -199,   148,  -199,   863,
     863,   863,   863,   863,   863,   863,   863,   863,   863,   863,
     863,   863,   863,   863,   863,   863,   863,   863,   863,   201,
     205,   742,   863,   863,  -199,   863,   863,    11,   165,   207,
    -199,  -199,    72,   535,   167,   169,   212,   221,   222,   176,
     863,    27,  -199,  -199,  -199,  -199,  -199,   214,  -199,  -199,
     177,   179,    78,  -199,   863,   774,  -199,   713,   193,   193,
     117,  -199,   108,  -199,    16,     2,     2,    22,    22,    22,
      22,   210,   210,   110,   110,  -199,  -199,  -199,   187,  -199,
     194,   196,  -199,  -199,    79,  -199,   195,  -199,  -199,    39,
    -199,   863,   -35,     4,   199,    93,   863,   863,   203,  -199,
    -199,   863,   204,  -199,  -199,    13,  -199,  -199,  -199,   863,
    -199,   227,  -199,   921,  -199,   208,   206,   250,   211,   566,
     595,  -199,  -199,   803,   863,   863,   950,  -199,  -199,  -199,
      39,   202,   253,   -27,  -199,    98,  -199,   213,  -199,   863,
     219,   217,   216,   218,   228,   863,   209,   863,   224,   220,
     863,  -199,   624,  -199,  -199,  -199,    80,  -199,  -199,  -199,
    -199,  -199,   223,   202,   262,   207,  -199,   226,  -199,   236,
     535,  -199,   230,   231,  -199,   237,  -199,  -199,   834,   863,
    -199,  -199,  -199,   202,  -199,   245,     5,   535,   271,   325,
     535,   535,   863,   653,  -199,  -199,   202,  -199,   367,   535,
    -199,  -199,   409,   451,   238,  -199,  -199,  -199,   493,  -199,
    -199,  -199,  -199
  };

  const unsigned char
  parser::yydefact_[] =
  {
       3,     0,     0,     0,     0,     0,     0,     2,     5,     6,
      12,    13,     7,     0,     0,     0,     0,     0,     0,     0,
       1,     4,   120,   121,   122,   127,     0,     0,   125,   126,
       0,   124,     0,     0,   123,     0,     0,   129,    29,    18,
      73,    74,     0,    75,    79,    81,    83,    85,    87,    89,
      92,    97,   100,   103,   107,   112,    69,     0,     0,     0,
      68,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   109,   108,     0,   120,     0,    33,     0,    19,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    70,     0,     0,     0,     0,     0,
      20,    29,     0,     0,     0,   120,     0,    53,     0,     0,
       0,     0,    39,    45,    46,    42,    43,    47,     8,    10,
       0,     0,     0,   128,     0,     0,    28,     0,    76,    77,
      80,    82,    84,    86,    88,    90,    91,    93,    94,    95,
      96,    98,    99,   101,   102,   104,   105,   106,   116,   115,
     120,   125,   114,   133,     0,   132,     0,    71,    72,    15,
      67,     0,     0,     0,     0,     0,     0,     0,     0,    52,
      54,     0,     0,    35,    38,     0,    44,     9,    11,     0,
     110,     0,    32,     0,    30,   120,     0,     0,   125,     0,
       0,    62,    66,     0,     0,     0,     0,   113,   119,    17,
      14,     0,     0,     0,    22,     0,    37,     0,    40,     0,
       0,     0,     0,     0,     0,     0,   121,     0,     0,     0,
       0,    58,     0,    59,    61,   118,     0,   134,   135,   131,
      16,    26,     0,     0,     0,     0,    21,     0,    41,     0,
       0,    51,     0,     0,   111,     0,    31,    63,     0,     0,
      65,    57,   117,     0,    24,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    64,    27,     0,    23,     0,     0,
      49,    50,     0,     0,     0,    60,    25,    36,     0,    56,
      55,    34,    48
  };

  const short
  parser::yypgoto_[] =
  {
    -199,  -199,  -199,   265,  -199,  -199,  -199,    47,   -60,  -199,
     -58,  -199,   -72,  -109,  -199,  -199,  -199,  -199,  -199,  -160,
    -199,  -193,  -198,   232,   192,   -13,  -199,  -199,  -199,    86,
     225,   215,   229,   233,   234,    84,    46,    89,    94,   -28,
    -199,  -199,    99,    95
  };

  const unsigned char
  parser::yydefgoto_[] =
  {
       0,     6,     7,     8,     9,    10,    11,   173,    37,    75,
      38,    12,   121,   122,   123,   124,   125,   126,   186,    78,
     199,   200,   201,    59,    60,   127,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,   164,   165
  };

  const short
  parser::yytable_[] =
  {
      39,   110,   234,   111,    71,    72,   232,   213,   213,   209,
     211,    16,   184,    56,    56,   103,   222,    15,   243,    18,
     104,   212,    73,    76,   107,    57,    57,   223,   108,   244,
     115,    23,    24,    25,   234,    88,    89,    90,    91,   116,
      13,   175,    28,    29,   117,   118,    14,    86,    87,    30,
     240,    31,   119,    17,    32,    20,    61,   132,    77,   214,
     277,    58,    62,    92,    93,   273,   184,    33,   155,   156,
     157,    79,    99,    81,    80,   234,    34,    35,   -78,    36,
       1,   -78,   183,   120,     2,     3,     4,     5,   163,   166,
     167,    19,   168,   169,    65,   135,   115,    23,    24,    25,
     100,   136,   101,    66,   102,   116,    67,   182,    28,    29,
     117,   118,   251,    68,    63,    30,    64,    31,   119,   107,
      32,   191,   194,   174,   202,   189,   206,   206,    83,   190,
     207,   262,    69,    33,   147,   148,   149,   150,    96,    97,
      98,    70,    34,    35,    36,    36,   109,    84,   216,   120,
      36,   241,   245,   111,    82,   246,    85,   111,   210,   280,
     184,    94,    95,   217,   218,   138,   139,   105,   220,   184,
     145,   146,   106,   184,   184,   113,   224,   114,   269,   184,
      76,   151,   152,   264,   128,   111,   202,   202,   153,   154,
     163,   237,   238,   163,   129,   278,   130,   131,   282,   283,
     133,   134,   137,   275,   158,   111,   248,   288,   159,   171,
     172,   176,   255,   177,   257,   178,   286,   260,   111,   202,
     115,    23,    24,    25,   179,   180,   181,   185,   187,   116,
     188,    81,    28,    29,   117,   118,    94,   203,   204,    30,
     205,    31,   119,   215,    32,   202,   274,   219,   208,   221,
     225,   228,   227,   229,    36,   230,   242,    33,   247,   284,
     202,   252,   256,   253,   259,   265,    34,    35,   263,    36,
     249,   250,    21,   120,   115,    23,    24,    25,   258,   254,
     267,   268,   272,   116,   270,   271,    28,    29,   117,   118,
     276,   291,   266,    30,   112,    31,   119,   141,    32,   170,
       0,   239,   236,     0,     0,     0,   140,     0,     0,     0,
       0,    33,   142,     0,     0,     0,     0,   143,     0,   144,
      34,    35,     0,    36,     0,   279,     0,   120,   115,    23,
      24,    25,     0,     0,     0,     0,     0,   116,     0,     0,
      28,    29,   117,   118,     0,     0,     0,    30,     0,    31,
     119,     0,    32,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    33,     0,     0,     0,     0,
     115,    23,    24,    25,    34,    35,     0,    36,     0,   116,
     281,   120,    28,    29,   117,   118,     0,     0,     0,    30,
       0,    31,   119,     0,    32,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    33,     0,     0,
       0,     0,   115,    23,    24,    25,    34,    35,     0,    36,
       0,   116,   287,   120,    28,    29,   117,   118,     0,     0,
       0,    30,     0,    31,   119,     0,    32,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    33,
       0,     0,     0,     0,   115,    23,    24,    25,    34,    35,
       0,    36,     0,   116,   289,   120,    28,    29,   117,   118,
       0,     0,     0,    30,     0,    31,   119,     0,    32,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    33,     0,     0,     0,     0,   115,    23,    24,    25,
      34,    35,     0,    36,     0,   116,   290,   120,    28,    29,
     117,   118,     0,     0,     0,    30,     0,    31,   119,     0,
      32,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    33,     0,     0,     0,     0,   115,    23,
      24,    25,    34,    35,     0,    36,     0,   116,   292,   120,
      28,    29,   117,   118,     0,     0,     0,    30,     0,    31,
     119,     0,    32,     0,     0,     0,     0,     0,     0,   195,
      23,    24,    25,     0,     0,    33,     0,     0,   197,     0,
       0,   198,    29,     0,    34,    35,     0,    36,    30,     0,
      31,   120,     0,    32,     0,     0,     0,     0,   195,    23,
      24,    25,     0,     0,     0,     0,    33,   197,     0,     0,
     198,    29,     0,     0,     0,    34,    35,    30,    36,    31,
       0,   231,    32,     0,     0,     0,     0,   195,    23,    24,
      25,     0,     0,     0,     0,    33,   197,     0,     0,   198,
      29,     0,     0,     0,    34,    35,    30,    36,    31,     0,
     233,    32,     0,     0,     0,     0,   195,    23,    24,    25,
       0,     0,     0,     0,    33,   197,     0,     0,   198,    29,
       0,     0,     0,    34,    35,    30,    36,    31,     0,   261,
      32,     0,     0,     0,     0,    22,    23,    24,    25,     0,
       0,    26,    27,    33,     0,     0,     0,    28,    29,     0,
       0,     0,    34,    35,    30,    36,    31,     0,   285,    32,
       0,     0,     0,     0,     0,     0,   195,    23,    24,    25,
       0,     0,    33,     0,   196,   197,     0,     0,   198,    29,
       0,    34,    35,     0,    36,    30,     0,    31,     0,     0,
      32,     0,     0,     0,     0,   160,    23,    24,    25,     0,
       0,     0,     0,    33,     0,     0,     0,   161,    29,     0,
       0,     0,    34,    35,    30,    36,    31,     0,     0,    32,
       0,     0,     0,     0,     0,     0,     0,    22,    23,    24,
      25,     0,    33,     0,     0,     0,     0,     0,     0,    28,
      29,    34,    35,   162,    36,   192,    30,     0,    31,     0,
       0,    32,     0,     0,     0,     0,   160,    23,    24,    25,
       0,     0,     0,     0,    33,     0,     0,     0,   161,    29,
       0,     0,     0,    34,    35,    30,   193,    31,     0,     0,
      32,     0,     0,     0,     0,     0,     0,   195,    23,    24,
      25,     0,     0,    33,     0,     0,   197,     0,     0,   198,
      29,     0,    34,    35,   235,    36,    30,     0,    31,     0,
       0,    32,     0,     0,     0,     0,    22,    23,    24,    25,
       0,     0,     0,     0,    33,     0,     0,     0,    28,    29,
       0,     0,     0,    34,    35,    30,    36,    31,     0,     0,
      32,     0,     0,     0,     0,    74,    23,    24,    25,     0,
       0,     0,     0,    33,     0,     0,     0,    28,    29,     0,
       0,     0,    34,    35,    30,    36,    31,     0,     0,    32,
       0,     0,     0,     0,    74,   226,    24,    25,     0,     0,
       0,     0,    33,     0,     0,     0,    28,    29,     0,     0,
       0,    34,    35,    30,    36,    31,     0,     0,    32,     0,
       0,     0,     0,   160,    23,    24,    25,     0,     0,     0,
       0,    33,     0,     0,     0,   161,    29,     0,     0,     0,
      34,    35,    30,    36,    31,     0,     0,    32,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      33,     0,     0,     0,     0,     0,     0,     0,     0,    34,
      35,     0,    36
  };

  const short
  parser::yycheck_[] =
  {
      13,    61,   200,    61,    32,    33,   199,     3,     3,   169,
      45,     3,   121,     3,     3,    44,     3,     3,    45,    50,
      49,    56,    35,    36,    47,    15,    15,    14,    51,    56,
       3,     4,     5,     6,   232,    33,    34,    35,    36,    12,
      44,   113,    15,    16,    17,    18,    50,    31,    32,    22,
     210,    24,    25,    45,    27,     0,    44,    70,    19,    55,
      55,    51,    50,    41,    42,   258,   175,    40,    96,    97,
      98,    43,    20,    38,    46,   273,    49,    50,    43,    52,
       3,    46,    55,    56,     7,     8,     9,    10,   101,   102,
     103,    50,   105,   106,    54,    47,     3,     4,     5,     6,
      48,    53,    50,     6,    52,    12,     6,   120,    15,    16,
      17,    18,   221,    50,    45,    22,    47,    24,    25,    47,
      27,   134,   135,    51,   137,    47,    47,    47,    38,    51,
      51,    51,    50,    40,    88,    89,    90,    91,    28,    29,
      30,    50,    49,    50,    52,    52,    54,    39,    55,    56,
      52,   211,    54,   211,    37,   215,    37,   215,   171,   268,
     269,    26,    27,   176,   177,    79,    80,    44,   181,   278,
      86,    87,    44,   282,   283,    54,   189,    16,   250,   288,
     193,    92,    93,   243,    51,   243,   199,   200,    94,    95,
     203,   204,   205,   206,    51,   267,     6,     6,   270,   271,
      51,    44,    54,   263,     3,   263,   219,   279,     3,    44,
       3,    44,   225,    44,   227,     3,   276,   230,   276,   232,
       3,     4,     5,     6,     3,     3,    50,    13,    51,    12,
      51,    38,    15,    16,    17,    18,    26,    50,    44,    22,
      44,    24,    25,    44,    27,   258,   259,    44,    53,    45,
      23,    45,    44,     3,    52,    44,     3,    40,    45,   272,
     273,    45,    53,    45,    44,     3,    49,    50,    45,    52,
      51,    54,     7,    56,     3,     4,     5,     6,    54,    51,
      54,    45,    45,    12,    54,    54,    15,    16,    17,    18,
      45,    53,   245,    22,    62,    24,    25,    82,    27,   107,
      -1,   206,   203,    -1,    -1,    -1,    81,    -1,    -1,    -1,
      -1,    40,    83,    -1,    -1,    -1,    -1,    84,    -1,    85,
      49,    50,    -1,    52,    -1,    54,    -1,    56,     3,     4,
       5,     6,    -1,    -1,    -1,    -1,    -1,    12,    -1,    -1,
      15,    16,    17,    18,    -1,    -1,    -1,    22,    -1,    24,
      25,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,    49,    50,    -1,    52,    -1,    12,
      55,    56,    15,    16,    17,    18,    -1,    -1,    -1,    22,
      -1,    24,    25,    -1,    27,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,    49,    50,    -1,    52,
      -1,    12,    55,    56,    15,    16,    17,    18,    -1,    -1,
      -1,    22,    -1,    24,    25,    -1,    27,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,    49,    50,
      -1,    52,    -1,    12,    55,    56,    15,    16,    17,    18,
      -1,    -1,    -1,    22,    -1,    24,    25,    -1,    27,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    40,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
      49,    50,    -1,    52,    -1,    12,    55,    56,    15,    16,
      17,    18,    -1,    -1,    -1,    22,    -1,    24,    25,    -1,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    40,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,    49,    50,    -1,    52,    -1,    12,    55,    56,
      15,    16,    17,    18,    -1,    -1,    -1,    22,    -1,    24,
      25,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,    -1,    -1,    40,    -1,    -1,    12,    -1,
      -1,    15,    16,    -1,    49,    50,    -1,    52,    22,    -1,
      24,    56,    -1,    27,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,    -1,    -1,    -1,    -1,    40,    12,    -1,    -1,
      15,    16,    -1,    -1,    -1,    49,    50,    22,    52,    24,
      -1,    55,    27,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    -1,    40,    12,    -1,    -1,    15,
      16,    -1,    -1,    -1,    49,    50,    22,    52,    24,    -1,
      55,    27,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    -1,    40,    12,    -1,    -1,    15,    16,
      -1,    -1,    -1,    49,    50,    22,    52,    24,    -1,    55,
      27,    -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,
      -1,     9,    10,    40,    -1,    -1,    -1,    15,    16,    -1,
      -1,    -1,    49,    50,    22,    52,    24,    -1,    55,    27,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    -1,    40,    -1,    11,    12,    -1,    -1,    15,    16,
      -1,    49,    50,    -1,    52,    22,    -1,    24,    -1,    -1,
      27,    -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    -1,    40,    -1,    -1,    -1,    15,    16,    -1,
      -1,    -1,    49,    50,    22,    52,    24,    -1,    -1,    27,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,    40,    -1,    -1,    -1,    -1,    -1,    -1,    15,
      16,    49,    50,    51,    52,    21,    22,    -1,    24,    -1,
      -1,    27,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    -1,    40,    -1,    -1,    -1,    15,    16,
      -1,    -1,    -1,    49,    50,    22,    52,    24,    -1,    -1,
      27,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,    -1,    40,    -1,    -1,    12,    -1,    -1,    15,
      16,    -1,    49,    50,    51,    52,    22,    -1,    24,    -1,
      -1,    27,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    -1,    40,    -1,    -1,    -1,    15,    16,
      -1,    -1,    -1,    49,    50,    22,    52,    24,    -1,    -1,
      27,    -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    -1,    40,    -1,    -1,    -1,    15,    16,    -1,
      -1,    -1,    49,    50,    22,    52,    24,    -1,    -1,    27,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    -1,    40,    -1,    -1,    -1,    15,    16,    -1,    -1,
      -1,    49,    50,    22,    52,    24,    -1,    -1,    27,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,
      -1,    40,    -1,    -1,    -1,    15,    16,    -1,    -1,    -1,
      49,    50,    22,    52,    24,    -1,    -1,    27,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    -1,    52
  };

  const signed char
  parser::yystos_[] =
  {
       0,     3,     7,     8,     9,    10,    58,    59,    60,    61,
      62,    63,    68,    44,    50,     3,     3,    45,    50,    50,
       0,    60,     3,     4,     5,     6,     9,    10,    15,    16,
      22,    24,    27,    40,    49,    50,    52,    65,    67,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,     3,    15,    51,    80,
      81,    44,    50,    45,    47,    54,     6,     6,    50,    50,
      50,    96,    96,    82,     3,    66,    82,    19,    76,    43,
      46,    38,    37,    38,    39,    37,    31,    32,    33,    34,
      35,    36,    41,    42,    26,    27,    28,    29,    30,    20,
      48,    50,    52,    44,    49,    44,    44,    47,    51,    54,
      65,    67,    80,    54,    16,     3,    12,    17,    18,    25,
      56,    69,    70,    71,    72,    73,    74,    82,    51,    51,
       6,     6,    82,    51,    44,    47,    53,    54,    86,    86,
      87,    88,    89,    90,    91,    92,    92,    93,    93,    93,
      93,    94,    94,    95,    95,    96,    96,    96,     3,     3,
       3,    15,    51,    82,    99,   100,    82,    82,    82,    82,
      81,    44,     3,    64,    51,    69,    44,    44,     3,     3,
       3,    50,    82,    55,    70,    13,    75,    51,    51,    47,
      51,    82,    21,    52,    82,     3,    11,    12,    15,    77,
      78,    79,    82,    50,    44,    44,    47,    51,    53,    76,
      82,    45,    56,     3,    55,    44,    55,    82,    82,    44,
      82,    45,     3,    14,    82,    23,     4,    44,    45,     3,
      44,    55,    78,    55,    79,    51,    99,    82,    82,   100,
      76,    65,     3,    45,    56,    54,    65,    45,    82,    51,
      54,    70,    45,    45,    51,    82,    53,    82,    54,    44,
      82,    55,    51,    45,    65,     3,    64,    54,    45,    69,
      54,    54,    45,    78,    82,    65,    45,    55,    69,    54,
      70,    55,    69,    69,    82,    55,    65,    55,    69,    55,
      55,    53,    55
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    57,    58,    58,    59,    59,    60,    60,    60,    60,
      60,    60,    61,    61,    62,    62,    62,    62,    62,    62,
      63,    63,    63,    63,    64,    64,    64,    64,    65,    65,
      66,    66,    66,    66,    67,    68,    68,    68,    69,    69,
      70,    70,    70,    70,    70,    70,    70,    70,    71,    71,
      72,    72,    73,    73,    74,    75,    75,    76,    76,    76,
      77,    78,    78,    79,    79,    79,    79,    80,    80,    81,
      81,    81,    81,    82,    83,    84,    84,    85,    85,    86,
      86,    87,    87,    88,    88,    89,    89,    90,    90,    91,
      91,    91,    92,    92,    92,    92,    92,    93,    93,    93,
      94,    94,    94,    95,    95,    95,    95,    96,    96,    96,
      96,    96,    97,    97,    97,    97,    97,    97,    97,    97,
      98,    98,    98,    98,    98,    98,    98,    98,    98,    98,
      98,    99,    99,   100,   100,   100
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     1,     0,     2,     1,     1,     1,     4,     6,
       4,     6,     1,     1,     6,     5,     7,     6,     3,     4,
       4,     7,     6,     9,     4,     6,     3,     5,     3,     1,
       3,     5,     3,     1,     9,     5,    10,     6,     2,     1,
       3,     4,     1,     1,     2,     1,     1,     1,     8,     6,
       6,     4,     2,     1,     2,     6,     6,     5,     4,     4,
       5,     2,     1,     3,     4,     3,     1,     3,     1,     1,
       2,     3,     3,     1,     1,     1,     3,     3,     1,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     3,     1,     3,     3,     3,     3,     1,     3,     3,
       1,     3,     3,     1,     3,     3,     3,     1,     2,     2,
       4,     6,     1,     4,     3,     3,     3,     6,     5,     4,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       1,     3,     1,     1,     3,     3
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
       0,   114,   114,   121,   130,   135,   143,   144,   145,   153,
     162,   170,   182,   183,   187,   197,   207,   218,   229,   239,
     253,   268,   283,   293,   306,   316,   327,   337,   351,   363,
     367,   372,   389,   398,   402,   420,   431,   443,   457,   462,
     470,   480,   490,   494,   498,   504,   508,   512,   523,   532,
     544,   553,   565,   573,   584,   595,   604,   616,   625,   634,
     645,   656,   661,   669,   679,   689,   699,   710,   715,   719,
     728,   737,   746,   759,   763,   767,   768,   784,   789,   796,
     797,   814,   815,   832,   833,   850,   851,   868,   869,   886,
     887,   901,   918,   919,   933,   947,   961,   978,   979,   993,
    1010,  1011,  1025,  1042,  1043,  1057,  1071,  1088,  1089,  1102,
    1115,  1127,  1143,  1144,  1157,  1170,  1183,  1197,  1211,  1225,
    1241,  1253,  1265,  1294,  1305,  1316,  1328,  1340,  1352,  1360,
    1361,  1365,  1370,  1374,  1383,  1392
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
#line 3627 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"

#line 1403 "/home/johann/Documents/git/idyl/src/parser/idyl.y"


void yy::parser::error(const location_type& l, const std::string& m) {
    std::cerr << l << ": " << m << std::endl;
}
