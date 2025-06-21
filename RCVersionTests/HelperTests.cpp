#include "stdafx.h"
#include "TestLogger.h"
#include "Logger.h"
#include <MessageBuffer.h>

TEST(Logger, Normal)
{
  TestLogger tl{};
  Logger logger{tl};
  EXPECT_FALSE(logger.Error(11, L"Number=%d String=%s", 1234, L"test001"));
  EXPECT_NE(nullptr, wcsstr(tl.messages.c_str(), L"1234"));
  EXPECT_NE(nullptr, wcsstr(tl.messages.c_str(), L"test001"));
  logger.Log(0, L"Number=%d String=%s", 321, L"test02");
  EXPECT_NE(nullptr, wcsstr(tl.messages.c_str(), L"321"));
  EXPECT_NE(nullptr, wcsstr(tl.messages.c_str(), L"test02"));
}

TEST(MessageBuffer, SetClearSetClear)
{
   MessageBuffer mb{};
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
   MessageBuffer mb{};
   mb.set(L"abcde");
   EXPECT_STREQ(L"abcde", mb.message());
   mb.set("bcdefg");
   EXPECT_STREQ(L"bcdefg", mb.message());
}

TEST(MessageBuffer, AppendAppendClear)
{
   MessageBuffer mb{};
   mb.append(L"abcde");
   EXPECT_STREQ(L"abcde", mb.message());
   mb.append("bcdefg");
   EXPECT_STREQ(L"abcdebcdefg", mb.message());
   mb.clear();
   EXPECT_STREQ(L"", mb.message());
}

TEST(MessageBuffer, FormatFormat)
{
   MessageBuffer mb{};
   mb.format(L"Number=%d String=%hs", 123, "abcd");
   EXPECT_STREQ(L"Number=123 String=abcd", mb.message());
   mb.format(L" Number=%d String=%s", 234, L"pqrs");
   EXPECT_STREQ(L"Number=123 String=abcd Number=234 String=pqrs", mb.message());
}
