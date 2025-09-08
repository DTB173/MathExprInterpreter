#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <variant>
#include <stdexcept>
#include <algorithm>
#include <ranges>

#include <unordered_map>
#include "Token.h"

class Lexer {
    std::string m_input{};
public:
    Lexer(std::string_view str)
        :m_input{str} {
        m_input.erase(
            std::remove_if(m_input.begin(), m_input.end(), 
                [](char c) { return std::isspace(c); }), 
            m_input.end());
    }
    [[nodiscard]] std::vector<Token> tokenize();
};