#pragma once
#include <array>
#include <vector>
#include <iostream>
#include <variant>

#define PRINT_SYMBOL

enum class TokenType :int { Number, Operator, Variable, Function, Parenthesis, Comma };
enum class OperatorType :int { Add, Subtract, Multiply, Divide, Int_divide, Power, Mod, Factorial, Assignment, UnaryMinus, UnaryPlus };


inline std::ostream& operator<<(std::ostream& out, OperatorType op) {
#ifndef PRINT_SYMBOL
    constexpr std::array<std::string_view, 11> s_opNames = { "Add", "Subtract", "Multiply", "Divide", "IntDivide", "Power", "Mod", "Factorial", "Assignment","UnaryMinus","UnaryPlus"};
    out << s_opNames[static_cast<size_t>(op)];
#else
    constexpr std::array<std::string_view, 11> s_opSymbols = { "+", "-", "*", "/", "\\", "^", "%", "!", "=","u-","u+"};
    out << s_opSymbols[static_cast<size_t>(op)];
#endif
    return out;
}

struct Token {
    static constexpr std::array<std::string_view, 6> sm_tTypeNames = { "Number", "Operator", "Variable", "Function", "Parenthesis", "Comma" };

    TokenType m_tType{};
    std::variant<std::string, OperatorType, double, char> m_value{};
    size_t m_position{};
    // Number
    explicit Token(double value)
        : m_tType{ TokenType::Number }
        , m_value{ value } {
    }

    // Operator
    explicit Token(OperatorType type)
        : m_tType{ TokenType::Operator }
        , m_value{ type } {
    }

    // Parenthesis
    explicit Token(TokenType type, char parent)
        : m_tType{ type }
        , m_value{ parent } {
    }

    // Variable or Function
    explicit Token(TokenType type, const std::string& name)
        : m_tType{ type }
        , m_value{ name } {
        if (type != TokenType::Variable && type != TokenType::Function) {
            throw std::invalid_argument("Only Variable/Function can hold names");
        }
    }

    template<typename T>
    const T& getValue() const {
        return std::get<T>(m_value);
    }
    template<typename T>
    T& getValue() {
        return std::get<T>(m_value);
    }

    friend std::ostream& operator<<(std::ostream& out, const Token& t) {
        size_t namesidx = static_cast<size_t>(t.m_tType);
        out << t.sm_tTypeNames[namesidx] << ' ';
        switch (t.m_tType) {
        case TokenType::Number: out << t.getValue<double>(); break;
        case TokenType::Operator: out << t.getValue<OperatorType>(); break;
        case TokenType::Variable:
        case TokenType::Function: out << t.getValue<std::string>(); break;
        case TokenType::Parenthesis:
        case TokenType::Comma:out << t.getValue<char>(); break;
        default: out << "Unknown"; break;
        }
        return out;
    }
};