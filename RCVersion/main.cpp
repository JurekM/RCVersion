// RCVersion.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "RCVersionOptions.h"
#include "RCFileUpdater.h"
#include "RCFileHandler.h"

static const wchar_t szTitle[] = L"RCVersion - Modify version number in a resource RC file";

class Logger : public ILogger
{
   void Log(const wchar_t* message) override
   {
      wprintf(L"\n%s", message);
   }
};

int _tmain(int argc, _TCHAR* argv[])
{
   Logger logger;

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

   //RCFileUpdater updater(logger);

   //updater.majorVersion = options.majorVersion;
   //updater.minorVersion = options.minorVersion;
   //updater.buildNumber = options.buildNumber;
   //updater.revision = options.revision;
   //updater.inputFile = options.inputFile;
   //updater.outputFile = options.outputFile;
   //updater.verbose = verbose;

   //bool ok = updater.UpdateFile();

   //if (verbose)
   //   wprintf(L"\n");

   //if (ok)
   //   return 0;

   //wprintf(L"\nFile [%s] update failed, error: %u [0x%08X] %s\n", updater.inputFile.c_str(), updater.errorCode, updater.errorCode, updater.errorMessage.c_str());

   //return updater.errorCode ? updater.errorCode : 1;

   printf("\nDone.\n");
}
