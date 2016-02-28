#include "stdafx.h"
#include "RCFileHandler.h"
#include "TestLogger.h"

class AutoDeleteFiles
{
public:
   std::vector<std::wstring> filesToDelete;
   void Add(wchar_t* path)
   {
      filesToDelete.push_back(path);
   }

   ~AutoDeleteFiles()
   {
      for (auto file : filesToDelete)
      {
         DeleteFile(file.c_str());
      }
   }

   bool MakeTempFileName(wchar_t*path, size_t chars)
   {
      wchar_t dir[MAX_PATH + 1] = {0};
      GetTempPath(_countof(dir), dir);
      wchar_t temp[MAX_PATH + 1] = { 0 };
      GetTempFileName(dir, L"xyz", 0, temp);
      size_t length = wcslen(temp);
      if (chars <= length)
         return false;
      wcsncpy_s(path, chars, temp, _TRUNCATE);
      Add(path);
      return true;
   }

};

TEST(RCFileHandler, UpdateWcharFileWithBom)
{
   wchar_t before[] =
      L"\xFEFF"
      L"// Example resource file with BOM"
      L"\r\n// Some non-ASCII characters: |\x0105\x0107\x0119\x0142\x0144\x00F3\x015B|"
      L"\r\n"
      L"\r\nVS_VERSION_INFO VERSIONINFO"
      L"\r\nFILEVERSION 15,   23,3456, 891"
      L"\r\nPRODUCTVERSION 16, 24, 4567, 892"
      L"\r\nBEGIN"
      L"\r\nBLOCK \"StringFileInfo\""
      L"\r\nBEGIN"
      L"\r\nBLOCK \"040904b0\""
      L"\r\nBEGIN"
      L"\r\nVALUE \"FileVersion\", \"17 . 25.5678  , 893\""
      L"\r\nVALUE \"InternalName\", \"TestFile\""
      L"\r\nVALUE \"ProductVersion\", \"18, 26,6789,894\""
      L"\r\nEND"
      L"\r\nEND"
      L"\r\n"
      ;
   wchar_t after[] =
      L"\xFEFF"
      L"// Example resource file with BOM"
      L"\r\n// Some non-ASCII characters: |\x0105\x0107\x0119\x0142\x0144\x00F3\x015B|"
      L"\r\n"
      L"\r\nVS_VERSION_INFO VERSIONINFO"
      L"\r\nFILEVERSION 12, 23, 345, 45"
      L"\r\nPRODUCTVERSION 12, 23, 345, 45"
      L"\r\nBEGIN"
      L"\r\nBLOCK \"StringFileInfo\""
      L"\r\nBEGIN"
      L"\r\nBLOCK \"040904b0\""
      L"\r\nBEGIN"
      L"\r\nVALUE \"FileVersion\", \"12, 23, 345, 45\""
      L"\r\nVALUE \"InternalName\", \"TestFile\""
      L"\r\nVALUE \"ProductVersion\", \"12, 23, 345, 45\""
      L"\r\nEND"
      L"\r\nEND"
      L"\r\n"
      ;

   wchar_t temp[MAX_PATH+1] = { 0 };
   AutoDeleteFiles adf;
   adf.MakeTempFileName(temp, _countof(temp));

   FILE*ofile = _wfopen(temp, L"wb");
   fwrite(before, 1, sizeof(before) - sizeof(before[0]), ofile);
   fclose(ofile);

   TestLogger logger;
   RCFileHandler handler(logger);

   EXPECT_TRUE(handler.UpdateFile(temp, temp, 12, 23, 345, 45));

   wchar_t buffer[_countof(after) + 256] = { 0 };
   FILE*ifile = _wfopen(temp, L"rb");
   fread(buffer, 1, sizeof(buffer), ifile);
   fclose(ifile);
   
   EXPECT_STREQ(after, buffer);
}

