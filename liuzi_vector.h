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
			if (new_cap <= m_capacity)
				return;

			value_type* mem = reinterpret_cast<value_type*>(malloc(sizeof(value_type)*new_cap));

			if (mem == 0)
				throw std::bad_alloc();

			for (size_type i = 0; i < this->m_size; i++) {
				new(&mem[i]) value_type(std::move(this->m_array[i]));
				this->m_array[i].~value_type();
			}

			free(this->m_array);

			this->m_array = mem;
			this->m_capacity = new_cap;
		}

		void resize(const size_type& new_size) {
			if (new_size == this->m_size)
				return;

			if (new_size > this->m_size) {
				if (new_size > this->m_capacity)
					this->reserve(new_size);

				for (size_type i = this->m_size; i < new_size; i++)
					new(&this->m_array[i]) value_type();

				this->m_size = new_size;

				return;
			}

			if (new_size < this->m_size) {
				if (new_size == 0) {
					for (size_type i = 0; i < this->m_size; i++)
						this->m_array[i].~value_type();
				}
				else {
					for (size_type i = this->m_size - 1; i >= new_size; i--)
						this->m_array[i].~value_type();
				}

				this->m_size = new_size;

				return;
			}
		}

		void resize(const size_type& new_size, const value_type& val) {
			if (new_size == this->m_size)
				return;

			if (new_size > this->m_size) {
				if (new_size > this->m_capacity)
					this->reserve(new_size);

				for (size_type i = this->m_size; i < new_size; i++)
					new(&this->m_array[i]) value_type(val);

				this->m_size = new_size;

				return;
			}

			if (new_size < this->m_size) {
				if (new_size == 0) {
					for (size_type i = 0; i < this->m_size; i++)
						this->m_array[i].~value_type();
				}
				else {
					for (size_type i = this->m_size - 1; i >= new_size; i--)
						this->m_array[i].~value_type();
				}

				this->m_size = new_size;

				return;
			}
		}

		template <typename ValueType>
		void push_back(ValueType&& val) {
			if (this->m_size == this->m_capacity)
				this->reserve(this->m_capacity * 2);

			new(&this->m_array[this->m_size]) value_type(std::forward<ValueType>(val));

			this->m_size++;
		}

		template <typename ...ARGS>
		void emplace_back(ARGS&& ...args) {
			if (this->m_size == this->m_capacity)
				this->reserve(this->m_capacity * 2);

			new(&this->m_array[this->m_size]) value_type(args...);

			this->m_size++;
		}

		void erase() {}

		void begin() {}

		void end() {}

		constexpr size_type size()const noexcept {
			return m_size;
		}

		constexpr size_type capacity()const noexcept {
			return m_capacity;
		}

	private:
		_Ty* m_array{ nullptr };
		size_type m_size{ 0 }, m_capacity{ 0 };
	};

}