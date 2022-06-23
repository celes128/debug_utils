#pragma once

#include <string>
#include <vector>

#define IN
#define OUT
#define OPTIONAL

std::wstring wstr_concat(
	IN const std::vector<std::wstring> &strs,
	IN const std::wstring &sep = L""
);

bool wstr_find_substr(
	IN const std::wstring &str,
	IN const std::wstring &substr,
	IN size_t offset,
	OUT size_t *i
);

std::vector<std::wstring> wstr_split(IN const std::wstring &str);

// trim from start (in place)
void wstr_ltrim(IN OUT std::wstring &s);

// trim from end (in place)
void wstr_rtrim(IN OUT std::wstring &s);

// trim from both ends (in place)
void wstr_trim(IN OUT std::wstring &s);
