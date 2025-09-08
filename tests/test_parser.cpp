#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "Parser.h"
#include "Token.h"
#include "AST.h"

TEST_CASE("Parser: Unary minus and addition (-3+2)") {
    std::vector<Token> tokens = {
        Token(OperatorType::UnaryMinus),
        Token(3.0),
        Token(OperatorType::Add),
        Token(2.0)
    };

    Parser parser(tokens);
    auto ast = parser.parseExpression();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->m_type == NodeType::Operator);
    REQUIRE(ast->getValue<OperatorType>() == OperatorType::Add);
    REQUIRE(ast->m_children.size() == 2);

    // Left child: Unary minus
    auto& left = *ast->m_children[0];
    REQUIRE(left.m_type == NodeType::Operator);
    REQUIRE(left.getValue<OperatorType>() == OperatorType::UnaryMinus);
    REQUIRE(left.m_children.size() == 1);
    REQUIRE(left.m_children[0]->m_type == NodeType::Number);
    REQUIRE(left.m_children[0]->getValue<double>() == Catch::Approx(3.0));

    // Right child: Number 2
    auto& right = *ast->m_children[1];
    REQUIRE(right.m_type == NodeType::Number);
    REQUIRE(right.getValue<double>() == Catch::Approx(2.0));
}

TEST_CASE("Parser: Unary plus and multiplication (+5*2)") {
    std::vector<Token> tokens = {
        Token(OperatorType::UnaryPlus),
        Token(5.0),
        Token(OperatorType::Multiply),
        Token(2.0)
    };

    Parser parser(tokens);
    auto ast = parser.parseExpression();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->m_type == NodeType::Operator);
    REQUIRE(ast->getValue<OperatorType>() == OperatorType::Multiply);
    REQUIRE(ast->m_children.size() == 2);

    // Left child: Unary plus
    auto& left = *ast->m_children[0];
    REQUIRE(left.m_type == NodeType::Operator);
    REQUIRE(left.getValue<OperatorType>() == OperatorType::UnaryPlus);
    REQUIRE(left.m_children.size() == 1);
    REQUIRE(left.m_children[0]->m_type == NodeType::Number);
    REQUIRE(left.m_children[0]->getValue<double>() == Catch::Approx(5.0));

    // Right child: Number 2
    auto& right = *ast->m_children[1];
    REQUIRE(right.m_type == NodeType::Number);
    REQUIRE(right.getValue<double>() == Catch::Approx(2.0));
}

TEST_CASE("Parser: Parenthesized expression (-(4+5))") {
    std::vector<Token> tokens = {
        Token(OperatorType::UnaryMinus),
        Token(TokenType::Parenthesis, '('),
        Token(4.0),
        Token(OperatorType::Add),
        Token(5.0),
        Token(TokenType::Parenthesis, ')')
    };

    Parser parser(tokens);
    auto ast = parser.parseExpression();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->m_type == NodeType::Operator);
    REQUIRE(ast->getValue<OperatorType>() == OperatorType::UnaryMinus);
    REQUIRE(ast->m_children.size() == 1);

    // Child: Addition
    auto& child = *ast->m_children[0];
    REQUIRE(child.m_type == NodeType::Operator);
    REQUIRE(child.getValue<OperatorType>() == OperatorType::Add);
    REQUIRE(child.m_children.size() == 2);
    REQUIRE(child.m_children[0]->m_type == NodeType::Number);
    REQUIRE(child.m_children[0]->getValue<double>() == Catch::Approx(4.0));
    REQUIRE(child.m_children[1]->m_type == NodeType::Number);
    REQUIRE(child.m_children[1]->getValue<double>() == Catch::Approx(5.0));
}

TEST_CASE("Parser: Function call (-sin(x))") {
    std::vector<Token> tokens = {
        Token(OperatorType::UnaryMinus),
        Token(TokenType::Function, "sin"),
        Token(TokenType::Parenthesis, '('),
        Token(TokenType::Variable, "x"),
        Token(TokenType::Parenthesis, ')')
    };

    Parser parser(tokens);
    auto ast = parser.parseExpression();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->m_type == NodeType::Operator);
    REQUIRE(ast->getValue<OperatorType>() == OperatorType::UnaryMinus);
    REQUIRE(ast->m_children.size() == 1);

    // Child: Function sin
    auto& child = *ast->m_children[0];
    REQUIRE(child.m_type == NodeType::Function);
    REQUIRE(child.getValue<std::string>() == "sin");
    REQUIRE(child.m_children.size() == 1);
    REQUIRE(child.m_children[0]->m_type == NodeType::Variable);
    REQUIRE(child.m_children[0]->getValue<std::string>() == "x");
}

