#include <iostream>
#include <cctype>
#include <stack>
#include <map>
#include <vector>
#include <functional>
#include <cmath>
#include <string>

constexpr unsigned char VALUE_NODE = 0;
constexpr unsigned char OPERATOR_NODE = 1;

struct node
{
	unsigned char type;
	int16_t value;
};

struct operator_info
{
	uint8_t priority;
	std::function<int(int, int)> functor;
};

const std::map<int16_t, operator_info> operators{
	{'+', {0, [](int a, int b) { return a + b; }}},
	{'-', {1, [](int a, int b) { return a - b; }}},
	{'*', {2, [](int a, int b) { return a * b; }}},
	{'/', {2, [](int a, int b) { return a / b; }}},
	{'^', {3, [](int a, int b) { return static_cast<int>(std::pow(a, b)); }}}};

bool isoperator(int16_t ch)
{
	return operators.find(ch) != operators.cend();
}

std::vector<node> shunting_yard(const std::string &infix) //noexcept
{
	std::vector<node> reverse_polish;
	reverse_polish.reserve(infix.size());

	std::stack<int16_t> operator_stack;
	auto pop_stack = [&operator_stack, &reverse_polish](int16_t stop_if = 0) {
		while (!operator_stack.empty())
		{
			auto top = operator_stack.top();
			operator_stack.pop();
			if (top == stop_if)
				break;
			reverse_polish.push_back(node{OPERATOR_NODE, top});
		}
	};

	std::string number;
	auto push_number = [&reverse_polish, &number] {
		if (!number.empty())
		{
			reverse_polish.push_back(node{VALUE_NODE, static_cast<int16_t>(std::stoi(number))});
			number.clear();
		}
	};

	for (char ch : infix)
	{
		if (std::isdigit(ch))
		{
			number.push_back(ch);
			continue;
		}
		else
		{
			push_number();
			if (isoperator(ch))
			{
				if (!operator_stack.empty())
				{
					auto top = operator_stack.top();
					if (isoperator(top) && operators.at(ch).priority < operators.at(operator_stack.top()).priority)
					{
						pop_stack();
					}
				}
				operator_stack.push(ch);
				continue;
			}
			if (ch == '(')
			{
				operator_stack.push(ch);
				continue;
			}
			if (ch == ')')
			{
				pop_stack('(');
			}
		}
	}
	push_number();
	pop_stack();	

	return reverse_polish;
}

int calculate(const std::vector<node> &reverse_polish)
{
	std::stack<int> result_stack;
	for (node ch : reverse_polish)
	{
		if (ch.type == VALUE_NODE)
		{
			result_stack.push(ch.value);
			continue;
		}
		if (ch.type == OPERATOR_NODE)
		{
			auto second = result_stack.top();
			result_stack.pop();
			auto first = result_stack.top();
			result_stack.pop();
			result_stack.push(operators.at(ch.value).functor(first, second));
		}
	}
	return result_stack.top();
}

int main()
{
	//auto look = shunting_yard("(5+2)*3+2");
	//auto look = shunting_yard("2+2^2");
	auto look = shunting_yard("3+2-12");
	auto look2 = calculate(look);
	std::cout << "";
}
