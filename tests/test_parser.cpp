#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "Parser.h"
#include "Token.h"
#include "AST.h"

TEST_CASE("Parser: Unary minus and addition (-3+2)") {
    std::vector<tok::Token> tokens = {
        tok::Token(tok::Operator{ OperatorType::Subtract }),
        tok::Token(tok::Number{ 3.0 }),
        tok::Token(tok::Operator{ OperatorType::Add }),
        tok::Token(tok::Number{ 2.0 })
    };

    Parser parser(tokens);
    auto ast = parser.parse();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->is<ast::Binary>());
    auto* bin = ast->get_if<ast::Binary>();
    REQUIRE(bin->op == OperatorType::Add);

    // Left child: Unary minus
    REQUIRE(bin->lhs->is<ast::Unary>());
    auto* unary = bin->lhs->get_if<ast::Unary>();
    REQUIRE(unary->op == OperatorType::UnaryMinus);
    REQUIRE(unary->operand->is<tok::Number>());
    // Let's check how Number is stored in ASTNode: ASTNode::Value variant contains tok::Number directly!
    REQUIRE(unary->operand->is<tok::Number>());
    REQUIRE(unary->operand->get_if<tok::Number>()->value == Catch::Approx(3.0));

    // Right child: Number 2
    REQUIRE(bin->rhs->is<tok::Number>());
    REQUIRE(bin->rhs->get_if<tok::Number>()->value == Catch::Approx(2.0));
}

TEST_CASE("Parser: Unary plus and multiplication (+5*2)") {
    std::vector<tok::Token> tokens = {
        tok::Token(tok::Operator{ OperatorType::Add }),
        tok::Token(tok::Number{ 5.0 }),
        tok::Token(tok::Operator{ OperatorType::Multiply }),
        tok::Token(tok::Number{ 2.0 })
    };

    Parser parser(tokens);
    auto ast = parser.parse();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->is<ast::Binary>());
    auto* bin = ast->get_if<ast::Binary>();
    REQUIRE(bin->op == OperatorType::Multiply);

    // Left child: Unary plus
    REQUIRE(bin->lhs->is<ast::Unary>());
    auto* unary = bin->lhs->get_if<ast::Unary>();
    REQUIRE(unary->op == OperatorType::UnaryPlus);
    REQUIRE(unary->operand->is<tok::Number>());
    REQUIRE(unary->operand->get_if<tok::Number>()->value == Catch::Approx(5.0));

    // Right child: Number 2
    REQUIRE(bin->rhs->is<tok::Number>());
    REQUIRE(bin->rhs->get_if<tok::Number>()->value == Catch::Approx(2.0));
}

TEST_CASE("Parser: Parenthesized expression (-(4+5))") {
    std::vector<tok::Token> tokens = {
        tok::Token(tok::Operator{ OperatorType::Subtract }),
        tok::Token(tok::Parenthesis{ '(' }),
        tok::Token(tok::Number{ 4.0 }),
        tok::Token(tok::Operator{ OperatorType::Add }),
        tok::Token(tok::Number{ 5.0 }),
        tok::Token(tok::Parenthesis{ ')' })
    };

    Parser parser(tokens);
    auto ast = parser.parse();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->is<ast::Unary>());
    auto* unary = ast->get_if<ast::Unary>();
    REQUIRE(unary->op == OperatorType::UnaryMinus);

    // Child: Addition
    REQUIRE(unary->operand->is<ast::Binary>());
    auto* bin = unary->operand->get_if<ast::Binary>();
    REQUIRE(bin->op == OperatorType::Add);
    REQUIRE(bin->lhs->is<tok::Number>());
    REQUIRE(bin->lhs->get_if<tok::Number>()->value == Catch::Approx(4.0));
    REQUIRE(bin->rhs->is<tok::Number>());
    REQUIRE(bin->rhs->get_if<tok::Number>()->value == Catch::Approx(5.0));
}

TEST_CASE("Parser: Function call (-sin(x))") {
    std::vector<tok::Token> tokens = {
        tok::Token(tok::Operator{ OperatorType::Subtract }),
        tok::Token(tok::Function{ "sin" }),
        tok::Token(tok::Parenthesis{ '(' }),
        tok::Token(tok::Variable{ "x" }),
        tok::Token(tok::Parenthesis{ ')' })
    };

    Parser parser(tokens);
    auto ast = parser.parse();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->is<ast::Unary>());
    auto* unary = ast->get_if<ast::Unary>();
    REQUIRE(unary->op == OperatorType::UnaryMinus);

    // Child: Function sin
    REQUIRE(unary->operand->is<ast::Call>());
    auto* call = unary->operand->get_if<ast::Call>();
    REQUIRE(call->callee.name == "sin");
    REQUIRE(call->args.size() == 1);
    REQUIRE(call->args[0]->is<tok::Variable>());
    REQUIRE(call->args[0]->get_if<tok::Variable>()->name == "x");
}

