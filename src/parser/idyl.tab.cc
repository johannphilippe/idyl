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

  case 41: // catch_block: CATCH END COLON LBRACE process_body_statements RBRACE
#line 472 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto catch_b = std::make_shared<idyl::parser::catch_block>();
        catch_b->event_type_ = "end";
        catch_b->handler_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        catch_b->line_ = yystack_[5].location.begin.line;
        catch_b->column_ = yystack_[5].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::catch_block> > () = catch_b;
    }
#line 1505 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 42: // lambda_block: LAMBDA_BLOCK LBRACE init_block lambda_statements RBRACE
#line 484 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->init_ = yystack_[2].value.as < std::shared_ptr<idyl::parser::init_block> > ();
        lambda->update_statements_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        lambda->line_ = yystack_[4].location.begin.line;
        lambda->column_ = yystack_[4].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::lambda_block> > () = lambda;
    }
#line 1518 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 43: // lambda_block: LAMBDA_BLOCK LBRACE lambda_statements RBRACE
#line 493 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->update_statements_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        lambda->line_ = yystack_[3].location.begin.line;
        lambda->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::lambda_block> > () = lambda;
    }
#line 1530 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 44: // init_block: INIT COLON LBRACE lambda_statements RBRACE
#line 504 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto init = std::make_shared<idyl::parser::init_block>();
        init->statements_ = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        init->line_ = yystack_[4].location.begin.line;
        init->column_ = yystack_[4].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::init_block> > () = init;
    }
#line 1542 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 45: // lambda_statements: lambda_statements lambda_statement
#line 515 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = yystack_[1].value.as < std::vector<idyl::parser::stmt_ptr> > ();
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1551 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 46: // lambda_statements: lambda_statement
#line 520 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > () = {};
        if (yystack_[0].value.as < idyl::parser::stmt_ptr > ()) yylhs.value.as < std::vector<idyl::parser::stmt_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::stmt_ptr > ());
    }
#line 1560 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 47: // lambda_statement: IDENTIFIER ASSIGN expression
#line 528 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = yystack_[2].value.as < std::string > ();
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = false;
        assign->line_ = yystack_[2].location.begin.line;
        assign->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1574 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 48: // lambda_statement: EMIT IDENTIFIER ASSIGN expression
#line 538 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = yystack_[2].value.as < std::string > ();
        assign->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        assign->is_emit_ = true;
        assign->line_ = yystack_[3].location.begin.line;
        assign->column_ = yystack_[3].location.begin.column;
        yylhs.value.as < idyl::parser::stmt_ptr > () = assign;
    }
#line 1588 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 49: // parameter_list: parameter_list COMMA parameter
#line 551 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::param_ptr> > () = yystack_[2].value.as < std::vector<idyl::parser::param_ptr> > ();
        if (yystack_[0].value.as < idyl::parser::param_ptr > ()) yylhs.value.as < std::vector<idyl::parser::param_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::param_ptr > ());
    }
#line 1597 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 50: // parameter_list: parameter
#line 555 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                { yylhs.value.as < std::vector<idyl::parser::param_ptr> > () = {yystack_[0].value.as < idyl::parser::param_ptr > ()}; }
#line 1603 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 51: // parameter: IDENTIFIER
#line 560 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = yystack_[0].value.as < std::string > ();
        param->is_trigger_parameter_ = false;
        param->line_ = yystack_[0].location.begin.line;
        param->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 1616 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 52: // parameter: IDENTIFIER TRIGGER
#line 569 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = yystack_[1].value.as < std::string > ();
        param->is_trigger_parameter_ = true;
        param->line_ = yystack_[1].location.begin.line;
        param->column_ = yystack_[1].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 1629 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 53: // parameter: IDENTIFIER ASSIGN expression
#line 578 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = yystack_[2].value.as < std::string > ();
        param->default_value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        param->line_ = yystack_[2].location.begin.line;
        param->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 1642 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 54: // parameter: DT ASSIGN expression
#line 587 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = "dt";
        param->default_value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        param->has_default_time_ = true;
        param->line_ = yystack_[2].location.begin.line;
        param->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::param_ptr > () = param;
    }
#line 1656 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 55: // expression: assignment_expression
#line 599 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1662 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 56: // assignment_expression: ternary_expression
#line 603 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1668 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 57: // ternary_expression: logical_or_expression
#line 607 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1674 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 58: // ternary_expression: ternary_options QUESTION logical_or_expression
#line 609 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1691 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 59: // ternary_options: ternary_options SEMICOLON logical_or_expression
#line 625 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = yystack_[2].value.as < std::vector<idyl::parser::expr_ptr> > ();
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > ().push_back(yystack_[0].value.as < idyl::parser::expr_ptr > ());
    }
