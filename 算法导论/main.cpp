#include <iostream>
#include <vector>

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

template <typename IterType, typename PredType>
void merge_sort(IterType begin1, IterType end1, IterType begin2, IterType end2, IterType output, const PredType &pred)
{
    while (begin1 != end1 && begin2 != end2)
    {
        if (pred(*begin1, *begin2) > 0)
            *output++ = *begin1++;
        else
            *output++ = *begin2++;
    }
    const auto &push_back = [&output](IterType begin, IterType end) -> void {
        while (begin != end)
            *output++ = *begin++;
    };
    if (begin1 == end1)
        push_back(begin2, end2);
    else
        push_back(begin1, end1);
}

int main()
{
    std::vector<int> vec{2, 1, -1, 10, 7, 8, 9, 4, 2, 100, -10, -2};
    insertion_sort(vec.begin(), vec.end(), [](auto arg0, auto arg1) { return arg1 - arg0; });

    std::vector<int> first{1, 3, 5, 7, 9}, second{2, 4, 6, 8, 10}, output(10);
    merge_sort(first.begin(), first.end(), second.begin(), second.end(), output.begin(), [](auto arg0, auto arg1) { return arg1 - arg0; });
    std::cout << "";
}
