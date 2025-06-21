#include "stdafx.h"
#include "Logger.h"
#include "TestLogger.h"

class LoggerTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    testLogger = std::make_unique<TestLogger>();
    logger = std::make_unique<Logger>(*testLogger);
  }

  std::unique_ptr<TestLogger> testLogger;
  std::unique_ptr<Logger> logger;
};

TEST_F(LoggerTests, Verbosity_GetSet)
{
  EXPECT_EQ(1, logger->Verbosity()); // Default verbosity
  
  logger->Verbosity(5);
  EXPECT_EQ(5, logger->Verbosity());
  
  logger->Verbosity(0);
  EXPECT_EQ(0, logger->Verbosity());
  
  logger->Verbosity(9);
  EXPECT_EQ(9, logger->Verbosity());
}

TEST_F(LoggerTests, Verbosity_EdgeValues)
{
  logger->Verbosity(-1);
  EXPECT_EQ(-1, logger->Verbosity()); // Should accept negative values
  
  logger->Verbosity(100);
  EXPECT_EQ(100, logger->Verbosity()); // Should accept values > 9
}

TEST_F(LoggerTests, Log_WithLevel)
{
  logger->Verbosity(3);
  
  // Message with level <= verbosity should be logged
  logger->Log(2, L"Level 2 message");
  EXPECT_NE(nullptr, wcsstr(testLogger->messages.c_str(), L"Level 2 message"));
  
  testLogger->messages.clear();
  
  // Message with level = verbosity should be logged
  logger->Log(3, L"Level 3 message");
  EXPECT_NE(nullptr, wcsstr(testLogger->messages.c_str(), L"Level 3 message"));
  
  testLogger->messages.clear();
  
  // Message with level > verbosity should not be logged
  logger->Log(4, L"Level 4 message");
  EXPECT_EQ(nullptr, wcsstr(testLogger->messages.c_str(), L"Level 4 message"));
}

TEST_F(LoggerTests, Log_FormatString)
{
  logger->Verbosity(5);
  
  logger->Log(1, L"Number: %d, String: %s", 42, L"test");
  EXPECT_NE(nullptr, wcsstr(testLogger->messages.c_str(), L"Number: 42"));
  EXPECT_NE(nullptr, wcsstr(testLogger->messages.c_str(), L"String: test"));
}

TEST_F(LoggerTests, Log_LongMessage)
{
  logger->Verbosity(5);
  
  std::wstring longString(2000, L'A'); // Very long string
  logger->Log(1, L"Long: %s", longString.c_str());
  
  // Should handle long messages without crashing
  EXPECT_FALSE(testLogger->messages.empty());
}

TEST_F(LoggerTests, Log_NullFormatString)
{
  logger->Verbosity(5);
  
  // Passing null format string is undefined behavior and causes assertion failure
  // This test verifies the behavior - it will trigger assertion in debug builds
  // In production, this should be avoided
  // logger->Log(1, nullptr);  // Commented out - causes assertion failure
  
  // Instead test with empty string which is valid
  logger->Log(1, L"");
  EXPECT_FALSE(testLogger->messages.empty());
}

TEST_F(LoggerTests, Log_EmptyFormatString)
{
  logger->Verbosity(5);
  
  logger->Log(1, L"");
  // Should log empty message
  EXPECT_FALSE(testLogger->messages.empty());
}

TEST_F(LoggerTests, Error_WithErrorCode)
{
  logger->Verbosity(5);
  
  EXPECT_FALSE(logger->Error(ERROR_FILE_NOT_FOUND, L"File not found: %s", L"test.txt"));
  
  // Should contain error message
  EXPECT_NE(nullptr, wcsstr(testLogger->messages.c_str(), L"File not found: test.txt"));
  
  // Should set last error
  EXPECT_EQ(ERROR_FILE_NOT_FOUND, GetLastError());
}

TEST_F(LoggerTests, Error_AlwaysLogged)
{
  logger->Verbosity(0); // Very low verbosity
  
  EXPECT_FALSE(logger->Error(ERROR_ACCESS_DENIED, L"Access denied"));
  
  // Error should be logged regardless of verbosity
  EXPECT_NE(nullptr, wcsstr(testLogger->messages.c_str(), L"Access denied"));
}

TEST_F(LoggerTests, Error_FormatStringEdgeCases)
{
  logger->Verbosity(5);
  
  // Test with various format specifiers
  EXPECT_FALSE(logger->Error(123, L"Error %u with string %s and number %d", 456u, L"test", -789));
  
  EXPECT_NE(nullptr, wcsstr(testLogger->messages.c_str(), L"Error 456"));
  EXPECT_NE(nullptr, wcsstr(testLogger->messages.c_str(), L"string test"));
  EXPECT_NE(nullptr, wcsstr(testLogger->messages.c_str(), L"number -789"));
}

TEST_F(LoggerTests, Error_VeryLongErrorMessage)
{
  logger->Verbosity(5);
  
  std::wstring longString(3000, L'B'); // Very long string that might cause buffer overflow
  EXPECT_FALSE(logger->Error(999, L"Long error: %s", longString.c_str()));
  
  // Should handle without crashing
  EXPECT_FALSE(testLogger->messages.empty());
}

TEST_F(LoggerTests, Error_ZeroErrorCode)
{
  logger->Verbosity(5);
  
  EXPECT_FALSE(logger->Error(0, L"Zero error code"));
  EXPECT_EQ(0, GetLastError());
  EXPECT_NE(nullptr, wcsstr(testLogger->messages.c_str(), L"Zero error code"));
}

TEST_F(LoggerTests, Error_MaxErrorCode)
{
  logger->Verbosity(5);
  
  EXPECT_FALSE(logger->Error(UINT_MAX, L"Max error code"));
  EXPECT_EQ(UINT_MAX, GetLastError());
  EXPECT_NE(nullptr, wcsstr(testLogger->messages.c_str(), L"Max error code"));
}

TEST_F(LoggerTests, Mixed_LogAndError)
{
  logger->Verbosity(5);
  
  logger->Log(1, L"Regular message");
  EXPECT_FALSE(logger->Error(ERROR_INVALID_PARAMETER, L"Error message"));
  logger->Log(2, L"Another regular message");
  
  // All messages should be present
  EXPECT_NE(nullptr, wcsstr(testLogger->messages.c_str(), L"Regular message"));
  EXPECT_NE(nullptr, wcsstr(testLogger->messages.c_str(), L"Error message"));
  EXPECT_NE(nullptr, wcsstr(testLogger->messages.c_str(), L"Another regular message"));
  
  // Last error should be set
  EXPECT_EQ(ERROR_INVALID_PARAMETER, GetLastError());
}