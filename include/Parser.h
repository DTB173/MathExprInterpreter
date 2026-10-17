#pragma once
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "AST.h"
#include "Token.h"

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& msg, size_t pos)
        : std::runtime_error(msg + " at position " + std::to_string(pos + 1)), m_pos{ pos } {}
    size_t position() const { return m_pos; }
private:
    size_t m_pos;
};

class Parser {
public:
    explicit Parser(std::vector<tok::Token> tokens) : m_tokens{ std::move(tokens) } {}
    [[nodiscard]] ast::NodePtr parse();

private:
    ast::NodePtr parseStatement();
    ast::NodePtr parseExpr(int minBP);
    ast::NodePtr parsePrefix();
    ast::NodePtr parseCall(tok::Function callee);
    ast::NodePtr parseFuncDef();

    bool looksLikeFuncDef() const;

    const tok::Token* at(size_t i) const { return i < m_tokens.size() ? &m_tokens[i] : nullptr; }
    bool   atEnd() const { return m_pos >= m_tokens.size(); }
    size_t errorPos() const;                    // position of current token, or of the last one
    bool   matchParen(char c);                  // consumes if present
    void   expectParen(char c);
    bool   matchComma();

    std::vector<tok::Token> m_tokens;
    size_t m_pos{};
};