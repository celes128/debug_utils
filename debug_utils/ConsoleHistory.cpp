#include "pch.h"
#include "ConsoleHistory.h"

namespace dbgutils {

	void ConsoleHistory::push(const std::string &line)
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

		reset_iteration();
	}

	std::string ConsoleHistory::get() const
	{
		if (it_inside_stack()) {
			return m_buf[m_it];
		}
		else {
			return "";
		}
	}

	ConsoleHistory::ITEREVENT ConsoleHistory::go_to_previous()
	{
		if (empty()) {
			return ITEREVENT_CANNOT_ITERATE;
		}

		if (!it_inside_stack()) {
			point_at_the_latest_entry();
			return ITEREVENT_AT_NEW_ENTRY;
		}

		if (at_the_oldest_entry()) {
			return ITEREVENT_ALREADY_AT_OLDEST;
		}

		decrement_ptr(&m_it);
		return ITEREVENT_AT_NEW_ENTRY;
	}

	ConsoleHistory::ITEREVENT ConsoleHistory::go_to_next()
	{
		if (empty()) {
			return ITEREVENT_CANNOT_ITERATE;
		}

		if (!it_inside_stack()) {
			return ITEREVENT_ALREADY_OUTSIDE;
		}

		if (at_the_most_recent_entry()) {
			reset_iteration();
			return ITEREVENT_LEAVING_HISTORY;
		}

		increment_ptr(&m_it);
		return ITEREVENT_AT_NEW_ENTRY;
	}
}