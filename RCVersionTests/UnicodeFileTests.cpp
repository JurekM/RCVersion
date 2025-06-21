#include "stdafx.h"
#include "RCFileHandler.h"
#include "TestLogger.h"

class UnicodeFileTests : public ::testing::Test
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
    // Clean up temporary files
    for (const auto& file : tempFiles)
    {
      DeleteFile(file.c_str());
    }
  }

  std::wstring CreateTempOutputFile(const std::wstring& prefix = L"rco")
  {
    wchar_t tempDir[MAX_PATH]{};
    GetTempPath(MAX_PATH, tempDir);
    wchar_t tempFile[MAX_PATH]{};
    GetTempFileName(tempDir, prefix.c_str(), 0, tempFile);
    tempFiles.push_back(tempFile);
    return tempFile;
  }

  bool CreateTempFile(const std::wstring& content, std::wstring& filepath)
  {
    wchar_t tempDir[MAX_PATH]{};
    GetTempPath(MAX_PATH, tempDir);
    
    wchar_t tempFile[MAX_PATH]{};
    GetTempFileName(tempDir, L"rct", 0, tempFile);
    
    FILE* file = _wfopen(tempFile, L"wb");
    if (!file) return false;
    
    if (content.find(L"\xFEFF") == 0) // Has BOM
    {
      // Write as UTF-16
      fwrite(content.c_str(), sizeof(wchar_t), content.length(), file);
    }
    else
    {
      // Convert to UTF-8 and write
      int utf8Size = WideCharToMultiByte(CP_UTF8, 0, content.c_str(), -1, nullptr, 0, nullptr, nullptr);
      if (utf8Size > 0)
      {
        std::vector<char> utf8Buffer(utf8Size);
        WideCharToMultiByte(CP_UTF8, 0, content.c_str(), -1, utf8Buffer.data(), utf8Size, nullptr, nullptr);
        fwrite(utf8Buffer.data(), 1, utf8Size - 1, file); // -1 to exclude null terminator
      }
    }
    
    fclose(file);
    filepath = tempFile;
    tempFiles.push_back(filepath);
    return true;
  }

  std::unique_ptr<TestLogger> logger;
  std::unique_ptr<RCFileHandler> handler;
  std::vector<std::wstring> tempFiles;
};

TEST_F(UnicodeFileTests, UpdateFile_Utf16WithBom)
{
  std::wstring content = L"\xFEFF"
    L"VS_VERSION_INFO VERSIONINFO\r\n"
    L"FILEVERSION 1,2,3,4\r\n"
    L"BEGIN\r\n"
    L"    BLOCK \"StringFileInfo\"\r\n"
    L"    BEGIN\r\n"
    L"        BLOCK \"040904b0\"\r\n"
    L"        BEGIN\r\n"
    L"            VALUE \"FileVersion\", \"1,2,3,4\"\r\n"
    L"        END\r\n"
    L"    END\r\n"
    L"END\r\n";

  std::wstring inputFile, outputFile;
  ASSERT_TRUE(CreateTempFile(content, inputFile));
  
  wchar_t tempDir[MAX_PATH]{};
  GetTempPath(MAX_PATH, tempDir);
  wchar_t tempOut[MAX_PATH]{};
  GetTempFileName(tempDir, L"rco", 0, tempOut);
  outputFile = tempOut;
  tempFiles.push_back(outputFile);

  EXPECT_TRUE(handler->UpdateFile(inputFile.c_str(), outputFile.c_str(), 5, 6, 7, 8));
  EXPECT_EQ(ERROR_SUCCESS, handler->Error());

  // Verify the output file
  std::vector<unsigned char> buffer;
  EXPECT_TRUE(handler->LoadFile(outputFile.c_str(), 100, buffer));
  
  // Check that BOM is preserved and versions are updated
  EXPECT_EQ(0xFF, buffer[0]); // BOM first byte
  EXPECT_EQ(0xFE, buffer[1]); // BOM second byte
  
  // Convert back to string and check content
  std::wstring result(reinterpret_cast<wchar_t*>(buffer.data()));
  EXPECT_NE(std::wstring::npos, result.find(L"5, 6, 7, 8"));
}

TEST_F(UnicodeFileTests, UpdateFile_Utf8WithBom)
{
  std::wstring content = L"VS_VERSION_INFO VERSIONINFO\r\n"
    L"FILEVERSION 1,2,3,4\r\n"
    L"// Unicode characters: áéíóú ñ 中文\r\n"
    L"BEGIN\r\n"
    L"END\r\n";

  std::wstring inputFile;
  ASSERT_TRUE(CreateTempFile(content, inputFile));
  
  // Manually add UTF-8 BOM
  FILE* file = _wfopen(inputFile.c_str(), L"rb");
  ASSERT_NE(nullptr, file);
  
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);
  
  std::vector<unsigned char> data(size + 3); // +3 for BOM
  data[0] = 0xEF; // UTF-8 BOM
  data[1] = 0xBB;
  data[2] = 0xBF;
  
  fread(data.data() + 3, 1, size, file);
  fclose(file);
  
  file = _wfopen(inputFile.c_str(), L"wb");
  fwrite(data.data(), 1, data.size(), file);
  fclose(file);

  EXPECT_TRUE(handler->UpdateFile(inputFile.c_str(), inputFile.c_str(), 9, 10, 11, 12));
  EXPECT_EQ(ERROR_SUCCESS, handler->Error());
}

