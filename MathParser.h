//
// Created by peter on 8.5.2016.
//

#ifndef FIXEDPOINTLIB_MATHPARSER_H
#define FIXEDPOINTLIB_MATHPARSER_H

#include <unordered_map>
#include <array>
#include <vector>
#include <stack>
#include <algorithm>
#include <map>
#include "fixed.h"

namespace Parser {

class MathParser {
public:
	enum class Assoc {
		left_to_right,
		right_to_left
	};

	//for operator his precedence(0), arity/number of required values(1) and associativity(2)
	using OperatorTable = std::unordered_map< std::string, std::tuple< int, int, Assoc > >;

	using VariableTable = std::map< std::string, Fixed >;

	MathParser() : ibase(10), obase(10), precision(20) {
		const std::array<OperatorTable::value_type,22> oper
		{
			OperatorTable::value_type("*=",  std::make_tuple(1, 1, Assoc::right_to_left) ),
			OperatorTable::value_type("/=",  std::make_tuple(1, 1, Assoc::right_to_left) ),
			OperatorTable::value_type("+=",  std::make_tuple(1, 1, Assoc::right_to_left) ),
			OperatorTable::value_type("-=",  std::make_tuple(1, 1, Assoc::right_to_left) ),
			OperatorTable::value_type("=",   std::make_tuple(1, 1, Assoc::right_to_left) ),
			OperatorTable::value_type("==",  std::make_tuple(2, 2, Assoc::left_to_right) ),
			OperatorTable::value_type("!=",  std::make_tuple(2, 2, Assoc::left_to_right) ),
			OperatorTable::value_type(">",   std::make_tuple(3, 2, Assoc::left_to_right) ),
			OperatorTable::value_type(">=",  std::make_tuple(3, 2, Assoc::left_to_right) ),
			OperatorTable::value_type("<",   std::make_tuple(3, 2, Assoc::left_to_right) ),
			OperatorTable::value_type("<=",  std::make_tuple(3, 2, Assoc::left_to_right) ),
			OperatorTable::value_type("+",   std::make_tuple(4, 2, Assoc::left_to_right) ),
			OperatorTable::value_type("-",   std::make_tuple(4, 2, Assoc::left_to_right) ),
			OperatorTable::value_type("*",   std::make_tuple(5, 2, Assoc::left_to_right) ),
			OperatorTable::value_type("/",   std::make_tuple(5, 2, Assoc::left_to_right) ),
			OperatorTable::value_type("++a", std::make_tuple(6, 1, Assoc::right_to_left) ),
			OperatorTable::value_type("--a", std::make_tuple(6, 1, Assoc::right_to_left) ),
			OperatorTable::value_type("a++", std::make_tuple(7, 1, Assoc::left_to_right) ),
			OperatorTable::value_type("a--", std::make_tuple(7, 1, Assoc::left_to_right) ),
			OperatorTable::value_type("-a",  std::make_tuple(8, 1, Assoc::right_to_left) ),
			OperatorTable::value_type("++",  std::make_tuple(9, 0, Assoc::left_to_right) ),
			OperatorTable::value_type("--",  std::make_tuple(9, 0, Assoc::left_to_right) )
		};

		operators.reserve(oper.size());
		operators.insert(std::begin(oper), std::end(oper));
	}

