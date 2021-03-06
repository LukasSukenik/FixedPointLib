//
// Created by peter on 8.5.2016.
//

#include <iostream>
#include "MathParser.h"

using namespace Parser;

void MathParser::runInteractiveMode() {
	std::string line;

	while(std::getline(std::cin, line)) {
		try {
			line = trim(line);

			if (line.empty()) {
				continue;
			}

			if (line.find("quit") == line.find_first_not_of(" ")) {
				break;
			}
			else if (line.find("ibase") == line.find_first_not_of(" ")) {
				auto tmp = line.find_first_of('=');
				setIBase(std::stoi(line.substr(tmp + 1)));
			}
			else if (line.find("obase") == line.find_first_not_of(" ")) {
				auto tmp = line.find_first_of('=');
				setOBase(std::stoi(line.substr(tmp + 1)));
			}
			else if (line.find("precision") == line.find_first_not_of(" ")) {
				auto tmp = line.find_first_of('=');
				setPrecision(std::stoi(line.substr(tmp + 1)));
			}
			else if (line.find("help") == line.find_first_not_of(" ")) {
				printHelp();
			}
			else {
				Fixed result;
				if (parseAndEvaluate(line, result)) {
					result.setOBase(this->obase);
					result.setPrecision(this->precision);
					std::cout /*<< ">> "*/ << result << std::endl;
				}
			}
		}
		catch (const std::invalid_argument & e) {
			std::cout << "Invalid argument" << std::endl;
		}
		catch (const std::out_of_range & e) {
			std::cout << "Out of range" << std::endl;
		}
	}
}

bool MathParser::parseAndEvaluate(const std::string & line, Fixed & result) {
	//tokenize input
	auto tokens = tokenize(line);

	//convert infix to rpn
	std::vector<std::string> rpn;
	if (!convertToRPN(tokens, rpn)) {
		std::cout << "Mismatched braces" << std::endl;
		return false;
	}

	//evaluate rpn
	return evaluateRPN(rpn, result);
}

std::vector< std::string > MathParser::tokenize(const std::string & expression) {
	std::vector< std::string > tokens;
	std::string buffer;

	for (uint64_t i = 0; i < expression.length(); ++i) {
		std::string token(1, expression[i]);

		if (i < expression.length()-1 && isOperator(token + std::string(1,expression[i+1]))){
			std::string unary(token + std::string(1,expression[i+1]));
			if (!buffer.empty()) {
				tokens.push_back(buffer);
			}
			buffer = "";
			if (unary == "++" || unary == "--") {
				if (!tokens.empty() && (isAlphaNum(tokens.back()) || isRightParenthesis(tokens.back()))) {
					tokens.push_back("a" + unary);
				}
				else {
					tokens.push_back(unary + "a");
				}
			}
			else {
				tokens.push_back(unary);
			}

			++i;
		}
		else if (isOperator(token) || isParenthesis(token)) {
			if (!buffer.empty()) {
				tokens.push_back(buffer);
			}
			buffer = "";
			if (token == "-" && (tokens.empty() || isLeftParenthesis(tokens.back()))) {
				tokens.push_back("-a");
			}
			else {
				tokens.push_back(token);
			}
		}
		else {
			if (token != " " && !token.empty()) {
				buffer += token;
			}
			else if (!buffer.empty()) {
				tokens.push_back(buffer);
				buffer = "";
			}
		}
	}
	if (!buffer.empty()) {
		tokens.push_back(buffer);
	}
	return tokens;
}

bool MathParser::convertToRPN(const std::vector< std::string > & tokens, std::vector < std::string > & output) {
	std::stack< std::string > stack;

	for (auto & token : tokens) {
		if (isOperator(token)) {
			while (!stack.empty() && isOperator(stack.top()) &&
			       ((isAssociative(token, Assoc::left_to_right) && comparePrecedence(token, stack.top()) <= 0)
			        || (isAssociative(token, Assoc::right_to_left) && comparePrecedence(token, stack.top()) < 0))) {
				output.push_back(stack.top());
				stack.pop();
			}
			stack.push(token);
		}
		else if (isLeftParenthesis(token)) {
			stack.push(token);
		}
		else if (isRightParenthesis(token)) {
			auto top = stack.top();
			while (!isLeftParenthesis(top)) {
				output.push_back(top);
				stack.pop();

				if (stack.empty()) {
					break;
				}
				top = stack.top();
			}
			if (isLeftParenthesis(top)) {
				stack.pop();
			}
			else {
				return false;
			}
		}
		else {
			output.push_back(token);
		}
	}

	while (!stack.empty()) {
		auto top = stack.top();
		if (isParenthesis(top)) {
			return false;
		}
		output.push_back(top);
		stack.pop();
	}

	return true;
}

