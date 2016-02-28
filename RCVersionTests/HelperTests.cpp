#include "stdafx.h"
#include "AutoFree.h"
#include "AutoHClose.h"
#include "TestLogger.h"
#include "Logger.h"
#include <MessageBuffer.h>
#include <trim.h>

TEST(AutoFree, NullNoCrash)
{
   void* ptr = nullptr;
   {
      ASSERT_EQ(nullptr, ptr);
      AutoFree af(ptr);
   }
}

TEST(AutoFree, Normal)
{
   void* ptr = malloc(100);
   {
      ASSERT_NE(nullptr, ptr);
      AutoFree af(ptr);
   }
}

TEST(AutoFree, ExplicitFree)
{
   void* ptr = malloc(100);
   {
      ASSERT_NE(nullptr, ptr);
      AutoFree af(ptr);
      af.Free();
   }
}

TEST(AutoHClose, NullNoCrash)
{
   HANDLE h = nullptr;
   {
      ASSERT_NE(INVALID_HANDLE_VALUE, h);
      AutoHClose ahc(h);
   }
}

TEST(AutoHClose, Normal)
{
   HANDLE h = CreateEvent(nullptr, false, false, nullptr);
   {
      ASSERT_NE(INVALID_HANDLE_VALUE, h);
      AutoHClose ahc(h);
   }
   ASSERT_EQ(0, CloseHandle(h));
   ASSERT_EQ(ERROR_INVALID_HANDLE, GetLastError());
}

TEST(AutoHClose, ExplicitClose)
{
   HANDLE h = CreateEvent(nullptr, false, false, nullptr);
   {
      ASSERT_NE(INVALID_HANDLE_VALUE, h);
      AutoHClose ahc(h);
      ahc.Close();
      ASSERT_EQ(0, CloseHandle(h));
      ASSERT_EQ(ERROR_INVALID_HANDLE, GetLastError());
   }
}

TEST(Logger, Normal)
{
   TestLogger tl;
   Logger logger(tl);
   EXPECT_FALSE(logger.Error(11, L"Number=%d String=%s", 1234, L"test001"));
   EXPECT_NE(nullptr, wcsstr(tl.messages.c_str(), L"1234"));
   EXPECT_NE(nullptr, wcsstr(tl.messages.c_str(), L"test001"));
   logger.Log(L"Number=%d String=%s", 321, L"test02");
   EXPECT_NE(nullptr, wcsstr(tl.messages.c_str(), L"321"));
   EXPECT_NE(nullptr, wcsstr(tl.messages.c_str(), L"test02"));
}

TEST(MessageBuffer, SetClearSetClear)
{
   MessageBuffer mb;
   mb.set(L"abcde");
   EXPECT_STREQ(L"abcde", mb.message());
   mb.clear();
   EXPECT_STREQ(L"", mb.message());
   mb.set("bcdefg");
   EXPECT_STREQ(L"bcdefg", mb.message());
   mb.clear();
   EXPECT_STREQ(L"", mb.message());
}

TEST(MessageBuffer, SetSetClear)
{
   MessageBuffer mb;
   mb.set(L"abcde");
   EXPECT_STREQ(L"abcde", mb.message());
   mb.set("bcdefg");
   EXPECT_STREQ(L"bcdefg", mb.message());
}

TEST(MessageBuffer, AppendAppendClear)
{
   MessageBuffer mb;
   mb.append(L"abcde");
   EXPECT_STREQ(L"abcde", mb.message());
   mb.append("bcdefg");
   EXPECT_STREQ(L"abcdebcdefg", mb.message());
   mb.clear();
   EXPECT_STREQ(L"", mb.message());
}

TEST(MessageBuffer, FormatFormat)
{
   MessageBuffer mb;
   mb.format(L"Number=%d String=%hs", 123, "abcd");
   EXPECT_STREQ(L"Number=123 String=abcd", mb.message());
   mb.format(L" Number=%d String=%s", 234, L"pqrs");
   EXPECT_STREQ(L"Number=123 String=abcd Number=234 String=pqrs", mb.message());
}

/*
inline char* LTrim(char* text)
inline char* LTrim(char* text, const char* chaff)
inline char* NextLine(char* text)
inline char* SkipString(char* text)
inline char* SkipTextItem(char* text)
inline char* SkipWord(char* text)

inline const char* LTrim(const char* text)
inline const char* LTrim(const char* text, const char* chaff)
inline const char* SkipString(const char* text)
inline const char* SkipWord(const char* text)
inline const wchar_t* LTrim(const wchar_t* text, const wchar_t* chaff)
inline wchar_t* LTrim(wchar_t* text, const wchar_t* chaff)
*/

TEST(LTrim, Char)
{
   //EXPECT_STREQ("", LTrim(""));
   //EXPECT_STREQ("abcd", LTrim("abcd"));
   //EXPECT_STREQ("abcd \t", LTrim(" \t /r /n \tabcd \t"));
   //EXPECT_STREQ("a bcd", LTrim(" a bcd"));

   //char s1[] = "";
   //EXPECT_STREQ("", LTrim(s1));

   //char s2[] = "abcd";
   //EXPECT_STREQ("abcd", LTrim(s2));

   //char s3[] = " \t /r /n \tabcd \t";
   //EXPECT_STREQ("abcd", LTrim(s3));

   //char s4[] = " a bcd";
   //EXPECT_STREQ("a bcd", LTrim(s4));
}

TEST(LTrim, Wchar)
{
   //EXPECT_STREQ(L"", LTrim(L""));
   //EXPECT_STREQ(L"abcd", LTrim(L"abcd"));
   //EXPECT_STREQ(L"abcd \t", LTrim(L" \t /r /n \tabcd \t"));
   //EXPECT_STREQ(L"a bcd", LTrim(L" a bcd"));

   //wchar_t s1[] = L"";
   //EXPECT_STREQ(L"", LTrim(s1));

   //wchar_t s2[] = L"abcd";
   //EXPECT_STREQ(L"abcd", LTrim(s2));

   //wchar_t s3[] = L" \t /r /n \tabcd \t";
   //EXPECT_STREQ(L"abcd", LTrim(s3));

   //wchar_t s4[] = L" a bcd";
   //EXPECT_STREQ(L"a bcd", LTrim(s4));
}
