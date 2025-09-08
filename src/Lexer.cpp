#include "Lexer.h"

double toDouble(const std::string& str) {
	double num{};
	try {
		num = std::stod(str);
	}
	catch (std::exception& e) {
		std::cout << "Error parsing number: \"" << e.what()<<"\"";
	}
	return num;
}

std::vector<Token> Lexer::tokenize() {
	std::vector<Token> tokens;
	tokens.reserve(m_input.size());
	std::string buffer;
	buffer.reserve(10);
	enum class CharType {None, Digit, Alpha, Parenthesis, Operator, Dot, Comma};
	static const std::unordered_map<char, OperatorType> validOperators = { 
		{'+', OperatorType::Add},
		{'-', OperatorType::Subtract},
		{'*', OperatorType::Multiply},
		{'/', OperatorType::Divide},
		{'\\', OperatorType::Int_divide},
		{'^', OperatorType::Power},
		{'!', OperatorType::Factorial},
		{'%', OperatorType::Mod},
		{'=', OperatorType::Assignment}
	};

	auto isOperator = [](char c) -> bool {
		return validOperators.contains(c);
		};

	auto getCharType = [&isOperator](char c) -> CharType {
		if (c == '(' || c == ')') return CharType::Parenthesis;
		if (c == '.') return CharType::Dot;
		if (c == ',') return CharType::Comma;
		if (std::isdigit(c)) return CharType::Digit;
		if (std::isalpha(c)) return CharType::Alpha;
		if (isOperator(c)) return CharType::Operator;
		return CharType::None;
		};

	bool wasDot = false;
	bool isScientific = false;
	CharType currType{};
	for (int i{}; i < m_input.length(); ++i) {
		currType = getCharType(m_input[i]);

		if (!buffer.empty()) {
			if (getCharType(buffer.back()) == CharType::Digit && std::tolower(m_input[i]) == 'e') {
				if (!isScientific) {
					buffer += m_input[i];
					currType = CharType::Digit;
					isScientific = true;
					wasDot = false;
					continue;
				}
				else {
					throw std::runtime_error("Invalid argument at position " + std::to_string(i+1));
				}
			}
		}
		switch (currType) {
			case CharType::Operator:
				if(!isScientific)
					tokens.emplace_back(validOperators.at(m_input[i])); 
				else if (isScientific) {
					if ((m_input[i] == '+' || m_input[i] == '-') && buffer.back() == 'e') {
						buffer += m_input[i];
						currType = CharType::Digit;
						continue;
					}
					else {
						throw std::runtime_error("Invalid character in exponent at position " + std::to_string(i));
					}
				}
				break;
			case CharType::Parenthesis:tokens.emplace_back(TokenType::Parenthesis, m_input[i]); break;
			case CharType::Digit:buffer += m_input[i]; break;
			case CharType::Alpha:buffer += m_input[i]; break;
			case CharType::Dot:
				if (isScientific)
					throw std::runtime_error("Malformed scientific notation at position " + std::to_string(i + 1));
				if (buffer.empty()) 
					buffer += "0";
				if (wasDot)
					throw std::runtime_error("Invalid character at position " + std::to_string(i));
				buffer += "."; 
				wasDot = true;
				currType = CharType::Digit;
				break;
			case CharType::Comma:tokens.emplace_back(TokenType::Comma, ','); break;
		}

		if (i + 1 < m_input.length()) {
			CharType nextType = getCharType(m_input[i + 1]);
			if ((currType == CharType::Digit) && 
				!(nextType == CharType::Digit || nextType == CharType::Dot) && 
				std::tolower(m_input[i+1])!='e') {

				if (isScientific && (buffer.back() == 'e' || buffer.back() == '+' || buffer.back() == '-')) {
					throw std::runtime_error("Incomplete scientific notation at position " + std::to_string(i));
				}

				tokens.emplace_back(toDouble(buffer));
				buffer.clear(); wasDot = false; isScientific = false;
			}
			else if (currType == CharType::Alpha) {
				if (buffer == "log" && i + 3 < m_input.size() && m_input.substr(i+1, 2) == "10" && m_input[i+3] == '(') {
					tokens.emplace_back(TokenType::Function, buffer+"10");
					buffer.clear(); wasDot = false; isScientific = false;
					i += 2;
					continue;
				}
				else if (buffer == "atan" && i + 2 < m_input.size() && m_input[i + 1] == '2' && m_input[i + 2] == '(') {
					tokens.emplace_back(TokenType::Function, buffer + "2");
					buffer.clear(); wasDot = false; isScientific = false;
					i++;
					continue;
				}
				else if (currType == CharType::Alpha && nextType != CharType::Alpha) {
					TokenType alphaType = m_input[i + 1] == '(' ? TokenType::Function : TokenType::Variable;
					tokens.emplace_back(alphaType, buffer);
					buffer.clear(); wasDot = false; isScientific = false;
				}
			}
		}
	}
	if (!buffer.empty()) {
		if (currType == CharType::Digit) {
			if (isScientific && (buffer.back() == 'e' || buffer.back() == '+' || buffer.back() == '-')) {
				throw std::runtime_error("Incomplete scientific notation at position " + std::to_string(m_input.length()));
			}
			tokens.emplace_back(toDouble(buffer));
		}
		else if (currType == CharType::Alpha) {
			tokens.emplace_back(TokenType::Variable, buffer);
		}
	}
	return tokens;
}