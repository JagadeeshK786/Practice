#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <cctype>
#include <cmath>
#include <cstring>

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
static const int32_t int_min = INT_MIN;

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

class LRUCache {
public:
	LRUCache(int capacity) :cap{ capacity } {
	}

	int get(int key) {
		auto value_iter = m_map.find(key);
		if (value_iter == m_map.end())
			return -1;
		m_key.erase(std::find(m_key.begin(), m_key.end(), key));
		m_key.push_front(key);
		return value_iter->second;
	}

	void put(int key, int value) {
		if (size == cap) {
			m_map.erase(*m_key.rbegin());
			m_key.erase(--m_key.rbegin().base());
		}
		m_map.insert_or_assign(key, value);
		m_key.push_front(value);
		if (size != cap)
			size++;
	}

private:
	const int cap;
	int size = 0;
	std::unordered_map<int, int> m_map;
	std::list<int> m_key;
};

//868. 二进制间距
int binaryGap(int N) {
	static unsigned int pos[32];
	unsigned int index = 0;
	for (unsigned int i = 0; i < 32; i++)
		if ((N & 1 << i) == 1 << i)
			pos[index++] = i;
	if (index < 2)
		return 0;
	unsigned int  result = 0;
	for (unsigned int i = 1; i < index; i++) {
		auto tmp = pos[i] - pos[i - 1];
		if (tmp > result)
			result = tmp;
	}
	return static_cast<int>(result);
}

//869. 重新排序得到 2 的幂

char* internal_itoa(int num, char* str, int base) {
	int i = 0;
	bool isNegative = false;
	if (num == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}
	if (num < 0 && base == 10) {
		isNegative = true;
		num = -num;
	}
	while (num != 0) {
		int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}
	if (isNegative)
		str[i++] = '-';
	str[i] = '\0';
	std::reverse(str, str + i);
	return str;
}

int internal_atoi(char *str) {
	int res = 0;
	for (int i = 0; str[i] != '\0'; ++i)
		res = res * 10 + str[i] - '0';
	return res;
}

struct carray_with_size {
	carray_with_size() {}

	carray_with_size(const carray_with_size& rhs) :data(new char[rhs.size + 1]), size(rhs.size) {
		memcpy(this->data, rhs.data, rhs.size);
		data[size] = 0;
	}

	carray_with_size(carray_with_size&& rhs) :data(rhs.data), size(rhs.size) {
		rhs.data = nullptr;
		rhs.size = 0;
	}

	char operator[](unsigned int index)const {
		return data[index];
	}

	~carray_with_size() {
		if (data != nullptr)
			delete[] data;
	}

	char* begin() {
		return data;
	}

	char* end() {
		return data + size;
	}

	char *data = nullptr;
	unsigned int size = 0;
};

//https://www.geeksforgeeks.org/number-distinct-permutation-string-can/
int factorial(int n) {
	int fact = 1;
	for (int i = 2; i <= n; i++)
		fact = fact * i;
	return fact;
}

int count_distinct_permutations(const carray_with_size& str) {
	int freq[26];
	memset(freq, 0, sizeof(freq));
	for (int i = 0; i < str.size; i++)
		if (str[i] >= 'a')
			freq[str[i] - 'a']++;
	int fact = 1;
	for (int i = 0; i < 26; i++)
		fact = fact * factorial(freq[i]);
	return factorial(str.size) / fact;
}

//https://www.cnblogs.com/bakari/archive/2012/08/02/2620826.html
template <typename iterator_t>
iterator_t find_max_for_one(iterator_t p, iterator_t q) {
	while (*q <= *p)
		q--;
	return q;
}

carray_with_size make_carray_with_size(const char* str, unsigned int length) {
	carray_with_size returnme;
	returnme.data = new char[length + 1];
	returnme.size = length;
	memcpy(returnme.data, str, length);
	returnme.data[length] = 0;
	return returnme;
}

std::vector<carray_with_size> returnme;

void prem(const char* inputstr, unsigned int length) {
	returnme.clear();
	auto str = make_carray_with_size(inputstr, length);	
	returnme.reserve(count_distinct_permutations(str) + 1);
	std::sort(str.begin(), str.end());
	returnme.push_back(str);
	//p代表替换点					 
	auto p = str.end() - 1;
	while (p != str.begin()) {
		p--;
		if (*p < *(p + 1)) {
			//找与替换点交换的点
			decltype(p) max = find_max_for_one(p, str.end() - 1);
			//交换
			std::swap(*p, *max);
			//将替换点后所有数进行反转
			std::reverse(p + 1, str.end());
			returnme.push_back(str);
			//将替换点置最后一个点，开始下一轮循环
			p = str.end() - 1;
		}
		if (str.begin() == p)
			break;
	}
}

bool ispow2(int number) {
	return (number & number - 1) == 0;
}

template<typename vector_t>
void vector_fast_erase(vector_t& vec, typename vector_t::iterator iterator) {
	if (vec.end() - 1 != iterator)
		std::swap(*iterator, *(vec.end() - 1));
	vec.erase(vec.end() - 1);
}

void just_peek(const std::vector<carray_with_size>& source);

bool reorderedPowerOf2(int n) {
	if (n < 10)
		return ispow2(n);
	static char n_carray_representation[10];
	memset(n_carray_representation, 0, 10);
	internal_itoa(n, n_carray_representation, 10);
	prem(n_carray_representation, strlen(n_carray_representation));
	auto& permutation_sequence = returnme;
	//just_peek(permutation_sequence);
	for (auto& p : permutation_sequence) {
		if ((*(p.end() - 1) != '0')
			&& (*(p.end() - 1) - '0') % 2 != 0)
			continue;
		//auto look = atoi(p.data);
		if (ispow2(internal_atoi(p.data)))
			return true;
	}
	return false;
}

void just_peek(const std::vector<carray_with_size>& source) {
	std::vector<std::string> look_me;
	look_me.reserve(source.size() + 1);
	for (const auto& p : source)
		look_me.emplace_back(p.data, p.size);
	std::cout << "";
}

int main() {
	auto look = reorderedPowerOf2(60985376);
	look = reorderedPowerOf2(6094);
	std::cout << "";
}