TEST_CASE("Parser: Complex expression (-3*-2+4)") {
    std::vector<Token> tokens = {
        Token(OperatorType::UnaryMinus),
        Token(3.0),
        Token(OperatorType::Multiply),
        Token(OperatorType::UnaryMinus),
        Token(2.0),
        Token(OperatorType::Add),
        Token(4.0)
    };

    Parser parser(tokens);
    auto ast = parser.parseExpression();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->m_type == NodeType::Operator);
    REQUIRE(ast->getValue<OperatorType>() == OperatorType::Add);
    REQUIRE(ast->m_children.size() == 2);

    // Left child: Multiplication
    auto& left = *ast->m_children[0];
    REQUIRE(left.m_type == NodeType::Operator);
    REQUIRE(left.getValue<OperatorType>() == OperatorType::Multiply);
    REQUIRE(left.m_children.size() == 2);

    // Left child's left: Unary minus
    REQUIRE(left.m_children[0]->m_type == NodeType::Operator);
    REQUIRE(left.m_children[0]->getValue<OperatorType>() == OperatorType::UnaryMinus);
    REQUIRE(left.m_children[0]->m_children.size() == 1);
    REQUIRE(left.m_children[0]->m_children[0]->m_type == NodeType::Number);
    REQUIRE(left.m_children[0]->m_children[0]->getValue<double>() == Catch::Approx(3.0));

    // Left child's right: Unary minus
    REQUIRE(left.m_children[1]->m_type == NodeType::Operator);
    REQUIRE(left.m_children[1]->getValue<OperatorType>() == OperatorType::UnaryMinus);
    REQUIRE(left.m_children[1]->m_children.size() == 1);
    REQUIRE(left.m_children[1]->m_children[0]->m_type == NodeType::Number);
    REQUIRE(left.m_children[1]->m_children[0]->getValue<double>() == Catch::Approx(2.0));

    // Right child: Number 4
    auto& right = *ast->m_children[1];
    REQUIRE(right.m_type == NodeType::Number);
    REQUIRE(right.getValue<double>() == Catch::Approx(4.0));
}

TEST_CASE("Parser: Double unary minus (--5)") {
    std::vector<Token> tokens = {
        Token(OperatorType::UnaryMinus),
        Token(OperatorType::UnaryMinus),
        Token(5.0)
    };

    Parser parser(tokens);
    auto ast = parser.parseExpression();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->m_type == NodeType::Operator);
    REQUIRE(ast->getValue<OperatorType>() == OperatorType::UnaryMinus);
    REQUIRE(ast->m_children.size() == 1);

    // Child: Unary minus
    auto& child = *ast->m_children[0];
    REQUIRE(child.m_type == NodeType::Operator);
    REQUIRE(child.getValue<OperatorType>() == OperatorType::UnaryMinus);
    REQUIRE(child.m_children.size() == 1);
    REQUIRE(child.m_children[0]->m_type == NodeType::Number);
    REQUIRE(child.m_children[0]->getValue<double>() == Catch::Approx(5.0));
}

