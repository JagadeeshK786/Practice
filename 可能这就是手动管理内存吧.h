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
		//if (ptr == nullptr) return;
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
		m_head.store(LockFreeLinkedListSpace::make_node<node_type>(malloc(sizeof(test))));
		for (int i = 0; i < 3; i++) {
			LockFreeLinkedListSpace::node<value_type>* it = m_head.load();
			while (1) {
				if (it->load_next() == nullptr) {
					it->store_next(LockFreeLinkedListSpace::make_node<node_type>(malloc(sizeof(test))));
					break;
				}
				it = it->load_next();
			}
		}
		//*/
	}

	~LockFreeLinkedList()noexcept {
		clear();
	}

public:
	template <typename object_t>
	value_type& insert(object_t&& obj) {//将对象副本插入链表
		static_assert(std::is_same<value_type, typename std::decay<object_t>::type>::value, "input type should be value_type");

		auto new_node = LockFreeLinkedListSpace::make_node<node_type>(std::forward<object_t>(obj));
		if (new_node == nullptr)
			throw DC::Exception("LockFreeLinkedList::insert", "can not make node");

		put_into_head(new_node);
		return *(new_node->object);
	}

	template <typename ...ARGS>
	value_type& emplace(ARGS&& ...args) {
		auto new_node = LockFreeLinkedListSpace::make_node<node_type>(std::forward<ARGS>(args)...);
		if (new_node == nullptr)
			throw DC::Exception("LockFreeLinkedList::insert", "can not make node");

		put_into_head(new_node);
		return *(new_node->object);
	}

	bool erase(const value_type& item) {
		auto node = this->is_inside_and_get_node(item);
		if (node == nullptr) return false;

		auto temp_head = LockFreeLinkedListSpace::load_acquire(m_head);
		if (node == temp_head || node == temp_head->load_next()) {
			erase_head(node);
		}
		else {
			if (!erase_normal(node, false))
				erase_head(node);
		}

		return true;
	}

	inline bool is_inside(const value_type& item) {
		if (&item == nullptr) return false;
		bool rv = false;
		this->traverse([&rv](const value_type& it) {
			if (&it == &item) {
				rv = true;
				return true;
			}
			return false;
		});
		return rv;
	}

	void clear()noexcept {//清空所有内存，在此期间不能对对象做其他操作，否则会导致未定义行为
		node_type* it = LockFreeLinkedListSpace::load_acquire(m_head);

		while (it != nullptr) {
			auto temp = it->load_next();
			LockFreeLinkedListSpace::free_node(it);
			it = temp;
		}

		LockFreeLinkedListSpace::store_release(m_head, nullptr);
	}

	bool empty() {
		return LockFreeLinkedListSpace::load_acquire(m_head) == nullptr;
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
	void traverse(const _Pr& _Pred)noexcept {//传入可调用对象，traverse函数将会遍历所有节点并以节点储存的对象作为参数来调用可调用对象。
										//可调用对象返回true时停止遍历，否则继续
		for (LockFreeLinkedListSpace::node<value_type>* ptr(LockFreeLinkedListSpace::load_acquire(m_head)); ptr != nullptr; ptr = ptr->load_next())
			if (_Pred(*ptr->object)) break;
	}

private:
	inline node_type* is_inside_and_get_node(const value_type& item) {
		if (&item == nullptr) return nullptr;
		node_type* rv = nullptr;
		this->node_traverse([&rv, &item](node_type* it) {
			if (it->object == &item) {
				rv = it;
				return true;
			}
			return false;
		});
		return rv;
	}

	template <typename _Pr>
	void node_traverse(const _Pr& _Pred)noexcept {
		for (LockFreeLinkedListSpace::node<value_type>* ptr(LockFreeLinkedListSpace::load_acquire(m_head)); ptr != nullptr; ptr = ptr->load_next())
			if (_Pred(ptr)) break;
	}

	node_type* get_front(node_type* ptr)noexcept {//获得一个节点的前一个节点，此节点必须有效并且不是头节点。如果ptr是头节点，将会返回nullptr。
		node_type *rv = LockFreeLinkedListSpace::load_acquire(m_head);
		if (ptr == rv) return rv;//如果ptr是头节点，返回nullptr
		this->node_traverse([&rv, &ptr](node_type* it) {
			if (it == ptr) return true;
			rv = it;
			return false;
		});
		return rv;
	}

	inline bool is_next(node_type* ptr, node_type* ptr2) {//判断ptr2的前一个节点是否是ptr
		return ptr->load_next() == ptr2;
	}

	void put_into_head(node_type *ptr)noexcept {//把一个已经构造的节点设为头节点
		std::lock_guard<SpinLock> head_locker(m_head_lock);//拿头指针修改锁
		if (LockFreeLinkedListSpace::load_acquire(m_head) == nullptr) {
			//头节点当前为空
			LockFreeLinkedListSpace::store_release(m_head, ptr);
			return;
		}
		//头节点当前非空
		std::lock_guard<SpinLock> node_locker(LockFreeLinkedListSpace::load_acquire(m_head)->sl);//锁当前头节点的锁
		ptr->store_next(LockFreeLinkedListSpace::load_acquire(m_head));
		LockFreeLinkedListSpace::store_release(m_head, ptr);
	}
	
	void erase_head(node_type *ptr)noexcept {//通过本函数删除第一个node或第二个node
		std::lock_guard<SpinLock> head_locker(m_head_lock);//拿头指针修改锁

		//检查是否还是头节点
		if (ptr == LockFreeLinkedListSpace::load_acquire(m_head)) {//还是头节点
			//std::lock_guard<SpinLock> node_locker(ptr->sl);
			ptr->sl.lock();//使用这个删，因为删完之后不需要unlock(这片内存已经被回收了)
			//auto freethis = LockFreeLinkedListSpace::load_acquire(m_head);
			LockFreeLinkedListSpace::store_release(m_head, ptr->load_next());//将头结点设为老节点的下一个节点
			LockFreeLinkedListSpace::free_node(ptr);
		}
		else {//不是头节点了
			erase_normal(ptr, true);
		}
	}

	bool erase_normal(node_type *ptr, bool secondok) {//return false说明该节点是头结点或者该节点是第二个节点
																					   //第二个参数是true的话就允许这是一个第二个节点，可以正常删除
		std::unique_lock<SpinLock> node_locker(ptr->sl);//锁上ptr
		std::unique_lock<SpinLock> node_locker2;

		node_type* front = this->get_front(ptr);
		while (true) {
			if (front == nullptr) return false;
			std::unique_lock<SpinLock> node_locker_temp(front->sl);
			if ((!secondok) && front == LockFreeLinkedListSpace::load_acquire(m_head)) return false;
			if (this->is_next(front, ptr)) {//确定关系有效
				node_locker2 = std::move(node_locker_temp);
				break;
			}
			node_locker_temp.unlock();
			//关系无效，重新获取前一个node
			front = this->get_front(ptr);
		}

		//到这里就已经拿到了两个锁了（并且确认了前后关系）
		front->store_next(ptr->load_next());//前一个节点的next=ptr的next
		node_locker2.unlock();//此时ptr事实上已经从链表移除了，所以可以解锁前一个节点了

		LockFreeLinkedListSpace::free_node(ptr);
		return true;
	}
	
private:
	std::atomic<node_type*> m_head;
	SpinLock m_head_lock;
};

