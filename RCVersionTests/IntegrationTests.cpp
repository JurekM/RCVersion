#include "stdafx.h"
#include "RCVersionOptions.h"
#include "RCFileHandler.h"
#include "TestLogger.h"

class IntegrationTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    logger = std::make_unique<TestLogger>();
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
  
  std::wstring CreateNonExistentPath(const std::wstring& subPath)
  {
    wchar_t tempDir[MAX_PATH]{};
    GetTempPath(MAX_PATH, tempDir);
    return std::wstring(tempDir) + subPath;
  }
  
  std::wstring GetTempBasePath()
  {
    wchar_t tempDir[MAX_PATH]{};
    GetTempPath(MAX_PATH, tempDir);
    return tempDir;
  }

  bool CreateTempRCFile(const std::string& content, std::wstring& filepath)
  {
    wchar_t tempDir[MAX_PATH]{};
    GetTempPath(MAX_PATH, tempDir);
    
    wchar_t tempFile[MAX_PATH]{};
    GetTempFileName(tempDir, L"rci", 0, tempFile);
    
    FILE* file = _wfopen(tempFile, L"wb");
    if (!file) return false;
    
    fwrite(content.c_str(), 1, content.length(), file);
    fclose(file);
    
    filepath = tempFile;
    tempFiles.push_back(filepath);
    return true;
  }

  std::unique_ptr<TestLogger> logger;
  std::vector<std::wstring> tempFiles;
};

TEST_F(IntegrationTests, EndToEnd_SimpleRCFile)
{
  std::string rcContent = 
    "// Test RC file\r\n"
    "VS_VERSION_INFO VERSIONINFO\r\n"
    "FILEVERSION 1,0,0,1\r\n"
    "PRODUCTVERSION 1,0,0,1\r\n"
    "BEGIN\r\n"
    "    BLOCK \"StringFileInfo\"\r\n"
    "    BEGIN\r\n"
    "        BLOCK \"040904b0\"\r\n"
    "        BEGIN\r\n"
    "            VALUE \"FileVersion\", \"1.0.0.1\"\r\n"
    "            VALUE \"ProductVersion\", \"1.0.0.1\"\r\n"
    "        END\r\n"
    "    END\r\n"
    "END\r\n";

  std::wstring inputFile;
  ASSERT_TRUE(CreateTempRCFile(rcContent, inputFile));

  RCVersionOptions options{*logger};
  RCFileHandler handler{*logger};
  
  // Simulate command line: program.exe input.rc /m:2 /n:1 /b:5 /r:3
  const wchar_t* argv[] = {
    L"RCVersion.exe",
    inputFile.c_str(),
    L"/m:2",
    L"/n:1", 
    L"/b:5",
    L"/r:3"
  };
  
  EXPECT_TRUE(options.Parse(6, argv));
  EXPECT_TRUE(options.Validate());
  
  EXPECT_EQ(2, options.majorVersion);
  EXPECT_EQ(1, options.minorVersion);
  EXPECT_EQ(5, options.buildNumber);
  EXPECT_EQ(3, options.revision);
  EXPECT_EQ(inputFile, options.inputFile);
  EXPECT_EQ(inputFile, options.outputFile); // Should default to input file

  // Test the actual file update
  EXPECT_TRUE(handler.UpdateFile(
    options.inputFile.c_str(),
    options.outputFile.c_str(),
    options.majorVersion,
    options.minorVersion,
    options.buildNumber,
    options.revision));

  // Verify the results
  std::vector<unsigned char> buffer;
  EXPECT_TRUE(handler.LoadFile(inputFile.c_str(), 100, buffer));
  
  std::string result(reinterpret_cast<char*>(buffer.data()));
  EXPECT_NE(std::string::npos, result.find("2, 1, 5, 3"));
}

TEST_F(IntegrationTests, EndToEnd_OnlyBuildNumberIncrement)
{
  std::string rcContent = 
    "VS_VERSION_INFO VERSIONINFO\r\n"
    "FILEVERSION 1,2,100,4\r\n"
    "BEGIN\r\n"
    "END\r\n";

  std::wstring inputFile;
  ASSERT_TRUE(CreateTempRCFile(rcContent, inputFile));

  RCVersionOptions options{*logger};
  RCFileHandler handler{*logger};
  
  // Only specify the input file (build number should auto-increment)
  const wchar_t* argv[] = {
    L"RCVersion.exe",
    inputFile.c_str()
  };
  
  EXPECT_TRUE(options.Parse(2, argv));
  EXPECT_TRUE(options.Validate());
  
  // All should be -1 except auto-incrementing build number
  EXPECT_EQ(-1, options.majorVersion);
  EXPECT_EQ(-1, options.minorVersion);
  EXPECT_EQ(-1, options.buildNumber); // -1 means auto-increment
  EXPECT_EQ(-1, options.revision);

  EXPECT_TRUE(handler.UpdateFile(
    options.inputFile.c_str(),
    options.outputFile.c_str(),
    options.majorVersion,
    options.minorVersion,
    options.buildNumber,
    options.revision));

  // Verify build number was incremented (100 -> 101)
  std::vector<unsigned char> buffer;
  EXPECT_TRUE(handler.LoadFile(inputFile.c_str(), 100, buffer));
  
  std::string result(reinterpret_cast<char*>(buffer.data()));
  EXPECT_NE(std::string::npos, result.find("1, 2, 101, 4"));
}

