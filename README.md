# JIPL
James' Interpreted Programming Language

lexical analysis is done in the lexer folder:
-token: token structure, 

-lexer_main: iterates the source file provided, feeding characters into the buffer

-lexeme: handles whatever parsing is done in the lexical analysis phase. checking for delimiters (new line, whitespace) managing buffer and communicating with the token files to produce tokens

The buffer works by analyzing the next character, thus deciding what to do with the current buffer

The term "tokentype" refers to the relation between the string format of a lexeme, and a token which it will produce. For example, one "tokentype" might be "if", and the token value if