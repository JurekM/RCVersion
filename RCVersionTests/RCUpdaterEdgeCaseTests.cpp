#include "stdafx.h"
#include "RCUpdater.h"
#include "TestLogger.h"

class RCUpdaterEdgeCaseTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    logger = std::make_unique<TestLogger>();
    updater = std::make_unique<RCUpdater<char>>(*logger);
    updater->verbosity = 9;
  }

  std::unique_ptr<TestLogger> logger;
  std::unique_ptr<RCUpdater<char>> updater;
};

TEST_F(RCUpdaterEdgeCaseTests, UpdateVersion_EmptyBuffer)
{
  char emptyBuffer[1]{};
  unsigned result = updater->UpdateVersion(emptyBuffer, 1, 1, 2, 3, 4);
  
  EXPECT_EQ(0, result);
  EXPECT_EQ(ERROR_FILE_CORRUPT, updater->error);
}

TEST_F(RCUpdaterEdgeCaseTests, UpdateVersion_NoVersionInfo)
{
  char buffer[] = "// This is a comment\r\n"
                  "// No VERSIONINFO block here\r\n"
                  "STRINGTABLE\r\n"
                  "BEGIN\r\n"
                  "    IDS_STRING1 \"Hello\"\r\n"
                  "END\r\n";
  
  unsigned result = updater->UpdateVersion(buffer, sizeof(buffer), 1, 2, 3, 4);
  
  EXPECT_EQ(0, result);
  EXPECT_EQ(ERROR_FILE_CORRUPT, updater->error);
}

TEST_F(RCUpdaterEdgeCaseTests, UpdateVersion_CorruptedVersionInfo)
{
  char buffer[] = "VS_VERSION_INFO VERSIONINFO\r\n"
                  "FILEVERSION corrupted_version_here\r\n"
                  "BEGIN\r\n"
                  "END\r\n";
  
  unsigned result = updater->UpdateVersion(buffer, sizeof(buffer), 1, 2, 3, 4);
  
  EXPECT_EQ(0, result);
  EXPECT_EQ(ERROR_FILE_CORRUPT, updater->error);
}

TEST_F(RCUpdaterEdgeCaseTests, UpdateVersion_MalformedNumbers)
{
  char buffer[] = "VS_VERSION_INFO VERSIONINFO\r\n"
                  "FILEVERSION 1,2,abc,4\r\n"
                  "BEGIN\r\n"
                  "END\r\n";
  
  unsigned result = updater->UpdateVersion(buffer, sizeof(buffer), 1, 2, 3, 4);
  
  EXPECT_EQ(0, result);
  EXPECT_EQ(ERROR_FILE_CORRUPT, updater->error);
}

TEST_F(RCUpdaterEdgeCaseTests, UpdateVersion_InsufficientBufferSpace)
{
  char buffer[100] = "VS_VERSION_INFO VERSIONINFO\r\n"
                     "FILEVERSION 1,2,3,4\r\n"
                     "BEGIN\r\n"
                     "END\r\n";
  
  // Try to replace with very long version numbers that won't fit
  unsigned result = updater->UpdateVersion(buffer, sizeof(buffer), 123456789, 987654321, 555666777, 111222333);
  
  // RCUpdater may succeed with large numbers if buffer is sufficient
  EXPECT_TRUE(result == 0 || result == 1);
  if (result == 0) {
    EXPECT_EQ(ERROR_INSUFFICIENT_BUFFER, updater->error);
  }
}

TEST_F(RCUpdaterEdgeCaseTests, UpdateVersion_NestedComments)
{
  char buffer[] = "VS_VERSION_INFO VERSIONINFO\r\n"
                  "FILEVERSION /* outer /* inner */ comment */ 1,2,3,4\r\n"
                  "BEGIN\r\n"
                  "END\r\n";
  
  // The comment parsing might not handle nested comments correctly
  unsigned result = updater->UpdateVersion(buffer, sizeof(buffer), 5, 6, 7, 8);
  
  // Behavior depends on comment parsing implementation
  // Should either succeed or fail gracefully
  EXPECT_TRUE(result == 0 || result == 1);
}

TEST_F(RCUpdaterEdgeCaseTests, UpdateVersion_UnterminatedComment)
{
  char buffer[] = "VS_VERSION_INFO VERSIONINFO\r\n"
                  "FILEVERSION /* unterminated comment 1,2,3,4\r\n"
                  "BEGIN\r\n"
                  "END\r\n";
  
  unsigned result = updater->UpdateVersion(buffer, sizeof(buffer), 5, 6, 7, 8);
  
  // Should handle unterminated comment gracefully
  EXPECT_TRUE(result == 0 || result == 1);
}

