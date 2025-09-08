#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cstdint>
#include "Lexer.h"

TEST_CASE("Decimal numbers and leading dots") {
    Lexer lexer("0.5 + .25");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens[0].m_tType == TokenType::Number);
    REQUIRE(tokens[0].getValue<double>() == Catch::Approx(0.5));

    REQUIRE(tokens[1].m_tType == TokenType::Operator);
    REQUIRE(tokens[1].getValue<OperatorType>() == OperatorType::Add);

    REQUIRE(tokens[2].m_tType == TokenType::Number);
    REQUIRE(tokens[2].getValue<double>() == Catch::Approx(0.25));
}

TEST_CASE("Factorial, power, mod, and integer division") {
    Lexer lexer("5! ^ 2 % 3 \\ 2");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 8);

    REQUIRE(tokens[0].getValue<double>() == Catch::Approx(5));
    REQUIRE(tokens[1].getValue<OperatorType>() == OperatorType::Factorial);
    REQUIRE(tokens[2].getValue<OperatorType>() == OperatorType::Power);
    REQUIRE(tokens[3].getValue<double>() == Catch::Approx(2));
    REQUIRE(tokens[4].getValue<OperatorType>() == OperatorType::Mod);
    REQUIRE(tokens[5].getValue<double>() == Catch::Approx(3));
    REQUIRE(tokens[6].getValue<OperatorType>() == OperatorType::Int_divide);
}

TEST_CASE("Variable assignment and expressions") {
    Lexer lexer("x = 3 + y");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 5);

    REQUIRE(tokens[0].m_tType == TokenType::Variable);
    REQUIRE(tokens[0].getValue<std::string>() == "x");

    REQUIRE(tokens[1].m_tType == TokenType::Operator);
    REQUIRE(tokens[1].getValue<OperatorType>() == OperatorType::Assignment);

    REQUIRE(tokens[2].getValue<double>() == Catch::Approx(3));

    REQUIRE(tokens[3].getValue<OperatorType>() == OperatorType::Add);

    REQUIRE(tokens[4].m_tType == TokenType::Variable);
    REQUIRE(tokens[4].getValue<std::string>() == "y");
}

TEST_CASE("Nested functions and parentheses") {
    Lexer lexer("sqrt(abs(x+1))");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 9);

    REQUIRE(tokens[0].m_tType == TokenType::Function);
    REQUIRE(tokens[0].getValue<std::string>() == "sqrt");

    REQUIRE(tokens[1].m_tType == TokenType::Parenthesis);
    REQUIRE(tokens[1].getValue<char>() == '(');

    REQUIRE(tokens[2].m_tType == TokenType::Function);
    REQUIRE(tokens[2].getValue<std::string>() == "abs");

    REQUIRE(tokens[3].m_tType == TokenType::Parenthesis);
    REQUIRE(tokens[3].getValue<char>() == '(');

    REQUIRE(tokens[4].m_tType == TokenType::Variable);
    REQUIRE(tokens[4].getValue<std::string>() == "x");

    REQUIRE(tokens[5].getValue<OperatorType>() == OperatorType::Add);

    REQUIRE(tokens[6].m_tType == TokenType::Number);
    REQUIRE(tokens[6].getValue<double>() == Catch::Approx(1));

    REQUIRE(tokens[7].m_tType == TokenType::Parenthesis);
    REQUIRE(tokens[7].getValue<char>() == ')');

    REQUIRE(tokens[8].m_tType == TokenType::Parenthesis);
    REQUIRE(tokens[8].getValue<char>() == ')');
}

