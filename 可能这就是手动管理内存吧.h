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

	template <typename ...ARGS>
	static pointer make_norecord(ARGS&& ...args)noexcept {
		auto ptr = reinterpret_cast<pointer>(malloc(sizeof(value_type)));
		if (ptr == NULL)
			return nullptr;

		try {
			new(ptr) value_type(std::forward<ARGS>(args)...);
		}
		catch (...) {
			free(ptr);
			ptr = nullptr;
		}
		return ptr;
	}

	inline bool put(value_type& obj)noexcept {
		try {
			return m_list.push(&obj);
		}
		catch (...) {
			return false;
		}
	}

	template <typename Functor>
	void remove_if(const DC::size_t& timelimit, const Functor& _Pred)noexcept {//_Pred返回1删除，_Pred返回2不删除，_Pred返回3停止。timelimit到了也会停止。_Pred不能抛异常。
		pointer ptr = nullptr;
		DC::timer timer;
		timer.reset();
		timer.start();
		while (true) {
			if (!m_list.pop(ptr))
				return;

			if (timer.getms() >= timelimit)
				return;

			switch (_Pred(*ptr)) {
			case 1: {//删除
				destory(ptr);
				continue;
			}break;
			case 2: {//下一个
				if (!m_list.push(ptr))
					destory(ptr);
			}break;
			case 3: {//立刻退出
				return;
			}break;
			}
		}
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

	inline void destory(void* ptr) {
		reinterpret_cast<pointer>(ptr)->~value_type();
		free(ptr);
	}

private:
	boost::lockfree::queue<pointer> m_list;
};