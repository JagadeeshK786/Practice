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

		~node() {
			if (object)
				delete object;
		}

	public:
		inline node<_Ty>* load_next()const {
			return load_acquire(next_ptr);
		}

		inline void store_next(node<_Ty>* new_ptr) {
			store_release(next_ptr, new_ptr);
		}

	public:
		_Ty* object;
		SpinLock sl;

	private:
		std::atomic<node<_Ty>*> next_ptr;
	};

}

template <typename _Ty>
class LockFreeLinkedList final {
public:
	using value_type = _Ty;
	using size_type = DC::size_t;

public:
	LockFreeLinkedList() :m_head(nullptr) {
		//debug
		m_head.store(new LockFreeLinkedListSpace::node<value_type>("9710"));
		for (int i = 0; i < 3; i++) {
			LockFreeLinkedListSpace::node<value_type>* it = m_head.load();
			while (1) {
				if (it->load_next() == nullptr) {
					it->store_next(new LockFreeLinkedListSpace::node<value_type>(DC::STR::toString(i + 1)));
					break;
				}
				it = it->load_next();
			 }
		}
	}

	~LockFreeLinkedList() {
		clear();
	}

public:
	void insert(const value_type&) {}

	template <typename ...ARGS>
	void emplace(ARGS&& ...args) {}

	bool erase(const value_type&) {}

	void clear() {

	}

	template <typename _Pr>
	value_type& find(const _Pr& _Pred) {//_Pr��һ���ɵ��ö���Ӧ�ý���_Pr(const value_type&)��ʽ�ĵ��ã�����ҵ��ˣ�����true�����򷵻�false
		if (LockFreeLinkedListSpace::load_acquire(m_head) == nullptr)
			throw DC::Exception("LockFreeLinkedList::find", "object not found");

		for (LockFreeLinkedListSpace::node<value_type>* ptr(LockFreeLinkedListSpace::load_acquire(m_head)); ; ptr = ptr->load_next()) {
			if (_Pred(*ptr->object)) return *ptr->object;
			if (ptr->load_next() == nullptr) break;
		}

		throw DC::Exception("LockFreeLinkedList::find", "object not found");
	}

private:
	template <typename _Pr>
	LockFreeLinkedListSpace::node<value_type>* internal_find(const _Pr& _Pred)noexcept {
		if (LockFreeLinkedListSpace::load_acquire(m_head) == nullptr)
			return nullptr;

		for (LockFreeLinkedListSpace::node<value_type>* ptr(LockFreeLinkedListSpace::load_acquire(m_head)); ; ptr = ptr->load_next()) {
			if (_Pred(ptr)) return ptr;
			if (ptr->load_next() == nullptr) break;
		}

		return nullptr;
	}

private:
	std::atomic<LockFreeLinkedListSpace::node<value_type>*> m_head;
	SpinLock m_head_lock;
};

template <typename _Ty>
class MemoryPool final {
	/*
	����һ���̰߳�ȫ���ڴ�ء�
	1.�̰߳�ȫ��֤
	2.RAII��֤
	3.֧���Զ���ɾ����

	�ӿ�(���http://zh.cppreference.com/w/cpp/concept/Allocator)
	allocate(size_t)�����������size_t��value_type����
	allocate(size_t,pointer)
	deallocate(pointer,size_t)
	construct(pointer,args)
	destory(pointer) ע�⣬���Ĭ���ǵ��ù��캯���������������deleter�Ļ����Ǿ���deleter��ɾ��
	max_size()
	operator==()
	operator!=()
	shrink_to_fit() ɾ��δʹ�õ��ڴ棬���

	ʵ��
	1.�ڴ��(һƬ������value_type������ڴ�)ʹ�õ����������һ���ڵ����һ��ptr��һ��boolֵ�Լ�����std::automic<bool>����boolֵָʾ�ýڵ��ptr�Ƿ��ѱ����䣬��һ��std::automic<bool>��ʶ�Ƿ�����ڴ�ʱ�Ըýڵ�����޸ģ��ڶ���std::automic<bool>���ʶ�˽ڵ��Ƿ����ڱ�ɾ��
	2.allocateʱ����β������һ���ڵ㣬Ȼ�󷵻ظýڵ�ָ����ڴ�
	3.��Ҫ�޸�һ���ڵ�ʱ���������ϸýڵ��std::automic<bool>��Ȼ��Ըýڵ�����޸�
	4.��Ҫ��һ��λ����ɾ���ڵ�ʱ��
	5.��������һ�������в���һ���ڵ�

	֮����ʹ������+std::automic<bool>�����ǻ������������������ǣ�1.std::mutexռ�ÿռ�̫�󣬲����㣻2.����https://www.zhihu.com/question/38857029/answer/78480263���ڵȴ�ʱ��̵�����£����������ܱȻ��������ã�
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