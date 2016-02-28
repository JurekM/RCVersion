#include "stdafx.h"
#include "RCUpdater.h"
#include "TestLogger.h"
#include <RCUpdater.h>


TEST(RCUpdater, LTrimChar)
{
char s1[] = "";
EXPECT_STREQ("", RCUpdater<char>::LTrim(s1,""));
EXPECT_STREQ("", RCUpdater<char>::LTrim(s1, " "));
EXPECT_STREQ("", RCUpdater<char>::LTrim(s1, " \t\r\n"));
EXPECT_STREQ("", RCUpdater<char>::LTrim(s1, "., \t\r\n"));

char s2[] = "abcd";
EXPECT_STREQ("abcd", RCUpdater<char>::LTrim(s2, ""));
EXPECT_STREQ("abcd", RCUpdater<char>::LTrim(s2, " "));
EXPECT_STREQ("abcd", RCUpdater<char>::LTrim(s2, " \t\r\n"));
EXPECT_STREQ("abcd", RCUpdater<char>::LTrim(s2, "., \t\r\n"));

char s3[] = " \t \r \n .\tabcd \t";
EXPECT_STREQ(" \t \r \n .\tabcd \t", RCUpdater<char>::LTrim(s3, ""));
EXPECT_STREQ("\t \r \n .\tabcd \t", RCUpdater<char>::LTrim(s3, " "));
EXPECT_STREQ(".\tabcd \t", RCUpdater<char>::LTrim(s3, " \t\r\n"));
EXPECT_STREQ("abcd \t", RCUpdater<char>::LTrim(s3, "., \t\r\n"));
}

TEST(RCUpdater, LTrimWchar)
{
wchar_t s1[] = L"";
EXPECT_STREQ(L"", RCUpdater<wchar_t>::LTrim(s1, L""));
EXPECT_STREQ(L"", RCUpdater<wchar_t>::LTrim(s1, L" "));
EXPECT_STREQ(L"", RCUpdater<wchar_t>::LTrim(s1, L" \t\r\n"));
EXPECT_STREQ(L"", RCUpdater<wchar_t>::LTrim(s1, L"., \t\r\n"));

wchar_t s2[] = L"abcd";
EXPECT_STREQ(L"abcd", RCUpdater<wchar_t>::LTrim(s2, L""));
EXPECT_STREQ(L"abcd", RCUpdater<wchar_t>::LTrim(s2, L" "));
EXPECT_STREQ(L"abcd", RCUpdater<wchar_t>::LTrim(s2, L" \t\r\n"));
EXPECT_STREQ(L"abcd", RCUpdater<wchar_t>::LTrim(s2, L"., \t\r\n"));

wchar_t s3[] = L" \t \r \n .\tabcd \t";
EXPECT_STREQ(L" \t \r \n .\tabcd \t", RCUpdater<wchar_t>::LTrim(s3, L""));
EXPECT_STREQ(L"\t \r \n .\tabcd \t", RCUpdater<wchar_t>::LTrim(s3, L" "));
EXPECT_STREQ(L".\tabcd \t", RCUpdater<wchar_t>::LTrim(s3, L" \t\r\n"));
EXPECT_STREQ(L"abcd \t", RCUpdater<wchar_t>::LTrim(s3, L"., \t\r\n"));
}
