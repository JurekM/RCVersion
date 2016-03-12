#include "stdafx.h"
#include "RCVersionOptions.h"
#include "TestLogger.h"


// L"\nSyntax: RCVersion <resource-file.rc> [<options>]"
// L"\n /b:<build-number>  new build number, default: increment by one"
// L"\n /m:<major-version> new major version, default: unchanged"
// L"\n /n:<minor-version> new minor version, default: unchanged"
// L"\n /r:<revision>      new revision number, default: 0"
// L"\n /o:<output-file>   output file path, default: same as input"
// L"\n /v:{0|1}           verbose, default on"
// L"\n"
// L"\n"

TEST(RCVersionOptions, AllOptions1)
{
   TestLogger logger;
   RCVersionOptions vo(logger);

   wchar_t* argv[] = {
      L"",
      L"/m:11",
      L"/n:22",
      L"/b:33",
      L"/r:44",
      L"/v:0",
      L"/o:outfile.txt",
      L"..\\test-in.rc",
   };

   EXPECT_TRUE(vo.Parse(_countof(argv), argv));
   EXPECT_TRUE(vo.Validate()) << logger.messages;
   EXPECT_EQ(11, vo.majorVersion);
   EXPECT_EQ(22, vo.minorVersion);
   EXPECT_EQ(33, vo.buildNumber);
   EXPECT_EQ(44, vo.revision);
   EXPECT_EQ(0, vo.verbosity);
   EXPECT_EQ(L"outfile.txt", vo.outputFile);
   EXPECT_EQ(L"..\\test-in.rc", vo.inputFile);
}


TEST(RCVersionOptions, AllOptions2)
{
   TestLogger logger;
   RCVersionOptions vo(logger);

   wchar_t* argv[] = {
      L"",
      L"..\\test-in.rc",
      L"/o:outfile.txt",
      L"/v:3",
      L"/r:41",
      L"/b:31",
      L"/n:21",
      L"/m:10",
   };

   EXPECT_TRUE(vo.Parse(_countof(argv), argv));
   EXPECT_TRUE(vo.Validate());
   EXPECT_EQ(10, vo.majorVersion);
   EXPECT_EQ(21, vo.minorVersion);
   EXPECT_EQ(31, vo.buildNumber);
   EXPECT_EQ(41, vo.revision);
   EXPECT_EQ(3, vo.verbosity);
   EXPECT_EQ(L"outfile.txt", vo.outputFile);
   EXPECT_EQ(L"..\\test-in.rc", vo.inputFile);
}

TEST(RCVersionOptions, MinimumOptions)
{
   TestLogger logger;
   RCVersionOptions vo(logger);

   wchar_t* argv[] = {
      L"",
      L"..\\test-in.rc",
   };

   EXPECT_TRUE(vo.Parse(_countof(argv), argv));
   EXPECT_TRUE(vo.Validate());
   EXPECT_EQ(-1, vo.majorVersion);
   EXPECT_EQ(-1, vo.minorVersion);
   EXPECT_EQ(-1, vo.buildNumber);
   EXPECT_EQ(-1, vo.revision);
   EXPECT_EQ(3, vo.verbosity);
   EXPECT_EQ(L"..\\test-in.rc", vo.outputFile);
   EXPECT_EQ(L"..\\test-in.rc", vo.inputFile);
}

TEST(RCVersionOptions, BadOptions)
{
   TestLogger logger;
   RCVersionOptions vo(logger);

   wchar_t* argv[] = {
      L"",
      L"..\\test-in.rc",
      L"/z:zara",
      L"/ciara",
   };

   EXPECT_FALSE(vo.Parse(_countof(argv), argv));
   EXPECT_NE(nullptr, wcsstr(logger.messages.c_str(),L"/z:zara"));
   EXPECT_NE(nullptr, wcsstr(logger.messages.c_str(),L"/ciara"));
}

TEST(RCVersionOptions, DuplicateFileName)
{
   TestLogger logger;
   RCVersionOptions vo(logger);

   wchar_t* argv[] = {
      L"",
      L"..\\test-in.rc",
      L"duplicate.txt",
   };

   EXPECT_FALSE(vo.Parse(_countof(argv), argv));
   EXPECT_NE(nullptr, wcsstr(logger.messages.c_str(),L"test-in.rc"));
   EXPECT_NE(nullptr, wcsstr(logger.messages.c_str(),L"duplicate.txt"));
}

TEST(RCVersionOptions, MissingFileName)
{
   TestLogger logger;
   RCVersionOptions vo(logger);

   wchar_t* argv[] = {
      L"",
      L"/v:1",
      L"/o:duplicate.txt",
   };

   EXPECT_TRUE(vo.Parse(_countof(argv), argv));
   EXPECT_FALSE(vo.Validate());
}