TEST_CASE("Parser: Complex expression (-3*-2+4)") {
    std::vector<tok::Token> tokens = {
        tok::Token(tok::Operator{ OperatorType::Subtract }),
        tok::Token(tok::Number{ 3.0 }),
        tok::Token(tok::Operator{ OperatorType::Multiply }),
        tok::Token(tok::Operator{ OperatorType::Subtract }),
        tok::Token(tok::Number{ 2.0 }),
        tok::Token(tok::Operator{ OperatorType::Add }),
        tok::Token(tok::Number{ 4.0 })
    };

    Parser parser(tokens);
    auto ast = parser.parse();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->is<ast::Binary>());
    auto* rootBin = ast->get_if<ast::Binary>();
    REQUIRE(rootBin->op == OperatorType::Add);

    // Left child: Multiplication
    REQUIRE(rootBin->lhs->is<ast::Binary>());
    auto* multBin = rootBin->lhs->get_if<ast::Binary>();
    REQUIRE(multBin->op == OperatorType::Multiply);

    // Left child's left: Unary minus
    REQUIRE(multBin->lhs->is<ast::Unary>());
    auto* leftUnary = multBin->lhs->get_if<ast::Unary>();
    REQUIRE(leftUnary->op == OperatorType::UnaryMinus);
    REQUIRE(leftUnary->operand->is<tok::Number>());
    REQUIRE(leftUnary->operand->get_if<tok::Number>()->value == Catch::Approx(3.0));

    // Left child's right: Unary minus
    REQUIRE(multBin->rhs->is<ast::Unary>());
    auto* rightUnary = multBin->rhs->get_if<ast::Unary>();
    REQUIRE(rightUnary->op == OperatorType::UnaryMinus);
    REQUIRE(rightUnary->operand->is<tok::Number>());
    REQUIRE(rightUnary->operand->get_if<tok::Number>()->value == Catch::Approx(2.0));

    // Right child: Number 4
    REQUIRE(rootBin->rhs->is<tok::Number>());
    REQUIRE(rootBin->rhs->get_if<tok::Number>()->value == Catch::Approx(4.0));
}

TEST_CASE("Parser: Double unary minus (--5)") {
    std::vector<tok::Token> tokens = {
        tok::Token(tok::Operator{ OperatorType::Subtract }),
        tok::Token(tok::Operator{ OperatorType::Subtract }),
        tok::Token(tok::Number{ 5.0 })
    };

    Parser parser(tokens);
    auto ast = parser.parse();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->is<ast::Unary>());
    auto* outerUnary = ast->get_if<ast::Unary>();
    REQUIRE(outerUnary->op == OperatorType::UnaryMinus);

    // Child: Unary minus
    REQUIRE(outerUnary->operand->is<ast::Unary>());
    auto* innerUnary = outerUnary->operand->get_if<ast::Unary>();
    REQUIRE(innerUnary->op == OperatorType::UnaryMinus);
    REQUIRE(innerUnary->operand->is<tok::Number>());
    REQUIRE(innerUnary->operand->get_if<tok::Number>()->value == Catch::Approx(5.0));
}

