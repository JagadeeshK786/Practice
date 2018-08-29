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
//不要一次性拿全部子串，按个数拿子
std::vector<std::string> getAllSubstring(const std::string& str) {
	std::unordered_set<std::size_t> hash_value_set;
	std::hash<std::string> string_hash;
	std::vector<std::string> returnMe;
	for (std::size_t i = str.size(); i > 0; i--) {
		for (std::size_t i2 = 0; i2 <= str.size() - i; i2++) {
			auto tmp = str.substr(i2, i);
			auto tmp_hash = string_hash(tmp);
			if (hash_value_set.find(tmp_hash) == hash_value_set.end()) {
				returnMe.push_back(std::move(tmp));
				hash_value_set.insert(tmp_hash);
			}
		}
	}
	return returnMe;
}

int lengthOfLongestSubstring(const std::string& s) {
	std::unordered_set<std::size_t> hash_value_set;
	std::hash<std::string> string_hash;
	int returnMe = 0;
	for (std::size_t i = s.size(); i > 0; i--) {
		for (std::size_t i2 = 0; i2 <= s.size() - i; i2++) {
			auto tmp = s.substr(i2, i);
			auto tmp_hash = string_hash(tmp);
			if (hash_value_set.find(tmp_hash) == hash_value_set.end()) {
				const auto& p = tmp;				
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
				hash_value_set.insert(tmp_hash);
			}
		}
	}
	return returnMe;
}

int main() {
	auto look2 = lengthOfLongestSubstring("abcabcbb");
	auto look = lengthOfLongestSubstring(DC::File::read("1.txt"));
	std::cout << "";
}
