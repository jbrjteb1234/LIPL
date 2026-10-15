# LIPL
Light Interpreted Programming Language: a small interpreted language written in C.

The lexer and parser are complete and build an AST. There's no interpreter yet, so running a program prints its syntax tree.

## Build, run, test

```bash
cd src
make                # builds ./program
make test           # runs everything in tests/cases
make SANITIZE=1     # AddressSanitizer + UBSan build (Linux/WSL)
make clean
```

Without make: `cd src && gcc -std=c11 -I. $(find . -name '*.c' -not -path './obj/*') -o program`

On WSL with a 6.x kernel, GCC 11's sanitizer runtime randomly hangs printing `AddressSanitizer:DEADLYSIGNAL`. That's the toolchain, not the code: run sanitized builds with address randomisation off, e.g. `setarch $(uname -m) -R ./program file.lipl`.

```bash
./program file.lipl            # print the AST
./program --tokens file.lipl   # print the tokens first
```

Exit codes: `0` parsed fine, `1` errors in the source (printed as `file:line:column: error: ...`), `2` bad command line.

## The language so far

```
// comments run to the end of the line
var x = 5;
var later;                      // no initial value
global const var limit = 10;    // global and const go before var or func

x = (x + 1) * 2 ^ 3;

func add(a, b) {
    return a + b;
}

if x > 10 && !done {
    print("big");
} elif x == 10 {
    print("ten");
} else {
    print("small");
}

while x > 0 {
    x = x - 1;
}

for var i = 0; i < 10; i = i + 1 {     // brackets around the header are optional
    print(i);
}

object.field = object.method(1, 2);
```

Operators, loosest to tightest:

| Operators | Associativity |
|---|---|
| `=` | right |
| `\|\|` | left |
| `&&` | left |
| `==` `!=` | left |
| `<` `>` `<=` `>=` | left |
| `+` `-` | left |
| `*` `/` | left |
| prefix `-` `!` | |
| `^` (power) `_` (root) | right |
| `.` (member access, method calls) | left |

`_` is the root operator, but inside a word it's part of the name (`my_var`), so put spaces around it when you mean root: `27 _ 3`.

Literals: 64-bit integers, decimals (`3.14`), strings with `\n` `\t` `\r` `\"` `\\` escapes, `true`, `false`.

## Layout

| Path | What's there |
|---|---|
| `src/grammar/grammar.h` | every fixed lexeme: operators, delimiters, keywords |
| `src/lexer/` | tokenizer and the identifier symbol table |
| `src/ast/parser.c` | recursive-descent parser, precedence climbing for expressions |
| `src/ast/ast_utility/` | AST node types, ownership, printer |
| `src/utility/` | allocation helpers, stack, pooled allocator |
| `tests/` | `.lipl` cases and their expected output; `run_tests.sh --update` regenerates them |
