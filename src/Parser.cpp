#include "Parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) {
    m_tokens.reserve(tokens.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].m_tType == TokenType::Operator &&
            (tokens[i].getValue<OperatorType>() == OperatorType::Add ||
                tokens[i].getValue<OperatorType>() == OperatorType::Subtract))
        {
            bool isUnary = false;
            if (i == 0) {
                isUnary = true;
            }
            else {
                const Token& prev = tokens[i - 1];
                if (prev.m_tType == TokenType::Operator ||
                    (prev.m_tType == TokenType::Parenthesis && prev.getValue<char>() == '('))
                {
                    isUnary = true;
                }
            }

            if (isUnary) {
                OperatorType unaryOp = (tokens[i].getValue<OperatorType>() == OperatorType::Subtract)
                    ? OperatorType::UnaryMinus
                    : OperatorType::UnaryPlus;
                m_tokens.push_back(Token(unaryOp));
                continue;
            }
        }

        m_tokens.push_back(tokens[i]);
    }
}

std::unique_ptr<ASTNode> Parser::parseExpression(int minBP) {
    auto lhs = parseTerm();
    if (!lhs) {
        throw std::runtime_error("Expected expression at position " + std::to_string(m_pos));
    }

    while (!atEnd()) {
        auto opToken = peek();
        if (opToken.m_tType != TokenType::Operator) {
            break;
        }
        auto op = opToken.getValue<OperatorType>();
        if (op == OperatorType::UnaryMinus || op == OperatorType::UnaryPlus || op == OperatorType::Factorial) {
            break; // Handled in parseTerm
        }

        int bp = getBindingPower(op);
        if (bp < minBP) {
            break;
        }

        next();
        auto rhs = parseExpression(bp);
        if (!rhs) {
            throw std::runtime_error("Expected right operand at position " + std::to_string(m_pos));
        }

        auto node = std::make_unique<ASTNode>(op);
        node->appendChild(std::move(lhs));
        node->appendChild(std::move(rhs));
        lhs = std::move(node);
    }

    // Handle postfix factorial after any expression
    while (!atEnd() && peek().m_tType == TokenType::Operator && peek().getValue<OperatorType>() == OperatorType::Factorial) {
        next();
        auto node = std::make_unique<ASTNode>(OperatorType::Factorial);
        node->appendChild(std::move(lhs));
        lhs = std::move(node);
    }

    return lhs;
}

std::unique_ptr<ASTNode> Parser::parseTerm() {
    if (atEnd()) {
        throw std::runtime_error("Unexpected end of input");
    }

    auto token = next();

    switch (token.m_tType) {
    case TokenType::Number: {
        auto node = std::make_unique<ASTNode>(token.getValue<double>());
        // Check for postfix factorial (e.g., 5!)
        while (!atEnd() && peek().m_tType == TokenType::Operator && peek().getValue<OperatorType>() == OperatorType::Factorial) {
            next();
            auto factorialNode = std::make_unique<ASTNode>(OperatorType::Factorial);
            factorialNode->appendChild(std::move(node));
            node = std::move(factorialNode);
        }
        return node;
    }

    case TokenType::Variable:
        return std::make_unique<ASTNode>(token.getValue<std::string>(), NodeType::Variable);

    case TokenType::Function: {
        auto node = std::make_unique<ASTNode>(token.getValue<std::string>(), NodeType::Function);
        if (atEnd() || peek().m_tType != TokenType::Parenthesis || peek().getValue<char>() != '(') {
            throw std::runtime_error("Expected '(' after function at position " + std::to_string(token.m_position));
        }
        next();
        auto args = parseFunctionArgs();
        for (auto& arg : args) {
            node->appendChild(std::move(arg));
        }
        if (atEnd() || peek().m_tType != TokenType::Parenthesis || peek().getValue<char>() != ')') {
            throw std::runtime_error("Expected ')' after function arguments at position " + std::to_string(m_pos));
        }
        next();
        // Check for factorial after function (e.g., max(1,2)!)
        while (!atEnd() && peek().m_tType == TokenType::Operator && peek().getValue<OperatorType>() == OperatorType::Factorial) {
            next();
            auto factorialNode = std::make_unique<ASTNode>(OperatorType::Factorial);
            factorialNode->appendChild(std::move(node));
            node = std::move(factorialNode);
        }
        return node;
    }

    case TokenType::Parenthesis: {
        if (token.getValue<char>() != '(') {
            throw std::runtime_error("Expected '(' at position " + std::to_string(token.m_position));
        }
        auto expr = parseExpression(0);
        if (atEnd() || peek().m_tType != TokenType::Parenthesis || peek().getValue<char>() != ')') {
            throw std::runtime_error("Expected ')' at position " + std::to_string(m_pos));
        }
        next();
        // Check for factorial after parenthesized expression (e.g., (-5)!)
        while (!atEnd() && peek().m_tType == TokenType::Operator && peek().getValue<OperatorType>() == OperatorType::Factorial) {
            next();
            auto factorialNode = std::make_unique<ASTNode>(OperatorType::Factorial);
            factorialNode->appendChild(std::move(expr));
            expr = std::move(factorialNode);
        }
        return expr;
    }

    case TokenType::Operator: {
        auto op = token.getValue<OperatorType>();
        if (op != OperatorType::UnaryMinus && op != OperatorType::UnaryPlus) {
            throw std::runtime_error("Expected unary operator at position " + std::to_string(token.m_position));
        }
        int bp = getBindingPower(op);
        auto operand = parseExpression(bp);
        if (!operand) {
            throw std::runtime_error("Expected operand after unary operator at position " + std::to_string(m_pos));
        }
        auto node = std::make_unique<ASTNode>(op);
        node->appendChild(std::move(operand));
        return node;
    }

    default:
        throw std::runtime_error("Unexpected token at position " + std::to_string(token.m_position));
    }
}

std::vector<std::unique_ptr<ASTNode>> Parser::parseFunctionArgs() {
    std::vector<std::unique_ptr<ASTNode>> args;
    if (!atEnd() && peek().m_tType == TokenType::Parenthesis && peek().getValue<char>() == ')') {
        return args;
    }

    while (true) {
        auto arg = parseExpression(0);
        args.push_back(std::move(arg));
        if (atEnd() || peek().m_tType != TokenType::Comma) {
            break;
        }
        next();
    }

    return args;
}