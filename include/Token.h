#pragma once
#include <array>
#include <ostream>
#include <variant>
#include <string>
#include <utility>
#include <string_view>
#include <stdexcept>

enum class OperatorType :int { Add, Subtract, Multiply, Divide, IntDivide, Power, Mod, Factorial, Assignment, UnaryMinus, UnaryPlus };

inline constexpr bool k_PrintSymbols = true;

inline std::ostream& operator<<(std::ostream& out, OperatorType op) {
	static constexpr std::array<std::string_view, 11> names = {
		"Add", "Subtract", "Multiply", "Divide", "IntDivide", "Power",
		"Mod", "Factorial", "Assignment", "UnaryMinus", "UnaryPlus" };
	static constexpr std::array<std::string_view, 11> symbols = {
		"+", "-", "*", "/", "\\", "^", "%", "!", "=", "u-", "u+" };
	return out << (k_PrintSymbols ? symbols : names)[static_cast<size_t>(op)];
}

namespace tok {
	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

	struct Number { double value; };
	struct Operator { OperatorType type; };
	struct Variable { std::string name; };
	struct Function { std::string name; };
	struct Parenthesis { char symbol; };
	struct Comma {};

	struct Token {
		using Value = std::variant<Number, Operator, Variable, Function, Parenthesis, Comma>;

		Value m_value;
		size_t m_position{};

		Token(Value v, size_t pos = 0)
			: m_value{ std::move(v) }, m_position{ pos } {}

		template<typename T> const T* get_if() const { return std::get_if<T>(&m_value); }
		template<typename T> bool     is()     const { return std::holds_alternative<T>(m_value); }
	};

	struct Printer {
		std::ostream& out;
		void operator()(const Number& n)      { out << "Number " << n.value; }
		void operator()(const Operator& o)    { out << "Operator " << o.type; }
		void operator()(const Variable& v)    { out << "Variable " << v.name; }
		void operator()(const Function& f)    { out << "Function " << f.name; }
		void operator()(const Parenthesis& p) { out << "Parenthesis " << p.symbol; }
		void operator()(const Comma&)         { out << "Comma ,"; }
	};

	inline std::ostream& operator<<(std::ostream& out, const Token& t) {
		std::visit(Printer{ out }, t.m_value);
		return out;
	}
}
