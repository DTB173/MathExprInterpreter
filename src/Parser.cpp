#include "Parser.h"
#include <algorithm>
#include <optional>

namespace {

    struct BP { int left, right; };

    std::optional<BP> infixPower(OperatorType op) {
        switch (op) {
        case OperatorType::Add:
        case OperatorType::Subtract:  return BP{ 1, 2 };
        case OperatorType::Multiply:
        case OperatorType::Divide:
        case OperatorType::IntDivide:
        case OperatorType::Mod:       return BP{ 3, 4 };
        case OperatorType::Power:     return BP{ 8, 7 };   // right-associative
        default:                      return std::nullopt;
        }
    }

    constexpr int kUnaryPower = 5;
    constexpr int kFactorialPower = 9;

    bool isParen(const tok::Token* t, char c) {
        if (!t) return false;
        const auto* p = t->get_if<tok::Parenthesis>();
        return p && p->symbol == c;
    }

    bool isOp(const tok::Token* t, OperatorType op) {
        if (!t) return false;
        const auto* o = t->get_if<tok::Operator>();
        return o && o->type == op;
    }

} // namespace

size_t Parser::errorPos() const {
    if (m_tokens.empty()) return 0;
    return m_tokens[std::min(m_pos, m_tokens.size() - 1)].m_position;
}

bool Parser::matchParen(char c) {
    if (isParen(at(m_pos), c)) { ++m_pos; return true; }
    return false;
}

void Parser::expectParen(char c) {
    if (!matchParen(c))
        throw ParseError(std::string("Expected '") + c + "'", errorPos());
}

bool Parser::matchComma() {
    if (at(m_pos) && at(m_pos)->is<tok::Comma>()) { ++m_pos; return true; }
    return false;
}

ast::NodePtr Parser::parse() {
    if (m_tokens.empty()) throw ParseError("Empty expression", 0);
    m_pos = 0;
    auto root = parseStatement();
    if (!atEnd()) throw ParseError("Unexpected token", errorPos());   // "1 2", "3 )", "a = = b"
    return root;
}

ast::NodePtr Parser::parseStatement() {
    if (looksLikeFuncDef())
        return parseFuncDef();

    // x = expr
    if (const auto* v = at(m_pos) ? at(m_pos)->get_if<tok::Variable>() : nullptr;
        v && isOp(at(m_pos + 1), OperatorType::Assignment)) {
        tok::Variable target = *v;
        m_pos += 2;
        return ast::makeNode(ast::Assign{ std::move(target), parseExpr(0) });
    }
    return parseExpr(0);
}

ast::NodePtr Parser::parseExpr(int minBP) {
    auto lhs = parsePrefix();

    while (!atEnd()) {
        const auto* op = at(m_pos)->get_if<tok::Operator>();
        if (!op) break;                                   // ')' ',' number... ends the expression

        if (op->type == OperatorType::Factorial) {        // postfix
            if (kFactorialPower < minBP) break;
            ++m_pos;
            lhs = ast::makeNode(ast::Unary{ OperatorType::Factorial, std::move(lhs) });
            continue;
        }

        auto bp = infixPower(op->type);
        if (!bp || bp->left < minBP) break;               // '=' also lands here -> parse() reports it

        ++m_pos;
        auto rhs = parseExpr(bp->right);
        lhs = ast::makeNode(ast::Binary{ op->type, std::move(lhs), std::move(rhs) });
    }
    return lhs;
}

ast::NodePtr Parser::parsePrefix() {
    if (atEnd()) throw ParseError("Unexpected end of input", errorPos());

    const tok::Token& token = m_tokens[m_pos++];

    if (const auto* n = token.get_if<tok::Number>())   return ast::makeNode(*n);
    if (const auto* v = token.get_if<tok::Variable>()) return ast::makeNode(*v);
    if (const auto* f = token.get_if<tok::Function>()) return parseCall(*f);

    if (const auto* p = token.get_if<tok::Parenthesis>()) {
        if (p->symbol != '(') throw ParseError("Unexpected ')'", token.m_position);
        auto inner = parseExpr(0);
        expectParen(')');
        return inner;                                     // parentheses leave no node: the tree is the grouping
    }

    if (const auto* o = token.get_if<tok::Operator>()) {
        if (o->type == OperatorType::Add || o->type == OperatorType::Subtract) {
            auto kind = o->type == OperatorType::Subtract ? OperatorType::UnaryMinus
                : OperatorType::UnaryPlus;
            return ast::makeNode(ast::Unary{ kind, parseExpr(kUnaryPower) });
        }
        throw ParseError("Operator needs a left operand", token.m_position);
    }

    throw ParseError("Unexpected token", token.m_position);   // Comma
}

ast::NodePtr Parser::parseCall(tok::Function callee) {
    expectParen('(');
    std::vector<ast::NodePtr> args;
    if (!matchParen(')')) {                               // empty arg list is allowed
        do { args.push_back(parseExpr(0)); } while (matchComma());
        expectParen(')');
    }
    return ast::makeNode(ast::Call{ std::move(callee), std::move(args) });
}

bool Parser::looksLikeFuncDef() const {
    size_t i = m_pos;
    if (!at(i) || !at(i)->is<tok::Function>()) return false;
    if (!isParen(at(++i), '(')) return false;
    ++i;
    while (at(i) && (at(i)->is<tok::Variable>() || at(i)->is<tok::Comma>())) ++i;
    if (!isParen(at(i), ')')) return false;
    return isOp(at(i + 1), OperatorType::Assignment);
}

ast::NodePtr Parser::parseFuncDef() {
    auto name = at(m_pos)->get_if<tok::Function>();
    m_pos += 2;                                           // name and '('

    std::vector<tok::Variable> params;
    if (!matchParen(')')) {
        do {
            const auto* v = atEnd() ? nullptr : at(m_pos)->get_if<tok::Variable>();
            if (!v) throw ParseError("Expected parameter name", errorPos());
            params.push_back(*v);
            ++m_pos;
        } while (matchComma());
        expectParen(')');
    }
    ++m_pos;                                              // '='

    std::shared_ptr<const ast::ASTNode> body = parseExpr(0);
    return ast::makeNode(ast::FuncDef{ std::move(*name), std::move(params), std::move(body) });
}