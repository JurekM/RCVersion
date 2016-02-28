#pragma once
#include <locale>

inline const char* LTrim(const char* text)
{
	while (0 != *text && isspace(unsigned char(*text)))
		++text;
	return text;
}

inline char* LTrim(char* text)
{
	while (0 != *text && isspace(unsigned char(*text)))
		++text;
	return text;
}

inline LPCWSTR LTrim(LPCWSTR psz, LPCWSTR chaff)
{
	while (*psz) {
		bool bTrimmed = false;
		for (LPCWSTR c = chaff; *c; ++c) {
			if (*psz == *c) {
				bTrimmed = true;
				++psz;
				break;
			}
		}
		if (!bTrimmed)
			break;
	}
	return psz;
}

inline LPWSTR LTrim(LPWSTR psz, LPCWSTR chaff)
{
	LPCWSTR ptr = LTrim(const_cast<LPCWSTR>(psz), chaff);
	return const_cast<LPWSTR>(ptr);
}

inline LPCSTR LTrim(LPCSTR psz, LPCSTR chaff)
{
	while (*psz) {
		bool bTrimmed = false;
		for (LPCSTR c = chaff; *c; ++c) {
			if (*psz == *c) {
				bTrimmed = true;
				++psz;
				break;
			}
		}
		if (!bTrimmed)
			break;
	}
	return psz;
}

inline LPSTR LTrim(LPSTR psz, LPCSTR chaff)
{
	LPCSTR ptr = LTrim(const_cast<LPCSTR>(psz), chaff);
	return const_cast<LPSTR>(ptr);
}


inline const char* SkipString(const char* text)
{
	if ('\"' != *text)
		return text;
	++text;
	while (0 != *text && '\"' != *text)
		++text;
	if ('\"' == *text)
		++text;
	return text;
}

inline char* SkipString(char* text)
{
	if ('\"' != *text)
		return text;
	++text;
	while (0 != *text && '\"' != *text)
		++text;
	if ('\"' == *text)
		++text;
	return text;
}

inline const char* SkipWord(const char* text)
{
	while (0 != *text && !isspace(unsigned char(*text)))
		++text;
	return text;
}

inline char* SkipWord(char* text)
{
	while (0 != *text && !isspace(unsigned char(*text)))
		++text;
	return text;
}

inline char* SkipTextItem(char* text)
{
	return ('"' == *text) ? SkipString(text) : SkipWord(text);
}

inline char* NextLine(char* text)
{
	while (*text && '\n' != *text)
		++text;
	return *text ? ++text : text;
}
