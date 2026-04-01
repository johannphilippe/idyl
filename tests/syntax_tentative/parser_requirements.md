# Parser attemps 01 - requirements 

## Specification 

The current specification of the language is tests/syntax_tentative/syntax_attempt_04.md 
For now, the parser should only implement the solid specification (not the part of upcoming / not finished features). 

## Filenames 

Parser is based on flex/yacc.

idyl.l for lexer
idyl.y for grammar

## Requirements 

1. Parser must keep track of line & column, and provide a relevant error message when parsing fails 
2. When possible, parser must see all errors it can (for example, using `import` or `lib` keyword in process block, or in a function, is not allowed, this can be seen by parser)
3. When a rule or syntax element is context sensitive, it must be seen either by lexer or parser. 
4. There will probably be a moderate to important number of rules. Try to make the parser as accurate as it can (all information it can give to AST is relevant). 


## What to do 

1. Write lexer and grammar files inside src/parser
2. Create ast.hpp file, and provide data structures for nodes identified by the grammar (ast.hpp can be inspired from old_parser/ast.hpp)
3. Inside the grammar file, already create the AST, by nesting nodes into each others 
4. Only use smart pointers for nodes 
5. ast.hpp must provide an AST printer function (pretty print, with indentation)
