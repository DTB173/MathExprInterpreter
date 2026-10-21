# MathExprInterpreter

Interactive math expression interpreter in C++20: a REPL with a hand-written lexer, a Pratt parser that builds an AST, and a visitor-based evaluator. Supports variables, user-defined functions and built-in math functions.

![REPL screenshot](https://github.com/user-attachments/assets/59801904-78d3-4e43-92f0-005d9e006d38)

## Features
 
- Arithmetic with operator precedence and parentheses
- Variable assignment: `x = 3`
- User-defined functions, including nested calls: `f(x) = x^2 + 1`, then `f(f(2))`
- Built-in math functions (the full list is printed on the welcome screen)
- Errors reported with a message instead of crashing the REPL

Example session:
 
```
>> x = 3
3
>> f(x) = x^2 + 1
>> f(x)
10
>> f(f(2))
26
>> 5/0
Error: "Division by zero"
>> 2+2
4
>> 2+2-
Error: "Unexpected end of input at position 4"
```

## How it works
 
Input goes through three stages:
 
1. **Lexer**: turns the input line into tokens (numbers, operators, variables, function names, parentheses, commas).
2. **Parser**: a Pratt parser uses binding powers to handle operator precedence and associativity while constructing the AST.
3. **Evaluator**: walks the tree with a visitor and looks up variables, user functions and built-ins.

## Design decisions
 
- **`std::variant` for tokens and AST nodes:** represents each token and node kind with a dedicated type (e.g. `ast::Binary`, `ast::Call` and `ast::FuncDef`), enabling type-safe visitation without an inheritance hierarchy.
- **Built-in functions in a lookup table** so the evaluator stays free of one-off cases.
- **Function bodies are pure expressions:** assignment is allowed only as a top-level statement, not inside a function body. This prevents assignment side effects within expressions and simplifies evaluation semantics.


## Build and test
 
Requirements: CMake, a C++20 compiler. Catch2 is fetched automatically during configuration.

```
mkdir build && cd build
cmake -G "Ninja" .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Run the test executable to execute the Catch2 test suite.
Run cmdCalc to launch the interactive interpreter.

The test suite contains 38 test cases (252 assertions), covering lexing, parsing, evaluation and error handling.