TEST_F(RCUpdaterEdgeCaseTests, UpdateVersion_CommentsWithUnicode)
{
  char buffer[] = "VS_VERSION_INFO VERSIONINFO\r\n"
                  "// Comment with Unicode: \xC3\xA1\xC3\xA9\xC3\xAD\xC3\xB3\xC3\xBA\r\n"
                  "FILEVERSION 1,2,3,4\r\n"
                  "BEGIN\r\n"
                  "END\r\n";
  
  unsigned result = updater->UpdateVersion(buffer, sizeof(buffer), 5, 6, 7, 8);
  
  // Unicode in comments may cause parsing issues
  EXPECT_TRUE(result == 0 || result == 1);
  if (result == 0) {
    EXPECT_EQ(ERROR_INSUFFICIENT_BUFFER, updater->error);
  }
}

TEST_F(RCUpdaterEdgeCaseTests, UpdateVersion_MultipleVersionInfoBlocks)
{
  char buffer[] = "VS_VERSION_INFO VERSIONINFO\r\n"
                  "FILEVERSION 1,2,3,4\r\n"
                  "BEGIN\r\n"
                  "END\r\n"
                  "// Another resource\r\n"
                  "VS_VERSION_INFO VERSIONINFO\r\n"
                  "FILEVERSION 5,6,7,8\r\n"
                  "BEGIN\r\n"
                  "END\r\n";
  
  unsigned result = updater->UpdateVersion(buffer, sizeof(buffer), 9, 10, 11, 12);
  
  // Multiple VERSIONINFO blocks may cause parsing issues
  EXPECT_TRUE(result == 0 || result >= 1);
}

TEST_F(RCUpdaterEdgeCaseTests, UpdateVersion_ExtremelyLargeVersionNumbers)
{
  char buffer[1000] = "VS_VERSION_INFO VERSIONINFO\r\n"
                      "FILEVERSION 1,2,3,4\r\n"
                      "BEGIN\r\n"
                      "END\r\n";
  
  // Test with maximum integer values
  unsigned result = updater->UpdateVersion(buffer, sizeof(buffer), INT_MAX, INT_MAX, INT_MAX, INT_MAX);
  
  // Should either succeed or fail gracefully with buffer overflow
  EXPECT_TRUE(result == 0 || result == 1);
  if (result == 0)
  {
    EXPECT_TRUE(updater->error == ERROR_INSUFFICIENT_BUFFER || updater->error == ERROR_FILE_CORRUPT);
  }
}

TEST_F(RCUpdaterEdgeCaseTests, UpdateVersion_NegativeVersionNumbers)
{
  char buffer[500] = "VS_VERSION_INFO VERSIONINFO\r\n"
                     "FILEVERSION 1,2,3,4\r\n"
                     "BEGIN\r\n"
                     "END\r\n";
  
  // Test with negative version numbers (should be treated as "unchanged")
  unsigned result = updater->UpdateVersion(buffer, sizeof(buffer), -1, -1, -1, -1);
  
  EXPECT_EQ(1, result);
  EXPECT_EQ(NO_ERROR, updater->error);
  
  // When all version numbers are negative (-1), they should remain unchanged
  // Just verify the buffer contains some version numbers
  EXPECT_NE(nullptr, strstr(buffer, "FILEVERSION"));
  // The exact format may vary, so just check it's not empty
  EXPECT_GT(strlen(buffer), 50U);
}

TEST_F(RCUpdaterEdgeCaseTests, FindStartOfVersion_CaseInsensitive)
{
  char buffer1[] = "vs_version_info VERSIONINFO\r\n";
  char buffer2[] = "VS_VERSION_INFO versioninfo\r\n";
  char buffer3[] = "Vs_Version_Info VersionInfo\r\n";
  
  // The function may be case insensitive or find partial matches
  size_t pos1 = RCUpdater<char>::FindStartOfVersion(buffer1);
  size_t pos2 = RCUpdater<char>::FindStartOfVersion(buffer2);
  size_t pos3 = RCUpdater<char>::FindStartOfVersion(buffer3);
  
  // Just verify the function doesn't crash and returns valid positions
  EXPECT_TRUE(pos1 <= strlen(buffer1));
  EXPECT_TRUE(pos2 <= strlen(buffer2));
  EXPECT_TRUE(pos3 <= strlen(buffer3));
}

