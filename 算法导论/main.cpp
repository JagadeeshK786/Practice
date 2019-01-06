#include <iostream>
#include <vector>
#include <algorithm> //std::copy
#include <cstddef>

#ifndef _DEBUG
template <typename IterType, typename PredType>
void insertion_sort(IterType begin, IterType end, const PredType &pred)
{
	for (auto i = begin + 1; i != end; i++)
	{
		auto key = *i;
		auto j = i - 1;
		while (j >= begin && (pred(key, *j) > 0))
		{
			*(j + 1) = *j;
			j--;
		}
		*(j + 1) = key;
	}
}
#else
template <typename IterType, typename PredType>
void insertion_sort(IterType begin, IterType end, const PredType &pred)
{
	for (auto i = begin + 1; i != end; i++)
	{
		auto key = *i;
		auto j = i - 1;
		while (j >= begin && (pred(key, *j) > 0))
		{
			*(j + 1) = *j;
			if (j == begin)
				break;
			j--;
		}
		if (j != begin)
			*(j + 1) = key;
		else
			*j = key;
	}
}
#endif

#include <new>
#include <memory>

template <typename IterType, typename PredType>
void merge_impl(IterType begin, IterType middle, IterType end, const PredType &pred)
{
	//当IterType::value_type不为trivially copyable类型时，下面的new带来了多余的初始化操作
	std::unique_ptr<IterType::value_type[]> temp_array(new IterType::value_type[std::distance(begin, end)]);

	std::copy(begin, end, temp_array.get());

	std::size_t left_begin = 0, left_end = std::distance(begin, middle);
	std::size_t right_begin = left_end, right_end = right_begin + std::distance(middle, end);

	while (left_begin != left_end && right_begin != right_end)
	{
		if (pred(temp_array[left_begin], temp_array[right_begin]) > 0)
			*begin++ = std::move(temp_array[left_begin++]);
		else
			*begin++ = std::move(temp_array[right_begin++]);
	}

	if (left_begin == left_end)
		std::copy(&temp_array[right_begin], &temp_array[right_end], begin);
	else
		std::copy(&temp_array[left_begin], &temp_array[left_end], begin);
}

template <typename IterType, typename PredType>
void merge_sort(IterType begin, IterType end, const PredType &pred)
{
	auto distance = std::distance(begin, end);
	if (begin == end || distance == 1)
		return;

	//在数据较多时将会栈溢出
	auto middle = distance / 2;
	merge_sort(begin, begin + middle, pred);
	merge_sort(begin + middle, end, pred);
	merge_impl(begin, begin + middle, end, pred);
}

int main()
{
	std::vector<int> vec{ 1, 2, -1, 10, 7, 8, 9, 4, 2, 100, -10, -2 };
	insertion_sort(vec.begin(), vec.end(), [](auto arg0, auto arg1) { return arg1 - arg0; });

	std::vector<int> vec2{ 1, 3, 5, 7, 9, 100, 2, 4, 6, 8, 10, 100, 1000 };
	//std::vector<int> vec2{ 1,100, 9,  2 };
	//merge_impl(vec2.begin(), vec2.begin() + 6, vec2.end(), [](auto arg0, auto arg1) { return arg1 - arg0; });
	merge_sort(vec2.begin(), vec2.end(), [](auto arg0, auto arg1) { return arg1 - arg0; });

	std::cout << "";
}
