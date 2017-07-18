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
	atomicXD* make_node(ARGS&& ...args)noexcept {//�������쳣������ڴ����ʧ�ܻ��߹��캯�������쳣��ֻ�᷵��nullptr
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
class LockFreeLinkedList final {//����OK֮���m_head�ĳɷ�ԭ�ӣ���Ϊ��������˵Ӧ���ǿ��Է�ԭ��
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
	void insert(object_t&& obj) {//�����󸱱���������
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

	void clear()noexcept {//��������ڴ棬�ڴ��ڼ䲻�ܶԶ�������������������ᵼ��δ������Ϊ
		node_type* it = LockFreeLinkedListSpace::load_acquire(m_head);

		while (it != nullptr) {
			auto temp = it->load_next();
			LockFreeLinkedListSpace::free_node(it);
			it = temp;
		}
	}

	template <typename _Pr>
	value_type& find(const _Pr& _Pred) {//����ɵ��ö���find��������������нڵ㲢�Խڵ㴢��Ķ�����Ϊ���������ÿɵ��ö���
		                                                       //�ɵ��ö���ķ���ֵ������bool���ɵ��ö��󷵻�false����ζ�ż����������ɵ��ö��󷵻�true����find���������������������ã�
		                                                       //���û���ҵ����󣬽����׳�DC::Exception�쳣��
		if (LockFreeLinkedListSpace::load_acquire(m_head) == nullptr)
			throw DC::Exception("LockFreeLinkedList::find", "object not found");

		for (LockFreeLinkedListSpace::node<value_type>* ptr(LockFreeLinkedListSpace::load_acquire(m_head)); ptr != nullptr; ptr = ptr->load_next())
			if (_Pred(*ptr->object)) return *ptr->object;

		throw DC::Exception("LockFreeLinkedList::find", "object not found");
	}

	template <typename _Pr>
	void traverse(const _Pr& _Pred) {//����ɵ��ö���traverse��������������нڵ㲢�Խڵ㴢��Ķ�����Ϊ���������ÿɵ��ö���
										//�ɵ��ö��󷵻�ֵ���ᱻ����
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

	void put_into_head(node_type *ptr)noexcept {//��һ���Ѿ�����Ľڵ���Ϊͷ���
		std::lock_guard<SpinLock> head_locker(m_head_lock);//��ͷָ���޸���
		if (LockFreeLinkedListSpace::load_acquire(m_head) == nullptr) {
			//ͷ��㵱ǰΪ��
			LockFreeLinkedListSpace::store_release(m_head, ptr);
			return;
		}
		//ͷ��㵱ǰ�ǿ�
		std::lock_guard<SpinLock> node_locker(LockFreeLinkedListSpace::load_acquire(m_head)->sl);//����ǰͷ�ڵ����
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