#include "pch.h"
#include "string_utils.h"
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cctype>
#include <locale>

std::wstring wstr_concat(IN const std::vector<std::wstring> &strs, IN const std::wstring &sep)
{
	if (strs.size() == 0) {
		return L"";
	}

	std::wostringstream s;
	for (size_t i = 0; i < strs.size(); i++) {
		s << strs[i];

		if (i + 1 < strs.size()) {
			s << " ";
		}
	}

	return s.str();
}

bool wstr_find_substr(IN const std::wstring &str, IN const std::wstring &substr, IN size_t offset, OUT size_t *i)
{
	*i = str.find(substr, offset);
	return *i == std::wstring::npos ? false : true;
}

std::vector<std::wstring> wstr_split(IN const std::wstring &str)
{
	using namespace std;

	wistringstream iss(str);
	vector<wstring> tokens{ istream_iterator<wstring, wchar_t>{iss}, istream_iterator<wstring, wchar_t>{} };

	return tokens;
}

// trim from start (in place)
void wstr_ltrim(IN OUT std::wstring &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
}

// trim from end (in place)
void wstr_rtrim(IN OUT std::wstring &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

// trim from both ends (in place)
void wstr_trim(IN OUT std::wstring &s)
{
	wstr_ltrim(s);
	wstr_rtrim(s);
}
