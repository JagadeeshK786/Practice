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
		//����Ѿ�������ˣ�����false
		if (this->has_memory()) return false;

		if ((entire.ptr = malloc(_Size)) == NULL) {
			//����ʧ��
			entire.ptr = nullptr;
			return false;
		}

		//����ɹ�
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
	//�ӻ���������ڴ���Ϣ
	ChunkInfo entire;

	//������С������ڴ���Ϣ������
	ChunkInfo *idle, *used;
	//���鳤��
	DC::size_t idle_size, used_size;
};