template <typename _Ty>
class MemoryPool final {
	/*
	应该改叫memorylist
	这是一个线程安全的内存池。
	1.线程安全保证
	2.RAII保证

	行为
	1.alloc

	接口(详见http://zh.cppreference.com/w/cpp/concept/Allocator)
	allocate(size_t)分配可以容纳size_t个value_type对象
	allocate(size_t,pointer)
	deallocate(pointer,size_t)
	construct(pointer,args)
	destory(pointer) 注意，这个默认是调用析构函数，但是如果是有deleter的话，那就用deleter来删除
	max_size()
	operator==()
	operator!=()
	shrink_to_fit() 删掉未使用的内存
	*/
public:
	using value_type = _Ty;
	using pointer = _Ty*;
	using const_pointer = const pointer;
	using size_type = DC::size_t;

private:
	using size_t = DC::size_t;

public:
	~MemoryPool() {
		clear();
	}

public:
	pointer allocate(const size_t& num) {
		if (num < 1) return nullptr;
		auto ptr = malloc(num * sizeof(value_type));
		if (ptr == NULL) return nullptr;

		return reinterpret_cast<value_type*>(m_list.insert(ptr));
	}

	template <typename ...ARGS>
	pointer allocate_construct(const size_t& num, ARGS&& ...args) {//此方法可确保分配的内存在被traverse之前就已经在其上构造过了对象
																										   //此方法包含异常安全保证，如果你的构造函数抛了异常（将返回nullptr），也不会导致内存泄露
		if (num < 1) return nullptr;
		auto ptr = reinterpret_cast<value_type*>(malloc(num * sizeof(value_type)));
		if (ptr == NULL) return nullptr;

		size_t constructed = 0;

		try {
			for (size_t i = 0; i < num; i++, constructed++)
				new(&ptr[i]) value_type(std::forward<ARGS>(args)...);
		}
		catch (...) {//某一个构造失败，现在把之前构造的全部析构，然后删掉内存
			for (size_t i = 0; i < constructed; i++)
				ptr[i].~value_type();
			free(ptr);
			return nullptr;
		}

		return reinterpret_cast<value_type*>(m_list.insert(reinterpret_cast<void*>(ptr)));
	}

	inline void deallocate(pointer ptr, const size_t&) {
		auto& fuck_this = m_list.find([&ptr](const void* it) {
			if (it == ptr) 
				return true;
			return false;
		});
		//获取object指针在m_list中的引用
		free(ptr);
		m_list.erase(fuck_this);
	}

	inline void clear() {//调用前请确认所有对象都已经析构
		m_list.traverse([](void*& it) {
			free(it);
			it = nullptr;
			return false;
		});
		m_list.clear();		
	}

	inline bool empty() {
		return m_list.empty();
	}

	template <typename _Pr>
	inline void traverse(const _Pr& _Pred)noexcept {//传入可调用对象，traverse函数将会遍历所有节点并以节点储存的对象作为参数来调用可调用对象。
											                                     //可调用对象返回true时停止遍历，否则继续
		                                                                         //注意，由于这个类是不管你内存区域是否构造了对象的，所以本函数可能会拿到一些未构造的对象。用户在编写本函数的参数函数时应该考虑到此因素。
		m_list.traverse([&_Pred](void* ptr) {
			return _Pred(reinterpret_cast<pointer>(ptr));
		});
	}

private:
	LockFreeLinkedList<void*> m_list;
};