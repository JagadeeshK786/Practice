#pragma once
#include <stdlib.h>
#include <memory.h>

namespace DC {
	using byte_t = unsigned char;
	using size_t = unsigned int;
}

struct ChunkInfo {
	void *ptr;
	DC::size_t size;
};

namespace AllocatorSpace {

	static const DC::size_t AllocatorInternalArrayDefaultSize(5);

	bool construct(ChunkInfo*& ptr, const DC::size_t& _Size) {
		auto temp = malloc(sizeof(ChunkInfo)*_Size);
		if (temp == NULL) return false;
		memset(temp, 0, sizeof(ChunkInfo)*_Size);
		ptr = reinterpret_cast<ChunkInfo*>(temp);
		return true;
	}

	void destruct(ChunkInfo* ptr) {
		if (ptr == nullptr || ptr == NULL) return;
		free(ptr);
	}

	bool expand(ChunkInfo*& ptr, const DC::size_t& _Old_Size, const DC::size_t& _New_Size) {
		if (_New_Size < _Old_Size) return false;
		if (_New_Size == _Old_Size) return true;

		ChunkInfo *new_ptr = nullptr;
		if (!construct(new_ptr, _New_Size)) return false;

		memcpy(new_ptr, ptr, sizeof(ChunkInfo)*_Old_Size);
		destruct(ptr);

		ptr = new_ptr;

		return true;
	}

}

class Allocator final {
public:
	using size_t = DC::size_t;

private:
	using byte_t = DC::byte_t;

public:
	Allocator() :entire{ nullptr,0 }, idle(nullptr), used(nullptr), idle_size(0), used_size(0) {
		construct_info();
	}

	Allocator(const size_t& _Size) :entire{ nullptr,0 }, idle(nullptr), used(nullptr), idle_size(0), used_size(0) {
		construct_info();
		if (!set_memory(_Size)) {
			this->~Allocator();
			//在这里抛无法分配内存异常
		}
	}

	Allocator(const Allocator&) = delete;

	~Allocator()noexcept {
		free_memory();
		destruct_info();
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
	void construct_info() {
		if (!(AllocatorSpace::construct(idle, AllocatorSpace::AllocatorInternalArrayDefaultSize) && AllocatorSpace::construct(used, AllocatorSpace::AllocatorInternalArrayDefaultSize))) {
			this->~Allocator();
			//在这里抛无法分配内存异常
		}

		idle_size = used_size = AllocatorSpace::AllocatorInternalArrayDefaultSize;
	}

	void destruct_info()noexcept {
		AllocatorSpace::destruct(idle);
		AllocatorSpace::destruct(used);
	}

private:
	//从环境申请的内存信息
	ChunkInfo entire;

	//储存空闲、已用内存信息的数组
	ChunkInfo *idle, *used;
	//数组长度
	DC::size_t idle_size, used_size;
};