TEST_CASE("Parser: Complex function expression (-3*min(1,2,3)+(-4)/x)") {
    std::vector<tok::Token> tokens = {
        tok::Token(tok::Operator{ OperatorType::Subtract }),
        tok::Token(tok::Number{ 3.0 }),
        tok::Token(tok::Operator{ OperatorType::Multiply }),
        tok::Token(tok::Function{ "min" }),
        tok::Token(tok::Parenthesis{ '(' }),
        tok::Token(tok::Number{ 1.0 }),
        tok::Token(tok::Comma{}),
        tok::Token(tok::Number{ 2.0 }),
        tok::Token(tok::Comma{}),
        tok::Token(tok::Number{ 3.0 }),
        tok::Token(tok::Parenthesis{ ')' }),
        tok::Token(tok::Operator{ OperatorType::Add }),
        tok::Token(tok::Parenthesis{ '(' }),
        tok::Token(tok::Operator{ OperatorType::Subtract }),
        tok::Token(tok::Number{ 4.0 }),
        tok::Token(tok::Parenthesis{ ')' }),
        tok::Token(tok::Operator{ OperatorType::Divide }),
        tok::Token(tok::Variable{ "x" })
    };

    Parser parser(tokens);
    auto ast = parser.parse();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->is<ast::Binary>());
    auto* rootBin = ast->get_if<ast::Binary>();
    REQUIRE(rootBin->op == OperatorType::Add);

    // Left child: Multiplication
    REQUIRE(rootBin->lhs->is<ast::Binary>());
    auto* multBin = rootBin->lhs->get_if<ast::Binary>();
    REQUIRE(multBin->op == OperatorType::Multiply);

    // Left child's left: Unary minus
    REQUIRE(multBin->lhs->is<ast::Unary>());
    auto* leftUnary = multBin->lhs->get_if<ast::Unary>();
    REQUIRE(leftUnary->op == OperatorType::UnaryMinus);
    REQUIRE(leftUnary->operand->is<tok::Number>());
    REQUIRE(leftUnary->operand->get_if<tok::Number>()->value == Catch::Approx(3.0));

    // Left child's right: Function min
    REQUIRE(multBin->rhs->is<ast::Call>());
    auto* callNode = multBin->rhs->get_if<ast::Call>();
    REQUIRE(callNode->callee.name == "min");
    REQUIRE(callNode->args.size() == 3);
    REQUIRE(callNode->args[0]->is<tok::Number>());
    REQUIRE(callNode->args[0]->get_if<tok::Number>()->value == Catch::Approx(1.0));
    REQUIRE(callNode->args[1]->is<tok::Number>());
    REQUIRE(callNode->args[1]->get_if<tok::Number>()->value == Catch::Approx(2.0));
    REQUIRE(callNode->args[2]->is<tok::Number>());
    REQUIRE(callNode->args[2]->get_if<tok::Number>()->value == Catch::Approx(3.0));

    // Right child: Division
    REQUIRE(rootBin->rhs->is<ast::Binary>());
    auto* divBin = rootBin->rhs->get_if<ast::Binary>();
    REQUIRE(divBin->op == OperatorType::Divide);

    // Right child's left: Unary minus
    REQUIRE(divBin->lhs->is<ast::Unary>());
    auto* rightUnary = divBin->lhs->get_if<ast::Unary>();
    REQUIRE(rightUnary->op == OperatorType::UnaryMinus);
    REQUIRE(rightUnary->operand->is<tok::Number>());
    REQUIRE(rightUnary->operand->get_if<tok::Number>()->value == Catch::Approx(4.0));

    // Right child's right: Variable x
    REQUIRE(divBin->rhs->is<tok::Variable>());
    REQUIRE(divBin->rhs->get_if<tok::Variable>()->name == "x");
}

// Failure test cases
TEST_CASE("Parser: Empty input") {
    std::vector<tok::Token> tokens = {};

    Parser parser(tokens);
    REQUIRE_THROWS_AS(parser.parse(), ParseError);
}

TEST_CASE("Parser: Unary minus without operand (u-)") {
    std::vector<tok::Token> tokens = {
        tok::Token(tok::Operator{ OperatorType::Subtract })
    };

    Parser parser(tokens);
    REQUIRE_THROWS_AS(parser.parse(), ParseError);
}

TEST_CASE("Parser: Binary operator without right operand (3+)") {
    std::vector<tok::Token> tokens = {
        tok::Token(tok::Number{ 3.0 }),
        tok::Token(tok::Operator{ OperatorType::Add })
    };

    Parser parser(tokens);
    REQUIRE_THROWS_AS(parser.parse(), ParseError);
}

TEST_CASE("Parser: Unclosed parenthesis ( (3+4") {
    std::vector<tok::Token> tokens = {
        tok::Token(tok::Parenthesis{ '(' }),
        tok::Token(tok::Number{ 3.0 }),
        tok::Token(tok::Operator{ OperatorType::Add }),
        tok::Token(tok::Number{ 4.0 })
    };

    Parser parser(tokens);
    REQUIRE_THROWS_AS(parser.parse(), ParseError);
}

TEST_CASE("Parser: Function without opening parenthesis (sin x)") {
    std::vector<tok::Token> tokens = {
        tok::Token(tok::Function{ "sin" }),
        tok::Token(tok::Variable{ "x" }),
        tok::Token(tok::Parenthesis{ ')' })
    };

    Parser parser(tokens);
    REQUIRE_THROWS_AS(parser.parse(), ParseError);
}

TEST_CASE("Parser: Invalid operator as unary (3*+)") {
    std::vector<tok::Token> tokens = {
        tok::Token(tok::Number{ 3.0 }),
        tok::Token(tok::Operator{ OperatorType::Multiply }),
        tok::Token(tok::Operator{ OperatorType::Add })
    };

    Parser parser(tokens);
    REQUIRE_THROWS_AS(parser.parse(), ParseError);
}