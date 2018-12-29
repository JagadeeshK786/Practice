/*
随手写的一个表达式计算器，因为懒只支持个位数，反正就是写着玩的
*/
#include<iostream>
#include <cctype>
#include <stack>
#include <map>
#include <functional>
#include <cmath>

struct operator_info {
	unsigned char priority;
	std::function<int(int, int)> functor;
};

const std::map<unsigned char, operator_info> operators{
	{ '+',{ 0,[](int a,int b) {return a + b; } } },
{ '-',{ 0,[](int a,int b) {return a - b; } } },
{ '*',{ 1,[](int a,int b) {return a * b; } } },
{ '/',{ 1,[](int a,int b) {return a / b; } } },
{ '^',{ 2,[](int a,int b)->int {return std::pow(a,b); } } }
};

bool isoperator(unsigned char ch) {
	return operators.find(ch) != operators.cend();
}

std::string shunting_yard(const std::string& infix) {
	std::string reverse_polish;
	std::stack<unsigned char> operator_stack;
	auto pop_stack = [&operator_stack, &reverse_polish](unsigned char stop_if = 0) {
		while (!operator_stack.empty()) {
			auto top = operator_stack.top();
			operator_stack.pop();
			if (top == stop_if)
				break;
			reverse_polish.push_back(top);
		}
	};

	reverse_polish.reserve(infix.size());

	for (unsigned char ch : infix) {
		if (std::isdigit(ch)) {
			reverse_polish.push_back(ch);
			continue;
		}
		if (isoperator(ch)) {
			if (!operator_stack.empty()) {
				auto top = operator_stack.top();
				if (isoperator(top)
					&& operators.at(ch).priority < operators.at(operator_stack.top()).priority) {
					pop_stack();
				}
			}
			operator_stack.push(ch);
			continue;
		}
		if (ch == '(') {
			operator_stack.push(ch);
			continue;
		}
		if (ch == ')') {
			pop_stack('(');
		}
	}

	pop_stack();

	return reverse_polish;
}

int calculate(const std::string& reverse_polish) {
	std::stack<int> result_stack;
	for (unsigned char ch : reverse_polish) {
		if (std::isdigit(ch)) {
			result_stack.push(ch - '0');
			continue;
		}
		if (isoperator(ch)) {
			auto second = result_stack.top();
			result_stack.pop();
			auto first = result_stack.top();
			result_stack.pop();
			result_stack.push(operators.at(ch).functor(first, second));
		}
	}
	return result_stack.top();
}

int main()
{
	//auto look = shunting_yard("5+2*3+2");
	auto look = shunting_yard("5+((1+2)*4)-3+2^2");
	//auto look = shunting_yard("5+((1+2)*4)-3+2");
	auto look2 = calculate(look);
	std::cout << "";
}
