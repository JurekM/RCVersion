#include "stdafx.h"
#include "RCFileHandler.h"
#include "TestLogger.h"

class FileHandlerErrorTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    logger = std::make_unique<TestLogger>();
    handler = std::make_unique<RCFileHandler>(*logger);
    handler->Verbosity(9);
  }

  void TearDown() override
  {
    // Clean up temporary files and directories
    for (const auto& file : tempFiles)
    {
      DeleteFile(file.c_str());
    }
    for (const auto& dir : tempDirs)
    {
      SetFileAttributes(dir.c_str(), FILE_ATTRIBUTE_NORMAL);
      RemoveDirectory(dir.c_str());
    }
  }

  std::wstring CreateInvalidPath(const std::wstring& subPath)
  {
    wchar_t tempDir[MAX_PATH]{};
    GetTempPath(MAX_PATH, tempDir);
    return std::wstring(tempDir) + subPath;
  }
  
  std::wstring CreateTempFile(const std::wstring& prefix = L"tst")
  {
    wchar_t tempDir[MAX_PATH]{};
    GetTempPath(MAX_PATH, tempDir);
    wchar_t tempFile[MAX_PATH]{};
    GetTempFileName(tempDir, prefix.c_str(), 0, tempFile);
    tempFiles.push_back(tempFile);
    return tempFile;
  }
  
  std::wstring CreateTempDirectory(const std::wstring& name)
  {
    wchar_t tempDir[MAX_PATH]{};
    GetTempPath(MAX_PATH, tempDir);
    std::wstring path = std::wstring(tempDir) + name;
    CreateDirectory(path.c_str(), nullptr);
    tempDirs.push_back(path);
    return path;
  }

  std::unique_ptr<TestLogger> logger;
  std::unique_ptr<RCFileHandler> handler;
  std::vector<std::wstring> tempFiles;
  std::vector<std::wstring> tempDirs;
};

TEST_F(FileHandlerErrorTests, LoadFile_NullPath)
{
  std::vector<unsigned char> buffer;
  EXPECT_FALSE(handler->LoadFile(nullptr, 100, buffer));
  EXPECT_NE(ERROR_SUCCESS, handler->Error());
}

TEST_F(FileHandlerErrorTests, LoadFile_EmptyPath)
{
  std::vector<unsigned char> buffer;
  EXPECT_FALSE(handler->LoadFile(L"", 100, buffer));
  EXPECT_NE(ERROR_SUCCESS, handler->Error());
}

TEST_F(FileHandlerErrorTests, LoadFile_InvalidPath)
{
  std::vector<unsigned char> buffer;
  
  std::wstring invalidPath = CreateInvalidPath(L"NonExistent\\Path\\File.rc");
  
  EXPECT_FALSE(handler->LoadFile(invalidPath.c_str(), 100, buffer));
  // Could be either FILE_NOT_FOUND (2) or PATH_NOT_FOUND (3)
  EXPECT_TRUE(handler->Error() == ERROR_FILE_NOT_FOUND || handler->Error() == ERROR_PATH_NOT_FOUND);
}

TEST_F(FileHandlerErrorTests, LoadFile_PathTooLong)
{
  std::vector<unsigned char> buffer;
  std::wstring longPath(MAX_PATH + 100, L'A');
  longPath += L".rc";
  
  EXPECT_FALSE(handler->LoadFile(longPath.c_str(), 100, buffer));
  EXPECT_NE(ERROR_SUCCESS, handler->Error());
}

TEST_F(FileHandlerErrorTests, LoadFile_InvalidCharactersInPath)
{
  std::vector<unsigned char> buffer;
  
  std::wstring invalidPath = CreateInvalidPath(L"Invalid|Characters<>In:Path.rc");
  
  EXPECT_FALSE(handler->LoadFile(invalidPath.c_str(), 100, buffer));
  EXPECT_NE(ERROR_SUCCESS, handler->Error());
}

TEST_F(FileHandlerErrorTests, SaveFile_NullPath)
{
  char testData[] = "test data";
  EXPECT_FALSE(handler->SaveFile(nullptr, testData, sizeof(testData)));
  EXPECT_NE(ERROR_SUCCESS, handler->Error());
}

