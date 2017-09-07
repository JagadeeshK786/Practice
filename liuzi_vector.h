#pragma once
#include <initializer_list>
#include <memory>
#include <exception>

namespace DC {

	template <typename _Ty>
	class vector final {
		using size_type = unsigned int;
		using value_type = _Ty;

	public:
		constexpr vector() {}

		vector(const size_type& reserve_size) {
			void* mem = malloc(sizeof(value_type)*reserve_size);

			if (mem == 0)
				throw std::bad_alloc();

			m_array = reinterpret_cast<_Ty*>(mem);
			m_capacity = reserve_size;
		}

		vector(const std::initializer_list<value_type>& list) {
			void* mem = malloc(sizeof(value_type)*list.size());

			if (mem == 0)
				throw std::bad_alloc();

			m_array = reinterpret_cast<_Ty*>(mem);
			m_capacity = list.size();

			size_type index = 0;
			for (const auto& it : list) {
				new(&m_array[index]) value_type(it);
				index++;
			}

			m_size = index;
		}

		~vector()noexcept {
			if (m_array == 0)
				return;

			for (size_type i = 0; i < m_size; i++)
				m_array[i].~value_type();

			free(m_array);
		}

	public:
		void reserve(const size_type& new_cap) {
			if (new_cap == m_capacity)
				return;

		}

		void resize() {}

		void push_back() {}

		void emplace_back() {}

		void erase() {}

		void begin() {}

		void end() {}

		constexpr size_type size()const {
			return m_size;
		}

		constexpr size_type capacity()const {
			return m_capacity;
		}

	private:
		_Ty* m_array{ nullptr };
		size_type m_size{ 0 }, m_capacity{ 0 };
	};

}