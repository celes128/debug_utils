#pragma once

#include <cassert>
#include <vector>
#include <string>

namespace dbgutils {

	class ConsoleHistory {
	public:
		ConsoleHistory(size_t capacity)
			: m_capacity(capacity)
			, m_buf(capacity)
		{
			assert(capacity >= 1);
		}

		//		ACCESSORS
		//
		auto capacity() const { return m_capacity; }
		auto size() const { return m_size; }

		bool full() const
		{
			assert(m_size <= m_capacity);

			return m_size == m_capacity;
		}

		bool empty() const { return m_size == 0; }


		//		MANIPULATORS
		//
		void push(const std::string &line)
		{
			auto wasFull = full();

			m_buf[m_top] = line;

			increment_ptr(&m_top);

			if (wasFull) {
				m_bottom = m_top;
			}
			else {
				m_size++;
			}

			cancel_iteration();
		}

		std::string get() const
		{
			if (iterating()) {
				return m_buf[m_cur];
			}
			else {
				return "";
			}
		}

		// Iteration

		// RETURN VALUE
		//	Returns true iff the iteration pointer points to a new entry (this includes
		//	the case of starting the iteration).
		bool go_to_previous()
		{
			if (empty()) {
				return false;
			}

			if (!iterating()) {
				init_iteration();
				return true;
			}

			if (at_the_least_recent_entry()) {
				return false;
			}

			decrement_ptr(&m_cur);
			return true;
		}

		// RETURN VALUE
		//	Returns true iff the iteration pointer points to a new entry or the iteration ended.
		bool go_to_next()
		{
			if (empty()) {
				return false;
			}

			if (!iterating()) {
				return false;
			}

			if (at_the_most_recent_entry()) {
				cancel_iteration();
				return true;
			}

			increment_ptr(&m_cur);
			return true;
		}

	private:
		void cancel_iteration()
		{
			m_curIsDefined = false;
		}
		
		void init_iteration()
		{
			m_curIsDefined = true;

			m_cur = ptr_previous(m_top);
		}

		bool iterating() const
		{
			return m_curIsDefined;
		}

		bool at_the_least_recent_entry() const
		{
			return m_cur == m_bottom;
		}

		bool at_the_most_recent_entry() const
		{
			return m_cur == ptr_previous(m_top);				
		}

		void increment_ptr(size_t *i)
		{
			assert(i != nullptr);

			*i = ptr_next(*i);
		}

		void decrement_ptr(size_t *i)
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
		size_t		m_bottom{ 0 };
		size_t		m_top{ 0 };

		// Iteration ptr.
		bool		m_curIsDefined{ false };
		size_t		m_cur;

		// Underlying vector containig the strings.
		using StrBuffer = std::vector<std::string>;
		StrBuffer	m_buf;
	};
}
