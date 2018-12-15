#include <iostream>
#include <utility>
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

int main()
{
    std::vector<int> vec{2, 1, -1, 10, 7, 8, 9, 4, 2, 100, -10, -2};
    insertion_sort(vec.begin(), vec.end(), [](auto arg0, auto arg1) { return arg1 - arg0; });
    std::cout << "";
}
