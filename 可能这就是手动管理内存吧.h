#pragma once
#include <stdlib.h>

namespace DC {
	using byte_t = unsigned char;
	using size_t = unsigned int;
}

struct ChunkInfo {
	void *ptr;
	DC::size_t size;
};

class Allocator final {
public:
	using size_t = DC::size_t;

private:
	using byte_t = DC::byte_t;

public:
	Allocator() :entire{ nullptr,0 }, idle(nullptr), used(nullptr), idle_size(0), used_size(0) {}

	Allocator(const Allocator&) = delete;

	~Allocator() {
		free_memory();
	}

public:
	bool set_memory(const size_t& _Size)noexcept {
		//如果已经分配过了，返回false
		if (this->has_memory()) return false;

		if ((entire.ptr = malloc(_Size)) == NULL) {
			//分配失败
			entire.ptr = nullptr;
			return false;
		}

		//分配成功
		entire.size = _Size;
		return true;
	}

	void free_memory()noexcept {
		if (!this->has_memory()) return;

		free(entire.ptr);
		entire.ptr = nullptr;
		entire.size = 0;
	}

	bool has_memory()const noexcept {
		return entire.ptr != nullptr;
	}

	void* allocate(const size_t& _Size)noexcept {}

	template <typename _Ty>
	_Ty* allocate()noexcept {}

	void deallocate(void *_Ptr) {}

private:


private:
	//从环境申请的内存信息
	ChunkInfo entire;

	//储存空闲、已用内存信息的数组
	ChunkInfo *idle, *used;
	//数组长度
	DC::size_t idle_size, used_size;
};