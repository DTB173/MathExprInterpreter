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
>> x=3
3
>> f(x) = x^2 + 1
>> f(x)
10
>> f(f(2))
26
>>
```

## How it works
 
Input goes through three stages:
 
1. **Lexer**: turns the input line into tokens (numbers, operators, variables, function names, parentheses, commas).
2. **Parser**: a Pratt parser (precedence climbing with binding powers) builds an abstract syntax tree.
3. **Evaluator**: walks the tree with a visitor and looks up variables, user functions and built-ins.

## Design decisions
 
- **`std::variant` for tokens and AST nodes** instead of tagged unions or an inheritance hierarchy: each token and node kind is its own struct (for example `ast::Binary`, `ast::Call`, `ast::FuncDef`), and the compiler checks that every visitor handles every kind.
- **Built-in functions in a lookup table** so the evaluator stays free of one-off cases.
- **Function bodies are pure expressions:** assignment is only allowed as a statement at the start of a line, never inside a function body. This keeps functions free of side effects and makes evaluation order easy to reason about.


## Build and test
 
Requirements: CMake, a C++20 compiler. Catch2 is downloaded automatically.

```
mkdir build && cd build
cmake -G "Ninja" .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

To run tests, run tests exectutable.
To run application, run cmdCalc executable.
The test suite has 38 test cases (252 assertions) written with Catch2, covering the lexer, parser, evaluator and error handling.

## Roadmap
 
- Errors are currently reported with exceptions; moving to `std::expected` is a possible refactor
