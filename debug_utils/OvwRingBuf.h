#pragma once

#include <vector>
#include <cassert>

namespace dbgutils {

	//							OVERWRITING RING BUFFER
	//
	template <class T>
	class OvwRingBuf {
	public:
		OvwRingBuf(size_t capacity)
			: m_capacity(capacity)
		{
			assert(capacity >= 1);
		}

		//				ACCESSORS
		//

		// size returns the number of items in the buffer.
		auto size() const { return m_size; }

		// capacity returns the maximum number of items
		// that can be stored in the buffer.
		auto capacity() const { return m_capacity; }

		bool empty() const { return 0 == m_size; }

		bool full() const
		{
			assert(m_size <= m_capacity);

			return m_size == m_capacity;
		}

		//				ACCESSORS
		//

		// push_back inserts an item at the back of the buffer.
		// If the buffer was full before the call, the front item is overwritten.
		void push_back(const T &item)
		{
			auto wasFull = full();

			// Insert
			if (m_back < m_buf.size()) {
				m_buf[m_back] = item;
			}
			else {
				m_buf.push_back(item);
			}
			increment_ptr(&m_back);

			if (wasFull) {
				m_front = m_back;
			}
			else {
				m_size++;
			}
		}

		// pop_front removes and returns the item at the front of the buffer.
		// The buffer must not be empty.
		T pop_front()
		{
			assert(!empty());

			increment_ptr(&m_front);
			m_size--;

			return m_buf[ptr_previous(m_front)];
		}

		// peek inspects the item at a given index.
		// An index of 0 corresponds to the item at the front.
		// An index of size() - 1 corresponds to the item at the back,
		// under the condition that the buffer is not empty.
		const T &peek(size_t i) const
		{
			assert(!empty());
			assert(0 <= i && i < size());// redondant with the above but whatever, it's only in the debug version.

			auto k = m_front;
			for (size_t j = 0; j < i; j++) {
				increment_ptr(&k);
			}

			return m_buf[k];
		}

	private:
		//		Update functions for the top and bottom pointers.
		//
		void increment_ptr(size_t *i) const
		{
			assert(i != nullptr);

			*i = ptr_next(*i);
		}

		void decrement_ptr(size_t *i) const
		{
			assert(i != nullptr);

			*i = ptr_previous(*i);
		}

		size_t ptr_previous(size_t i) const
		{
			assert(i < m_capacity);

			if (i != 0) {
				return i - 1;
			}
			else {
				return m_capacity - 1;
			}
		}

		size_t ptr_next(size_t i) const
		{
			assert(i < m_capacity);

			if (i + 1 < m_capacity) {
				return i + 1;
			}
			else {
				return 0;
			}
		}

	private:
		// Maximum number of elements.
		size_t		m_capacity;

		// Number of elements.
		size_t		m_size{ 0 };

		// Ring buffer ptrs
		size_t		m_front{ 0 };
		size_t		m_back{ 0 };

		// Underlying vector containing the items.
		std::vector<T>	m_buf;
	};
}