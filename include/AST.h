#pragma once
#include <memory>
#include <variant>
#include <string>
#include <string_view>
#include <ostream>
#include <utility>
#include <vector>
#include "Token.h"

namespace ast {
    struct ASTNode;
    using NodePtr = std::unique_ptr<ASTNode>;

    struct Unary  { OperatorType op; NodePtr operand; };
    struct Binary { OperatorType op; NodePtr lhs, rhs; };
    struct Assign { tok::Variable target; NodePtr value; };
    struct Call   { tok::Function callee; std::vector<NodePtr> args; };
    struct FuncDef { tok::Function callee; std::vector<tok::Variable> params; std::shared_ptr<const ASTNode> body; };

    struct ASTNode {
        using Value    = std::variant<tok::Number, tok::Variable, Unary, Binary, Assign, Call, FuncDef>;
        using Children = std::vector<NodePtr>;

        Value    m_value;

        explicit ASTNode(Value v) : m_value{ std::move(v) } {}

        template<typename T> const T* get_if() const { return std::get_if<T>(&m_value); }
        template<typename T> bool     is()     const { return std::holds_alternative<T>(m_value); }
    };

    inline NodePtr makeNode(ASTNode::Value v) {
        return std::make_unique<ASTNode>(ASTNode{ std::move(v) });
    }

    inline void print(std::ostream& out, const ASTNode& n, int indent = 0) {
        const std::string pad(indent, ' ');
        std::visit(tok::overloaded{
            [&](const tok::Number& x)   { out << pad << "Number "   << x.value << '\n'; },
            [&](const tok::Variable& x) { out << pad << "Variable " << x.name  << '\n'; },
            [&](const Unary& u)  { out << pad << "Unary "  << u.op          << '\n'; print(out, *u.operand, indent + 2); },
            [&](const Binary& b) { out << pad << "Binary " << b.op          << '\n'; print(out, *b.lhs,     indent + 2); print(out, *b.rhs, indent + 2); },
            [&](const Assign& a) { out << pad << "Assign " << a.target.name << '\n'; print(out, *a.value,   indent + 2); },
            [&](const Call& c)   { out << pad << "Call "   << c.callee.name << '\n';  for (const auto& arg : c.args)     print(out, *arg, indent + 2); },
            [&](const FuncDef& f){ out << pad << "FuncDef " << f.callee.name << '\n'; for (const auto& param : f.params) out << "  " << "Param " << param.name << '\n'; print(out, *f.body, indent + 4); }
        }, n.m_value);
    }
}