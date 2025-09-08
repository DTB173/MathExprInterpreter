#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cstdint>

#include "Lexer.h"
#include "Parser.h"
#include "Evaluator.h"

TEST_CASE("Basic arithmetic operations") {
    Evaluator eval;
    SECTION("Addition") {
        Lexer lexer("1 + 2");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(3.0));
    }
    SECTION("Subtraction") {
        Lexer lexer("10 - 3");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(7.0));
    }
    SECTION("Multiplication") {
        Lexer lexer("4 * 5");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(20.0));
    }
    SECTION("Division") {
        Lexer lexer("20 / 4");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(5.0));
    }
    SECTION("Integer Division") {
        Lexer lexer("20 \\ 3");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(6.0));
    }
    SECTION("Power") {
        Lexer lexer("2 ^ 3");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(8.0));
    }
    SECTION("Modulus") {
        Lexer lexer("10 % 3");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(1.0));
    }
}

TEST_CASE("Factorial operation") {
    Evaluator eval;
    SECTION("Factorial of 5") {
        Lexer lexer("5!");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(120.0));
    }
    SECTION("Factorial of 0") {
        Lexer lexer("0!");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(1.0));
    }
    SECTION("Factorial of 3") {
        Lexer lexer("3!");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(6.0));
    }
}

TEST_CASE("Unary operations") {
    Evaluator eval;
    SECTION("Unary plus") {
        Lexer lexer("+5");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(5.0));
    }
    SECTION("Unary minus") {
        Lexer lexer("-3");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(-3.0));
    }
}

TEST_CASE("Trigonometric functions") {
    Evaluator eval;
    SECTION("sin(0)") {
        Lexer lexer("sin(0)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(0.0));
    }
    SECTION("cos(0)") {
        Lexer lexer("cos(0)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(1.0));
    }
    SECTION("tan(pi/4)") {
        Lexer lexer("tan(pi/4)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(1.0).epsilon(0.001));
    }
    SECTION("asin(0)") {
        Lexer lexer("asin(0)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(0.0));
    }
    SECTION("acos(1)") {
        Lexer lexer("acos(1)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(0.0));
    }
    SECTION("atan(1)") {
        Lexer lexer("atan(1)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(0.785398).epsilon(0.001));
    }
    SECTION("atan2(1,1)") {
        Lexer lexer("atan2(1,1)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(0.785398).epsilon(0.001));
    }
}

TEST_CASE("Exponential and logarithmic functions") {
    Evaluator eval;
    SECTION("exp(0)") {
        Lexer lexer("exp(0)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(1.0));
    }
    SECTION("sqrt(16)") {
        Lexer lexer("sqrt(16)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(4.0));
    }
    SECTION("log(1)") {
        Lexer lexer("log(1)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(0.0));
    }
    SECTION("log10(100)") {
        Lexer lexer("log10(100)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(2.0));
    }
}

TEST_CASE("Rounding and absolute functions") {
    Evaluator eval;
    SECTION("abs(-5)") {
        Lexer lexer("abs(-5)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(5.0));
    }
    SECTION("floor(3.7)") {
        Lexer lexer("floor(3.7)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(3.0));
    }
    SECTION("ceil(3.2)") {
        Lexer lexer("ceil(3.2)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(4.0));
    }
    SECTION("round(3.6)") {
        Lexer lexer("round(3.6)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(4.0));
    }
    SECTION("round(3.4)") {
        Lexer lexer("round(3.4)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(3.0));
    }
}

TEST_CASE("Aggregate functions") {
    Evaluator eval;
    SECTION("min(1,5,2)") {
        Lexer lexer("min(1,5,2)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(1.0));
    }
    SECTION("max(1,5,2)") {
        Lexer lexer("max(1,5,2)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(5.0));
    }
    SECTION("factorial(4)") {
        Lexer lexer("factorial(4)");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(24.0));
    }
}

TEST_CASE("Constants") {
    Evaluator eval;
    SECTION("pi") {
        Lexer lexer("pi");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(3.141592653589793).epsilon(0.001));
    }
    SECTION("e") {
        Lexer lexer("e");
        Parser parser(lexer.tokenize());
        auto ast = parser.parseExpression();
        REQUIRE(eval.evaluate(*ast) == Catch::Approx(2.718281828459045).epsilon(0.001));
    }
}

TEST_CASE("Variables and assignment") {
    Evaluator eval;
    SECTION("Assign and use x") {
        Lexer lexer1("x = 5");
        Parser parser1(lexer1.tokenize());
        auto ast1 = parser1.parseExpression();
        REQUIRE(eval.evaluate(*ast1) == Catch::Approx(5.0));

        Lexer lexer2("x * 2");
        Parser parser2(lexer2.tokenize());
        auto ast2 = parser2.parseExpression();
        REQUIRE(eval.evaluate(*ast2) == Catch::Approx(10.0));
    }
    SECTION("Assign and use y") {
        Lexer lexer1("y = 3");
        Parser parser1(lexer1.tokenize());
        auto ast1 = parser1.parseExpression();
        REQUIRE(eval.evaluate(*ast1) == Catch::Approx(3.0));

        Lexer lexer2("y + 2");
        Parser parser2(lexer2.tokenize());
        auto ast2 = parser2.parseExpression();
        REQUIRE(eval.evaluate(*ast2) == Catch::Approx(5.0));
    }
    SECTION("Multiple variables") {
        Lexer lexer1("x = 5");
        Parser parser1(lexer1.tokenize());
        auto ast1 = parser1.parseExpression();
        eval.evaluate(*ast1);

        Lexer lexer2("y = 3");
        Parser parser2(lexer2.tokenize());
        auto ast2 = parser2.parseExpression();
        eval.evaluate(*ast2);

        Lexer lexer3("x + y");
        Parser parser3(lexer3.tokenize());
        auto ast3 = parser3.parseExpression();
        REQUIRE(eval.evaluate(*ast3) == Catch::Approx(8.0));
    }
}

TEST_CASE("Function definition and evaluation") {
    Evaluator eval;
    SECTION("Define and evaluate f(x) = x^2 + 2") {
        Lexer lexer1("f(x) = x^2 + 2");
        Parser parser1(lexer1.tokenize());
        auto ast1 = parser1.parseExpression();
        eval.evaluate(*ast1); // Define function

        Lexer lexer2("f(3)");
        Parser parser2(lexer2.tokenize());
        auto ast2 = parser2.parseExpression();
        REQUIRE(eval.evaluate(*ast2) == Catch::Approx(11.0)); // f(3) = 3^2 + 2 = 11
    }
    SECTION("Define and evaluate f(x) = x * 2") {
        Lexer lexer1("f(x) = x * 2");
        Parser parser1(lexer1.tokenize());
        auto ast1 = parser1.parseExpression();
        eval.evaluate(*ast1);

        Lexer lexer2("f(4)");
        Parser parser2(lexer2.tokenize());
        auto ast2 = parser2.parseExpression();
        REQUIRE(eval.evaluate(*ast2) == Catch::Approx(8.0)); // f(4) = 4 * 2 = 8
    }
}