bool MathParser::evaluateRPN(const std::vector < std::string > & rpn, Fixed & output) {
	std::stack< Fixed > stack;
	std::deque< std::string > variable;

	for (uint64_t i = 0; i < rpn.size(); ++i) {
		std::string token = rpn[i];

		if (isName(token) && !isOperator(token)){
			if (i == variable.size() && isAssignment(rpn[rpn.size()-1-variable.size()])) {
				variable.push_back(token);
			}
			else if (isVariable(token)) {
				stack.push(getValue(token));
			}
			else {
				std::cout << "Variable '" << token << "' does not exist" << std::endl;
				return false;
			}
		}
		else if (!isOperator(token)) {
			stack.push(Fixed(token, this->ibase));
		}
		else {
			auto info = this->operators.find(token)->second;
			std::vector< Fixed > values;

			//get required number of values from stack
			for (int j = 0; j < std::get<1>(info); ++j) {
				if (stack.empty()) {
					std::cout << "Syntax error" << std::endl;
					return false;
				}
				values.push_back(stack.top());
				stack.pop();
			}

			Fixed result;
			if (!compute(token, variable, values, result)) {
				return false;
			}

			//unary operator was applied to variable
			if (isUnary(token) && isVariable(rpn[i-1])) {
				if (isIncrement(token)) {
					variables[rpn[i-1]] = ++getValue(rpn[i-1]);
				}
				else if (isDecrement(token)) {
					variables[rpn[i-1]] = --getValue(rpn[i-1]);
				}
			}
			stack.push(result);
		}
	}

	if (variable.empty()) {
		output = stack.top();
		stack.pop();
	}
	else {
		output = stack.top();
		while (!variable.empty()) {
			variables[variable.back()] = stack.top();
			variable.pop_back();
		}
		stack.pop();
	}

	if (!stack.empty()) {
		std::cout << "Syntax error" << std::endl;

		return false;
	}
	return true;
}

bool MathParser::compute(const std::string & oper, std::deque< std::string > & var, std::vector< Fixed > & values, Fixed & result) {
	if (oper == "+") {
		result = values[1] + values[0];
	}
	else if (oper == "-") {
		result = values[1] - values[0];
	}
	else if (oper == "*") {
		result = values[1] * values[0];
	}
	else if (oper == "/") {
		result = values[1] / values[0];
	}
	else if (oper == "-a") {
		result = values[0] * Fixed("-1");
	}
	else if (oper == "--a") {
		result = --values[0];
	}
	else if (oper == "++a") {
		result = ++values[0];
	}
	else if (oper == "a++") {
		result = values[0]++;
	}
	else if (oper == "a--") {
		result = values[0]--;
	}
	else if (oper == "=") {
		if (!var.empty()) {
			result = values[0];
			variables[var.back()] = result;
			var.pop_back();
		}
	}
	else if (isAssignment(oper)) {
		if (!var.empty()) {
			if (!isVariable(var.back())) {
				std::cout << "Variable '" << var.back() << "' does not exist: " << oper << std::endl;
				return false;
			}
			result = getValue(var.back());
			if (oper == "+=") {
				result += values[0];
			}
			else if (oper == "-=") {
				result -= values[0];
			}
			else if (oper == "*=") {
				result *= values[0];
			}
			else if (oper == "/=") {
				result /= values[0];
			}
			variables[var.back()] = result;
			var.pop_back();
		}
	}
	else if (isRelationalOperator(oper)) {
		bool cmp;
		if (oper == "==") {
			cmp = values[1] == values[0];
		}
		else if (oper == "!=") {
			cmp = values[1] != values[0];
		}
		else if (oper == "<") {
			cmp = values[1] < values[0];
		}
		else if (oper == "<=") {
			cmp = values[1] <= values[0];
		}
		else if (oper == ">") {
			cmp = values[1] > values[0];
		}
		else {
			cmp = values[1] >= values[0];
		}
		result = Fixed(std::to_string(cmp));
	}
	else {
		std::cout << "Unknown operator '" + oper + "'";
		return false;
	}

	return true;
}

int main()
{
	MathParser parser;
	parser.runInteractiveMode();

	return 0;
}
