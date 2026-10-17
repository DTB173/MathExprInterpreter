#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <cstdint>

#include "Lexer.h"
#include "Parser.h"
#include "Evaluator.h"

TEST_CASE("Basic arithmetic operations") {
    Evaluator eval;
    SECTION("Addition") {
        Lexer lexer("1 + 2");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(3.0));
    }
    SECTION("Subtraction") {
        Lexer lexer("10 - 3");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(7.0));
    }
    SECTION("Multiplication") {
        Lexer lexer("4 * 5");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(20.0));
    }
    SECTION("Division") {
        Lexer lexer("20 / 4");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(5.0));
    }
    SECTION("Integer Division") {
        Lexer lexer("20 \\ 3");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(6.0));
    }
    SECTION("Power") {
        Lexer lexer("2 ^ 3");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(8.0));
    }
    SECTION("Modulus") {
        Lexer lexer("10 % 3");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(1.0));
    }
}

TEST_CASE("Factorial operation") {
    Evaluator eval;
    SECTION("Factorial of 5") {
        Lexer lexer("5!");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(120.0));
    }
    SECTION("Factorial of 0") {
        Lexer lexer("0!");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(1.0));
    }
    SECTION("Factorial of 3") {
        Lexer lexer("3!");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(6.0));
    }
}

TEST_CASE("Unary operations") {
    Evaluator eval;
    SECTION("Unary plus") {
        Lexer lexer("+5");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(5.0));
    }
    SECTION("Unary minus") {
        Lexer lexer("-3");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(-3.0));
    }
}