TEST_F(IntegrationTests, EndToEnd_SeparateOutputFile)
{
  std::string rcContent = 
    "VS_VERSION_INFO VERSIONINFO\r\n"
    "FILEVERSION 1,0,0,1\r\n"
    "BEGIN\r\n"
    "END\r\n";

  std::wstring inputFile;
  ASSERT_TRUE(CreateTempRCFile(rcContent, inputFile));
  
  std::wstring outputFile = CreateTempOutputFile();

  RCVersionOptions options{*logger};
  RCFileHandler handler{*logger};
  
  std::wstring outputArg = L"/o:" + outputFile;
  const wchar_t* argv[] = {
    L"RCVersion.exe",
    inputFile.c_str(),
    outputArg.c_str(),
    L"/m:5",
    L"/n:4",
    L"/b:3",
    L"/r:2"
  };
  
  EXPECT_TRUE(options.Parse(7, argv));
  EXPECT_TRUE(options.Validate());
  
  EXPECT_EQ(inputFile, options.inputFile);
  EXPECT_EQ(outputFile, options.outputFile);

  EXPECT_TRUE(handler.UpdateFile(
    options.inputFile.c_str(),
    options.outputFile.c_str(),
    options.majorVersion,
    options.minorVersion,
    options.buildNumber,
    options.revision));

  // Verify original file is unchanged
  std::vector<unsigned char> originalBuffer;
  EXPECT_TRUE(handler.LoadFile(inputFile.c_str(), 100, originalBuffer));
  std::string original(reinterpret_cast<char*>(originalBuffer.data()));
  EXPECT_NE(std::string::npos, original.find("1,0,0,1"));

  // Verify output file has new versions
  std::vector<unsigned char> outputBuffer;
  EXPECT_TRUE(handler.LoadFile(outputFile.c_str(), 100, outputBuffer));
  std::string output(reinterpret_cast<char*>(outputBuffer.data()));
  EXPECT_NE(std::string::npos, output.find("5, 4, 3, 2"));
}

TEST_F(IntegrationTests, EndToEnd_VerbosityLevels)
{
  std::string rcContent = 
    "VS_VERSION_INFO VERSIONINFO\r\n"
    "FILEVERSION 1,0,0,1\r\n"
    "BEGIN\r\n"
    "END\r\n";

  // Test different verbosity levels
  for (int verbosity = 0; verbosity <= 9; ++verbosity)
  {
    // Create a separate file for each verbosity test
    std::wstring inputFile;
    ASSERT_TRUE(CreateTempRCFile(rcContent, inputFile));
    
    TestLogger testLogger{};
    RCVersionOptions options{testLogger};
    RCFileHandler handler{testLogger};
    
    std::wstring verbosityArg = L"/v:" + std::to_wstring(verbosity);
    const wchar_t* argv[] = {
      L"RCVersion.exe",
      inputFile.c_str(),
      verbosityArg.c_str(),
      L"/m:1"
    };
    
    EXPECT_TRUE(options.Parse(4, argv));
    EXPECT_TRUE(options.Validate());
    EXPECT_EQ(verbosity, options.verbosity);
    
    handler.Verbosity(options.verbosity);
    bool result = handler.UpdateFile(
      options.inputFile.c_str(),
      options.outputFile.c_str(),
      options.majorVersion,
      options.minorVersion,
      options.buildNumber,
      options.revision);
    
    if (!result)
    {
      // Log the error for debugging
      printf("UpdateFile failed for verbosity %d, error: %u\n", verbosity, handler.Error());
    }
    EXPECT_TRUE(result);
    
    // Check for log messages based on verbosity level
    // logNormal=2, so verbosity >= 2 should produce log messages
    // logDetail=5, so verbosity >= 5 should produce more detailed messages
    if (verbosity >= 2)
    {
      EXPECT_FALSE(testLogger.messages.empty()) << "Expected log messages for verbosity " << verbosity;
    }
    else
    {
      // For verbosity 0 and 1, there might not be any messages since logNormal=2
      // This is okay - just verify the test doesn't crash
    }
  }
}

TEST_F(IntegrationTests, EndToEnd_ErrorHandling)
{
  RCVersionOptions options{*logger};
  
  std::wstring nonExistentFile = CreateNonExistentPath(L"NonExistent\\File.rc");
  
  // Test with non-existent file
  const wchar_t* argv1[] = {
    L"RCVersion.exe",
    nonExistentFile.c_str()
  };
  
  EXPECT_TRUE(options.Parse(2, argv1));
  EXPECT_TRUE(options.Validate());
  
  RCFileHandler handler{*logger};
  EXPECT_FALSE(handler.UpdateFile(
    options.inputFile.c_str(),
    options.outputFile.c_str(),
    1, 2, 3, 4));
  
  // Could be either FILE_NOT_FOUND (2) or PATH_NOT_FOUND (3)
  EXPECT_TRUE(handler.Error() == ERROR_FILE_NOT_FOUND || handler.Error() == ERROR_PATH_NOT_FOUND);
}

