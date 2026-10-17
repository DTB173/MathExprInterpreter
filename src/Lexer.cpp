#include "Lexer.h"
#include <cctype>
#include <optional>

namespace {
	bool isDigit(char c)      { return std::isdigit(static_cast<unsigned char>(c)) != 0; }
	bool isIdentStart(char c) { return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_'; }
	bool isIdentChar(char c)  { return isIdentStart(c) || isDigit(c); }
	bool isSpace(char c)      { return std::isspace(static_cast<unsigned char>(c)) != 0; }

	std::optional<OperatorType> toOperator(char c) {
		switch (c) {
		case '+':  return OperatorType::Add;
		case '-':  return OperatorType::Subtract;
		case '*':  return OperatorType::Multiply;
		case '/':  return OperatorType::Divide;
		case '\\': return OperatorType::IntDivide;
		case '^':  return OperatorType::Power;
		case '%':  return OperatorType::Mod;
		case '!':  return OperatorType::Factorial;
		case '=':  return OperatorType::Assignment;
		default:   return std::nullopt;
		}
	}
}

template<typename P>
	requires std::predicate<P, char>
void Lexer::skip(P pred) {
	while (pred(peek())) ++m_pos;
}

tok::Number Lexer::lexNumber() {
	const size_t start = m_pos;

	skip(isDigit);
	if (peek() == '.') {
		++m_pos;
		skip(isDigit);
	}
	// Exponent only counts if a digit actually follows: "2e3", "2e-3", but "2e" is 2 and e.
	if (peek() == 'e' || peek() == 'E') {
		size_t k = 1;
		if (peek(k) == '+' || peek(k) == '-') ++k;
		if (isDigit(peek(k))) {
			m_pos += k;
			skip(isDigit);
		}
	}
	if (peek() == '.') throw LexError("Unexpected '.'", m_pos);

	try {
		return tok::Number{ std::stod(m_input.substr(start, m_pos - start)) };
	}
	catch (const std::out_of_range&) {
		throw LexError("Number out of range", start);
	}
}

std::string Lexer::lexIdentifier() {
	const size_t start = m_pos;
	skip(isIdentChar);
	return m_input.substr(start, m_pos - start);   // log10, atan2, x1
}

std::vector<tok::Token> Lexer::tokenize() {
	std::vector<tok::Token> tokens;
	m_pos = 0;

	for (;;) {
		skip(isSpace);
		if (m_pos >= m_input.size()) break;

		const size_t start = m_pos;
		const char   c = peek();

		if (isDigit(c) || (c == '.' && isDigit(peek(1)))) {
			tokens.emplace_back(lexNumber(), start);
		}
		else if (isIdentStart(c)) {
			std::string name = lexIdentifier();
			size_t k = 0;
			while (isSpace(peek(k))) ++k;                 // allow "sin (x)"
			if (peek(k) == '(') tokens.emplace_back(tok::Function{ std::move(name) }, start);
			else                tokens.emplace_back(tok::Variable{ std::move(name) }, start);
		}
		else if (c == '(' || c == ')') {
			tokens.emplace_back(tok::Parenthesis{ c }, start);
			++m_pos;
		}
		else if (c == ',') {
			tokens.emplace_back(tok::Comma{}, start);
			++m_pos;
		}
		else if (auto op = toOperator(c)) {
			tokens.emplace_back(tok::Operator{ *op }, start);
			++m_pos;
		}
		else {
			throw LexError(std::string("Unexpected character '") + c + "'", start);
		}
	}
	return tokens;
}