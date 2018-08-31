#include <iostream>
#include <vector>
#include <string>
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
	if (num == int_min)
		return 10;
	return static_cast<int32_t>(log10(abs(num))) + 1;
}

int32_t get_each_digit(int32_t x) {
	if (x >= 10)
		return get_each_digit(x / 10);
	return x % 10;
}

int myAtoi(const std::string& str) {
	//获得有效数字范围
	unsigned int begin = 0, end = 0;
	bool is_positive = true;
	for (; begin < str.size(); begin++)
		if (!std::isspace(str[begin]))
			break;
	if (str.empty() || std::isspace(str[begin])
		|| (!std::isdigit(str[begin]) && str[begin] != '-'))
		return 0;
	for (end = begin + 1; end < str.size(); end++)
		if (!std::isdigit(str[end]))
			break;
	if (end - begin == 1)
		if (str[begin] == '-')
			return 0;
	if (str[begin] == '-'&&end - begin > 1) {
		is_positive = false;
		begin++;
	}
	//判断是否超过32位有符号限制
	if (end - begin > 10) {
		if (is_positive)
			return int_max;
		else
			return int_min;
	}
	if (end - begin == 10) {
		std::cout << "";
	}
	int32_t result = 0;
	for (; begin != end; begin++)
		result = result * 10 + str[begin] - '0';
	if (!is_positive)
		result = ~result + 1;
	return result;
}

int main() {
	auto look = myAtoi("-9710");
	std::cout << "";
}