TEST_F(IntegrationTests, EndToEnd_EnvironmentVariableExpansion)
{
  std::wstring testDirectory = GetTempBasePath() + L"TestDirectory";
  
  // Set up test environment variable
  SetEnvironmentVariable(L"TEST_RC_DIR", testDirectory.c_str());
  
  std::string rcContent = 
    "VS_VERSION_INFO VERSIONINFO\r\n"
    "FILEVERSION 1,0,0,1\r\n"
    "BEGIN\r\n"
    "END\r\n";

  std::wstring inputFile;
  ASSERT_TRUE(CreateTempRCFile(rcContent, inputFile));

  RCVersionOptions options{*logger};
  
  const wchar_t* argv[] = {
    L"RCVersion.exe",
    inputFile.c_str(),
    L"/o:%TEST_RC_DIR%\\output.rc"  // Use Windows % syntax instead of $(...)
  };
  
  EXPECT_TRUE(options.Parse(3, argv));
  EXPECT_TRUE(options.Validate());
  
  // The output file should have environment variable expanded
  std::wstring expectedOutput = testDirectory + L"\\output.rc";
  EXPECT_EQ(expectedOutput, options.outputFile);
  
  // Cleanup
  SetEnvironmentVariable(L"TEST_RC_DIR", nullptr);
}

TEST_F(IntegrationTests, EndToEnd_ComplexRCFileWithMultipleBlocks)
{
  std::string rcContent = 
    "// Complex RC file with multiple version blocks\r\n"
    "#include \"resource.h\"\r\n"
    "\r\n"
    "VS_VERSION_INFO VERSIONINFO\r\n"
    "FILEVERSION 1,0,0,1\r\n"
    "PRODUCTVERSION 1,0,0,1\r\n"
    "FILEFLAGSMASK 0x3fL\r\n"
    "FILEFLAGS 0x0L\r\n"
    "FILEOS 0x40004L\r\n"
    "FILETYPE 0x1L\r\n"
    "FILESUBTYPE 0x0L\r\n"
    "BEGIN\r\n"
    "    BLOCK \"StringFileInfo\"\r\n"
    "    BEGIN\r\n"
    "        BLOCK \"040904b0\"\r\n"
    "        BEGIN\r\n"
    "            VALUE \"CompanyName\", \"Test Company\"\r\n"
    "            VALUE \"FileDescription\", \"Test Application\"\r\n"
    "            VALUE \"FileVersion\", \"1.0.0.1\"\r\n"
    "            VALUE \"InternalName\", \"TestApp\"\r\n"
    "            VALUE \"LegalCopyright\", \"Copyright (C) 2024\"\r\n"
    "            VALUE \"OriginalFilename\", \"TestApp.exe\"\r\n"
    "            VALUE \"ProductName\", \"Test Product\"\r\n"
    "            VALUE \"ProductVersion\", \"1.0.0.1\"\r\n"
    "        END\r\n"
    "    END\r\n"
    "    BLOCK \"VarFileInfo\"\r\n"
    "    BEGIN\r\n"
    "        VALUE \"Translation\", 0x409, 1200\r\n"
    "    END\r\n"
    "END\r\n"
    "\r\n"
    "// Other resources\r\n"
    "IDI_ICON1 ICON \"app.ico\"\r\n";

  std::wstring inputFile;
  ASSERT_TRUE(CreateTempRCFile(rcContent, inputFile));

  RCVersionOptions options{*logger};
  RCFileHandler handler{*logger};
  handler.Verbosity(9); // Maximum verbosity for detailed logging
  
  const wchar_t* argv[] = {
    L"RCVersion.exe",
    inputFile.c_str(),
    L"/m:2",
    L"/n:1",
    L"/b:4",
    L"/r:7"
  };
  
  EXPECT_TRUE(options.Parse(6, argv));
  EXPECT_TRUE(options.Validate());

  EXPECT_TRUE(handler.UpdateFile(
    options.inputFile.c_str(),
    options.outputFile.c_str(),
    options.majorVersion,
    options.minorVersion,
    options.buildNumber,
    options.revision));

  // Verify all version locations were updated
  std::vector<unsigned char> buffer;
  EXPECT_TRUE(handler.LoadFile(inputFile.c_str(), 100, buffer));
  
  std::string result(reinterpret_cast<char*>(buffer.data()));
  
  // Should find the new version in multiple places
  size_t count = 0;
  size_t pos = 0;
  while ((pos = result.find("2, 1, 4, 7", pos)) != std::string::npos)
  {
    count++;
    pos += 10;
  }
  
  EXPECT_GE(count, 2U); // Should update both FILEVERSION and string values
  
  // Verify other content is preserved
  EXPECT_NE(std::string::npos, result.find("Test Company"));
  EXPECT_NE(std::string::npos, result.find("IDI_ICON1"));
}