#line 1700 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 60: // ternary_options: logical_or_expression
#line 630 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<idyl::parser::expr_ptr> > () = {yystack_[0].value.as < idyl::parser::expr_ptr > ()};
    }
#line 1708 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 61: // logical_or_expression: logical_and_expression
#line 636 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1714 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 62: // logical_or_expression: logical_or_expression OR logical_and_expression
#line 638 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1732 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 63: // logical_and_expression: bitwise_or_expression
#line 654 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1738 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 64: // logical_and_expression: logical_and_expression AND bitwise_or_expression
#line 656 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1756 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 65: // bitwise_or_expression: bitwise_xor_expression
#line 672 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1762 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 66: // bitwise_or_expression: bitwise_or_expression OR bitwise_xor_expression
#line 674 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1780 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 67: // bitwise_xor_expression: bitwise_and_expression
#line 690 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                             { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1786 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 68: // bitwise_xor_expression: bitwise_xor_expression XOR bitwise_and_expression
#line 692 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1804 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 69: // bitwise_and_expression: equality_expression
#line 708 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                          { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1810 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 70: // bitwise_and_expression: bitwise_and_expression AND equality_expression
#line 710 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1828 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 71: // equality_expression: relational_expression
#line 726 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                            { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1834 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 72: // equality_expression: equality_expression EQ relational_expression
#line 728 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1852 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 73: // equality_expression: equality_expression NEQ relational_expression
#line 742 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1870 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 74: // relational_expression: shift_expression
#line 758 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                       { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1876 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 75: // relational_expression: relational_expression LT shift_expression
#line 760 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1894 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 76: // relational_expression: relational_expression GT shift_expression
#line 774 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1912 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 77: // relational_expression: relational_expression LE shift_expression
#line 788 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1930 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 78: // relational_expression: relational_expression GE shift_expression
#line 802 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1948 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 79: // shift_expression: additive_expression
#line 818 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                          { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1954 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 80: // shift_expression: shift_expression LSHIFT additive_expression
#line 820 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1972 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 81: // shift_expression: shift_expression RSHIFT additive_expression
#line 834 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 1990 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 82: // additive_expression: multiplicative_expression
#line 850 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                                { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 1996 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 83: // additive_expression: additive_expression PLUS multiplicative_expression
#line 852 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2014 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 84: // additive_expression: additive_expression MINUS multiplicative_expression
#line 866 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2032 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 85: // multiplicative_expression: unary_expression
#line 882 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                       { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2038 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 86: // multiplicative_expression: multiplicative_expression MUL unary_expression
#line 884 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2056 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 87: // multiplicative_expression: multiplicative_expression DIV unary_expression
#line 898 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2074 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 88: // multiplicative_expression: multiplicative_expression MOD unary_expression
#line 912 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2092 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 89: // unary_expression: postfix_expression
#line 928 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2098 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 90: // unary_expression: NOT unary_expression
#line 930 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2115 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 91: // unary_expression: MINUS unary_expression
#line 943 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2132 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 92: // unary_expression: MEMORY_OP LPAREN expression RPAREN
#line 956 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2148 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 93: // unary_expression: MEMORY_OP LPAREN expression COMMA expression RPAREN
#line 968 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2165 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 94: // postfix_expression: primary_expression
#line 983 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                         { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2171 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 95: // postfix_expression: postfix_expression LPAREN argument_list RPAREN
#line 985 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2188 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 96: // postfix_expression: postfix_expression LPAREN RPAREN
#line 998 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2205 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 97: // postfix_expression: postfix_expression DOT IDENTIFIER
#line 1011 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2222 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 98: // postfix_expression: postfix_expression NAMESPACE_DOT IDENTIFIER
#line 1024 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2240 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 99: // postfix_expression: postfix_expression NAMESPACE_DOT IDENTIFIER LPAREN argument_list RPAREN
#line 1038 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2258 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 100: // postfix_expression: postfix_expression NAMESPACE_DOT IDENTIFIER LPAREN RPAREN
#line 1052 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2276 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 101: // postfix_expression: postfix_expression LBRACKET expression RBRACKET
#line 1066 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2293 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 102: // primary_expression: IDENTIFIER
#line 1082 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2309 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 103: // primary_expression: NUMBER
#line 1094 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2325 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 104: // primary_expression: TIME_LITERAL
#line 1106 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2355 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 105: // primary_expression: TRIGGER
#line 1132 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2370 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 106: // primary_expression: REST
#line 1143 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2385 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 107: // primary_expression: DT
#line 1154 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2401 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 108: // primary_expression: STRING_LITERAL
#line 1166 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
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
#line 2417 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 109: // primary_expression: LPAREN expression RPAREN
#line 1178 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto paren = std::make_shared<idyl::parser::parenthesized_expr>();
        paren->expr_ = yystack_[1].value.as < idyl::parser::expr_ptr > ();
        paren->line_ = yystack_[2].location.begin.line;
        paren->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < idyl::parser::expr_ptr > () = paren;
    }
