#include <iostream>
#include <memory>
#include <mutex>
#include <new>
#include <string>
using namespace std;

/*
题目：在顺序表L中查找第一个值最大的元素，并删除该元素。
 */
void remove_max(int *input_array, std::size_t array_size) {
  remove(input_array, array_size, find_max(input_array, array_size));
}

/*
题目：删除顺序表L的第i个元素。
*/
void remove(int *input_array, std::size_t array_size, std::size_t index) {
  remove(input_array, array_size, input_array + index);
}

/*
此函数会找出数组中的最大元素，并返回指向它的指针。若数组不含元素，则返回NULL。
input_array:被操作的数组
array_size:数组元素数
*/
int *find_max(int *input_array, std::size_t array_size) {
  if (array_size == 0)
    return NULL;
  int *result = input_array;
  for (uint32_t index = 1; index < array_size; index++)
    if (input_array[index] > *result)
      result = &input_array[index];
  return result;
}

/*
此函数将会移除数组中的指定元素，并将其后元素逐个向前复制。数组末尾的空位将被设为0。
input_array:被操作的数组
array_size:数组元素数
iter:指向要删除的数组元素的指针（若该指针所指对象不存在于数组中，则此函数行为未定义）
*/
void remove(int *input_array, std::size_t array_size, int *iter) {
  for (; iter != input_array + array_size - 1; iter++)
    *iter = *(iter + 1);
  *(input_array + array_size - 1) = 0;
}

int main() {
  int L[12]{51, 99, 55, 44, 177, 22, 0, -14, -33, 7, 100, -2};
  const std::size_t L_size = sizeof(L) / sizeof(int);
  remove_max(L, L_size);
  return 0;
}