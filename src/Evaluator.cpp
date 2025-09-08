#include "Evaluator.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <limits>

Evaluator::Evaluator() {
    // Initialize mathematical constants
    variables["pi"] = 3.141592653589793;
    variables["e"] = 2.718281828459045;
    variables["inf"] = std::numeric_limits<double>::infinity();
    variables["nan"] = std::numeric_limits<double>::quiet_NaN();
}

double Evaluator::evaluate(const ASTNode& node, std::unordered_map<std::string, double>* localVars) {
    switch (node.m_type) {
    case NodeType::Number:
        return node.getValue<double>();

    case NodeType::Variable: {
        auto name = node.getValue<std::string>();
        if (localVars && localVars->find(name) != localVars->end()) {
            return (*localVars)[name];
        }
        if (variables.find(name) != variables.end()) {
            return variables[name];
        }
        throw std::runtime_error("Undefined variable: " + name);
    }

    case NodeType::Operator: {
        auto op = node.getValue<OperatorType>();
        if (op == OperatorType::UnaryMinus) {
            return -evaluate(*node.m_children[0], localVars);
        }
        if (op == OperatorType::UnaryPlus) {
            return evaluate(*node.m_children[0], localVars);
        }
        if (op == OperatorType::Assignment) {
            if (node.m_children[0]->m_type == NodeType::Function) {
                auto name = node.m_children[0]->getValue<std::string>();
                if (node.m_children[0]->m_children.size() != 1 || node.m_children[0]->m_children[0]->m_type != NodeType::Variable) {
                    throw std::runtime_error("Function assignment requires one variable argument");
                }
                std::vector<std::string> argNames = { node.m_children[0]->m_children[0]->getValue<std::string>() };
                functions[name] = { std::move(argNames), node.m_children[1]->clone() };
                return 0.0;
            }
            if (node.m_children[0]->m_type != NodeType::Variable) {
                throw std::runtime_error("Assignment target must be a variable");
            }
            auto name = node.m_children[0]->getValue<std::string>();
            double value = evaluate(*node.m_children[1], localVars);
            variables[name] = value;
            return value;
        }
        if (op == OperatorType::Factorial) {
            double arg = evaluate(*node.m_children[0], localVars);
            if (arg < 0 || std::floor(arg) != arg) {
                throw std::runtime_error("Factorial requires a non-negative integer");
            }
            int n = static_cast<int>(arg);
            double result = 1.0;
            for (int i = 2; i <= n; ++i) {
                result *= i;
            }
            return result;
        }
        double left = evaluate(*node.m_children[0], localVars);
        double right = evaluate(*node.m_children[1], localVars);
        switch (op) {
        case OperatorType::Add: return left + right;
        case OperatorType::Subtract: return left - right;
        case OperatorType::Multiply: return left * right;
        case OperatorType::Divide:
            if (right == 0) throw std::runtime_error("Division by zero");
            return left / right;
        case OperatorType::Power: return std::pow(left, right);
        case OperatorType::Int_divide:
            if(right == 0) throw std::runtime_error("Division by zero");
            return std::floor(left / right);
        case OperatorType::Mod:return static_cast<double>(static_cast<int>(left) % static_cast<int>(right));
        default: throw std::runtime_error("Unsupported operator");
        }
    }

    case NodeType::Function: {
        auto name = node.getValue<std::string>();
        // Trigonometric functions
        if (name == "sin" || name == "cos" || name == "tan") {
            if (node.m_children.size() != 1) {
                throw std::runtime_error(name + " expects one argument");
            }
            double arg = evaluate(*node.m_children[0], localVars);
            if (name == "sin") return std::sin(arg);
            if (name == "cos") return std::cos(arg);
            if (name == "tan") {
                if (std::cos(arg) == 0) throw std::runtime_error("tan undefined at pi/2 + k*pi");
                return std::tan(arg);
            }
        }
        if (name == "asin" || name == "acos") {
            if (node.m_children.size() != 1) {
                throw std::runtime_error(name + " expects one argument");
            }
            double arg = evaluate(*node.m_children[0], localVars);
            if (arg < -1.0 || arg > 1.0) throw std::runtime_error(name + " requires argument in [-1, 1]");
            return name == "asin" ? std::asin(arg) : std::acos(arg);
        }
        if (name == "atan") {
            if (node.m_children.size() != 1) {
                throw std::runtime_error("atan expects one argument");
            }
            return std::atan(evaluate(*node.m_children[0], localVars));
        }
        if (name == "atan2") {
            if (node.m_children.size() != 2) {
                throw std::runtime_error("atan2 expects two arguments");
            }
            double y = evaluate(*node.m_children[0], localVars);
            double x = evaluate(*node.m_children[1], localVars);
            return std::atan2(y, x);
        }
        // Exponential and logarithmic
        if (name == "exp" || name == "sqrt") {
            if (node.m_children.size() != 1) {
                throw std::runtime_error(name + " expects one argument");
            }
            double arg = evaluate(*node.m_children[0], localVars);
            if (name == "sqrt" && arg < 0) throw std::runtime_error("sqrt requires non-negative argument");
            return name == "exp" ? std::exp(arg) : std::sqrt(arg);
        }
        if (name == "log" || name == "log10") {
            if (node.m_children.size() != 1) {
                throw std::runtime_error(name + " expects one argument");
            }
            double arg = evaluate(*node.m_children[0], localVars);
            if (arg <= 0) throw std::runtime_error(name + " requires positive argument");
            return name == "log" ? std::log(arg) : std::log10(arg);
        }
        // Rounding and utility
        if (name == "abs" || name == "floor" || name == "ceil" || name == "round") {
            if (node.m_children.size() != 1) {
                throw std::runtime_error(name + " expects one argument");
            }
            double arg = evaluate(*node.m_children[0], localVars);
            if (name == "abs") return std::abs(arg);
            if (name == "floor") return std::floor(arg);
            if (name == "ceil") return std::ceil(arg);
            if (name == "round") return std::round(arg);
        }
        if (name == "min") {
            if (node.m_children.empty()) {
                throw std::runtime_error("min requires at least one argument");
            }
            std::vector<double> args;
            for (const auto& child : node.m_children) {
                args.push_back(evaluate(*child, localVars));
            }
            return *std::min_element(args.begin(), args.end());
        }
        if (name == "factorial") {
            if (node.m_children.size() != 1) {
                throw std::runtime_error("factorial expects one argument");
            }
            double arg = evaluate(*node.m_children[0], localVars);
            if (arg < 0 || std::floor(arg) != arg) {
                throw std::runtime_error("factorial requires a non-negative integer");
            }
            int n = static_cast<int>(arg);
            double result = 1.0;
            for (int i = 2; i <= n; ++i) {
                result *= i;
            }
            return result;
        }
        if (name == "max") {
            if (node.m_children.empty()) {
                throw std::runtime_error("max requires at least one argument");
            }
            std::vector<double> args;
            for (const auto& child : node.m_children) {
                args.push_back(evaluate(*child, localVars));
            }
            return *std::max_element(args.begin(), args.end());
        }
        if (functions.find(name) == functions.end()) {
            throw std::runtime_error("Undefined function: " + name);
        }
        auto& func = functions[name];
        if (func.argNames.size() != node.m_children.size()) {
            throw std::runtime_error("Incorrect number of arguments for function: " + name);
        }
        std::unordered_map<std::string, double> funcVars;
        for (size_t i = 0; i < func.argNames.size(); ++i) {
            funcVars[func.argNames[i]] = evaluate(*node.m_children[i], localVars);
        }
        return evaluate(*func.body, &funcVars);
    }

    default:
        throw std::runtime_error("Unsupported node type");
    }
}