#line 2429 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 110: // primary_expression: flow_literal
#line 1185 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                   { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2435 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 111: // primary_expression: generator_expression
#line 1186 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
                           { yylhs.value.as < idyl::parser::expr_ptr > () = yystack_[0].value.as < idyl::parser::expr_ptr > (); }
#line 2441 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 112: // argument_list: argument_list COMMA argument
#line 1191 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > () = yystack_[2].value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > ();
        if (yystack_[0].value.as < std::shared_ptr<idyl::parser::argument> > ()) yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > ().push_back(yystack_[0].value.as < std::shared_ptr<idyl::parser::argument> > ());
    }
#line 2450 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 113: // argument_list: argument
#line 1195 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
               { yylhs.value.as < std::vector<std::shared_ptr<idyl::parser::argument>> > () = {yystack_[0].value.as < std::shared_ptr<idyl::parser::argument> > ()}; }
#line 2456 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 114: // argument: expression
#line 1200 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = "";
        arg->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        arg->line_ = yystack_[0].location.begin.line;
        arg->column_ = yystack_[0].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::argument> > () = arg;
    }
#line 2469 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 115: // argument: IDENTIFIER ASSIGN expression
#line 1209 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = yystack_[2].value.as < std::string > ();
        arg->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        arg->line_ = yystack_[2].location.begin.line;
        arg->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::argument> > () = arg;
    }
#line 2482 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;

  case 116: // argument: DT ASSIGN expression
#line 1218 "/home/johann/Documents/git/idyl/src/parser/idyl.y"
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = "dt";
        arg->value_ = yystack_[0].value.as < idyl::parser::expr_ptr > ();
        arg->line_ = yystack_[2].location.begin.line;
        arg->column_ = yystack_[2].location.begin.column;
        yylhs.value.as < std::shared_ptr<idyl::parser::argument> > () = arg;
    }
#line 2495 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"
    break;


