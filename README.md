# LIPL

[![CI](https://github.com/jbrjteb1234/lipl/actions/workflows/ci.yml/badge.svg)](https://github.com/jbrjteb1234/lipl/actions/workflows/ci.yml)

A small scripting language with closures, lists and clear error messages, run by a tree-walking interpreter in plain C with no dependencies.

```
func fib(n) {
    if n < 2 {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

var row = [];
for var i = 0; i < 12; i = i + 1 {
    row.push(fib(i));
}
print("Fibonacci:", row);
print("fib(25) =", fib(25));
```

```console
$ ./lipl fib.lipl
Fibonacci: [0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89]
fib(25) = 75025
```

Mistype `fib(25)` as `fibb(25)` and the error points at it:

```console
$ ./lipl fib.lipl
Fibonacci: [0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89]
fib.lipl:13:20: error: 'fibb' is not defined
   13 | print("fib(25) =", fibb(25));
      |                    ^
```

## Quick start

```bash
git clone https://github.com/jbrjteb1234/lipl.git && cd lipl/src
make
./lipl ../examples/guess.lipl
```

You need gcc and make. On Windows, [MSYS2](https://www.msys2.org)'s UCRT64 environment has both.

## Examples

| Program | What it shows |
|---|---|
| [`examples/fizzbuzz.lipl`](examples/fizzbuzz.lipl) | loops, `%`, and building up strings |
| [`examples/primes.lipl`](examples/primes.lipl) | the Sieve of Eratosthenes, with a list as an array of flags |
| [`examples/hanoi.lipl`](examples/hanoi.lipl) | Towers of Hanoi, solved by recursion |
| [`examples/word_count.lipl`](examples/word_count.lipl) | the most common words in a text file: `read_lines`, `split` and a sort |
| [`examples/guess.lipl`](examples/guess.lipl) | a number-guessing game that reads your guesses with `input()` |

`read_lines` opens paths relative to where you run `lipl`, so run the word counter from its own folder: `cd examples && ../src/lipl word_count.lipl`.

## Build, run, test

LIPL (Light Interpreted Programming Language) is a hand-written lexer, recursive-descent parser and tree-walking interpreter with reference-counted values, in under 5,000 lines of C11.

```bash
cd src
make                # builds ./lipl
make test           # runs everything in tests/, and checks the examples' output
make SANITIZE=1     # AddressSanitizer + UBSan build (Linux/WSL)
make clean
```

Without make: `cd src && gcc -std=c11 -I. $(find . -name '*.c' -not -path './obj/*') -o lipl -lm`

CI builds and runs the tests on Linux, on Windows with MSYS2, and on Linux with the sanitizers. On WSL with a 6.x kernel, GCC 11's sanitizer runtime randomly hangs printing `AddressSanitizer:DEADLYSIGNAL`. That's the toolchain, not the code: run sanitized builds with address randomisation off, e.g. `setarch $(uname -m) -R ./lipl file.lipl`.

```bash
./lipl file.lipl            # run a program
./lipl --ast file.lipl      # print its syntax tree instead
./lipl --tokens file.lipl   # print its tokens instead
./lipl --help               # list the options
./lipl --version            # lipl 1.0.0
```

Exit codes: `0` ran fine, `1` an error in the program, `2` bad command line. Errors, whether lexical, syntax or runtime, print as `file:line:column: error: ...` with the line quoted and a caret under the column. They're in colour when written to a terminal, unless the `NO_COLOR` environment variable is set.

## The language

```
// comments run to the end of the line
var x = 5;
var later;                          // starts as nil
const var limit = 10;               // constants can't be changed

x = (x + 1) * 2 ^ 3;                // 48: ^ binds tighter than *

func add(a, b) {
    return a + b;
}

if x > limit && !later {
    print("big:", x);
} elif x == limit {
    print("ten");
} else {
    print("small");
}

while x > 0 {
    x = x - 7;
    if x % 2 == 0 {
        continue;
    }
    if x < 20 {
        break;
    }
}

for var i = 0; i < 3; i = i + 1 {   // brackets around the header are optional
    print(i, add(i, 10));
}

var grid = [[1, 2], [3, 4], []];
grid[0][1] = grid[1][0] * 10;
grid[2].push("new");                // a.f(x) means f(a, x)
print(grid, len(grid), grid[-1][0]);

var words = "the quick brown fox".split();
print(words[1] + "!", "-" * 5, 27 _ 3, 7 / 2, 7 / 2.0);
```

prints

```
big: 48
0 10
1 11
2 12
[[1, 30], [3, 4], ["new"]] 3 new
quick! ----- 3 3 3.5
```

### Rules worth knowing
- **Truth:** only `false` and `nil` are false; `0`, `""` and `[]` are true.
- **Numbers:** integers are 64-bit and stay integers, so `7 / 2` is `3` (rounding toward zero, like C). A decimal anywhere makes the result a decimal: `7 / 2.0` is `3.5`. Integer overflow is an error, never a silent wraparound.
- **Roots:** `a _ b` is the b-th root of a: `27 _ 3` is `3`, `2 _ 2` is `1.4142135623731`. Inside a word `_` is part of the name (`my_var`), so put spaces around it when you mean root.
- **Method calls:** `a.f(x)` is shorthand for `f(a, x)`, so `list.push(4)` and `line.split(",")` work, and so does `4.double()` for your own `func double(n)`. Values have no fields, so `a.b` without a call is an error.
- **Text:** `+` with a string on either side joins text (`"n = " + 5`), and `"ab" * 3` repeats it.
- **Lists:** lists and strings index from 0, and negative indexes count from the end. Lists are shared, not copied: after `var b = a`, both names refer to the same list.
- **Variables:** a variable must be created with `var` before it's assigned. Every block has its own scope. Top-level variables can be declared again, but locals and constants can't. `global var x` inside a function creates `x` at the top level.
- **Functions:** functions are values. Pass them around and return them, and they remember the variables of the scope they were declared in (closures).
- **Recursion:** around 5,000 calls deep is fine. Going much deeper is reported as an error instead of crashing.

### Built-in functions

| Function | What it does |
|---|---|
| `print(a, b, ...)` | prints its arguments separated by spaces, then a newline |
| `input()`, `input(prompt)` | reads a line from standard input; `nil` at the end of input |
| `len(x)` | length of a string or a list |
| `push(list, x)` | adds `x` to the end of a list |
| `pop(list)` | removes and returns the last item |
| `split(s)`, `split(s, sep)` | splits on runs of whitespace, or on every `sep` |
| `str(x)`, `int(x)`, `float(x)` | conversions: `int("42")`, `int(3.9)` is `3`, `str([1])` is `"[1]"` |
| `type(x)` | `"int"`, `"float"`, `"string"`, `"list"`, `"bool"`, `"nil"` or `"function"` |
| `read_lines(path)` | every line of a text file, as a list of strings |

### Operators, loosest to tightest

| Operators | Associativity |
|---|---|
| `=` | right |
| `\|\|` | left |
| `&&` | left |
| `==` `!=` | left |
| `<` `>` `<=` `>=` | left |
| `+` `-` | left |
| `*` `/` `%` | left |
| prefix `-` `!` | |
| `^` (power) `_` (root) | right |
| `.` (method calls), `[ ]` (indexing) | left |

Literals: 64-bit integers, decimals (`3.14`), strings with `\n` `\t` `\r` `\"` `\\` escapes, `true`, `false`, `nil`, and lists (`[1, "two", [3]]`, `[]`).

## Layout

| Path | What's there |
|---|---|
| `examples/` | short programs showing the language off: FizzBuzz, a prime sieve, Towers of Hanoi, a word counter, a guessing game |
| `src/grammar/grammar.h` | every fixed lexeme: operators, delimiters, keywords |
| `src/lexer/` | tokenizer and the identifier symbol table |
| `src/ast/parser.c` | recursive-descent parser, precedence climbing for expressions |
| `src/ast/ast_utility/` | AST node types, ownership, printer |
| `src/interpreter/` | tree-walking evaluator, reference-counted runtime values, built-in functions |
| `src/utility/` | error messages, source files, allocation helpers, stack, pooled allocator, platform details |
| `tests/parser/` | programs and the syntax tree (or syntax error) each should produce |
| `tests/interpreter/` | programs and the output (or runtime error) each should produce |
| `tests/examples/` | the output each example should produce (and the guessing game's guesses) |

`tests/run_tests.sh --update` regenerates the expected output; read the diff before keeping it.
