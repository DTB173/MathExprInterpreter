#pragma once
#include <array>
#include "Token.h"
#include "AST.h"

class Parser {
    std::vector<Token> m_tokens;
    inline static constexpr std::array<int, 11> s_bindingPower = { 1, 2, 3, 3, 3, 4, 3, 5, 0, 4, 4 };
    size_t m_pos{};

public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ASTNode> parseExpression(int minBP = 0);
private:
    const Token& peek() const {
        return m_tokens[m_pos];
    }
    Token& next() {
        return m_tokens[m_pos++];
    }
    bool atEnd() const {
        return m_pos >= m_tokens.size();
    }
    int getBindingPower(OperatorType op) const {
        return s_bindingPower[static_cast<size_t>(op)];
    }

    std::unique_ptr<ASTNode> parseTerm();
    std::vector<std::unique_ptr<ASTNode>> parseFunctionArgs();
};