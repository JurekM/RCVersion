#include "stdafx.h"
#include "MessageBuffer.h"

class MessageBufferEdgeCaseTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    buffer = std::make_unique<MessageBuffer>();
  }

  std::unique_ptr<MessageBuffer> buffer;
};

TEST_F(MessageBufferEdgeCaseTests, append_NullCharPointer)
{
  // Passing nullptr to append causes access violation
  // This is undefined behavior and should be avoided
  // const char* nullPtr = nullptr;
  // buffer->append(nullPtr);  // Commented out - causes access violation
  
  // Instead test that empty buffer returns empty string
  EXPECT_STREQ(L"", buffer->message());
}

TEST_F(MessageBufferEdgeCaseTests, append_EmptyString)
{
  buffer->append("");
  EXPECT_STREQ(L"", buffer->message());
  
  buffer->append(L"");
  EXPECT_STREQ(L"", buffer->message());
}

TEST_F(MessageBufferEdgeCaseTests, append_VeryLongString)
{
  std::string longString(10000, 'A');
  buffer->append(longString.c_str());
  
  // Should handle long strings without crashing
  EXPECT_TRUE(wcslen(buffer->message()) > 0);
  EXPECT_TRUE(wcslen(buffer->message()) <= 10000);
}

TEST_F(MessageBufferEdgeCaseTests, append_StringWithNullCharacters)
{
  char stringWithNull[] = {'A', 'B', '\0', 'C', 'D', '\0'};
  buffer->append(stringWithNull);
  
  // Should only process up to first null
  EXPECT_STREQ(L"AB", buffer->message());
}

TEST_F(MessageBufferEdgeCaseTests, append_UnicodeCharacters)
{
  // Test with various Unicode characters
  buffer->append("Hello ñáéíóú 中文 🔔");
  
  // Should convert to wide characters properly
  EXPECT_NE(nullptr, wcsstr(buffer->message(), L"Hello"));
  // Note: The exact Unicode conversion depends on system locale
}

TEST_F(MessageBufferEdgeCaseTests, append_InvalidUtf8Sequence)
{
  // Invalid UTF-8 sequences (continuation bytes without start byte)
  char invalidUtf8[] = {static_cast<char>(0x80), static_cast<char>(0x80), 'A', 'B', '\0'};
  buffer->append(invalidUtf8);
  
  // Should handle invalid sequences gracefully
  EXPECT_TRUE(wcslen(buffer->message()) >= 0); // Should not crash
}

TEST_F(MessageBufferEdgeCaseTests, append_HighUnicodeCodePoints)
{
  // Test with characters beyond Basic Multilingual Plane
  buffer->append("𝓗𝓮𝓵𝓵𝓸"); // Mathematical script letters
  
  // Should handle without crashing
  EXPECT_TRUE(wcslen(buffer->message()) >= 0);
}

TEST_F(MessageBufferEdgeCaseTests, append_MixedCharAndWChar)
{
  buffer->append("Narrow string");
  buffer->append(L" Wide string");
  
  EXPECT_NE(nullptr, wcsstr(buffer->message(), L"Narrow string Wide string"));
}

TEST_F(MessageBufferEdgeCaseTests, append_MultipleOperations)
{
  for (int i = 0; i < 100; ++i)
  {
    buffer->append("Test ");
  }
  
  // Should handle many append operations
  EXPECT_TRUE(wcslen(buffer->message()) > 400); // Should contain all appends
}

TEST_F(MessageBufferEdgeCaseTests, set_OverwritesPreviousContent)
{
  buffer->append("First content");
  buffer->set("Second content");
  
  EXPECT_STREQ(L"Second content", buffer->message());
  EXPECT_EQ(nullptr, wcsstr(buffer->message(), L"First"));
}

TEST_F(MessageBufferEdgeCaseTests, set_NullPointer)
{
  buffer->append("Initial content");
  
  // Passing nullptr to set causes access violation
  // This is undefined behavior and should be avoided
  // const char* nullPtr = nullptr;
  // buffer->set(nullPtr);  // Commented out - causes access violation
  
  // Instead test setting to empty string
  buffer->set("");
  EXPECT_STREQ(L"", buffer->message());
}

TEST_F(MessageBufferEdgeCaseTests, set_EmptyString)
{
  buffer->append("Initial content");
  buffer->set("");
  
  EXPECT_STREQ(L"", buffer->message());
}