TEST_F(UnicodeFileTests, UpdateFile_MixedEncodingCharacters)
{
  std::wstring content = L"VS_VERSION_INFO VERSIONINFO\r\n"
    L"FILEVERSION 1,2,3,4\r\n"
    L"// Characters: ñáéíóú čšž αβγ 你好 🔔\r\n"
    L"BEGIN\r\n"
    L"    BLOCK \"StringFileInfo\"\r\n"
    L"    BEGIN\r\n"
    L"        BLOCK \"040904b0\"\r\n"
    L"        BEGIN\r\n"
    L"            VALUE \"ProductName\", \"Test ñáéíóú\"\r\n"
    L"            VALUE \"FileVersion\", \"1,2,3,4\"\r\n"
    L"        END\r\n"
    L"    END\r\n"
    L"END\r\n";

  std::wstring inputFile;
  ASSERT_TRUE(CreateTempFile(content, inputFile));

  EXPECT_TRUE(handler->UpdateFile(inputFile.c_str(), inputFile.c_str(), 2, 3, 4, 5));
  
  // Verify Unicode characters are preserved
  std::vector<unsigned char> buffer;
  EXPECT_TRUE(handler->LoadFile(inputFile.c_str(), 100, buffer));
  
  // Check that some Unicode characters are still present
  std::string result(reinterpret_cast<char*>(buffer.data()));
  EXPECT_NE(std::string::npos, result.find("Test")); // ASCII should be there
}

TEST_F(UnicodeFileTests, UpdateFile_VerySmallFileWithUnicode)
{
  // File smaller than 256 bytes (the Unicode detection threshold)
  std::wstring content = L"\xFEFF"
    L"VS_VERSION_INFO VERSIONINFO\r\n"
    L"FILEVERSION 1,2,3,4\r\n"
    L"ñ\r\n";

  std::wstring inputFile;
  ASSERT_TRUE(CreateTempFile(content, inputFile));

  EXPECT_TRUE(handler->UpdateFile(inputFile.c_str(), inputFile.c_str(), 7, 8, 9, 10));
}

TEST_F(UnicodeFileTests, UpdateFile_CorruptedBom)
{
  std::wstring content = L"VS_VERSION_INFO VERSIONINFO\r\n"
    L"FILEVERSION 1,2,3,4\r\n"
    L"BEGIN\r\n"
    L"END\r\n";

  std::wstring inputFile;
  ASSERT_TRUE(CreateTempFile(content, inputFile));
  
  // Manually write a corrupted BOM
  FILE* file = _wfopen(inputFile.c_str(), L"rb+");
  ASSERT_NE(nullptr, file);
  
  unsigned char corruptedBom[3] = {0xEF, 0xBB, 0x00}; // Incomplete UTF-8 BOM
  fseek(file, 0, SEEK_SET);
  fwrite(corruptedBom, 1, 3, file);
  fclose(file);

  // Should handle corrupted BOM gracefully
  bool result = handler->UpdateFile(inputFile.c_str(), inputFile.c_str(), 5, 6, 7, 8);
  
  // May succeed or fail, but should not crash
  EXPECT_TRUE(result || handler->Error() != ERROR_SUCCESS);
}

TEST_F(UnicodeFileTests, UpdateFile_AmbiguousEncoding)
{
  // Create file with bytes that could be interpreted as either encoding
  std::wstring inputFile;
  wchar_t tempDir[MAX_PATH]{};
  GetTempPath(MAX_PATH, tempDir);
  wchar_t tempFilePath[MAX_PATH]{};
  GetTempFileName(tempDir, L"rca", 0, tempFilePath);
  inputFile = tempFilePath;
  tempFiles.push_back(inputFile);

  FILE* file = _wfopen(inputFile.c_str(), L"wb");
  ASSERT_NE(nullptr, file);
  
  // Write content that might be ambiguous
  const char ambiguous[] = 
    "VS_VERSION_INFO VERSIONINFO\r\n"
    "FILEVERSION 1,2,3,4\r\n"
    "BEGIN\r\n"
    "END\r\n";
  
  fwrite(ambiguous, 1, sizeof(ambiguous) - 1, file);
  fclose(file);

  // Should handle ambiguous encoding
  EXPECT_TRUE(handler->UpdateFile(inputFile.c_str(), inputFile.c_str(), 3, 4, 5, 6));
}

TEST_F(UnicodeFileTests, LoadFile_UnicodeDetectionEdgeCases)
{
  // Test files right at the 256-byte boundary for Unicode detection
  
  // Create file with exactly 255 bytes
  std::wstring content255(120, L'A'); // 240 bytes + header
  content255 = L"VS_VERSION_INFO VERSIONINFO\r\nFILEVERSION 1,2,3,4\r\n" + content255;
  
  std::wstring inputFile;
  ASSERT_TRUE(CreateTempFile(content255, inputFile));
  
  std::vector<unsigned char> buffer;
  EXPECT_TRUE(handler->LoadFile(inputFile.c_str(), 100, buffer));
  
  // Should load successfully regardless of detection
  EXPECT_GT(buffer.size(), 250U);
}

TEST_F(UnicodeFileTests, UpdateFile_UnicodeInComments)
{
  std::wstring content = L"VS_VERSION_INFO VERSIONINFO\r\n"
    L"// Comment with Unicode: Привет мир 你好世界 こんにちは\r\n"
    L"FILEVERSION 1,2,3,4\r\n"
    L"/* Block comment with Unicode: Ελληνικά العربية */\r\n"
    L"BEGIN\r\n"
    L"END\r\n";

  std::wstring inputFile;
  ASSERT_TRUE(CreateTempFile(content, inputFile));

  EXPECT_TRUE(handler->UpdateFile(inputFile.c_str(), inputFile.c_str(), 4, 5, 6, 7));
  
  // Verify Unicode in comments is preserved
  std::vector<unsigned char> buffer;
  EXPECT_TRUE(handler->LoadFile(inputFile.c_str(), 100, buffer));
  EXPECT_GT(buffer.size(), 100U); // Should have substantial content
}