TEST_CASE("Parser: Complex function expression (-3*min(1,2,3)+(-4)/x)") {
    std::vector<Token> tokens = {
        Token(OperatorType::UnaryMinus),
        Token(3.0),
        Token(OperatorType::Multiply),
        Token(TokenType::Function, "min"),
        Token(TokenType::Parenthesis, '('),
        Token(1.0),
        Token(TokenType::Comma, ','),
        Token(2.0),
        Token(TokenType::Comma, ','),
        Token(3.0),
        Token(TokenType::Parenthesis, ')'),
        Token(OperatorType::Add),
        Token(TokenType::Parenthesis, '('),
        Token(OperatorType::UnaryMinus),
        Token(4.0),
        Token(TokenType::Parenthesis, ')'),
        Token(OperatorType::Divide),
        Token(TokenType::Variable, "x")
    };

    Parser parser(tokens);
    auto ast = parser.parseExpression();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->m_type == NodeType::Operator);
    REQUIRE(ast->getValue<OperatorType>() == OperatorType::Add);
    REQUIRE(ast->m_children.size() == 2);

    // Left child: Multiplication
    auto& left = *ast->m_children[0];
    REQUIRE(left.m_type == NodeType::Operator);
    REQUIRE(left.getValue<OperatorType>() == OperatorType::Multiply);
    REQUIRE(left.m_children.size() == 2);

    // Left child's left: Unary minus
    REQUIRE(left.m_children[0]->m_type == NodeType::Operator);
    REQUIRE(left.m_children[0]->getValue<OperatorType>() == OperatorType::UnaryMinus);
    REQUIRE(left.m_children[0]->m_children.size() == 1);
    REQUIRE(left.m_children[0]->m_children[0]->m_type == NodeType::Number);
    REQUIRE(left.m_children[0]->m_children[0]->getValue<double>() == Catch::Approx(3.0));

    // Left child's right: Function min
    auto& minNode = *left.m_children[1];
    REQUIRE(minNode.m_type == NodeType::Function);
    REQUIRE(minNode.getValue<std::string>() == "min");
    REQUIRE(minNode.m_children.size() == 3);
    REQUIRE(minNode.m_children[0]->m_type == NodeType::Number);
    REQUIRE(minNode.m_children[0]->getValue<double>() == Catch::Approx(1.0));
    REQUIRE(minNode.m_children[1]->m_type == NodeType::Number);
    REQUIRE(minNode.m_children[1]->getValue<double>() == Catch::Approx(2.0));
    REQUIRE(minNode.m_children[2]->m_type == NodeType::Number);
    REQUIRE(minNode.m_children[2]->getValue<double>() == Catch::Approx(3.0));

    // Right child: Division
    auto& right = *ast->m_children[1];
    REQUIRE(right.m_type == NodeType::Operator);
    REQUIRE(right.getValue<OperatorType>() == OperatorType::Divide);
    REQUIRE(right.m_children.size() == 2);

    // Right child's left: Unary minus
    REQUIRE(right.m_children[0]->m_type == NodeType::Operator);
    REQUIRE(right.m_children[0]->getValue<OperatorType>() == OperatorType::UnaryMinus);
    REQUIRE(right.m_children[0]->m_children.size() == 1);
    REQUIRE(right.m_children[0]->m_children[0]->m_type == NodeType::Number);
    REQUIRE(right.m_children[0]->m_children[0]->getValue<double>() == Catch::Approx(4.0));

    // Right child's right: Variable x
    REQUIRE(right.m_children[1]->m_type == NodeType::Variable);
    REQUIRE(right.m_children[1]->getValue<std::string>() == "x");
}

// Failure test cases
TEST_CASE("Parser: Empty input") {
    std::vector<Token> tokens = {};

    Parser parser(tokens);
    REQUIRE_THROWS_AS(parser.parseExpression(), std::runtime_error);
}

TEST_CASE("Parser: Unary minus without operand (u-)") {
    std::vector<Token> tokens = {
        Token(OperatorType::UnaryMinus)
    };

    Parser parser(tokens);
    REQUIRE_THROWS_AS(parser.parseExpression(), std::runtime_error);
}

TEST_CASE("Parser: Binary operator without right operand (3+)") {
    std::vector<Token> tokens = {
        Token(3.0),
        Token(OperatorType::Add)
    };

    Parser parser(tokens);
    REQUIRE_THROWS_AS(parser.parseExpression(), std::runtime_error);
}

TEST_CASE("Parser: Unclosed parenthesis ( (3+4") {
    std::vector<Token> tokens = {
        Token(TokenType::Parenthesis, '('),
        Token(3.0),
        Token(OperatorType::Add),
        Token(4.0)
    };

    Parser parser(tokens);
    REQUIRE_THROWS_AS(parser.parseExpression(), std::runtime_error);
}

TEST_CASE("Parser: Function without opening parenthesis (sin x)") {
    std::vector<Token> tokens = {
        Token(TokenType::Function, "sin"),
        Token(TokenType::Variable, "x"),
        Token(TokenType::Parenthesis, ')')
    };

    Parser parser(tokens);
    REQUIRE_THROWS_AS(parser.parseExpression(), std::runtime_error);
}

TEST_CASE("Parser: Invalid operator as unary (3*+)") {
    std::vector<Token> tokens = {
        Token(3.0),
        Token(OperatorType::Multiply),
        Token(OperatorType::Add)
    };

    Parser parser(tokens);
    REQUIRE_THROWS_AS(parser.parseExpression(), std::runtime_error);
}