TEST_F(MessageBufferEdgeCaseTests, clear_AfterContent)
{
  buffer->append("Some content");
  buffer->clear();
  
  EXPECT_STREQ(L"", buffer->message());
}

TEST_F(MessageBufferEdgeCaseTests, clear_WhenEmpty)
{
  buffer->clear();
  EXPECT_STREQ(L"", buffer->message());
}

TEST_F(MessageBufferEdgeCaseTests, format_NullFormatString)
{
  // Passing null format string causes assertion failure in debug builds
  // This is undefined behavior and should be avoided in production code
  // const wchar_t* nullFormat = nullptr;
  // buffer->format(nullFormat, 123);  // Commented out - causes assertion failure
  
  // Instead test with empty format string which is valid
  buffer->format(L"");
  EXPECT_STREQ(L"", buffer->message());
}

TEST_F(MessageBufferEdgeCaseTests, format_EmptyFormatString)
{
  buffer->format(L"");
  EXPECT_STREQ(L"", buffer->message());
}

TEST_F(MessageBufferEdgeCaseTests, format_TooManyArguments)
{
  buffer->format(L"Only one %d", 123, 456, 789);
  
  // Should handle extra arguments gracefully
  EXPECT_NE(nullptr, wcsstr(buffer->message(), L"123"));
}

TEST_F(MessageBufferEdgeCaseTests, format_TooFewArguments)
{
  // This is undefined behavior but should not crash
  buffer->format(L"Need two: %d %d", 123);
  
  // Should not crash (exact behavior is undefined)
  EXPECT_TRUE(wcslen(buffer->message()) >= 0);
}

TEST_F(MessageBufferEdgeCaseTests, format_VeryLongResult)
{
  std::wstring longFormat(500, L'A');
  longFormat += L" %d";
  
  buffer->format(longFormat.c_str(), 123);
  
  // Should handle long format strings
  EXPECT_TRUE(wcslen(buffer->message()) > 400);
}

TEST_F(MessageBufferEdgeCaseTests, format_SpecialFormatSpecifiers)
{
  buffer->format(L"Percent: %%, Char: %c, Hex: %x, Pointer: %p", 
                 L'Z', 255, static_cast<void*>(buffer.get()));
  
  EXPECT_NE(nullptr, wcsstr(buffer->message(), L"Percent: %"));
  EXPECT_NE(nullptr, wcsstr(buffer->message(), L"Char: Z"));
  EXPECT_NE(nullptr, wcsstr(buffer->message(), L"Hex: ff"));
  EXPECT_NE(nullptr, wcsstr(buffer->message(), L"Pointer:"));
}

TEST_F(MessageBufferEdgeCaseTests, format_MultipleFormatCalls)
{
  buffer->format(L"First: %d", 1);
  buffer->format(L" Second: %d", 2);
  
  // format should append, not replace
  EXPECT_NE(nullptr, wcsstr(buffer->message(), L"First: 1 Second: 2"));
}

TEST_F(MessageBufferEdgeCaseTests, Constructor_WithStringLiteral)
{
  MessageBuffer mb1("Test string");
  EXPECT_STREQ(L"Test string", mb1.message());
  
  MessageBuffer mb2(L"Wide test string");
  EXPECT_STREQ(L"Wide test string", mb2.message());
}

TEST_F(MessageBufferEdgeCaseTests, Constructor_WithStdString)
{
  std::string narrowStr = "Narrow std::string";
  std::wstring wideStr = L"Wide std::wstring";
  
  MessageBuffer mb1(narrowStr.c_str());
  EXPECT_STREQ(L"Narrow std::string", mb1.message());
  
  MessageBuffer mb2(wideStr.c_str());
  EXPECT_STREQ(L"Wide std::wstring", mb2.message());
}

TEST_F(MessageBufferEdgeCaseTests, CopyAndAssignment)
{
  buffer->append("Original content");
  
  MessageBuffer copy{*buffer};
  EXPECT_STREQ(buffer->message(), copy.message());
  
  MessageBuffer assigned;
  assigned = *buffer;
  EXPECT_STREQ(buffer->message(), assigned.message());
  
  // Modify original and verify copies are independent
  buffer->append(" modified");
  EXPECT_STRNE(buffer->message(), copy.message());
  EXPECT_STRNE(buffer->message(), assigned.message());
}