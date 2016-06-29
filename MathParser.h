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

    MathParser() : operators({OperatorTable::value_type("*=",  std::make_tuple(1, 1, Assoc::right_to_left) ),
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
                             OperatorTable::value_type("--",  std::make_tuple(9, 0, Assoc::left_to_right) )}),
        ibase(10), obase(10), precision(20) {}

	void runInteractiveMode();

	bool parseAndEvaluate(const std::string & line, Fixed & result);

	void setIBase(int32_t ibase) {
		if(ibase >= 2 && ibase <= 16) {
			this->ibase = ibase;
		}
		else {
			std::cout << "Input base must be from range 2-16." << std::endl;
		}
	}

	void setOBase(int32_t obase) {
		if(obase >= 2 && obase <= 999) {
			this->obase = obase;
		}
		else {
			std::cout << "Output base must be from range 2-99." << std::endl;
		}
	}

	void setPrecision(int32_t precision) {
		if (precision >= 0) {
			this->precision = precision;
		}
		else {
			std::cout << "Precision must be at least 0." << std::endl;
		}
	}

private:
	OperatorTable operators;
	VariableTable variables;

	uint32_t ibase;
	uint32_t obase;
	uint32_t precision;

	void printHelp() const {
		std::cout << "operators={ +,-,*,/,++,--,==,!=,<,<=,>,>=,=,+=,-=,*=,/= }" << std::endl;
		std::cout << "type 'quit' for quitting application" << std::endl;
		std::cout << "type 'ibase=<2,16>' for changing input base" << std::endl;
		std::cout << "type 'obase=<2,999>' for changing output base" << std::endl;
		std::cout << "type 'precision=<0...>' for changing precision of numbers" << std::endl;
		std::cout << "you can declare variables and assign them value" << std::endl;
	}

	bool isOperator(const std::string & token) const {
		return operators.find(token) != operators.end();
	}

	bool isAssociative(const std::string & token, Assoc associativity) const {
		auto tuple = operators.find(token)->second;
		return std::get<2>(tuple) == associativity;
	}

	int comparePrecedence(const std::string & first, const std::string & second) const {
		auto tuple1 = operators.find(first)->second;
		auto tuple2 = operators.find(second)->second;
		return std::get<0>(tuple1) - std::get<0>(tuple2);
	}

	bool isParenthesis(const std::string & token) const {
		return isLeftParenthesis(token) || isRightParenthesis(token);
	}

	bool isLeftParenthesis(const std::string & token) const {
		return token == "(";
	}

	bool isRightParenthesis(const std::string & token) const {
		return token == ")";
	}

	bool isAssignment(const std::string & token) const {
		return token == "=" || token == "+=" || token == "-=" || token == "*=" || token == "/=" || token == "%=";
	}

	bool isRelationalOperator(const std::string & token) const {
		return token == "==" || token == "!=" || token == ">" || token == ">=" || token == "<" || token == "<=";
	}

	bool isUnary(const std::string & token) const {
		return isIncrement(token) || isDecrement(token);
	}

	bool isIncrement(const std::string & token) const {
		return token == "++a" || token == "a++";
	}

	bool isDecrement(const std::string & token) const {
		return token == "--a" || token == "a--";
	}

	bool isAlphaNum(const std::string &token) const {
		return std::find_if(token.begin(), token.end(), [](char c){
			return !isalnum(c);
		}) == token.end();
	}

	bool isName(const std::string & token) const {
		return isalpha(token[0]);
	}

	bool isVariable(const std::string & token) const {
		return variables.find(token) != variables.end();
	}

	Fixed getValue(const std::string & token) const {
		return variables.find(token)->second;
	}

	std::string trim(std::string & str) const
	{
		size_t first = str.find_first_not_of(' ');
		size_t last = str.find_last_not_of(' ');
		if (first == std::string::npos || last == std::string::npos) {
			return "";
		}
		return str.substr(first, (last-first+1));
	}

	std::vector< std::string > tokenize(const std::string & expression);

	bool convertToRPN(const std::vector< std::string > & tokens, std::vector < std::string > & output);

	bool evaluateRPN(const std::vector < std::string > & rpn, Fixed & output);

	bool compute(const std::string & oper, std::deque< std::string > & var, std::vector< Fixed > & values, Fixed & result);
};

}


#endif //FIXEDPOINTLIB_MATHPARSER_H
