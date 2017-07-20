#pragma once
#include <stdlib.h>
#include <memory.h>
#include "DC_Exception.h"
#include "DC_STR.h"
#include "boost\lockfree\queue.hpp"

namespace DC {
	using byte_t = unsigned char;
	using size_t = unsigned int;
}

template <typename _Ty>
class QueueAllocator final {//designed for DC::IOCP
public:
	using value_type = _Ty;
	using pointer = _Ty*;
	using const_pointer = const pointer;
	using size_type = DC::size_t;

private:
	using size_t = DC::size_t;

public:
	QueueAllocator(size_t size) :m_list(size) {}

	QueueAllocator(const QueueAllocator&) = delete;

	~QueueAllocator()noexcept {
		this->clear();
	}

public:
	//create a new value_type object in list and return the pointer of the new object.
	//if malloc or construction failed,it will return false.
	//thread-safe but maybe not lock-free(depending on the implementation of malloc)
	template <typename ...ARGS>
	pointer make(ARGS&& ...args)noexcept {
		auto ptr = reinterpret_cast<pointer>(malloc(sizeof(value_type)));
		if (ptr == NULL)
			return nullptr;

		try {
			new(ptr) value_type(std::forward<ARGS>(args)...);
			if (!m_list.push(ptr)) {
				destory(ptr);
				ptr = nullptr;
			}
		}
		catch (...) {
			free(ptr);
			ptr = nullptr;
		}
		return ptr;
	}

	inline bool put(const value_type& obj)noexcept {
		try {
			return m_list.push(obj);
		}
		catch (...) {
			return false;
		}
	}

	template <typename Functor>
	bool remove_if(const Functor& _Pred)noexcept {//deconstruct and deallocate the back item in the queue, if _Pred return true.
												  //otherwise push the item(already popped) back to queue.
												  //_Pred should not throw exception
												  //if _Pred has been executed, return true, otherwise return false
		pointer ptr = nullptr;
		if (!m_list.pop(ptr))
			return false;

		bool PredRv;
		try {
			PredRv = _Pred(*ptr);
		}
		catch (...) {
			if (!m_list.push(ptr))
				destory(ptr);
			return true;
		}

		try {
			if (!PredRv) {
				if (!m_list.push(ptr))
					destory(ptr);
			}
			else
				destory(ptr);
		}
		catch (...) {}

		return true;
	}

	void clear()noexcept {//not thread-safe		 
		pointer ptr = nullptr;
		while (m_list.pop(ptr)) {
			if (ptr == nullptr) continue;
			destory(ptr);
		}

		m_list.reserve(0);
	}

	bool empty()const {
		return m_list.empty();
	}

private:
	inline void destory(pointer ptr) {
		ptr->~value_type();
		free(ptr);
	}

private:
	boost::lockfree::queue<pointer> m_list;
};