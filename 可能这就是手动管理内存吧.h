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
		//if (ptr == nullptr) return;
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
	value_type& insert(object_t&& obj) {//�����󸱱���������
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

	void clear()noexcept {//��������ڴ棬�ڴ��ڼ䲻�ܶԶ�������������������ᵼ��δ������Ϊ
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
	void traverse(const _Pr& _Pred)noexcept {//����ɵ��ö���traverse��������������нڵ㲢�Խڵ㴢��Ķ�����Ϊ���������ÿɵ��ö���
										//�ɵ��ö��󷵻�trueʱֹͣ�������������
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

	node_type* get_front(node_type* ptr)noexcept {//���һ���ڵ��ǰһ���ڵ㣬�˽ڵ������Ч���Ҳ���ͷ�ڵ㡣���ptr��ͷ�ڵ㣬���᷵��nullptr��
		node_type *rv = LockFreeLinkedListSpace::load_acquire(m_head);
		if (ptr == rv) return rv;//���ptr��ͷ�ڵ㣬����nullptr
		this->node_traverse([&rv, &ptr](node_type* it) {
			if (it == ptr) return true;
			rv = it;
			return false;
		});
		return rv;
	}

	inline bool is_next(node_type* ptr, node_type* ptr2) {//�ж�ptr2��ǰһ���ڵ��Ƿ���ptr
		return ptr->load_next() == ptr2;
	}

	void put_into_head(node_type *ptr)noexcept {//��һ���Ѿ�����Ľڵ���Ϊͷ�ڵ�
		std::lock_guard<SpinLock> head_locker(m_head_lock);//��ͷָ���޸���
		if (LockFreeLinkedListSpace::load_acquire(m_head) == nullptr) {
			//ͷ�ڵ㵱ǰΪ��
			LockFreeLinkedListSpace::store_release(m_head, ptr);
			return;
		}
		//ͷ�ڵ㵱ǰ�ǿ�
		std::lock_guard<SpinLock> node_locker(LockFreeLinkedListSpace::load_acquire(m_head)->sl);//����ǰͷ�ڵ����
		ptr->store_next(LockFreeLinkedListSpace::load_acquire(m_head));
		LockFreeLinkedListSpace::store_release(m_head, ptr);
	}
	
	void erase_head(node_type *ptr)noexcept {//ͨ��������ɾ����һ��node��ڶ���node
		std::lock_guard<SpinLock> head_locker(m_head_lock);//��ͷָ���޸���

		//����Ƿ���ͷ�ڵ�
		if (ptr == LockFreeLinkedListSpace::load_acquire(m_head)) {//����ͷ�ڵ�
			//std::lock_guard<SpinLock> node_locker(ptr->sl);
			ptr->sl.lock();//ʹ�����ɾ����Ϊɾ��֮����Ҫunlock(��Ƭ�ڴ��Ѿ���������)
			//auto freethis = LockFreeLinkedListSpace::load_acquire(m_head);
			LockFreeLinkedListSpace::store_release(m_head, ptr->load_next());//��ͷ�����Ϊ�Ͻڵ����һ���ڵ�
			LockFreeLinkedListSpace::free_node(ptr);
		}
		else {//����ͷ�ڵ���
			erase_normal(ptr, true);
		}
	}

	bool erase_normal(node_type *ptr, bool secondok) {//return false˵���ýڵ���ͷ�����߸ýڵ��ǵڶ����ڵ�
																					   //�ڶ���������true�Ļ�����������һ���ڶ����ڵ㣬��������ɾ��
		std::unique_lock<SpinLock> node_locker(ptr->sl);//����ptr
		std::unique_lock<SpinLock> node_locker2;

		node_type* front = this->get_front(ptr);
		while (true) {
			if (front == nullptr) return false;
			std::unique_lock<SpinLock> node_locker_temp(front->sl);
			if ((!secondok) && front == LockFreeLinkedListSpace::load_acquire(m_head)) return false;
			if (this->is_next(front, ptr)) {//ȷ����ϵ��Ч
				node_locker2 = std::move(node_locker_temp);
				break;
			}
			node_locker_temp.unlock();
			//��ϵ��Ч�����»�ȡǰһ��node
			front = this->get_front(ptr);
		}

		//��������Ѿ��õ����������ˣ�����ȷ����ǰ���ϵ��
		front->store_next(ptr->load_next());//ǰһ���ڵ��next=ptr��next
		node_locker2.unlock();//��ʱptr��ʵ���Ѿ��������Ƴ��ˣ����Կ��Խ���ǰһ���ڵ���

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
	Ӧ�øĽ�memorylist
	����һ���̰߳�ȫ���ڴ�ء�
	1.�̰߳�ȫ��֤
	2.RAII��֤

	��Ϊ
	1.alloc

	�ӿ�(���http://zh.cppreference.com/w/cpp/concept/Allocator)
	allocate(size_t)�����������size_t��value_type����
	allocate(size_t,pointer)
	deallocate(pointer,size_t)
	construct(pointer,args)
	destory(pointer) ע�⣬���Ĭ���ǵ������������������������deleter�Ļ����Ǿ���deleter��ɾ��
	max_size()
	operator==()
	operator!=()
	shrink_to_fit() ɾ��δʹ�õ��ڴ�
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
	pointer allocate_construct(const size_t& num, ARGS&& ...args) {//�˷�����ȷ��������ڴ��ڱ�traverse֮ǰ���Ѿ������Ϲ�����˶���
																										   //�˷��������쳣��ȫ��֤�������Ĺ��캯�������쳣��������nullptr����Ҳ���ᵼ���ڴ�й¶
		if (num < 1) return nullptr;
		auto ptr = reinterpret_cast<value_type*>(malloc(num * sizeof(value_type)));
		if (ptr == NULL) return nullptr;

		size_t constructed = 0;

		try {
			for (size_t i = 0; i < num; i++, constructed++)
				new(&ptr[i]) value_type(std::forward<ARGS>(args)...);
		}
		catch (...) {//ĳһ������ʧ�ܣ����ڰ�֮ǰ�����ȫ��������Ȼ��ɾ���ڴ�
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
		//��ȡobjectָ����m_list�е�����
		free(ptr);
		m_list.erase(fuck_this);
	}

	inline void clear() {//����ǰ��ȷ�����ж����Ѿ�����
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
	inline void traverse(const _Pr& _Pred)noexcept {//����ɵ��ö���traverse��������������нڵ㲢�Խڵ㴢��Ķ�����Ϊ���������ÿɵ��ö���
											                                     //�ɵ��ö��󷵻�trueʱֹͣ�������������
		                                                                         //ע�⣬����������ǲ������ڴ������Ƿ����˶���ģ����Ա��������ܻ��õ�һЩδ����Ķ����û��ڱ�д�������Ĳ�������ʱӦ�ÿ��ǵ������ء�
		m_list.traverse([&_Pred](void* ptr) {
			return _Pred(reinterpret_cast<pointer>(ptr));
		});
	}

private:
	LockFreeLinkedList<void*> m_list;
};