	void runInteractiveMode() {
		std::string line;

		while(true) {
			try {
				std::cout << ">> ";
				std::getline(cin, line);

				if (line.empty()) {
					continue;
				}

				if (line.find("quit") == line.find_first_not_of(" ")) {
					break;
				}
				else if (line.find("ibase") == line.find_first_not_of(" ")) {
					auto tmp = line.find_first_of('=');
					setIBase(std::stoul(line.substr(tmp + 1)));
				}
				else if (line.find("obase") == line.find_first_not_of(" ")) {
					auto tmp = line.find_first_of('=');
					setOBase(std::stoul(line.substr(tmp + 1)));
				}
				else if (line.find("precision") == line.find_first_not_of(" ")) {
					auto tmp = line.find_first_of('=');
					setPrecision(std::stoul(line.substr(tmp + 1)));
				}
				else if (line.find("help") == line.find_first_not_of(" ")) {
					printHelp();
				}
				else {
					Fixed result;
					if (parseAndEvaluate(line, result)) {
						result.setOBase(this->obase);
						result.setPrecision(this->precision);
						std::cout << ">> " << result << std::endl;
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

	bool parseAndEvaluate(const std::string & line, Fixed & result) {
		//tokenize input
		auto tokens = tokenize(line);
		/*std::cout << "Infix: ";
		for (auto &token : tokens) {
			std::cout << '"' << token << "\" ";
		}
		std::cout << std::endl;*/

		//convert infix to rpn
		std::vector<string> rpn;
		if (!convertToRPN(tokens, rpn)) {
			std::cout << "Mismatched braces" << std::endl;
			return false;
		}
		/*std::cout << "Rpn: ";
		for (auto &token : rpn) {
			std::cout << '"' << token << "\" ";
		}
		std::cout << std::endl;*/

		//evaluate rpn
		return evaluateRPN(rpn, result);
	}

	void setIBase(uint32_t ibase) {
		if(ibase >= 2 && ibase <= 16) {
			this->ibase = ibase;
		}
	}

	void setOBase(uint32_t obase) {
		if(obase >= 2 && obase <= 999) {
			this->obase = obase;
		}
	}

	void setPrecision(uint32_t precision) {
		this->precision = precision;
	}

private:
	OperatorTable operators;
	VariableTable variables;

	uint32_t ibase;
	uint32_t obase;
	uint32_t precision;

	void printHelp() {
		std::cout << "operators={ +,-,*,/,++,--,==,!=,<,<=,>,>=,=,+=,-=,*=,/= }" << std::endl;
		std::cout << "type 'quit' for quitting application" << std::endl;
		std::cout << "type 'ibase=<2,16>' for changing input base" << std::endl;
		std::cout << "type 'obase=<2,99>' for changing output base" << std::endl;
		std::cout << "type 'precision=<0...>' for changing precision of numbers" << std::endl;
		std::cout << "you can declare variables and assign them value" << std::endl;
	}

	bool isOperator(const std::string & token) {
		return operators.find(token) != operators.end();
	}

	bool isAssociative(const std::string & token, Assoc associativity) {
		auto tuple = operators.find(token)->second;
		return std::get<2>(tuple) == associativity;
	}

	int comparePrecedence(const std::string & first, const std::string & second) {
		auto tuple1 = operators.find(first)->second;
		auto tuple2 = operators.find(second)->second;
		return std::get<0>(tuple1) - std::get<0>(tuple2);
	}

	bool isParenthesis(const std::string & token) {
		return isLeftParenthesis(token) || isRightParenthesis(token);
	}

	bool isLeftParenthesis(const std::string & token) {
		return token == "(";
	}

	bool isRightParenthesis(const std::string & token) {
		return token == ")";
	}

	bool isAssignment(const std::string & token) {
		return token == "=" || token == "+=" || token == "-=" || token == "*=" || token == "/=" || token == "%=";
	}

	bool isRelationalOperator(const std::string & token) {
		return token == "==" || token == "!=" || token == ">" || token == ">=" || token == "<" || token == "<=";
	}

	bool isUnary(const std::string & token) {
		return isIncrement(token) || isDecrement(token);
	}

	bool isIncrement(const std::string & token) {
		return token == "++a" || token == "a++";
	}

	bool isDecrement(const std::string & token) {
		return token == "--a" || token == "a--";
	}

	bool isAlphaNum(const std::string &token) {
		return std::find_if(token.begin(), token.end(), [](char c){
			return !isalnum(c);
		}) == token.end();
	}

	bool isName(const std::string & token) {
		return isalpha(token[0]);
	}

	bool isVariable(const std::string & token) {
		return variables.find(token) != variables.end();
	}

	Fixed getValue(const std::string & token) {
		return variables.find(token)->second;
	}

	std::vector< std::string > tokenize(const std::string & expression) {
		std::vector< std::string > tokens;
		std::string buffer;

		for (uint64_t i = 0; i < expression.length(); ++i) {
			std::string token(1, expression[i]);
			//std::cout << i << ":" << expression[i] << std::endl;

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

	bool convertToRPN(const std::vector< std::string > & tokens, std::vector < std::string > & output) {
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

	bool evaluateRPN(const std::vector < std::string > & rpn, Fixed & output) {
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

			//print remaining stack
			/*while (!stack.empty()) {
				std::cout << stack.top() << " ";
				stack.pop();
			}*/
			return false;
		}
		return true;
	}

	bool compute(const std::string & oper, std::deque< std::string > & var, std::vector< Fixed > & values, Fixed & result) {
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
};

}


#endif //FIXEDPOINTLIB_MATHPARSER_H