#line 2499 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"

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


  const short parser::yypact_ninf_ = -181;

  const signed char parser::yytable_ninf_ = -61;

  const short
  parser::yypact_[] =
  {
      87,   -20,    22,     8,    47,    82,    54,    87,  -181,  -181,
    -181,  -181,  -181,   286,    24,   -16,    -3,    23,   127,   133,
    -181,  -181,  -181,  -181,  -181,  -181,    96,   104,  -181,   118,
    -181,   395,   395,  -181,   395,   400,  -181,  -181,   129,  -181,
    -181,    76,    42,   134,   132,   135,   136,    99,    93,    48,
     113,    74,  -181,    -1,  -181,    27,   128,   139,    35,  -181,
      16,    14,   138,   169,   314,   145,   147,   179,   180,   395,
    -181,  -181,   149,   157,   239,  -181,   148,  -181,   395,   395,
     395,   395,   395,   395,   395,   395,   395,   395,   395,   395,
     395,   395,   395,   395,   395,   395,   395,   395,   196,   197,
     342,   395,   395,  -181,   395,   395,    14,   161,   199,  -181,
    -181,    36,   314,   163,   165,   203,    29,  -181,   194,  -181,
    -181,   174,   175,    62,  -181,   395,  -181,   446,  -181,  -181,
      25,   178,   178,   134,  -181,   135,  -181,    99,    93,    93,
      48,    48,    48,    48,   193,   193,    74,    74,  -181,  -181,
    -181,   177,  -181,   187,   189,  -181,  -181,    64,  -181,   181,
    -181,  -181,   129,  -181,   395,   190,     9,   192,    98,   395,
     395,   195,  -181,  -181,   219,  -181,  -181,  -181,   395,  -181,
     214,   188,   198,   206,   233,    39,     6,  -181,   349,   395,
     395,   451,  -181,  -181,  -181,   129,   191,   207,  -181,    58,
    -181,   208,  -181,   395,   211,   212,   395,  -181,   395,   213,
     200,     7,  -181,  -181,  -181,    73,  -181,  -181,  -181,  -181,
    -181,   191,   199,  -181,   215,  -181,   216,  -181,   220,  -181,
      39,   395,  -181,  -181,  -181,    12,   314,   314,   395,    11,
    -181,  -181,   168,   205,   218,  -181,  -181,  -181,  -181
  };

  const signed char
  parser::yydefact_[] =
  {
       3,     0,     0,     0,     0,     0,     0,     2,     5,     6,
      12,    13,     7,     0,     0,     0,     0,     0,     0,     0,
       1,     4,   102,   103,   104,   108,     0,     0,   107,     0,
     106,     0,     0,   105,     0,     0,   110,    27,    18,    55,
      56,     0,    57,    61,    63,    65,    67,    69,    71,    74,
      79,    82,    85,    89,    94,    51,     0,     0,     0,    50,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      91,    90,     0,   102,     0,    31,     0,    19,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    52,     0,     0,     0,     0,     0,    20,
      27,     0,     0,     0,   102,     0,     0,    37,     0,     8,
      10,     0,     0,     0,   109,     0,    30,     0,    26,    28,
       0,    58,    59,    62,    64,    66,    68,    70,    72,    73,
      75,    76,    77,    78,    80,    81,    83,    84,    86,    87,
      88,    98,    97,   102,   107,    96,   114,     0,   113,     0,
      53,    54,    15,    49,     0,     0,     0,     0,     0,     0,
       0,     0,    33,    36,     0,    40,     9,    11,     0,    92,
       0,   103,     0,     0,     0,     0,     0,    46,     0,     0,
       0,     0,    95,   101,    17,    14,     0,     0,    22,     0,
      35,     0,    38,     0,     0,     0,     0,    29,     0,     0,
       0,     0,    43,    45,   100,     0,   115,   116,   112,    16,
      25,     0,     0,    21,     0,    39,     0,    93,     0,    47,
       0,     0,    42,    99,    24,     0,     0,     0,     0,     0,
      48,    23,     0,     0,     0,    44,    34,    41,    32
  };

  const short
  parser::yypgoto_[] =
  {
    -181,  -181,  -181,   248,  -181,  -181,  -181,    46,   -59,  -181,
     -58,  -181,  -105,  -113,  -181,  -149,  -181,  -177,  -180,   185,
     158,   -13,  -181,  -181,  -181,    69,   201,   202,   204,   186,
     209,    75,    65,    67,    88,   -27,  -181,  -181,    83,    79
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
      38,   109,   110,   173,    70,    71,   213,   168,   211,   182,
     182,    16,   197,   194,   182,   197,    98,    55,   184,   184,
      13,    72,    75,   184,    60,    15,    14,    55,   182,    56,
      61,   213,   114,    23,    24,    25,   183,   184,    62,    56,
      63,   115,   182,    99,    28,   100,   219,   101,    29,    17,
      30,   184,    31,   239,    20,   173,   123,   212,   232,   213,
     198,   129,   245,   241,    35,    32,   108,   102,   148,   149,
     150,    57,   103,    64,    33,    34,    80,    35,   106,   106,
     172,   -60,   107,   167,   -60,    91,    92,   156,   159,   160,
       1,   161,   162,    18,     2,     3,     4,     5,    95,    96,
      97,   114,    23,    24,    25,   178,    35,   191,   222,   179,
     115,   192,   180,    28,    75,    78,   191,    29,    79,    30,
     233,    31,    87,    88,    89,    90,    85,    86,    19,   173,
     173,   242,   243,    65,    32,    93,    94,   220,   110,    66,
     223,   110,    67,    33,    34,    76,    35,   131,   132,   200,
      68,   195,   140,   141,   142,   143,   201,   202,   144,   145,
     138,   139,   234,   110,    69,   205,    82,    81,   104,    84,
      83,   114,    23,    24,    25,   156,   216,   217,   156,   105,
     115,   146,   147,    28,   113,   121,   122,    29,   112,    30,
     225,    31,   119,   228,   120,   229,   124,   125,   130,   151,
     152,   164,   165,   169,    32,   170,   171,   174,   114,    23,
      24,    25,    80,    33,    34,    93,    35,   115,   240,   246,
      28,   176,   177,   188,    29,   244,    30,   189,    31,   190,
     193,   196,   199,   204,   206,   203,   210,   207,   208,    35,
     231,    32,    22,    23,    24,    25,   111,   209,   221,   224,
      33,    34,   226,    35,    28,    21,   247,   126,    29,   227,
      30,   238,    31,   230,   163,   236,   237,   248,   235,   136,
     218,   215,     0,     0,     0,    32,     0,     0,     0,     0,
       0,   133,     0,   134,    33,    34,   135,   127,   128,    22,
      23,    24,    25,   137,     0,    26,    27,     0,     0,     0,
       0,    28,     0,     0,     0,    29,     0,    30,     0,    31,
       0,     0,     0,     0,     0,     0,     0,   114,    23,    24,
      25,     0,    32,     0,     0,     0,   115,     0,     0,    28,
       0,    33,    34,    29,    35,    30,     0,    31,     0,     0,
       0,     0,     0,     0,     0,   153,    23,    24,    25,     0,
      32,     0,   153,    23,    24,    25,     0,   154,     0,    33,
      34,    29,    35,    30,   154,    31,     0,     0,    29,     0,
      30,     0,    31,     0,     0,     0,     0,     0,    32,     0,
       0,     0,     0,     0,     0,    32,     0,    33,    34,   155,
      35,     0,     0,     0,    33,    34,   214,    35,    22,    23,
      24,    25,     0,    73,    23,    24,    25,     0,     0,     0,
      28,     0,     0,     0,    29,    28,    30,     0,    31,    29,
       0,    30,     0,    31,     0,     0,     0,     0,     0,     0,
       0,    32,     0,     0,     0,     0,    32,     0,     0,     0,
      33,    34,     0,    35,     0,    33,    34,     0,    35,    73,
     181,    24,    25,     0,   153,    23,    24,    25,     0,     0,
       0,    28,     0,     0,     0,    29,   154,    30,     0,    31,
      29,     0,    30,     0,    31,     0,     0,     0,     0,     0,
       0,     0,    32,     0,     0,     0,     0,    32,     0,     0,
       0,    33,    34,     0,    35,     0,    33,    34,     0,    35
  };

  const short
  parser::yycheck_[] =
  {
      13,    60,    60,   116,    31,    32,   186,   112,   185,     3,
       3,     3,     3,   162,     3,     3,    17,     3,    12,    12,
      40,    34,    35,    12,    40,     3,    46,     3,     3,    15,
      46,   211,     3,     4,     5,     6,    11,    12,    41,    15,
      43,    12,     3,    44,    15,    46,   195,    48,    19,    41,
      21,    12,    23,   230,     0,   168,    69,    51,    51,   239,
      51,    74,    51,    51,    48,    36,    50,    40,    95,    96,
      97,    47,    45,    50,    45,    46,    34,    48,    43,    43,
      51,    39,    47,    47,    42,    37,    38,   100,   101,   102,
       3,   104,   105,    46,     7,     8,     9,    10,    24,    25,
      26,     3,     4,     5,     6,    43,    48,    43,    50,    47,
      12,    47,   125,    15,   127,    39,    43,    19,    42,    21,
      47,    23,    29,    30,    31,    32,    27,    28,    46,   242,
     243,   236,   237,     6,    36,    22,    23,   196,   196,     6,
     199,   199,    46,    45,    46,    16,    48,    78,    79,    51,
      46,   164,    87,    88,    89,    90,   169,   170,    91,    92,
      85,    86,   221,   221,    46,   178,    34,    33,    40,    33,
      35,     3,     4,     5,     6,   188,   189,   190,   191,    40,
      12,    93,    94,    15,    15,     6,     6,    19,    50,    21,
     203,    23,    47,   206,    47,   208,    47,    40,    50,     3,
       3,    40,     3,    40,    36,    40,     3,    13,     3,     4,
       5,     6,    34,    45,    46,    22,    48,    12,   231,    51,
      15,    47,    47,    46,    19,   238,    21,    40,    23,    40,
      49,    41,    40,    14,    20,    40,     3,    49,    40,    48,
      40,    36,     3,     4,     5,     6,    61,    41,    41,    41,
      45,    46,    41,    48,    15,     7,    51,    18,    19,    47,
      21,    41,    23,    50,   106,    50,    50,    49,   222,    83,
     191,   188,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      -1,    80,    -1,    81,    45,    46,    82,    48,    49,     3,
       4,     5,     6,    84,    -1,     9,    10,    -1,    -1,    -1,
      -1,    15,    -1,    -1,    -1,    19,    -1,    21,    -1,    23,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,    36,    -1,    -1,    -1,    12,    -1,    -1,    15,
      -1,    45,    46,    19,    48,    21,    -1,    23,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,
      36,    -1,     3,     4,     5,     6,    -1,    15,    -1,    45,
      46,    19,    48,    21,    15,    23,    -1,    -1,    19,    -1,
      21,    -1,    23,    -1,    -1,    -1,    -1,    -1,    36,    -1,
      -1,    -1,    -1,    -1,    -1,    36,    -1,    45,    46,    47,
      48,    -1,    -1,    -1,    45,    46,    47,    48,     3,     4,
       5,     6,    -1,     3,     4,     5,     6,    -1,    -1,    -1,
      15,    -1,    -1,    -1,    19,    15,    21,    -1,    23,    19,
      -1,    21,    -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    36,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,
      45,    46,    -1,    48,    -1,    45,    46,    -1,    48,     3,
       4,     5,     6,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    15,    -1,    -1,    -1,    19,    15,    21,    -1,    23,
      19,    -1,    21,    -1,    23,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    36,    -1,    -1,    -1,    -1,    36,    -1,    -1,
      -1,    45,    46,    -1,    48,    -1,    45,    46,    -1,    48
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
      51,    73,    73,    40,    14,    73,    20,    49,    40,    41,
       3,    69,    51,    70,    47,    90,    73,    73,    91,    67,
      60,    41,    50,    60,    41,    73,    41,    47,    73,    73,
      50,    40,    51,    47,    60,    59,    50,    50,    41,    69,
      73,    51,    64,    64,    73,    51,    51,    51,    49
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    52,    53,    53,    54,    54,    55,    55,    55,    55,
      55,    55,    56,    56,    57,    57,    57,    57,    57,    57,
      58,    58,    58,    58,    59,    59,    60,    60,    61,    61,
      61,    61,    62,    63,    63,    63,    64,    64,    65,    65,
      65,    66,    67,    67,    68,    69,    69,    70,    70,    71,
      71,    72,    72,    72,    72,    73,    74,    75,    75,    76,
      76,    77,    77,    78,    78,    79,    79,    80,    80,    81,
      81,    82,    82,    82,    83,    83,    83,    83,    83,    84,
      84,    84,    85,    85,    85,    86,    86,    86,    86,    87,
      87,    87,    87,    87,    88,    88,    88,    88,    88,    88,
      88,    88,    89,    89,    89,    89,    89,    89,    89,    89,
      89,    89,    90,    90,    91,    91,    91
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     1,     0,     2,     1,     1,     1,     4,     6,
       4,     6,     1,     1,     6,     5,     7,     6,     3,     4,
       4,     7,     6,     9,     4,     3,     3,     1,     2,     4,
       2,     1,     9,     5,    10,     6,     2,     1,     3,     4,
       2,     6,     5,     4,     5,     2,     1,     3,     4,     3,
       1,     1,     2,     3,     3,     1,     1,     1,     3,     3,
       1,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     3,     1,     3,     3,     3,     3,     1,
       3,     3,     1,     3,     3,     1,     3,     3,     3,     1,
       2,     2,     4,     6,     1,     4,     3,     3,     3,     6,
       5,     4,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     1,     3,     1,     1,     3,     3
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
     464,   471,   483,   492,   503,   514,   519,   527,   537,   550,
     555,   559,   568,   577,   586,   599,   603,   607,   608,   624,
     629,   636,   637,   654,   655,   672,   673,   690,   691,   708,
     709,   726,   727,   741,   758,   759,   773,   787,   801,   818,
     819,   833,   850,   851,   865,   882,   883,   897,   911,   928,
     929,   942,   955,   967,   983,   984,   997,  1010,  1023,  1037,
    1051,  1065,  1081,  1093,  1105,  1131,  1142,  1153,  1165,  1177,
    1185,  1186,  1190,  1195,  1199,  1208,  1217
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
#line 3185 "/home/johann/Documents/git/idyl/src/parser/idyl.tab.cc"

#line 1228 "/home/johann/Documents/git/idyl/src/parser/idyl.y"


void yy::parser::error(const location_type& l, const std::string& m) {
    std::cerr << l << ": " << m << std::endl;
}
