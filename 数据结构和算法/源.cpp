#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <DC_File.h>

//https://leetcode-cn.com/problems/two-sum/description/
std::vector<int> twoSum(const std::vector<int>& nums, int target) {
	for (int i = 0; i < nums.size(); i++)
		for (int i2 = i + 1; i2 < nums.size(); i2++)
			if (nums[i] + nums[i2] == target)
				return std::vector<int>{i, i2};
}

//https://leetcode-cn.com/problems/longest-substring-without-repeating-characters/description/
std::vector<std::string> getAllSubstring(const std::string& str) {
	std::vector<std::string> returnMe;
	returnMe.reserve([&str]()->unsigned int {
		if (str.size() % 2 == 0)
			return (1 + str.size()) * str.size() / 2;
		else
			return (1 + str.size()) * (str.size() - 1) / 2 + (str.size() - 1) / 2 + 1;
	}());
	for (std::size_t i = str.size(); i > 0; i--)
		for (std::size_t i2 = 0; i2 <= str.size() - i; i2++)
			returnMe.push_back(str.substr(i2, i));
	return returnMe;
}

/*
int lengthOfLongestSubstring(const std::string& s) {
int returnMe = 0;
std::unordered_set<char> set;
unsigned int testMe = s.size() / 2;
while (true) {
auto subs = getSubstringWithSize(s, testMe);
bool isOK = true;
for (const auto& p : subs) {
if (p.size() <= returnMe)
continue;
for (const auto& c : p) {
if (set.find(c) != set.end()) {
isOK = false;
break;
}
else {
set.insert(c);
}
}
if (isOK) {
returnMe = p.size();
testMe += testMe / 2;
}
else {
if ((testMe > returnMe&&returnMe != 0) || testMe == 1)
return returnMe;
testMe -= testMe / 2;
}
set.clear();
}
}
}*/

int lengthOfLongestSubstring(const std::string& s) {
	auto allSubstring = getAllSubstring(s);	
	int returnMe = 0;
	for (const auto& p : allSubstring) {
		std::unordered_set<char> set;
		if (p.size() <= returnMe)
			continue;
		bool isOK = true;
		for (const auto& c : p) {
			if (set.find(c) != set.end()) {
				isOK = false;
				break;
			}
			else {
				set.insert(c);
			}
		}
		if (isOK)
			returnMe = p.size();
	}
	return returnMe;
}

int main() {
	auto look2 = lengthOfLongestSubstring("abcabcbb");
	auto look = lengthOfLongestSubstring(DC::File::read("1.txt"));
	std::cout << "";
}
