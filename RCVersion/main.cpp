// RCVersion.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "RCVersionOptions.h"
#include "RCFileHandler.h"

static const wchar_t szTitle[] = L"RCVersion - Modify version number in a resource RC file";

class ConsoleLogger : public ILogger
{
   void Log(const wchar_t* message) override
   {
      wprintf(L"%s\n", message);
   }
};

int _tmain(int argc, _TCHAR* argv[])
{
   ConsoleLogger logger;

   bool verbose = true;
   for (int n = 1; n < argc; ++n)
      if (0 == _wcsicmp(L"/v:0", argv[n]))
         verbose = false;

   //VersionInfo version;
   //if (verbose)
   //   wprintf(L"%s\nVersion: %s\n", szTitle, version.displayVersion);

   RCVersionOptions options(logger);
   options.verbose = verbose;
   options.Parse(argc, argv);
   if (!options.Validate())
   {
      wprintf(L"\n%s\n", options.Help);
      return ERROR_INVALID_PARAMETER;
   }

   RCFileHandler handler(logger);
   handler.UpdateFile(options.inputFile.c_str(), options.outputFile.c_str(), options.majorVersion, options.minorVersion, options.buildNumber, options.revision);

   //if (ok)
   //   return 0;

   //wprintf(L"\nFile [%s] update failed, error: %u [0x%08X] %s\n", updater.inputFile.c_str(), updater.errorCode, updater.errorCode, updater.errorMessage.c_str());

   //return updater.errorCode ? updater.errorCode : 1;

   printf("\nDone.\n");
}
