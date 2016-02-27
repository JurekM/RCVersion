#pragma once
#include <locale>
#include <windows.h>

inline const char* LTrim(const char* text)
{
	while (0 != *text && isspace(*text))
		++text;
	return text;
}

inline char* LTrim(char* text)
{
	while (0 != *text && isspace(*text))
		++text;
	return text;
}

inline const wchar_t* LTrim(const wchar_t* text, const wchar_t* chaff)
{
	while (*text) {
		bool bTrimmed = false;
		for (LPCWSTR c = chaff; *c; ++c) {
			if (*text == *c) {
				bTrimmed = true;
				++text;
				break;
			}
		}
		if (!bTrimmed)
			break;
	}
	return text;
}

inline wchar_t* LTrim(wchar_t* text, const wchar_t* chaff)
{
	LPCWSTR ptr = LTrim(const_cast<LPCWSTR>(text), chaff);
	return const_cast<LPWSTR>(ptr);
}

inline const char* LTrim(const char* text, const char* chaff)
{
	while (*text) {
		bool bTrimmed = false;
		for (const char*  c = chaff; *c; ++c) {
			if (*text == *c) {
				bTrimmed = true;
				++text;
				break;
			}
		}
		if (!bTrimmed)
			break;
	}
	return text;
}

inline char* LTrim(char* text, const char* chaff)
{
	LPCSTR ptr = LTrim(const_cast<LPCSTR>(text), chaff);
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
	while (0 != *text && !isspace(*text))
		++text;
	return text;
}

inline char* SkipWord(char* text)
{
	while (0 != *text && !isspace(*text))
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