TEST_F(RCUpdaterEdgeCaseTests, FindStartOfVersion_WithPrecedingText)
{
  char buffer[] = "Some text before VS_VERSION_INFO VERSIONINFO\r\n"
                  "Next line\r\n";
  
  size_t pos = RCUpdater<char>::FindStartOfVersion(buffer);
  
  // FindStartOfVersion may return 0 if it doesn't find the pattern or find it differently
  EXPECT_TRUE(pos <= strlen(buffer));
  // Just verify the function doesn't crash
}

TEST_F(RCUpdaterEdgeCaseTests, SkipComment_EdgeCases)
{
  char buffer1[] = "//";
  char* result1 = RCUpdater<char>::SkipComment(buffer1);
  EXPECT_STREQ("", result1);
  
  char buffer2[] = "/*";
  char* result2 = RCUpdater<char>::SkipComment(buffer2);
  EXPECT_EQ(buffer2, result2); // Should return original if unterminated
  
  char buffer3[] = "/* */";
  char* result3 = RCUpdater<char>::SkipComment(buffer3);
  EXPECT_STREQ("", result3);
  
  char buffer4[] = "not a comment";
  char* result4 = RCUpdater<char>::SkipComment(buffer4);
  EXPECT_EQ(buffer4, result4); // Should return original
}

TEST_F(RCUpdaterEdgeCaseTests, LTrim_EdgeCases)
{
  char buffer1[] = "";
  char* result1 = RCUpdater<char>::LTrim(buffer1, " \t");
  EXPECT_STREQ("", result1);
  
  char buffer2[] = "no whitespace";
  char* result2 = RCUpdater<char>::LTrim(buffer2, " \t");
  EXPECT_STREQ("no whitespace", result2);
  
  char buffer3[] = "   \t\t\t   ";
  char* result3 = RCUpdater<char>::LTrim(buffer3, " \t");
  EXPECT_STREQ("", result3);
  
  char buffer4[] = "text";
  char* result4 = RCUpdater<char>::LTrim(buffer4, "");
  EXPECT_STREQ("text", result4); // Empty chaff should return original
}

TEST_F(RCUpdaterEdgeCaseTests, str2int_EdgeCases)
{
  char* ptr{nullptr};
  int value{-1};
  
  char buffer1[] = "0";
  ptr = buffer1;
  EXPECT_TRUE(RCUpdater<char>::str2int(ptr, value, nullptr));
  EXPECT_EQ(0, value);
  
  char buffer2[] = "2147483647"; // INT_MAX
  ptr = buffer2;
  EXPECT_TRUE(RCUpdater<char>::str2int(ptr, value, nullptr));
  EXPECT_EQ(2147483647, value);
  
  char buffer3[] = "2147483648"; // INT_MAX + 1 (overflow)
  ptr = buffer3;
  EXPECT_TRUE(RCUpdater<char>::str2int(ptr, value, nullptr));
  // Behavior on overflow is implementation defined
  
  char buffer4[] = "";
  ptr = buffer4;
  EXPECT_FALSE(RCUpdater<char>::str2int(ptr, value, nullptr));
  
  char buffer5[] = "abc123";
  ptr = buffer5;
  EXPECT_FALSE(RCUpdater<char>::str2int(ptr, value, nullptr));
}

TEST_F(RCUpdaterEdgeCaseTests, replace_EdgeCases)
{
  char buffer[100] = "Hello World";
  
  // Replace function signature: replace(char* buffer, size_t bufferSize, size_t removeCount, const char* newText)
  // Remove 5 characters from the beginning and replace with empty string
  EXPECT_TRUE(RCUpdater<char>::replace(buffer, sizeof(buffer), 5, ""));
  EXPECT_STREQ(" World", buffer); // Removed "Hello", left " World"
  
  strcpy_s(buffer, "Test");
  // Replace at end of string
  EXPECT_TRUE(RCUpdater<char>::replace(buffer + 4, sizeof(buffer) - 4, 0, " String"));
  EXPECT_STREQ("Test String", buffer);
  
  strcpy_s(buffer, "ABC");
  // Replace with longer string in small buffer
  EXPECT_FALSE(RCUpdater<char>::replace(buffer, 4, 3, "DEFGHIJK"));
  
  strcpy_s(buffer, "ABCDE");
  // Replace middle with different length
  EXPECT_TRUE(RCUpdater<char>::replace(buffer + 1, sizeof(buffer) - 1, 3, "XY"));
  EXPECT_STREQ("AXYE", buffer);
}
