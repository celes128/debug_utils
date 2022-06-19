#pragma once

namespace dbgutils {

	template <class T>
	class Range {
	public:
		Range(const T &begin, const T &end)
			: m_begin(begin)
			, m_end(end)
		{}

		T length() const { return m_end - m_begin; }
		T begin() const { return m_begin; }
		T end() const { return m_end; }
		bool empty() const { return m_begin >= m_end; }

	private:
		T	m_begin;
		T	m_end;
	};
}