TEST_F(FileHandlerErrorTests, SaveFile_EmptyPath)
{
  char testData[] = "test data";
  EXPECT_FALSE(handler->SaveFile(L"", testData, sizeof(testData)));
  EXPECT_NE(ERROR_SUCCESS, handler->Error());
}

TEST_F(FileHandlerErrorTests, SaveFile_ReadOnlyDirectory)
{
  std::wstring readOnlyDir = CreateTempDirectory(L"ReadOnlyTestDir");
  SetFileAttributes(readOnlyDir.c_str(), FILE_ATTRIBUTE_READONLY);
  
  std::wstring testFile = readOnlyDir + L"\\testfile.rc";
  
  char testData[] = "test data";
  bool result = handler->SaveFile(testFile.c_str(), testData, sizeof(testData));
  
  // May succeed or fail depending on Windows version and permissions
  // Just verify it doesn't crash
  EXPECT_TRUE(result || handler->Error() != ERROR_SUCCESS);
}

TEST_F(FileHandlerErrorTests, SaveFile_NullBuffer)
{
  std::wstring tempFile = CreateTempFile();
  
  EXPECT_FALSE(handler->SaveFile(tempFile.c_str(), nullptr, 100));
  EXPECT_NE(ERROR_SUCCESS, handler->Error());
}

TEST_F(FileHandlerErrorTests, SaveFile_ZeroBytes)
{
  std::wstring tempFile = CreateTempFile();
  
  char testData[] = "test data";
  bool result = handler->SaveFile(tempFile.c_str(), testData, 0);
  
  // SaveFile may allow writing 0 bytes (creating empty file)
  // This is valid behavior, so just verify it doesn't crash
  EXPECT_TRUE(result || handler->Error() != ERROR_SUCCESS);
}

TEST_F(FileHandlerErrorTests, UpdateFile_BothPathsNull)
{
  EXPECT_FALSE(handler->UpdateFile(nullptr, nullptr, 1, 2, 3, 4));
  EXPECT_NE(ERROR_SUCCESS, handler->Error());
}

TEST_F(FileHandlerErrorTests, UpdateFile_InputPathNull)
{
  EXPECT_FALSE(handler->UpdateFile(nullptr, L"output.rc", 1, 2, 3, 4));
  EXPECT_NE(ERROR_SUCCESS, handler->Error());
}

TEST_F(FileHandlerErrorTests, UpdateFile_OutputPathNull)
{
  EXPECT_FALSE(handler->UpdateFile(L"input.rc", nullptr, 1, 2, 3, 4));
  EXPECT_NE(ERROR_SUCCESS, handler->Error());
}

TEST_F(FileHandlerErrorTests, UpdateFile_NonexistentInput)
{
  std::wstring nonexistentFile = CreateInvalidPath(L"nonexistent.rc");
  std::wstring outputFile = CreateTempFile();
  
  EXPECT_FALSE(handler->UpdateFile(nonexistentFile.c_str(), outputFile.c_str(), 1, 2, 3, 4));
  // Could be either FILE_NOT_FOUND (2) or PATH_NOT_FOUND (3)
  EXPECT_TRUE(handler->Error() == ERROR_FILE_NOT_FOUND || handler->Error() == ERROR_PATH_NOT_FOUND);
}

TEST_F(FileHandlerErrorTests, UpdateFile_CorruptedRCFile)
{
  // Create a temporary file for testing
  wchar_t tempFile[MAX_PATH]{};
  GetTempPath(MAX_PATH, tempFile);
  wcscat_s(tempFile, L"test_corrupted.rc");

  // Create a corrupted RC file (missing proper VERSIONINFO structure)
  FILE* file = _wfopen(tempFile, L"wb");
  if (file)
  {
    const char content[] = "This is not a valid RC file\r\nNo version info here\r\n";
    fwrite(content, 1, sizeof(content) - 1, file);
    fclose(file);

    // Test with corrupted RC file should fail
    EXPECT_FALSE(handler->UpdateFile(tempFile, tempFile, 1, 2, 3, 4));
    EXPECT_EQ(ERROR_FILE_CORRUPT, handler->Error());
    
    DeleteFile(tempFile);
  }
}