TEST(RCFileHandler, UpdateWcharFileWithNoBom)
{
   wchar_t before[] =
      L"// Example resource file with no BOM"
      L"\r\n// Some non-ASCII characters: |\x0105\x0107\x0119\x0142\x0144\x00F3\x015B|"
      L"\r\n"
      L"\r\nVS_VERSION_INFO VERSIONINFO"
      L"\r\nFILEVERSION 15,   23,3456, 891"
      L"\r\nPRODUCTVERSION 16, 24, 4567, 892"
      L"\r\nBEGIN"
      L"\r\nBLOCK \"StringFileInfo\""
      L"\r\nBEGIN"
      L"\r\nBLOCK \"040904b0\""
      L"\r\nBEGIN"
      L"\r\nVALUE \"FileVersion\", \"17 . 25.5678  , 893\""
      L"\r\nVALUE \"InternalName\", \"TestFile\""
      L"\r\nVALUE \"ProductVersion\", \"18, 26,6789,894\""
      L"\r\nEND"
      L"\r\nEND"
      L"\r\n"
      ;
   wchar_t after[] =
      L"// Example resource file with no BOM"
      L"\r\n// Some non-ASCII characters: |\x0105\x0107\x0119\x0142\x0144\x00F3\x015B|"
      L"\r\n"
      L"\r\nVS_VERSION_INFO VERSIONINFO"
      L"\r\nFILEVERSION 12, 23, 345, 45"
      L"\r\nPRODUCTVERSION 12, 23, 345, 45"
      L"\r\nBEGIN"
      L"\r\nBLOCK \"StringFileInfo\""
      L"\r\nBEGIN"
      L"\r\nBLOCK \"040904b0\""
      L"\r\nBEGIN"
      L"\r\nVALUE \"FileVersion\", \"12, 23, 345, 45\""
      L"\r\nVALUE \"InternalName\", \"TestFile\""
      L"\r\nVALUE \"ProductVersion\", \"12, 23, 345, 45\""
      L"\r\nEND"
      L"\r\nEND"
      L"\r\n"
      ;
   ;

   wchar_t temp[MAX_PATH + 1] = { 0 };
   AutoDeleteFiles adf;
   adf.MakeTempFileName(temp, _countof(temp));

   FILE*ofile = _wfopen(temp, L"wb");
   fwrite(before, 1, sizeof(before) - sizeof(before[0]), ofile);
   fclose(ofile);

   TestLogger logger;
   RCFileHandler handler(logger);

   EXPECT_TRUE(handler.UpdateFile(temp,temp,12,23,345,45));

   wchar_t buffer[_countof(after) + 256] = { 0 };
   FILE*ifile = _wfopen(temp, L"rb");
   fread(buffer, 1, sizeof(buffer), ifile);
   fclose(ifile);

   EXPECT_STREQ(after, buffer);
}

TEST(RCFileHandler, UpdateCharFileWithBom)
{
   char before[] =
      "\xEF\xBB\xBF"
      "// Example resource file with BOM"
      "\r\n// Some non-ASCII characters: |\xC4\x85\xC4\x87\xC4\x99\xC5\x82\xC5\x84\xC3\xB3\xC5\x9B|"
      "\r\n"
      "\r\nVS_VERSION_INFO VERSIONINFO"
      "\r\nFILEVERSION 15,   23,3456, 891"
      "\r\nPRODUCTVERSION 16, 24, 4567, 892"
      "\r\nBEGIN"
      "\r\nBLOCK \"StringFileInfo\""
      "\r\nBEGIN"
      "\r\nBLOCK \"040904b0\""
      "\r\nBEGIN"
      "\r\nVALUE \"FileVersion\", \"17 . 25.5678  , 893\""
      "\r\nVALUE \"InternalName\", \"TestFile\""
      "\r\nVALUE \"ProductVersion\", \"18, 26,6789,894\""
      "\r\nEND"
      "\r\nEND"
      "\r\n"
      ;
   char after[] =
      "\xEF\xBB\xBF"
      "// Example resource file with BOM"
      "\r\n// Some non-ASCII characters: |\xC4\x85\xC4\x87\xC4\x99\xC5\x82\xC5\x84\xC3\xB3\xC5\x9B|"
      "\r\n"
      "\r\nVS_VERSION_INFO VERSIONINFO"
      "\r\nFILEVERSION 12, 23, 345, 45"
      "\r\nPRODUCTVERSION 12, 23, 345, 45"
      "\r\nBEGIN"
      "\r\nBLOCK \"StringFileInfo\""
      "\r\nBEGIN"
      "\r\nBLOCK \"040904b0\""
      "\r\nBEGIN"
      "\r\nVALUE \"FileVersion\", \"12, 23, 345, 45\""
      "\r\nVALUE \"InternalName\", \"TestFile\""
      "\r\nVALUE \"ProductVersion\", \"12, 23, 345, 45\""
      "\r\nEND"
      "\r\nEND"
      "\r\n"
      ;

   wchar_t temp[MAX_PATH + 1] = { 0 };
   AutoDeleteFiles adf;
   adf.MakeTempFileName(temp, _countof(temp));

   FILE*ofile = _wfopen(temp, L"wb");
   fwrite(before, 1, sizeof(before) - sizeof(before[0]), ofile);
   fclose(ofile);

   TestLogger logger;
   RCFileHandler handler(logger);

   EXPECT_TRUE(handler.UpdateFile(temp, temp, 12, 23, 345, 45));

   char buffer[_countof(after) + 256] = { 0 };
   FILE*ifile = _wfopen(temp, L"rb");
   fread(buffer, 1, sizeof(buffer), ifile);
   fclose(ifile);

   EXPECT_STREQ(after, buffer);
}

