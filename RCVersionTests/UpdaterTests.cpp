#include "stdafx.h"
#include "..\RCVersion\RCFileUpdater.h"

class AutoDeleteFiles
{
public:
   std::vector<std::wstring> filesToDelete;
   void Add(LPCWSTR path)
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

};

class Logger : public ILogger
{
public:
   std::wstring messages;
   void Log(const wchar_t* message) override
   {
      messages.append(message);
      messages.append(L"\r\n");
   }
};

TEST(RCFileUpdater, UpdateWcharFileWithBom)
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

   wchar_t temp[MAX_PATH+1] = { 0 };
   GetTempFileName(L".", L"xyz", 0, temp);

   AutoDeleteFiles adf;
   adf.Add(temp);

   FILE*ofile = _wfopen(temp, L"wb");
   fwrite(before, 1, sizeof(before) - sizeof(before[0]), ofile);
   fclose(ofile);

   Logger logger;
   RCFileUpdater updater(logger);
   updater.majorVersion = 12;
   updater.minorVersion = 23;
   updater.buildNumber = 345;
   updater.revision = 45;
   updater.inputFile = temp;
   updater.outputFile = temp;
   updater.verbose = false;

   EXPECT_TRUE(updater.UpdateFile());

   char buffer[sizeof(after) + 256] = { 0 };
   FILE*ifile = _wfopen(temp, L"rb");
   fread(buffer, 1, sizeof(buffer), ifile);
   fclose(ifile);
   
   EXPECT_STREQ(after, buffer);
}

TEST(RCFileUpdater, UpdateWcharFileWithNoBom)
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
   GetTempFileName(L".", L"xyz", 0, temp);

   AutoDeleteFiles adf;
   adf.Add(temp);

   FILE*ofile = _wfopen(temp, L"wb");
   fwrite(before, 1, sizeof(before) - sizeof(before[0]), ofile);
   fclose(ofile);

   Logger logger;
   RCFileUpdater updater(logger);
   updater.majorVersion = 12;
   updater.minorVersion = 23;
   updater.buildNumber = 345;
   updater.revision = 45;
   updater.inputFile = temp;
   updater.outputFile = temp;
   updater.verbose = false;

   EXPECT_TRUE(updater.UpdateFile());

   char buffer[sizeof(after) + 256] = { 0 };
   FILE*ifile = _wfopen(temp, L"rb");
   fread(buffer, 1, sizeof(buffer), ifile);
   fclose(ifile);

   EXPECT_STREQ(after, buffer);
}

