// RCVersion.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "RCVersionOptions.h"
#include "RCFileHandler.h"
#include "Logger.h"

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
   ConsoleLogger clogger;
   Logger logger(clogger);

   bool verbose = false;
   for (int n = 1; n < argc; ++n)
   {
      if (0 == _wcsicmp(L"/v:0", argv[n]))
         verbose = false;
      if (0 == _wcsicmp(L"/v:1", argv[n]))
         verbose = true;
   }

   //VersionInfo version;
   //if (verbose)
   //   wprintf(L"%s\nVersion: %s\n", szTitle, version.displayVersion);

   RCVersionOptions options(clogger);
   options.verbose = verbose;
   options.Parse(argc, argv);
   if (!options.Validate())
   {
      wprintf(L"\n%s\n", options.Help);
      return ERROR_INVALID_PARAMETER;
   }

   RCFileHandler handler(clogger);
   handler.Verbose(verbose);
   unsigned error = 0;
   if (!handler.UpdateFile(options.inputFile.c_str(), options.outputFile.c_str(), options.majorVersion, options.minorVersion, options.buildNumber, options.revision))
      error = handler.Error();

   logger.Log(L"ERRORLEVEL=%u", error);
   return error;
}
