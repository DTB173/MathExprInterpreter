#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include "Token.h"

class LexError : public std::runtime_error {
public:
    LexError(const std::string& msg, size_t pos)
        : std::runtime_error(msg + " at position " + std::to_string(pos + 1)), m_pos{ pos } {}
    size_t position() const { return m_pos; }
private:
    size_t m_pos;
};

class Lexer {
public:
    explicit Lexer(std::string_view input) : m_input{ input } {}
    [[nodiscard]] std::vector<tok::Token> tokenize();

private:
    char peek(size_t ahead = 0) const {
        return m_pos + ahead < m_input.size() ? m_input[m_pos + ahead] : '\0';
    }

    template<typename P>
        requires std::predicate<P, char>
    void skip(P pred);

    tok::Number lexNumber();
    std::string lexIdentifier();

    std::string m_input;
    size_t      m_pos{};
};