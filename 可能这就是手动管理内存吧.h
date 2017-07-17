#pragma once
#include <stdlib.h>
#include <memory.h>

namespace DC {
	using byte_t = unsigned char;
	using size_t = unsigned int;
}

template <typename _Ty>
class MemoryPool final {
	/*
	这是一个线程安全的内存池。
	1.线程安全保证
	2.RAII保证
	3.支持自定义删除器

	接口(详见http://zh.cppreference.com/w/cpp/concept/Allocator)
	allocate(size_t)分配可以容纳size_t个value_type对象
	allocate(size_t,pointer)
	deallocate(pointer,size_t)
	construct(pointer,args)
	destory(pointer) 注意，这个默认是调用构造函数，但是如果是有deleter的话，那就用deleter来删除
	max_size()
	operator==()
	operator!=()
	shrink_to_fit() 删掉未使用的内存，这个

	实现
	1.内存块(一片可容纳value_type对象的内存)使用单向链表管理，一个节点包含一个ptr、一个bool值以及两个std::automic<bool>，此bool值指示该节点的ptr是否已被分配，第一个std::automic<bool>标识是否可以在此时对该节点进行修改，第二个std::automic<bool>则标识此节点是否正在被删除
	2.allocate时，在尾部增加一个节点，然后返回该节点指向的内存
	3.需要修改一个节点时，首先锁上该节点的std::automic<bool>，然后对该节点进行修改
	4.需要在一个位置上删除节点时，
	5.不允许在一个链表中插入一个节点

	之所以使用自旋+std::automic<bool>而不是互斥量，出于两个考虑，1.std::mutex占用空间太大，不划算；2.根据https://www.zhihu.com/question/38857029/answer/78480263，在等待时间短的情况下，自旋的性能比互斥量更好；
	*/
public:
	using value_type = _Ty;
	using pointer = _Ty*;
	using const_pointer = const pointer;
	using size_type = DC::size_t;

private:
	using size_t = DC::size_t;

public:
private:
};