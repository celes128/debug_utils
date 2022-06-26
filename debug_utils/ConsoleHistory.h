#pragma once

#include <cassert>
#include <vector>
#include <string>

namespace dbgutils {

	//	
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
		// capacity returns the maximum number of entries that can
		// be stored in the history.
		auto capacity() const { return m_capacity; }

		// size returns the current number of entries in the history.
		auto size() const { return m_size; }

		bool full() const
		{
			assert(m_size <= m_capacity);

			return m_size == m_capacity;
		}

		bool empty() const { return m_size == 0; }


		//		MANIPULATORS
		//

		// push inserts a new entry in the history, which becomes the latest one, and
		// resets the iteration.
		void push(const std::wstring &line);

		// get returns an entry pointed by the iteration ptr if it is defined.
		// If it is undefined, get returns the empty string.
		std::wstring get() const;

		// ITEREVENTs are returned by the two iteration functions
		// go_to_previous() and got_to_next().
		enum ITEREVENT {
			ITEREVENT_AT_NEW_ENTRY,		// the iteration ptr moved from one entry to another one
			ITEREVENT_ALREADY_AT_OLDEST,// the iteration ptr was already at the oldest entry
			ITEREVENT_ALREADY_OUTSIDE,	// the iteration ptr was already undefined
			ITEREVENT_LEAVING_HISTORY,	// the iteration ptr was at the latest entry and became undefined
			ITEREVENT_CANNOT_ITERATE	// because the history is empty
		};

		// go_to_previous moves the iteration ptr towards the latest entries by one position.
		// If the ptr was pointing at the latest entry before the call, then the call will
		// terminate the iteration and mark the ptr as undefined.
		ITEREVENT go_to_previous();
		
		// go_to_next moves the iteration ptr towards the oldest entries by one position.
		// It the ptr was pointing at the oldest entry before the call, then the call will
		// do nothing, leaving the ptr on the oldest entry.
		ITEREVENT go_to_next();

		// reset_iteration ends the iteration, marking the iteration ptr as undefined.
		//
		// REMARKS
		//	Calling got_to_prev after reset_iteration will make the iteration ptr
		//	point to the latest entry (if the history is not empty).
		//
		// SAMPLE CODE
		//
		//	// This is how you make sure the iteration ptr points to the most recent entry
		//	// under the condition that the history is not empty.
		//	history.reset_iteration();
		//	history.go_to_prev();
		//
		void reset_iteration()
		{
			m_iterationPtrInsideStack = false;
		}
		
	private:
		//		Iteration ptr functions
		//
		void point_at_the_latest_entry()
		{
			assert(!empty());

			m_iterationPtrInsideStack = true;
			m_it = ptr_previous(m_top);
		}

		bool it_inside_stack() const
		{
			return m_iterationPtrInsideStack;
		}

		bool at_the_oldest_entry() const
		{
			return m_it == m_bottom;
		}

		bool at_the_most_recent_entry() const
		{
			return m_it == ptr_previous(m_top);				
		}


		//		Stack pointer update functions (top, bottom and it pointers)
		//
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
		size_t		m_bottom{ 0 };// points at the oldest entry (if the history is not empty)
		size_t		m_top{ 0 };// points at the empty slot right above the top-most entry

		// Iteration ptr.
		bool		m_iterationPtrInsideStack{ false };// is the iteration ptr defined?
		size_t		m_it;

		// Underlying vector containing the entries.
		std::vector<std::wstring>	m_buf;
	};
}
