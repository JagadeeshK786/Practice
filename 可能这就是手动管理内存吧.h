#pragma once
#include <stdlib.h>
#include <memory.h>
#include "DC_Exception.h"
#include "DC_STR.h"

namespace DC {
	using byte_t = unsigned char;
	using size_t = unsigned int;
}

class SpinLock final {
public:
	SpinLock() :flag{ 0 } {}

	SpinLock(const SpinLock&) = delete;

public:
	void lock() {
		while (flag.test_and_set(std::memory_order::memory_order_acquire));
	}

	bool try_lock() {
		return !flag.test_and_set(std::memory_order_acquire);
	}

	void unlock() {
		flag.clear(std::memory_order::memory_order_release);
	}

private:
	std::atomic_flag flag;
};

namespace LockFreeLinkedListSpace {

	template <typename atomic_t>
	inline auto load_acquire(const atomic_t& _Th)noexcept->decltype(_Th.load(std::memory_order::memory_order_acquire)) {
		return _Th.load(std::memory_order::memory_order_acquire);
	}

	template <typename atomic_t, typename value_t>
	inline void store_release(atomic_t& _Th, value_t&& _Val)noexcept {
		_Th.store(std::forward<value_t>(_Val), std::memory_order::memory_order_release);
	}

	template <typename _Ty>
	class node {
	public:
		node() : object(nullptr), next_ptr(nullptr) {}

		template <typename ...ARGS>
		node(ARGS&& ...args) : object(new _Ty(std::forward<ARGS>(args)...)), next_ptr(nullptr) {}

		~node()noexcept {
			if (object)
				delete object;
		}

	public:
		inline node<_Ty>* load_next()const noexcept {
			return load_acquire(next_ptr);
		}

		inline void store_next(node<_Ty>* new_ptr)noexcept {
			store_release(next_ptr, new_ptr);
		}

	public:
		_Ty* object;
		SpinLock sl;

	private:
		std::atomic<node<_Ty>*> next_ptr;
	};

	template <typename atomicXD, typename ...ARGS>
	atomicXD* make_node(ARGS&& ...args)noexcept {//不会抛异常，如果内存分配失败或者构造函数抛了异常，只会返回nullptr
		using node_type = atomicXD;

		auto ptr = malloc(sizeof(node_type));
		if (ptr == NULL) return nullptr;

		try {
			new(ptr) node_type(std::forward<ARGS>(args)...);
		}
		catch (...) {
			free(ptr);
			return nullptr;
		}

		return reinterpret_cast<node_type*>(ptr);
	}

	template <typename atomicXD>
	void free_node(atomicXD *ptr)noexcept {
		ptr->~atomicXD();
		free(ptr);
	}

}

template <typename _Ty>
class LockFreeLinkedList final {//测试OK之后把m_head改成非原子，因为理论上来说应该是可以非原子
public:
	using value_type = _Ty;
	using size_type = DC::size_t;

private:
	using node_type = LockFreeLinkedListSpace::node<_Ty>;

public:
	LockFreeLinkedList() :m_head(nullptr) {
		//debug
		/*
		m_head.store(LockFreeLinkedListSpace::make_node<node_type>("9710"));
		for (int i = 0; i < 3; i++) {
			LockFreeLinkedListSpace::node<value_type>* it = m_head.load();
			while (1) {
				if (it->load_next() == nullptr) {
					it->store_next(LockFreeLinkedListSpace::make_node<node_type>(DC::STR::toString(i + 1)));
					break;
				}
				it = it->load_next();
			}
		}
		*/
	}

	~LockFreeLinkedList()noexcept {
		clear();
	}

public:
	template <typename object_t>
	void insert(object_t&& obj) {//将对象副本插入链表
		static_assert(std::is_same<value_type, typename std::decay<object_t>::type>::value, "input type should be value_type");

		auto new_node = LockFreeLinkedListSpace::make_node<node_type>(std::forward<object_t>(obj));
		if (new_node == nullptr)
			throw DC::Exception("LockFreeLinkedList::insert", "can not make node");

		put_into_head(new_node);
	}

	template <typename ...ARGS>
	void emplace(ARGS&& ...args) {
		auto new_node = LockFreeLinkedListSpace::make_node<node_type>(std::forward<ARGS>(args)...);
		if (new_node == nullptr)
			throw DC::Exception("LockFreeLinkedList::insert", "can not make node");

		put_into_head(new_node);
	}

	bool erase(const value_type&) {}

	void clear()noexcept {//清空所有内存，在此期间不能对对象做其他操作，否则会导致未定义行为
		node_type* it = LockFreeLinkedListSpace::load_acquire(m_head);

		while (it != nullptr) {
			auto temp = it->load_next();
			LockFreeLinkedListSpace::free_node(it);
			it = temp;
		}
	}

	template <typename _Pr>
	value_type& find(const _Pr& _Pred) {//传入可调用对象，find函数将会遍历所有节点并以节点储存的对象作为参数来调用可调用对象。
		                                                       //可调用对象的返回值必须是bool。可调用对象返回false，意味着继续遍历；可调用对象返回true，则find函数将返回这个对象的引用；
		                                                       //如果没有找到对象，将会抛出DC::Exception异常。
		if (LockFreeLinkedListSpace::load_acquire(m_head) == nullptr)
			throw DC::Exception("LockFreeLinkedList::find", "object not found");

		for (LockFreeLinkedListSpace::node<value_type>* ptr(LockFreeLinkedListSpace::load_acquire(m_head)); ptr != nullptr; ptr = ptr->load_next())
			if (_Pred(*ptr->object)) return *ptr->object;

		throw DC::Exception("LockFreeLinkedList::find", "object not found");
	}

	template <typename _Pr>
	void traverse(const _Pr& _Pred) {//传入可调用对象，traverse函数将会遍历所有节点并以节点储存的对象作为参数来调用可调用对象。
										//可调用对象返回值将会被忽略
		for (LockFreeLinkedListSpace::node<value_type>* ptr(LockFreeLinkedListSpace::load_acquire(m_head)); ptr != nullptr; ptr = ptr->load_next())
			_Pred(*ptr->object);
	}

private:
	template <typename _Pr>
	node_type* internal_find(const _Pr& _Pred)noexcept {
		if (LockFreeLinkedListSpace::load_acquire(m_head) == nullptr)
			return nullptr;

		for (LockFreeLinkedListSpace::node<value_type>* ptr(LockFreeLinkedListSpace::load_acquire(m_head)); ptr == nullptr; ptr = ptr->load_next())
			if (_Pred(ptr)) return ptr;

		return nullptr;
	}

	void put_into_head(node_type *ptr)noexcept {//把一个已经构造的节点设为头结点
		std::lock_guard<SpinLock> head_locker(m_head_lock);//拿头指针修改锁
		if (LockFreeLinkedListSpace::load_acquire(m_head) == nullptr) {
			//头结点当前为空
			LockFreeLinkedListSpace::store_release(m_head, ptr);
			return;
		}
		//头结点当前非空
		std::lock_guard<SpinLock> node_locker(LockFreeLinkedListSpace::load_acquire(m_head)->sl);//锁当前头节点的锁
		ptr->store_next(LockFreeLinkedListSpace::load_acquire(m_head));
		LockFreeLinkedListSpace::store_release(m_head, ptr);
	}

private:
	std::atomic<node_type*> m_head;
	SpinLock m_head_lock;
};

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