TEST_CASE("Multiple variables and functions in a row") {
    Lexer lexer("a+b*sin(theta)-c");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 10);

    REQUIRE(tokens[0].m_tType == TokenType::Variable);
    REQUIRE(tokens[0].getValue<std::string>() == "a");

    REQUIRE(tokens[1].getValue<OperatorType>() == OperatorType::Add);

    REQUIRE(tokens[2].m_tType == TokenType::Variable);
    REQUIRE(tokens[2].getValue<std::string>() == "b");

    REQUIRE(tokens[3].m_tType == TokenType::Operator);
    REQUIRE(tokens[3].getValue<OperatorType>() == OperatorType::Multiply);

    REQUIRE(tokens[4].m_tType == TokenType::Function);
    REQUIRE(tokens[4].getValue<std::string>() == "sin");

    REQUIRE(tokens[5].m_tType == TokenType::Parenthesis);
    REQUIRE(tokens[5].getValue<char>() == '(');

    REQUIRE(tokens[6].m_tType == TokenType::Variable);
    REQUIRE(tokens[6].getValue<std::string>() == "theta");

    REQUIRE(tokens[7].m_tType == TokenType::Parenthesis);
    REQUIRE(tokens[7].getValue<char>() == ')');

    REQUIRE(tokens[8].m_tType == TokenType::Operator);
    REQUIRE(tokens[8].getValue<OperatorType>() == OperatorType::Subtract);
}

TEST_CASE("Valid scientific notation with positive exponent") {
    Lexer lexer("1e10");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);

    REQUIRE(tokens[0].m_tType == TokenType::Number);
    REQUIRE(tokens[0].getValue<double>() == Catch::Approx(1e10));
}

TEST_CASE("Valid scientific notation with negative exponent") {
    Lexer lexer("2.5e-3");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);

    REQUIRE(tokens[0].m_tType == TokenType::Number);
    REQUIRE(tokens[0].getValue<double>() == Catch::Approx(0.0025));
}

TEST_CASE("Valid scientific notation with explicit plus sign") {
    Lexer lexer("3e+7");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);

    REQUIRE(tokens[0].m_tType == TokenType::Number);
    REQUIRE(tokens[0].getValue<double>() == Catch::Approx(30000000));
}

TEST_CASE("Incomplete scientific notation missing exponent digits") {
    Lexer lexer("1e");
    REQUIRE_THROWS_AS(lexer.tokenize(), std::runtime_error);
}

TEST_CASE("Incomplete scientific notation with trailing plus") {
    Lexer lexer("2e+");
    REQUIRE_THROWS_AS(lexer.tokenize(), std::runtime_error);
}

TEST_CASE("Incomplete scientific notation with trailing minus") {
    Lexer lexer("3e-");
    REQUIRE_THROWS_AS(lexer.tokenize(), std::runtime_error);
}

TEST_CASE("Scientific notation with invalid operator in exponent") {
    Lexer lexer("1e*5");
    REQUIRE_THROWS_AS(lexer.tokenize(), std::runtime_error);
}

TEST_CASE("Scientific notation with second dot in exponent") {
    Lexer lexer("1.2e3.4");
    REQUIRE_THROWS_AS(lexer.tokenize(), std::runtime_error);
}

TEST_CASE("Expression with scientific notation and operators") {
    Lexer lexer("5.5e2+sin(x)");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 6);

    REQUIRE(tokens[0].m_tType == TokenType::Number);
    REQUIRE(tokens[0].getValue<double>() == Catch::Approx(550));

    REQUIRE(tokens[1].m_tType == TokenType::Operator);
    REQUIRE(tokens[1].getValue<OperatorType>() == OperatorType::Add);

    REQUIRE(tokens[2].m_tType == TokenType::Function);
    REQUIRE(tokens[2].getValue<std::string>() == "sin");

    REQUIRE(tokens[3].m_tType == TokenType::Parenthesis);
    REQUIRE(tokens[3].getValue<char>() == '(');

    REQUIRE(tokens[4].m_tType == TokenType::Variable);
    REQUIRE(tokens[4].getValue<std::string>() == "x");

    REQUIRE(tokens[5].m_tType == TokenType::Parenthesis);
    REQUIRE(tokens[5].getValue<char>() == ')');
}

TEST_CASE("Decimal without leading digit") {
    Lexer lexer(".5");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);

    REQUIRE(tokens[0].m_tType == TokenType::Number);
    REQUIRE(tokens[0].getValue<double>() == Catch::Approx(0.5));
}

TEST_CASE("Invalid double dot number") {
    Lexer lexer("1..2");
    REQUIRE_THROWS_AS(lexer.tokenize(), std::runtime_error);
}
