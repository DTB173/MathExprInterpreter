#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cstdint>
#include "Lexer.h"

TEST_CASE("Decimal numbers and leading dots") {
    Lexer lexer("0.5 + .25");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens[0].is<tok::Number>());
    REQUIRE(tokens[0].get_if<tok::Number>()->value == Catch::Approx(0.5));

    REQUIRE(tokens[1].is<tok::Operator>());
    REQUIRE(tokens[1].get_if<tok::Operator>()->type == OperatorType::Add);

    REQUIRE(tokens[2].is<tok::Number>());
    REQUIRE(tokens[2].get_if<tok::Number>()->value == Catch::Approx(0.25));
}

TEST_CASE("Factorial, power, mod, and integer division") {
    Lexer lexer("5! ^ 2 % 3 \\ 2");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 8);

    REQUIRE(tokens[0].get_if<tok::Number>()->value == Catch::Approx(5));
    REQUIRE(tokens[1].get_if<tok::Operator>()->type == OperatorType::Factorial);
    REQUIRE(tokens[2].get_if<tok::Operator>()->type == OperatorType::Power);
    REQUIRE(tokens[3].get_if<tok::Number>()->value == Catch::Approx(2));
    REQUIRE(tokens[4].get_if<tok::Operator>()->type == OperatorType::Mod);
    REQUIRE(tokens[5].get_if<tok::Number>()->value == Catch::Approx(3));
    REQUIRE(tokens[6].get_if<tok::Operator>()->type == OperatorType::IntDivide);
}

TEST_CASE("Variable assignment and expressions") {
    Lexer lexer("x = 3 + y");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 5);

    REQUIRE(tokens[0].is<tok::Variable>());
    REQUIRE(tokens[0].get_if<tok::Variable>()->name == "x");

    REQUIRE(tokens[1].is<tok::Operator>());
    REQUIRE(tokens[1].get_if<tok::Operator>()->type == OperatorType::Assignment);

    REQUIRE(tokens[2].get_if<tok::Number>()->value == Catch::Approx(3));

    REQUIRE(tokens[3].get_if<tok::Operator>()->type == OperatorType::Add);

    REQUIRE(tokens[4].is<tok::Variable>());
    REQUIRE(tokens[4].get_if<tok::Variable>()->name == "y");
}

TEST_CASE("Nested functions and parentheses") {
    Lexer lexer("sqrt(abs(x+1))");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 9);

    REQUIRE(tokens[0].is<tok::Function>());
    REQUIRE(tokens[0].get_if<tok::Function>()->name == "sqrt");

    REQUIRE(tokens[1].is<tok::Parenthesis>());
    REQUIRE(tokens[1].get_if<tok::Parenthesis>()->symbol == '(');

    REQUIRE(tokens[2].is<tok::Function>());
    REQUIRE(tokens[2].get_if<tok::Function>()->name == "abs");

    REQUIRE(tokens[3].is<tok::Parenthesis>());
    REQUIRE(tokens[3].get_if<tok::Parenthesis>()->symbol == '(');

    REQUIRE(tokens[4].is<tok::Variable>());
    REQUIRE(tokens[4].get_if<tok::Variable>()->name == "x");

    REQUIRE(tokens[5].get_if<tok::Operator>()->type == OperatorType::Add);

    REQUIRE(tokens[6].is<tok::Number>());
    REQUIRE(tokens[6].get_if<tok::Number>()->value == Catch::Approx(1));

    REQUIRE(tokens[7].is<tok::Parenthesis>());
    REQUIRE(tokens[7].get_if<tok::Parenthesis>()->symbol == ')');

    REQUIRE(tokens[8].is<tok::Parenthesis>());
    REQUIRE(tokens[8].get_if<tok::Parenthesis>()->symbol == ')');
}

TEST_CASE("Multiple variables and functions in a row") {
    Lexer lexer("a+b*sin(theta)-c");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 10);

    REQUIRE(tokens[0].is<tok::Variable>());
    REQUIRE(tokens[0].get_if<tok::Variable>()->name == "a");

    REQUIRE(tokens[1].get_if<tok::Operator>()->type == OperatorType::Add);

    REQUIRE(tokens[2].is<tok::Variable>());
    REQUIRE(tokens[2].get_if<tok::Variable>()->name == "b");

    REQUIRE(tokens[3].get_if<tok::Operator>()->type == OperatorType::Multiply);

    REQUIRE(tokens[4].is<tok::Function>());
    REQUIRE(tokens[4].get_if<tok::Function>()->name == "sin");

    REQUIRE(tokens[5].is<tok::Parenthesis>());
    REQUIRE(tokens[5].get_if<tok::Parenthesis>()->symbol == '(');

    REQUIRE(tokens[6].is<tok::Variable>());
    REQUIRE(tokens[6].get_if<tok::Variable>()->name == "theta");

    REQUIRE(tokens[7].is<tok::Parenthesis>());
    REQUIRE(tokens[7].get_if<tok::Parenthesis>()->symbol == ')');

    REQUIRE(tokens[8].get_if<tok::Operator>()->type == OperatorType::Subtract);
}

TEST_CASE("Valid scientific notation with positive exponent") {
    Lexer lexer("1e10");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);

    REQUIRE(tokens[0].is<tok::Number>());
    REQUIRE(tokens[0].get_if<tok::Number>()->value == Catch::Approx(1e10));
}

TEST_CASE("Valid scientific notation with negative exponent") {
    Lexer lexer("2.5e-3");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);

    REQUIRE(tokens[0].is<tok::Number>());
    REQUIRE(tokens[0].get_if<tok::Number>()->value == Catch::Approx(0.0025));
}

TEST_CASE("Valid scientific notation with explicit plus sign") {
    Lexer lexer("3e+7");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);

    REQUIRE(tokens[0].is<tok::Number>());
    REQUIRE(tokens[0].get_if<tok::Number>()->value == Catch::Approx(30000000));
}

TEST_CASE("Expression with scientific notation and operators") {
    Lexer lexer("5.5e2+sin(x)");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 6);

    REQUIRE(tokens[0].is<tok::Number>());
    REQUIRE(tokens[0].get_if<tok::Number>()->value == Catch::Approx(550));

    REQUIRE(tokens[1].get_if<tok::Operator>()->type == OperatorType::Add);

    REQUIRE(tokens[2].is<tok::Function>());
    REQUIRE(tokens[2].get_if<tok::Function>()->name == "sin");

    REQUIRE(tokens[3].is<tok::Parenthesis>());
    REQUIRE(tokens[3].get_if<tok::Parenthesis>()->symbol == '(');

    REQUIRE(tokens[4].is<tok::Variable>());
    REQUIRE(tokens[4].get_if<tok::Variable>()->name == "x");

    REQUIRE(tokens[5].is<tok::Parenthesis>());
    REQUIRE(tokens[5].get_if<tok::Parenthesis>()->symbol == ')');
}

TEST_CASE("Decimal without leading digit") {
    Lexer lexer(".5");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);

    REQUIRE(tokens[0].is<tok::Number>());
    REQUIRE(tokens[0].get_if<tok::Number>()->value == Catch::Approx(0.5));
}

TEST_CASE("Invalid double dot number") {
    Lexer lexer("1..2");
    REQUIRE_THROWS_AS(lexer.tokenize(), std::runtime_error);
}