TEST_CASE("Trigonometric functions") {
    Evaluator eval;
    SECTION("sin(0)") {
        Lexer lexer("sin(0)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(0.0));
    }
    SECTION("cos(0)") {
        Lexer lexer("cos(0)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(1.0));
    }
    SECTION("tan(pi/4)") {
        Lexer lexer("tan(pi/4)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(1.0).epsilon(0.001));
    }
    SECTION("asin(0)") {
        Lexer lexer("asin(0)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(0.0));
    }
    SECTION("acos(1)") {
        Lexer lexer("acos(1)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(0.0));
    }
    SECTION("atan(1)") {
        Lexer lexer("atan(1)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(0.785398).epsilon(0.001));
    }
    SECTION("atan2(1,1)") {
        Lexer lexer("atan2(1,1)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(0.785398).epsilon(0.001));
    }
}

TEST_CASE("Exponential and logarithmic functions") {
    Evaluator eval;
    SECTION("exp(0)") {
        Lexer lexer("exp(0)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(1.0));
    }
    SECTION("sqrt(16)") {
        Lexer lexer("sqrt(16)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(4.0));
    }
    SECTION("log(1)") {
        Lexer lexer("log(1)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(0.0));
    }
    SECTION("log10(100)") {
        Lexer lexer("log10(100)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(2.0));
    }
}

TEST_CASE("Rounding and absolute functions") {
    Evaluator eval;
    SECTION("abs(-5)") {
        Lexer lexer("abs(-5)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(5.0));
    }
    SECTION("floor(3.7)") {
        Lexer lexer("floor(3.7)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(3.0));
    }
    SECTION("ceil(3.2)") {
        Lexer lexer("ceil(3.2)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(4.0));
    }
    SECTION("round(3.6)") {
        Lexer lexer("round(3.6)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(4.0));
    }
    SECTION("round(3.4)") {
        Lexer lexer("round(3.4)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(3.0));
    }
}

TEST_CASE("Aggregate functions") {
    Evaluator eval;
    SECTION("min(1,5,2)") {
        Lexer lexer("min(1,5,2)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(1.0));
    }
    SECTION("max(1,5,2)") {
        Lexer lexer("max(1,5,2)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(5.0));
    }
    SECTION("factorial(4)") {
        Lexer lexer("factorial(4)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(24.0));
    }
}

TEST_CASE("Constants") {
    Evaluator eval;
    SECTION("pi") {
        Lexer lexer("pi");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(3.141592653589793).epsilon(0.001));
    }
    SECTION("e") {
        Lexer lexer("e");
        Parser parser(lexer.tokenize());
        auto ast = parser.parse();
        REQUIRE(eval.execute(*ast).value() == Catch::Approx(2.718281828459045).epsilon(0.001));
    }
}

TEST_CASE("Variables and assignment") {
    Evaluator eval;
    SECTION("Assign and use x") {
        Lexer lexer1("x = 5");
        Parser parser1(lexer1.tokenize());
        auto ast1 = parser1.parse();
        REQUIRE(eval.execute(*ast1).value() == Catch::Approx(5.0));

        Lexer lexer2("x * 2");
        Parser parser2(lexer2.tokenize());
        auto ast2 = parser2.parse();
        REQUIRE(eval.execute(*ast2).value() == Catch::Approx(10.0));
    }
    SECTION("Assign and use y") {
        Lexer lexer1("y = 3");
        Parser parser1(lexer1.tokenize());
        auto ast1 = parser1.parse();
        REQUIRE(eval.execute(*ast1).value() == Catch::Approx(3.0));

        Lexer lexer2("y + 2");
        Parser parser2(lexer2.tokenize());
        auto ast2 = parser2.parse();
        REQUIRE(eval.execute(*ast2).value() == Catch::Approx(5.0));
    }
    SECTION("Multiple variables") {
        Lexer lexer1("x = 5");
        Parser parser1(lexer1.tokenize());
        auto ast1 = parser1.parse();
        eval.execute(*ast1);

        Lexer lexer2("y = 3");
        Parser parser2(lexer2.tokenize());
        auto ast2 = parser2.parse();
        eval.execute(*ast2);

        Lexer lexer3("x + y");
        Parser parser3(lexer3.tokenize());
        auto ast3 = parser3.parse();
        REQUIRE(eval.execute(*ast3).value() == Catch::Approx(8.0));
    }
}

TEST_CASE("Function definition and evaluation") {
    Evaluator eval;
    SECTION("Define and evaluate f(x) = x^2 + 2") {
        Lexer lexer1("f(x) = x^2 + 2");
        Parser parser1(lexer1.tokenize());
        auto ast1 = parser1.parse();
        eval.execute(*ast1); // Define function

        Lexer lexer2("f(3)");
        Parser parser2(lexer2.tokenize());
        auto ast2 = parser2.parse();
        REQUIRE(eval.execute(*ast2).value() == Catch::Approx(11.0)); // f(3) = 3^2 + 2 = 11
    }
    SECTION("Define and evaluate f(x) = x * 2") {
        Lexer lexer1("f(x) = x * 2");
        Parser parser1(lexer1.tokenize());
        auto ast1 = parser1.parse();
        eval.execute(*ast1);

        Lexer lexer2("f(4)");
        Parser parser2(lexer2.tokenize());
        auto ast2 = parser2.parse();
        REQUIRE(eval.execute(*ast2).value() == Catch::Approx(8.0)); // f(4) = 4 * 2 = 8
    }
}

TEST_CASE("Complex end-to-end integration test") {
    Evaluator eval;

    Lexer lexer1("pythagoras(a, b) = sqrt(a^2 + b^2)");
    Parser parser1(lexer1.tokenize());
    REQUIRE_FALSE(eval.execute(*parser1.parse()).has_value());

    Lexer lexer2("angle = pi / 3");
    Parser parser2(lexer2.tokenize());
    eval.execute(*parser2.parse());

    Lexer lexer3("pythagoras(3, 4) + max(10! \\ 3, 5^2, sin(angle)*100) - min(factorial(5), 50) % 7");
    Parser parser3(lexer3.tokenize());

    auto ast3 = parser3.parse();
    auto result = eval.execute(*ast3);

    REQUIRE(result.has_value());
    REQUIRE(result.value() == Catch::Approx(1209604.0));
}

static std::optional<double> run(Evaluator& e, std::string_view line) {
    Parser p(Lexer(line).tokenize());
    return e.execute(*p.parse());
}
static double calc(std::string_view line) { Evaluator e; return run(e, line).value(); }

TEST_CASE("precedence and associativity") {
    auto [expr, expected] = GENERATE(table<const char*, double>({
        { "10-3-2", 5 },{ "100/10/5", 2 },{ "2^3^2", 512 },{ "-2^2", -4 },{ "2^-2", 0.25 },
        { "--3", 3 },{ "-+3", -3 },{ "-3!", -6 },{ "2^3!", 64 },{ "(-5+7)!", 2 },
        { "7\\2", 3 },{ "-7\\2", -4 },{ "-7%3", -1 },{ "2+3*4^2", 50 },{ "(2+3)*4", 20 },
        }));
    CHECK(calc(expr) == Catch::Approx(expected));
}

TEST_CASE("Error handling and exception propagation") {
    CHECK_THROWS_AS(calc("3 $ 4"), LexError);
    CHECK_THROWS_AS(calc("1 2"), ParseError);   // trailing garbage
    CHECK_THROWS_AS(calc("(1"), ParseError);
    CHECK_THROWS_AS(calc("3 +"), ParseError);
    CHECK_THROWS_AS(calc("1 = 2"), ParseError);
    CHECK_THROWS_AS(calc("f(x,,y) = x"), ParseError);
    CHECK_THROWS_AS(calc("1/0"), EvalError);
    CHECK_THROWS_AS(calc("5 % 0"), EvalError);
    CHECK_THROWS_AS(calc("sqrt(-1)"), EvalError);
    CHECK_THROWS_AS(calc("171!"), EvalError);
    CHECK_THROWS_AS(calc("(-1)!"), EvalError);
    CHECK_THROWS_AS(calc("2.5!"), EvalError);
    CHECK_THROWS_AS(calc("nope"), EvalError);    // undefined variable
    CHECK_THROWS_AS(calc("nope(1)"), EvalError);    // undefined function
    CHECK_THROWS_AS(calc("sin(1,2)"), EvalError);   // arity
}

TEST_CASE("state") {
    Evaluator e;
    CHECK(run(e, "x = 5") == 5.0);                 // assignment yields a value
    CHECK(run(e, "x = x + 1") == 6.0);             // reads before writing
    CHECK_FALSE(run(e, "f(a) = a * x").has_value());
    CHECK(run(e, "f(2)") == 12.0);
    run(e, "x = 10");
    CHECK(run(e, "f(2)") == 20.0);                 // globals read at call time
    run(e, "f(a) = a + 1");                        // redefinition overwrites
    CHECK(run(e, "f(2)") == 3.0);
}