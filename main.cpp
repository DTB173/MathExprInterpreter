#include "Lexer.h"
#include "Parser.h"
#include "Evaluator.h"
#include "AST.h"
#include <iostream>
#include <vector>

static std::string promptInput() {
	std::cout << ">> ";
	std::string input;
	std::getline(std::cin, input);
	return input;
}

static void printTokens(const std::vector<Token>& tokens) {
	for (const auto& t : tokens) {
		std::cout << t << '\n';
	}
}

void printWelcome() {
	std::cout << "====================================\n";
	std::cout << " Welcome to the Math Expression CLI \n";
	std::cout << "====================================\n\n";

	std::cout << "You can perform algebraic operations:\n";
	std::cout << "  Addition (+), Subtraction (-), Multiplication (*), Division (/)\n";
	std::cout << "  Integer Division (\\), Power (^), Modulus (%), Factorial (!)\n";
	std::cout << "  Unary plus (+), Unary minus (-), and variable assignment (x = 5)\n\n";

	std::cout << "Built-in functions supported:\n";
	std::cout << "  Trigonometry: sin, cos, tan, asin, acos, atan, atan2\n";
	std::cout << "  Exponential & logs: exp (e^x), sqrt (x), log (ln), log10\n";
	std::cout << "  Rounding & absolute: abs, floor, ceil, round\n";
	std::cout << "  Aggregates: min, max, factorial\n\n";

	std::cout << "Constants available: pi, e\n\n";
	std::cout << "Usage examples:\n";
	std::cout << "  x = 5\n";
	std::cout << "  y = 3\n";
	std::cout << "  f(x) = x^2 + 2\n";
	std::cout << "  f(3) -> 11\n";
	std::cout << "  max(1, 5, 2) -> 5\n\n";

	std::cout << "Type your expressions below. Press Ctrl+C or \"exit\" to exit.\n";
	std::cout << "------------------------------------\n";
}

int main(){
	printWelcome();
	std::string input{};
	std::vector<Token> tokens;
	Evaluator e;
	while (true) {
		input = promptInput();

		if (input == "exit") {
			std::cout << "See you again!\n";
			break;
		}
		if (input == "?" || input == "help") {
			printWelcome();
			continue;
		}
			
		if (input.empty())
			continue;
		Lexer lex{input};
		try {
			tokens = lex.tokenize();
			Parser parser{ tokens };
			auto astroot = parser.parseExpression();
			double value = e.evaluate(*astroot);
			std::cout << value<<'\n';
		}
		catch (std::exception& e) {
			std::cout << "Error: \"" << e.what() << "\"\n";
		}
	}
	return 0;
}
