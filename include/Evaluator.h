#pragma once
#include <functional>
#include <limits>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "AST.h"

class EvalError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Evaluator {
public:
    Evaluator();
    std::optional<double> execute(const ast::ASTNode& node);

private:
    using Scope = std::unordered_map<std::string, double>;

    struct Builtin {
        size_t minArgs;
        size_t maxArgs;
        std::function<double(std::span<const double>)> fn;
    };
    struct UserFunction {
        std::vector<tok::Variable> params;
        std::shared_ptr<const ast::ASTNode> body;
    };

    static constexpr size_t npos = std::numeric_limits<size_t>::max();
    static constexpr int kMaxDepth = 200;

    double eval(const ast::ASTNode& n, const Scope* locals);
    double evalCall(const ast::Call& c, const Scope* locals);
    static double applyBinary(OperatorType op, double l, double r);
    static double applyUnary(OperatorType op, double x);

    Scope m_globals;
    std::unordered_map<std::string, UserFunction> m_functions;
    std::unordered_map<std::string, Builtin> m_builtins;
    int m_depth{};
};