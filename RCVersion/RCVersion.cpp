// RCVersion.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "RCVersionOptions.h"
#include "RCFileUpdater.h"

static const wchar_t szTitle[] = L"RCVersion - Modify version number in a resource RC file";

void Print(const wchar_t* message)
{
   wprintf(L"\n%s", message);
}

int _tmain(int argc, _TCHAR* argv[])
{
   bool verbose = true;
   for (int n=1; n<argc; ++n)
      if (0==_wcsicmp(L"/v:0", argv[n]))
         verbose = false;

   //VersionInfo version;
   //if (verbose)
   //   wprintf(L"%s\nVersion: %s\n", szTitle, version.displayVersion);

   RCVersionOptions options(Print);
   options.verbose = verbose;
   options.Parse(argc ,argv);
   if (!options.Validate())
   {
      wprintf(L"\n%s\n", options.Help);
      return ERROR_INVALID_PARAMETER;
   }

   RCFileUpdater updater(Print);

   updater.majorVersion = options.majorVersion;
   updater.minorVersion = options.minorVersion;
   updater.buildNumber = options.buildNumber;
   updater.revision = options.revision;
   updater.inputFile = options.inputFile;
   updater.outputFile = options.outputFile;
   updater.verbose = verbose;

   bool ok = updater.UpdateFile();

   if (verbose)
      wprintf(L"\n");

   if (ok)
      return 0;

   wprintf(L"\nFile [%s] update failed, error: %u [0x%08X] %s\n", updater.inputFile.c_str(), updater.errorCode, updater.errorCode, updater.errorMessage.c_str());

	return updater.errorCode ? updater.errorCode : 1;
}
