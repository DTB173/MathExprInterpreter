#pragma once
#include <memory>
#include <variant>
#include <string>
#include <string_view>
#include <iostream>
#include "Token.h"

enum class NodeType {
    Number,
    Variable,
    Function,
    Operator
};

struct ASTNode {
    std::vector<std::unique_ptr<ASTNode>> m_children;
    std::variant<std::string, OperatorType, double> m_value;
    NodeType m_type;

    static constexpr std::array<std::string_view, 4> sm_nodeTypeNames = { "Number", "Variable", "Function", "Operator" };

    explicit ASTNode(double value)
        : m_type{ NodeType::Number }
        , m_value{ value } {
    }
    ASTNode(std::string_view name, NodeType type)
        : m_type{ type }
        , m_value{ static_cast<std::string>(name) } {
        if (type != NodeType::Variable && type != NodeType::Function) {
            throw std::invalid_argument("Name only valid for Variable/Function");
        }
    }
    explicit ASTNode(OperatorType op)
        : m_type{ NodeType::Operator }
        , m_value{ op } {
    }

    void appendChild(std::unique_ptr<ASTNode> child) {
        m_children.push_back(std::move(child));
    }

    template<typename T>
    T& getValue() {
        return std::get<T>(m_value);
    }
    template<typename T>
    const T& getValue() const {
        return std::get<T>(m_value);
    }

    // Deep copy method for ASTNode
    std::unique_ptr<ASTNode> clone() const {
        std::unique_ptr<ASTNode> node;
        if (m_type == NodeType::Number) {
            node = std::make_unique<ASTNode>(getValue<double>());
        }
        else if (m_type == NodeType::Variable || m_type == NodeType::Function) {
            node = std::make_unique<ASTNode>(getValue<std::string>(), m_type);
        }
        else if (m_type == NodeType::Operator) {
            node = std::make_unique<ASTNode>(getValue<OperatorType>());
        }
        for (const auto& child : m_children) {
            node->appendChild(child->clone());
        }
        return node;
    }

    static void print(const ASTNode& node, int indent = 0) {
        using std::cout;
        auto leftPad = std::string(indent, ' ');
        size_t typeIdx = static_cast<size_t>(node.m_type);
        cout << leftPad << node.sm_nodeTypeNames[typeIdx] << ": ";
        switch (node.m_type) {
        case NodeType::Number: cout << node.getValue<double>(); break;
        case NodeType::Variable:
        case NodeType::Function: cout << node.getValue<std::string>(); break;
        case NodeType::Operator: cout << node.getValue<OperatorType>(); break;
        }
        cout << '\n';

        for (const auto& child : node.m_children) {
            print(*child, indent + 2);
        }
    }
};