TEST(RCFileHandler, UpdateCharFileWithNoBom)
{
   char before[] =
      "// Example resource file with no BOM"
      "\r\n// Some non-ASCII characters: |\xC4\x85\xC4\x87\xC4\x99\xC5\x82\xC5\x84\xC3\xB3\xC5\x9B|"
      "\r\n"
      "\r\nVS_VERSION_INFO VERSIONINFO"
      "\r\nFILEVERSION 15,   23,3456, 891"
      "\r\nPRODUCTVERSION 16, 24, 4567, 892"
      "\r\nBEGIN"
      "\r\nBLOCK \"StringFileInfo\""
      "\r\nBEGIN"
      "\r\nBLOCK \"040904b0\""
      "\r\nBEGIN"
      "\r\nVALUE \"FileVersion\", \"17 . 25.5678  , 893\""
      "\r\nVALUE \"InternalName\", \"TestFile\""
      "\r\nVALUE \"ProductVersion\", \"18, 26,6789,894\""
      "\r\nEND"
      "\r\nEND"
      "\r\n"
      ;
   char after[] =
      "// Example resource file with no BOM"
      "\r\n// Some non-ASCII characters: |\xC4\x85\xC4\x87\xC4\x99\xC5\x82\xC5\x84\xC3\xB3\xC5\x9B|"
      "\r\n"
      "\r\nVS_VERSION_INFO VERSIONINFO"
      "\r\nFILEVERSION 12, 23, 345, 45"
      "\r\nPRODUCTVERSION 12, 23, 345, 45"
      "\r\nBEGIN"
      "\r\nBLOCK \"StringFileInfo\""
      "\r\nBEGIN"
      "\r\nBLOCK \"040904b0\""
      "\r\nBEGIN"
      "\r\nVALUE \"FileVersion\", \"12, 23, 345, 45\""
      "\r\nVALUE \"InternalName\", \"TestFile\""
      "\r\nVALUE \"ProductVersion\", \"12, 23, 345, 45\""
      "\r\nEND"
      "\r\nEND"
      "\r\n"
      ;

   wchar_t temp[MAX_PATH + 1] = { 0 };
   AutoDeleteFiles adf;
   adf.MakeTempFileName(temp, _countof(temp));

   FILE*ofile = _wfopen(temp, L"wb");
   fwrite(before, 1, sizeof(before) - sizeof(before[0]), ofile);
   fclose(ofile);

   TestLogger logger;
   RCFileHandler handler(logger);

   EXPECT_TRUE(handler.UpdateFile(temp, temp, 12, 23, 345, 45));

   char buffer[_countof(after) + 256] = { 0 };
   FILE*ifile = _wfopen(temp, L"rb");
   fread(buffer, 1, sizeof(buffer), ifile);
   fclose(ifile);

   EXPECT_STREQ(after, buffer);
}