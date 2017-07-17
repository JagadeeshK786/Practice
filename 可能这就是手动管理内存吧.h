#pragma once
#include <stdlib.h>
#include <memory.h>

namespace DC {
	using byte_t = unsigned char;
	using size_t = unsigned int;
}

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