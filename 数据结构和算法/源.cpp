#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <unordered_set>
#include <cctype>
#include <cmath>

//https://leetcode-cn.com/problems/two-sum/description/
std::vector<int> twoSum(const std::vector<int>& nums, int target) {
	for (int i = 0; i < nums.size(); i++)
		for (int i2 = i + 1; i2 < nums.size(); i2++)
			if (nums[i] + nums[i2] == target)
				return std::vector<int>{i, i2};
}

//https://leetcode-cn.com/problems/longest-substring-without-repeating-characters/description/
int lengthOfLongestSubstring(const std::string& s) {
	std::unordered_set<char> set;
	unsigned int returnme = 0, i = 0, j = 0;
	while (i < s.size() && j < s.size()) {
		for (j = i; j < s.size(); j++) {
			if (set.find(s[j]) == set.end()) {
				if (j - i + 1 > returnme)
					returnme = j - i + 1;
				set.insert(s[j]);
			}
			else {
				break;
			}
		}
		set.clear();
		i++;
	}
	return returnme;
}

//https://leetcode-cn.com/problems/string-to-integer-atoi/description/
static const int32_t int_max = 2147483647;
static const int32_t int_min = -2147483648;

int32_t get_digit_count(int32_t num) {
	if (num == 0)
		return 1;
	if (num == int_min)
		return 10;
	return static_cast<int32_t>(log10(abs(num))) + 1;
}

std::array<char, 10> get_each_digit(int32_t num) {
	//支持除了int_min以外的所有输入值
	std::array<char, 10> result;
	result.fill(-1);
	num = abs(num);
	char* iter = result.data() + get_digit_count(num) - 1;
	while (num > 0) {
		*iter-- = num % 10;
		num /= 10;
	}
	return result;
}

int myAtoi(const std::string& str) {
	//获得有效数字范围
	unsigned int begin = 0, end = 0;
	bool is_positive = true, is_positive_flag_set = false, meet_number_before = false;
	auto integer_overflow_return_value = [&is_positive] {
		if (is_positive)
			return int_max;
		else
			return int_min;
	};
	auto get_single_number = [](char c) {
		return c - '0';
	};

	for (; begin < str.size(); begin++)
		if (!std::isspace(str[begin]))
			break;
	for (; begin < str.size(); begin++) {
		if (std::isdigit(str[begin]))
			meet_number_before = true;
		if (str[begin] != '-'&&str[begin] != '+' && !std::isdigit(str[begin]))
			return 0;
		if (str[begin] == '-') {
			if (is_positive_flag_set || meet_number_before)
				return 0;
			is_positive = false;
			is_positive_flag_set = true;
		}
		else {
			if (str[begin] == '+') {
				if (is_positive_flag_set || meet_number_before)
					return 0;
				is_positive_flag_set = true;
			}
		}
		if (std::isdigit(str[begin]) && str[begin] != '0')
			break;
	}
	if (str.empty() || std::isspace(str[begin])
		|| (!std::isdigit(str[begin]) && str[begin] != '-'&& str[begin] != '+'))
		return 0;
	for (end = begin + 1; end < str.size(); end++)
		if (!std::isdigit(str[end]))
			break;
	//判断是否超过32位有符号限制
	if (end - begin > 10)
		return integer_overflow_return_value();
	if (end - begin == 10) {
		std::array<char, 10> digits = get_each_digit(int_max);
		bool is_equals_except_last = true;
		auto digits_iter = digits.cbegin();
		for (auto index = begin; index != end - 1; index++, digits_iter++) {
			if (get_single_number(str[index]) < *digits_iter) {
				is_equals_except_last = false;
				break;
			}
			if (get_single_number(str[index]) > *digits_iter)
				return integer_overflow_return_value();
		}
		if (is_equals_except_last) {
			if (is_positive) {
				if (get_single_number(str[begin + 9]) > digits[9])
					return integer_overflow_return_value();
			}
			else {
				if (get_single_number(str[begin + 9]) > digits[9] + 1)
					return integer_overflow_return_value();
			}
		}
	}
	//计算结果
	int32_t result = 0;
	for (auto iter = begin; iter != end; iter++)
		result = result * 10 + get_single_number(str[iter]);
	if (!is_positive)
		result = ~result + 1;
	return result;
}

int main() {
	auto look = myAtoi("0-1");
	look = myAtoi("42");
	std::cout << "";
}
