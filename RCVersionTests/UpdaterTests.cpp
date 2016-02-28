#include "stdafx.h"
#include "RCUpdater.h"
#include "TestLogger.h"
#include <RCUpdater.h>


TEST(RCUpdater, LTrimChar)
{
   char s1[] = "";
   EXPECT_STREQ("", RCUpdater<char>::LTrim(s1, ""));
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

TEST(RCUpdater, LSkipToChar)
{
   char s1[] = "";
   EXPECT_STREQ("", RCUpdater<char>::LSkipTo(s1, ""));
   EXPECT_STREQ("", RCUpdater<char>::LSkipTo(s1, " "));
   EXPECT_STREQ("", RCUpdater<char>::LSkipTo(s1, " \t\r\n"));
   EXPECT_STREQ("", RCUpdater<char>::LSkipTo(s1, "., \t\r\n"));

   char s2[] = " abcd";
   EXPECT_STREQ("", RCUpdater<char>::LSkipTo(s2, ""));
   EXPECT_STREQ(" abcd", RCUpdater<char>::LSkipTo(s2, " "));
   EXPECT_STREQ(" abcd", RCUpdater<char>::LSkipTo(s2, " \t\r\n"));
   EXPECT_STREQ(" abcd", RCUpdater<char>::LSkipTo(s2, "., \t\r\n"));
   EXPECT_STREQ("", RCUpdater<char>::LSkipTo(s2, ".,\t\r\n"));

   char s3[] = "abcd .,\t\r\n";
   EXPECT_STREQ("", RCUpdater<char>::LSkipTo(s3, ""));
   EXPECT_STREQ(" .,\t\r\n", RCUpdater<char>::LSkipTo(s3, " "));
   EXPECT_STREQ("\t\r\n", RCUpdater<char>::LSkipTo(s3, "\t\r\n"));
   EXPECT_STREQ(".,\t\r\n", RCUpdater<char>::LSkipTo(s3, ".,\t\r\n"));
   EXPECT_STREQ("", RCUpdater<char>::LSkipTo(s3, "xyz\a"));

   char s4[] = " \t \r \n .\tabcd \t";
   EXPECT_STREQ("", RCUpdater<char>::LSkipTo(s4, ""));
   EXPECT_STREQ(" \t \r \n .\tabcd \t", RCUpdater<char>::LSkipTo(s4, " \t\r\n"));
   EXPECT_STREQ("\t \r \n .\tabcd \t", RCUpdater<char>::LSkipTo(s4, "\t\r\n"));
   EXPECT_STREQ("\r \n .\tabcd \t", RCUpdater<char>::LSkipTo(s4, "\r\n"));
   EXPECT_STREQ("\r \n .\tabcd \t", RCUpdater<char>::LSkipTo(s4, "\n\r"));
   EXPECT_STREQ("", RCUpdater<char>::LSkipTo(s4, "xyz"));
   EXPECT_STREQ("abcd \t", RCUpdater<char>::LSkipTo(s4, "asf"));
}

TEST(RCUpdater, LSkipToWChar)
{
   wchar_t s1[] = L"";
   EXPECT_STREQ(L"", RCUpdater<wchar_t>::LSkipTo(s1, L""));
   EXPECT_STREQ(L"", RCUpdater<wchar_t>::LSkipTo(s1, L" "));
   EXPECT_STREQ(L"", RCUpdater<wchar_t>::LSkipTo(s1, L" \t\r\n"));
   EXPECT_STREQ(L"", RCUpdater<wchar_t>::LSkipTo(s1, L"., \t\r\n"));

   wchar_t s2[] = L" abcd";
   EXPECT_STREQ(L"", RCUpdater<wchar_t>::LSkipTo(s2, L""));
   EXPECT_STREQ(L" abcd", RCUpdater<wchar_t>::LSkipTo(s2, L" "));
   EXPECT_STREQ(L" abcd", RCUpdater<wchar_t>::LSkipTo(s2, L" \t\r\n"));
   EXPECT_STREQ(L" abcd", RCUpdater<wchar_t>::LSkipTo(s2, L"., \t\r\n"));
   EXPECT_STREQ(L"", RCUpdater<wchar_t>::LSkipTo(s2, L".,\t\r\n"));

   wchar_t s3[] = L"abcd .,\t\r\n";
   EXPECT_STREQ(L"", RCUpdater<wchar_t>::LSkipTo(s3, L""));
   EXPECT_STREQ(L" .,\t\r\n", RCUpdater<wchar_t>::LSkipTo(s3, L" "));
   EXPECT_STREQ(L"\t\r\n", RCUpdater<wchar_t>::LSkipTo(s3, L"\t\r\n"));
   EXPECT_STREQ(L".,\t\r\n", RCUpdater<wchar_t>::LSkipTo(s3, L".,\t\r\n"));
   EXPECT_STREQ(L"", RCUpdater<wchar_t>::LSkipTo(s3, L"xyz\a"));

   wchar_t s4[] = L" \t \r \n .\tabcd \t";
   EXPECT_STREQ(L"", RCUpdater<wchar_t>::LSkipTo(s4, L""));
   EXPECT_STREQ(L" \t \r \n .\tabcd \t", RCUpdater<wchar_t>::LSkipTo(s4, L" \t\r\n"));
   EXPECT_STREQ(L"\t \r \n .\tabcd \t", RCUpdater<wchar_t>::LSkipTo(s4, L"\t\r\n"));
   EXPECT_STREQ(L"\r \n .\tabcd \t", RCUpdater<wchar_t>::LSkipTo(s4, L"\r\n"));
   EXPECT_STREQ(L"\r \n .\tabcd \t", RCUpdater<wchar_t>::LSkipTo(s4, L"\n\r"));
   EXPECT_STREQ(L"", RCUpdater<wchar_t>::LSkipTo(s4, L"xyz"));
   EXPECT_STREQ(L"abcd \t", RCUpdater<wchar_t>::LSkipTo(s4, L"asf"));
}

TEST(RCUpdater, NextLineChar)
{
   char s1[] = "";
   EXPECT_STREQ("", RCUpdater<char>::NextLine(s1));

   char s2[] = " jkh jkl klj";
   EXPECT_STREQ("", RCUpdater<char>::NextLine(s2));

   char s3[] = "\n";
   EXPECT_STREQ("", RCUpdater<char>::NextLine(s3));

   char s4[] = "\r\n";
   EXPECT_STREQ("", RCUpdater<char>::NextLine(s4));

   char s5[] = "\r\n\n";
   EXPECT_STREQ("\n", RCUpdater<char>::NextLine(s5));

   char s6[] = "\r\n\r\n";
   EXPECT_STREQ("\r\n", RCUpdater<char>::NextLine(s6));

   char s7[] = "\r\ndef";
   EXPECT_STREQ("def", RCUpdater<char>::NextLine(s7));

   char s8[] = "abc\r\ndef";
   EXPECT_STREQ("def", RCUpdater<char>::NextLine(s8));

   char s9[] = "abc\r\ndef\r\nghi";
   EXPECT_STREQ("def\r\nghi", RCUpdater<char>::NextLine(s9));
   EXPECT_STREQ("ghi", RCUpdater<char>::NextLine(RCUpdater<char>::NextLine(s9)));
}

TEST(RCUpdater, NextLineWchar)
{
   wchar_t s1[] = L"";
   EXPECT_STREQ(L"", RCUpdater<wchar_t>::NextLine(s1));

   wchar_t s2[] = L" jkh jkl klj";
   EXPECT_STREQ(L"", RCUpdater<wchar_t>::NextLine(s2));

   wchar_t s3[] = L"\n";
   EXPECT_STREQ(L"", RCUpdater<wchar_t>::NextLine(s3));

   wchar_t s4[] = L"\r\n";
   EXPECT_STREQ(L"", RCUpdater<wchar_t>::NextLine(s4));

   wchar_t s5[] = L"\r\n\n";
   EXPECT_STREQ(L"\n", RCUpdater<wchar_t>::NextLine(s5));

   wchar_t s6[] = L"\r\n\r\n";
   EXPECT_STREQ(L"\r\n", RCUpdater<wchar_t>::NextLine(s6));

   wchar_t s7[] = L"\r\ndef";
   EXPECT_STREQ(L"def", RCUpdater<wchar_t>::NextLine(s7));

   wchar_t s8[] = L"abc\r\ndef";
   EXPECT_STREQ(L"def", RCUpdater<wchar_t>::NextLine(s8));

   wchar_t s9[] = L"abc\r\ndef\r\nghi";
   EXPECT_STREQ(L"def\r\nghi", RCUpdater<wchar_t>::NextLine(s9));
   EXPECT_STREQ(L"ghi", RCUpdater<wchar_t>::NextLine(RCUpdater<wchar_t>::NextLine(s9)));
}

TEST(RCUpdater, str2intCharNoChaff)
{
   char* ptr = nullptr;
   int value = -1;

   char s1[] = "";
   EXPECT_FALSE(RCUpdater<char>::str2int(ptr = s1, value, nullptr));
   EXPECT_EQ(ptr, s1);

   char s2[] = " 1234 ., 956";
   EXPECT_FALSE(RCUpdater<char>::str2int(ptr = s2, value, nullptr));
   EXPECT_EQ(ptr, s2);

   EXPECT_TRUE(RCUpdater<char>::str2int(ptr = s2 + 1, value, nullptr));
   EXPECT_STREQ(ptr, " ., 956");
   EXPECT_EQ(1234, value);

   EXPECT_TRUE(RCUpdater<char>::str2int(ptr = s2 + 2, value, ""));
   EXPECT_STREQ(ptr, " ., 956");
   EXPECT_EQ(234, value);

   EXPECT_TRUE(RCUpdater<char>::str2int(ptr = s2 + 3, value, ""));
   EXPECT_STREQ(ptr, " ., 956");
   EXPECT_EQ(34, value);

   EXPECT_TRUE(RCUpdater<char>::str2int(ptr = s2 + 4, value, nullptr));
   EXPECT_STREQ(ptr, " ., 956");
   EXPECT_EQ(4, value);

   EXPECT_FALSE(RCUpdater<char>::str2int(ptr = s2 + 5, value, ""));
   EXPECT_STREQ(ptr, " ., 956");
   EXPECT_EQ(4, value);
}

TEST(RCUpdater, str2intCharWithChaff)
{
   char* ptr = nullptr;
   int value = -1;

   char s1[] = "";
   EXPECT_FALSE(RCUpdater<char>::str2int(ptr = s1, value, " .,"));
   EXPECT_EQ(ptr, s1);

   char s2[] = " 1234 ., 956";
   EXPECT_FALSE(RCUpdater<char>::str2int(ptr = s2, value, " .,"));
   EXPECT_EQ(ptr, s2);

   EXPECT_TRUE(RCUpdater<char>::str2int(ptr = s2 + 1, value, " .,"));
   EXPECT_STREQ(ptr, "956");
   EXPECT_EQ(1234, value);

   EXPECT_TRUE(RCUpdater<char>::str2int(ptr = s2 + 2, value, "\a .,"));
   EXPECT_STREQ(ptr, "956");
   EXPECT_EQ(234, value);

   EXPECT_TRUE(RCUpdater<char>::str2int(ptr = s2 + 3, value, " ,.\t"));
   EXPECT_STREQ(ptr, "956");
   EXPECT_EQ(34, value);

   EXPECT_TRUE(RCUpdater<char>::str2int(ptr = s2 + 4, value, " ,."));
   EXPECT_STREQ(ptr, "956");
   EXPECT_EQ(4, value);

   EXPECT_FALSE(RCUpdater<char>::str2int(ptr = s2 + 5, value, " .,"));
   EXPECT_STREQ(ptr, " ., 956");
   EXPECT_EQ(4, value);
}

TEST(RCUpdater, str2intWcharNoChaff)
{
   wchar_t* ptr = nullptr;
   int value = -1;

   wchar_t s1[] = L"";
   EXPECT_FALSE(RCUpdater<wchar_t>::str2int(ptr = s1, value, nullptr));
   EXPECT_EQ(ptr, s1);

   wchar_t s2[] = L" 1234 ., 956";
   EXPECT_FALSE(RCUpdater<wchar_t>::str2int(ptr = s2, value, nullptr));
   EXPECT_EQ(ptr, s2);

   EXPECT_TRUE(RCUpdater<wchar_t>::str2int(ptr = s2 + 1, value, nullptr));
   EXPECT_STREQ(ptr, L" ., 956");
   EXPECT_EQ(1234, value);

   EXPECT_TRUE(RCUpdater<wchar_t>::str2int(ptr = s2 + 2, value, L""));
   EXPECT_STREQ(ptr, L" ., 956");
   EXPECT_EQ(234, value);

   EXPECT_TRUE(RCUpdater<wchar_t>::str2int(ptr = s2 + 3, value, L""));
   EXPECT_STREQ(ptr, L" ., 956");
   EXPECT_EQ(34, value);

   EXPECT_TRUE(RCUpdater<wchar_t>::str2int(ptr = s2 + 4, value, nullptr));
   EXPECT_STREQ(ptr, L" ., 956");
   EXPECT_EQ(4, value);

   EXPECT_FALSE(RCUpdater<wchar_t>::str2int(ptr = s2 + 5, value, L""));
   EXPECT_STREQ(ptr, L" ., 956");
   EXPECT_EQ(4, value);
}

TEST(RCUpdater, str2intWcharWithChaff)
{
   wchar_t* ptr = nullptr;
   int value = -1;

   wchar_t s1[] = L"";
   EXPECT_FALSE(RCUpdater<wchar_t>::str2int(ptr = s1, value, L" .,"));
   EXPECT_EQ(ptr, s1);

   wchar_t s2[] = L" 1234 ., 956";
   EXPECT_FALSE(RCUpdater<wchar_t>::str2int(ptr = s2, value, L" .,"));
   EXPECT_EQ(ptr, s2);

   EXPECT_TRUE(RCUpdater<wchar_t>::str2int(ptr = s2 + 1, value, L" .,"));
   EXPECT_STREQ(ptr, L"956");
   EXPECT_EQ(1234, value);

   EXPECT_TRUE(RCUpdater<wchar_t>::str2int(ptr = s2 + 2, value, L"\a .,"));
   EXPECT_STREQ(ptr, L"956");
   EXPECT_EQ(234, value);

   EXPECT_TRUE(RCUpdater<wchar_t>::str2int(ptr = s2 + 3, value, L" ,.\t"));
   EXPECT_STREQ(ptr, L"956");
   EXPECT_EQ(34, value);

   EXPECT_TRUE(RCUpdater<wchar_t>::str2int(ptr = s2 + 4, value, L" ,."));
   EXPECT_STREQ(ptr, L"956");
   EXPECT_EQ(4, value);

   EXPECT_FALSE(RCUpdater<wchar_t>::str2int(ptr = s2 + 5, value, L" .,"));
   EXPECT_STREQ(ptr, L" ., 956");
   EXPECT_EQ(4, value);
}

TEST(RCUpdater, formatChar)
{
   char buffer[32] = { 0 };

   EXPECT_TRUE(RCUpdater<char>::format(buffer, _countof(buffer), 1, 22, 333, 444));
   EXPECT_STREQ("1, 22, 333, 444", buffer);

   buffer[14] = '\xCD';
   buffer[15] = '\xCE';
   buffer[16] = '\xCF';
   buffer[17] = '\x00';
   EXPECT_TRUE(RCUpdater<char>::format(buffer, 16, 1, 22, 333, 444));
   EXPECT_STREQ("1, 22, 333, 444", buffer);
   EXPECT_EQ('\x00', buffer[15]);
   EXPECT_EQ('\xCF', buffer[16]);

   memset(buffer, 0xBE, sizeof(buffer));
   buffer[14] = '\xCD';
   buffer[15] = '\xCE';
   buffer[16] = '\xCF';
   buffer[17] = '\x00';
   EXPECT_EQ(17, strlen(buffer));
   EXPECT_FALSE(RCUpdater<char>::format(buffer, 15, 1, 22, 333, 444));
   EXPECT_GT(15, strlen(buffer));
   EXPECT_EQ('\xCE', buffer[15]);
   EXPECT_EQ('\xCF', buffer[16]);
}

TEST(RCUpdater, formatWchar)
{
   wchar_t buffer[32] = { 0 };

   EXPECT_TRUE(RCUpdater<wchar_t>::format(buffer, _countof(buffer), 1, 22, 333, 444));
   EXPECT_STREQ(L"1, 22, 333, 444", buffer);

   buffer[14] = L'\xCD';
   buffer[15] = L'\xCE';
   buffer[16] = L'\xCF';
   buffer[17] = L'\x00';
   EXPECT_TRUE(RCUpdater<wchar_t>::format(buffer, 16, 1, 22, 333, 444));
   EXPECT_STREQ(L"1, 22, 333, 444", buffer);
   EXPECT_EQ(L'\x00', buffer[15]);
   EXPECT_EQ(L'\xCF', buffer[16]);

   memset(buffer, 0xBE, sizeof(buffer));
   buffer[14] = L'\xCD';
   buffer[15] = L'\xCE';
   buffer[16] = L'\xCF';
   buffer[17] = L'\x00';
   EXPECT_EQ(17, wcslen(buffer));
   EXPECT_FALSE(RCUpdater<wchar_t>::format(buffer, 15, 1, 22, 333, 444));
   EXPECT_GT(15, wcslen(buffer));
   EXPECT_EQ(L'\xCE', buffer[15]);
   EXPECT_EQ(L'\xCF', buffer[16]);
}

TEST(RCUpdater, parseChar)
{
   int major = -1, minor = -1, build = -1, revision = -1;
   char* tail = nullptr;

   char s1[] = " \t \t 12.234,5678 . , .,. 54321 .,.,";
   EXPECT_TRUE(RCUpdater<char>::parse(s1, &tail, major, minor, build, revision));
   EXPECT_STREQ(" .,.,", tail);
   EXPECT_EQ(12, major);
   EXPECT_EQ(234, minor);
   EXPECT_EQ(5678, build);
   EXPECT_EQ(54321, revision);

   char s2[] = " \t x 12.234,5678 . , .,. 54321 .,.,";
   EXPECT_FALSE(RCUpdater<char>::parse(s2, &tail, major, minor, build, revision));
   EXPECT_STREQ("x 12.234,5678 . , .,. 54321 .,.,", tail);

   char s3[] = " \t \t 12.234, . , .,. 54321 .,.,";
   EXPECT_FALSE(RCUpdater<char>::parse(s3, &tail, major, minor, build, revision));
   EXPECT_STREQ("", tail);

   char s4[] = "123,2x,345,456";
   EXPECT_FALSE(RCUpdater<char>::parse(s4, &tail, major, minor, build, revision));
   EXPECT_STREQ("x,345,456", tail);
}

TEST(RCUpdater, parseWchar)
{
   int major = -1, minor = -1, build = -1, revision = -1;
   wchar_t* tail = nullptr;

   wchar_t s1[] = L" \t \t 12.234,5678 . , .,. 54321 .,.,";
   EXPECT_TRUE(RCUpdater<wchar_t>::parse(s1, &tail, major, minor, build, revision));
   EXPECT_STREQ(L" .,.,", tail);
   EXPECT_EQ(12, major);
   EXPECT_EQ(234, minor);
   EXPECT_EQ(5678, build);
   EXPECT_EQ(54321, revision);

   wchar_t s2[] = L" \t x 12.234,5678 . , .,. 54321 .,.,";
   EXPECT_FALSE(RCUpdater<wchar_t>::parse(s2, &tail, major, minor, build, revision));
   EXPECT_STREQ(L"x 12.234,5678 . , .,. 54321 .,.,", tail);

   wchar_t s3[] = L" \t \t 12.234, . , .,. 54321 .,.,";
   EXPECT_FALSE(RCUpdater<wchar_t>::parse(s3, &tail, major, minor, build, revision));
   EXPECT_STREQ(L"", tail);

   wchar_t s4[] = L"123,2x,345,456";
   EXPECT_FALSE(RCUpdater<wchar_t>::parse(s4, &tail, major, minor, build, revision));
   EXPECT_STREQ(L"x,345,456", tail);
}

TEST(RCUpdater, replaceChar)
{
   char s1[] = "1234";
   EXPECT_FALSE(RCUpdater<char>::replace(s1, 1, 4, "pqrs"));
   EXPECT_EQ('2', s1[1]);
   EXPECT_FALSE(RCUpdater<char>::replace(s1, 2, 4, "pqrs"));
   EXPECT_EQ('3', s1[2]);
   EXPECT_FALSE(RCUpdater<char>::replace(s1, 3, 4, "pqrs"));
   EXPECT_EQ('4', s1[3]);
   EXPECT_FALSE(RCUpdater<char>::replace(s1, 4, 4, "pqrs"));
   EXPECT_EQ('\0', s1[4]);
   EXPECT_TRUE(RCUpdater<char>::replace(s1, 5, 4, "pqrs"));
   EXPECT_STREQ("pqrs", s1);

   char s2[] = "1234\000678"; // Note: octal zero byte: \000
   EXPECT_TRUE(RCUpdater<char>::replace(s2, 5, 4, "pqrs"));
   EXPECT_STREQ("pqrs", s2);
   EXPECT_EQ('6', s2[5]);
   EXPECT_EQ('7', s2[6]);
   EXPECT_EQ('8', s2[7]);

   char s3[] = "1234\000678"; // Note: octal zero byte: \000
   EXPECT_TRUE(RCUpdater<char>::replace(s3, 6, 4, "pqrst"));
   EXPECT_STREQ("pqrst", s3);
   EXPECT_EQ('7', s3[6]);
   EXPECT_EQ('8', s3[7]);

   char s4[] = "1234\000678"; // Note: octal zero byte: \000
   EXPECT_TRUE(RCUpdater<char>::replace(s4, 7, 4, "pqrstu"));
   EXPECT_STREQ("pqrstu", s4);
   EXPECT_EQ('8', s4[7]);

   char s5[] = "1234\000678"; // Note: octal zero byte: \000
   EXPECT_FALSE(RCUpdater<char>::replace(s5, 7, 4, "pqrstuv"));
   EXPECT_EQ('8', s5[7]);

   char s6[] = "1234\000678"; // Note: octal zero byte: \000
   EXPECT_TRUE(RCUpdater<char>::replace(s6, 7, 2, "pqrs"));
   EXPECT_STREQ("pqrs34", s6);
   EXPECT_EQ('8', s6[7]);

   char s7[] = "1234\000678"; // Note: octal zero byte: \000
   EXPECT_TRUE(RCUpdater<char>::replace(s7, 7, 0, "pq"));
   EXPECT_STREQ("pq1234", s7);
   EXPECT_EQ('8', s7[7]);

   char s8[] = "1234\000678"; // Note: octal zero byte: \000
   EXPECT_TRUE(RCUpdater<char>::replace(s8, 7, 4, "pqr"));
   EXPECT_STREQ("pqr", s8);
   EXPECT_EQ('8', s8[7]);

   char s9[] = "1234\000678"; // Note: octal zero byte: \000
   EXPECT_TRUE(RCUpdater<char>::replace(s9, 7, 3, ""));
   EXPECT_STREQ("4", s9);
   EXPECT_EQ('8', s9[7]);
}

TEST(RCUpdater, replaceWchar)
{
   wchar_t s1[] = L"1234";
   EXPECT_FALSE(RCUpdater<wchar_t>::replace(s1, 1, 4, L"pqrs"));
   EXPECT_EQ('2', s1[1]);
   EXPECT_FALSE(RCUpdater<wchar_t>::replace(s1, 2, 4, L"pqrs"));
   EXPECT_EQ('3', s1[2]);
   EXPECT_FALSE(RCUpdater<wchar_t>::replace(s1, 3, 4, L"pqrs"));
   EXPECT_EQ('4', s1[3]);
   EXPECT_FALSE(RCUpdater<wchar_t>::replace(s1, 4, 4, L"pqrs"));
   EXPECT_EQ('\0', s1[4]);
   EXPECT_TRUE(RCUpdater<wchar_t>::replace(s1, 5, 4, L"pqrs"));
   EXPECT_STREQ(L"pqrs", s1);

   wchar_t s2[] = L"1234\000678"; // Note: octal zero byte: \000
   EXPECT_TRUE(RCUpdater<wchar_t>::replace(s2, 5, 4, L"pqrs"));
   EXPECT_STREQ(L"pqrs", s2);
   EXPECT_EQ('6', s2[5]);
   EXPECT_EQ('7', s2[6]);
   EXPECT_EQ('8', s2[7]);

   wchar_t s3[] = L"1234\000678"; // Note: octal zero byte: \000
   EXPECT_TRUE(RCUpdater<wchar_t>::replace(s3, 6, 4, L"pqrst"));
   EXPECT_STREQ(L"pqrst", s3);
   EXPECT_EQ('7', s3[6]);
   EXPECT_EQ('8', s3[7]);

   wchar_t s4[] = L"1234\000678"; // Note: octal zero byte: \000
   EXPECT_TRUE(RCUpdater<wchar_t>::replace(s4, 7, 4, L"pqrstu"));
   EXPECT_STREQ(L"pqrstu", s4);
   EXPECT_EQ('8', s4[7]);

   wchar_t s5[] = L"1234\000678"; // Note: octal zero byte: \000
   EXPECT_FALSE(RCUpdater<wchar_t>::replace(s5, 7, 4, L"pqrstuv"));
   EXPECT_EQ('8', s5[7]);

   wchar_t s6[] = L"1234\000678"; // Note: octal zero byte: \000
   EXPECT_TRUE(RCUpdater<wchar_t>::replace(s6, 7, 2, L"pqrs"));
   EXPECT_STREQ(L"pqrs34", s6);
   EXPECT_EQ('8', s6[7]);

   wchar_t s7[] = L"1234\000678"; // Note: octal zero byte: \000
   EXPECT_TRUE(RCUpdater<wchar_t>::replace(s7, 7, 0, L"pq"));
   EXPECT_STREQ(L"pq1234", s7);
   EXPECT_EQ('8', s7[7]);

   wchar_t s8[] = L"1234\000678"; // Note: octal zero byte: \000
   EXPECT_TRUE(RCUpdater<wchar_t>::replace(s8, 7, 4, L"pqr"));
   EXPECT_STREQ(L"pqr", s8);
   EXPECT_EQ('8', s8[7]);

   wchar_t s9[] = L"1234\000678"; // Note: octal zero byte: \000
   EXPECT_TRUE(RCUpdater<wchar_t>::replace(s9, 7, 3, L""));
   EXPECT_STREQ(L"4", s9);
   EXPECT_EQ('8', s9[7]);
}

TEST(RCUpdater, FindStartOfVersionChar)
{
   size_t pos = -1;
   char s1[] =
      "\r\n// Some non-ASCII characters: |\xC4\x85\xC4\x87\xC4\x99\xC5\x82\xC5\x84\xC3\xB3\xC5\x9B|"
      "\r\n"
      "\r\n // Here are some fake lines:"
      "\r\nVERSIONINFO ABC"
      "\r\n VERSIONINFO BCD"
      "\r\nVS_VERSION_INFO VERSIONINFO"
      "\r\nFILEVERSION 15,   23,3456, 891"
      "\r\nPRODUCTVERSION 16, 24, 4567, 892"
      "\r\nBEGIN"
      "\r\nBLOCK \"StringFileInfo\""
      "\r\nBEGIN"
      ;
   pos = RCUpdater<char>::FindStartOfVersion(s1);
   ASSERT_NE(0, pos);
   EXPECT_EQ('F', s1[pos + 0]);
   EXPECT_EQ('I', s1[pos + 1]);
   EXPECT_EQ('L', s1[pos + 2]);
   EXPECT_EQ('E', s1[pos + 3]);

   char s2[] =
      "\r\n // Here are some fake lines:"
      "\r\nVERSIONINFO ABC"
      "\r\n VERSIONINFO BCD"
      "\r\n1 VERSIONINFO"
      "\r\n FILEVERSION 15,   23,3456, 891"
      "\r\nPRODUCTVERSION 16, 24, 4567, 892"
      "\r\nBEGIN"
      "\r\nBLOCK \"StringFileInfo\""
      "\r\nBEGIN"
      ;
   pos = RCUpdater<char>::FindStartOfVersion(s2);
   ASSERT_NE(0, pos);
   EXPECT_EQ(' ', s2[pos + 0]);
   EXPECT_EQ('F', s2[pos + 1]);
   EXPECT_EQ('I', s2[pos + 2]);
   EXPECT_EQ('L', s2[pos + 3]);
   EXPECT_EQ('E', s2[pos + 4]);

   char s3[] =
      "\r\n // Here are some fake lines:"
      "\r\nVERSIONINFO ABC"
      "\r\n VERSIONINFO BCD"
      "\r\n   ABRA_KADABRA VERSIONINFO"
      "\r\n FILEVERSION 15,   23,3456, 891"
      "\r\nPRODUCTVERSION 16, 24, 4567, 892"
      "\r\nBEGIN"
      "\r\nBLOCK \"StringFileInfo\""
      "\r\nBEGIN"
      ;
   pos = RCUpdater<char>::FindStartOfVersion(s3);
   ASSERT_NE(0, pos);
   EXPECT_EQ(' ', s3[pos + 0]);
   EXPECT_EQ('F', s3[pos + 1]);
   EXPECT_EQ('I', s3[pos + 2]);
   EXPECT_EQ('L', s3[pos + 3]);
   EXPECT_EQ('E', s3[pos + 4]);

   char s4[] =
      "// Example resource file with no BOM"
      "\r\n\t"
      "\r\n\tVS_VERSION_INFO VERSIONINFO"
      "\r\n\tFILEVERSION 12, 23, 345, 45"
      "\r\n\tPRODUCTVERSION 12, 23, 345, 45"
      "\r\n\tBEGIN"
      "\r\n\tBLOCK \"StringFileInfo\""
      "\r\n\tBEGIN"
      "\r\n\tBLOCK \"040904b0\""
      "\r\n\tBEGIN"
      ;
   pos = RCUpdater<char>::FindStartOfVersion(s4);
   ASSERT_NE(0, pos);
   EXPECT_EQ('\t', s4[pos + 0]);
   EXPECT_EQ('F', s4[pos + 1]);
   EXPECT_EQ('I', s4[pos + 2]);
   EXPECT_EQ('L', s4[pos + 3]);
   EXPECT_EQ('E', s4[pos + 4]);

   char s5[] =
      "\r\n // Here are some fake lines:"
      "\r\nVERSIONINFO ABC"
      "\r\n VERSIONINFO BCD"
      "\r\n FILEVERSION 15,   23,3456, 891"
      "\r\nPRODUCTVERSION 16, 24, 4567, 892"
      "\r\nBEGIN"
      "\r\nBLOCK \"StringFileInfo\""
      "\r\nBEGIN"
      ;
   pos = RCUpdater<char>::FindStartOfVersion(s5);
   EXPECT_EQ(0, pos);
}
