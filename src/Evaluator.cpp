#include "Evaluator.h"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <string_view>
#include <ranges>

namespace {
    double require(bool ok, double value, std::string_view msg) {
        if (!ok) throw EvalError(msg.data());
        return value;
    }
}

Evaluator::Evaluator() {
    m_globals = { { "pi", std::numbers::pi },{ "e", std::numbers::e },
                  { "inf", std::numeric_limits<double>::infinity() },
                  { "nan", std::numeric_limits<double>::quiet_NaN() } };

    auto unary = [&](const char* name, double(*f)(double)) {
        m_builtins[name] = { 1, 1, [f](std::span<const double> a) { return f(a[0]); } };
        };

    auto factorial = [](double n) {
        int ni = static_cast<int>(n);
        
        };

    unary("sin", std::sin);     unary("cos", std::cos);   unary("tan", std::tan);
    unary("atan", std::atan);   unary("exp", std::exp);   unary("abs", std::fabs);
    unary("floor", std::floor); unary("ceil", std::ceil); unary("round", std::round);

    m_builtins["asin"] =  { 1, 1,    [](auto a) { return require(std::abs(a[0]) <= 1, std::asin(a[0]), "asin requires argument in [-1, 1]"); } };
    m_builtins["acos"] =  { 1, 1,    [](auto a) { return require(std::abs(a[0]) <= 1, std::acos(a[0]), "acos requires argument in [-1, 1]"); } };
    m_builtins["sqrt"] =  { 1, 1,    [](auto a) { return require(a[0] >= 0, std::sqrt(a[0]), "sqrt requires non-negative argument"); } };
    m_builtins["log"] =   { 1, 1,    [](auto a) { return require(a[0] > 0, std::log(a[0]),   "log requires positive argument"); } };
    m_builtins["log10"] = { 1, 1,    [](auto a) { return require(a[0] > 0, std::log10(a[0]), "log10 requires positive argument"); } };
    m_builtins["atan2"] = { 2, 2,    [](auto a) { return std::atan2(a[0], a[1]); } };
    m_builtins["min"] =   { 1, npos, [](auto a) { return *std::ranges::min_element(a); } };
    m_builtins["max"] =   { 1, npos, [](auto a) { return *std::ranges::max_element(a); } };
    m_builtins["factorial"] = { 1, 170, [](auto a) {
        int n = static_cast<int>(a[0]);
        if (n < 0) return 0.0;
        return static_cast<double>(
            std::ranges::fold_left(std::views::iota(1, n + 1), 1LL, std::multiplies<long long>())
        );
    } };
}

std::optional<double> Evaluator::execute(const ast::ASTNode& node) {
    if (const auto* def = node.get_if<ast::FuncDef>()) {
        if (m_builtins.contains(def->callee.name))
            throw EvalError("Cannot redefine builtin function: " + def->callee.name);
        m_functions[def->callee.name] = { def->params, def->body };
        return std::nullopt;
    }
    return eval(node, nullptr);
}

double Evaluator::eval(const ast::ASTNode& n, const Scope* locals) {
    return std::visit(tok::overloaded{
        [](const tok::Number& x) { return x.value; },

        [&](const tok::Variable& v) {
            if (locals)
                if (auto it = locals->find(v.name); it != locals->end()) return it->second;
            if (auto it = m_globals.find(v.name); it != m_globals.end()) return it->second;
            throw EvalError("Undefined variable: " + v.name);
        },
        [&](const ast::Unary& u)  { return applyUnary(u.op, eval(*u.operand, locals)); },
        [&](const ast::Binary& b) { return applyBinary(b.op, eval(*b.lhs, locals), eval(*b.rhs, locals)); },
        [&](const ast::Assign& a) { return m_globals[a.target.name] = eval(*a.value, locals); },
        [&](const ast::Call& c)   { return evalCall(c, locals); },
        [&](const ast::FuncDef&) -> double { throw EvalError("Function definition not allowed here"); },
        }, n.m_value);
}

double Evaluator::evalCall(const ast::Call& c, const Scope* locals) {
    std::vector<double> args;
    args.reserve(c.args.size());
    for (const auto& a : c.args) args.push_back(eval(*a, locals));

    const std::string& name = c.callee.name;

    if (auto it = m_builtins.find(name); it != m_builtins.end()) {
        const Builtin& b = it->second;
        if (args.size() < b.minArgs || args.size() > b.maxArgs)
            throw EvalError(name + ": wrong number of arguments");
        return b.fn(args);
    }

    auto it = m_functions.find(name);
    if (it == m_functions.end()) throw EvalError("Undefined function: " + name);
    const UserFunction& f = it->second;
    if (f.params.size() != args.size())
        throw EvalError(name + " expects " + std::to_string(f.params.size()) + " argument(s)");
    if (m_depth >= kMaxDepth) throw EvalError("Maximum recursion depth exceeded");

    Scope frame;
    for (size_t i = 0; i < args.size(); ++i) frame[f.params[i].name] = args[i];

    ++m_depth;
    struct Guard { int& d; ~Guard() { --d; } } guard{ m_depth };   // restores depth even on throw
    return eval(*f.body, &frame);
}

double Evaluator::applyBinary(OperatorType op, double l, double r) {
    switch (op) {
    case OperatorType::Add:       return l + r;
    case OperatorType::Subtract:  return l - r;
    case OperatorType::Multiply:  return l * r;
    case OperatorType::Divide:    return require(r != 0, l / r, "Division by zero");
    case OperatorType::IntDivide: return require(r != 0, std::floor(l / r), "Division by zero");
    case OperatorType::Mod:       return require(r != 0, std::fmod(l, r), "Modulo by zero");
    case OperatorType::Power:     return std::pow(l, r);
    default: throw EvalError("Not a binary operator");
    }
}

double Evaluator::applyUnary(OperatorType op, double x) {
    switch (op) {
    case OperatorType::UnaryMinus: return -x;
    case OperatorType::UnaryPlus:  return x;
    case OperatorType::Factorial: {
        if (x < 0 || std::floor(x) != x) throw EvalError("Factorial requires a non-negative integer");
        if (x > 170) throw EvalError("Factorial too large");
        double result = 1;
        for (int i = 2; i <= static_cast<int>(x); ++i) result *= i;
        return result;
    }
    default: throw EvalError("Not a unary operator");
    }
}