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

int _tmain(int argc, const wchar_t* argv[])
{
  ConsoleLogger clogger{};
  Logger logger{clogger};
  RCVersionOptions options{clogger};
  options.CheckVerbosity(argc, argv);

  //VersionInfo version;
  //if (verbose)
  //   wprintf(L"%s\nVersion: %s\n", szTitle, version.displayVersion);

  options.Parse(argc, argv);
  if (!options.Validate())
  {
    wprintf(L"\n%s\n", options.Help);
    if (0 < options.verbosity)
    {
      wprintf(L"\nERRORLEVEL=%u", ERROR_INVALID_PARAMETER);
    }
    return ERROR_INVALID_PARAMETER;
  }

  RCFileHandler handler{clogger};
  handler.Verbosity(options.verbosity);
  unsigned error{};
  if (!handler.UpdateFile(options.inputFile.c_str(), options.outputFile.c_str(), options.majorVersion, options.minorVersion, options.buildNumber, options.revision))
  {
    error = handler.Error();
  }

  if (0 < options.verbosity)
  {
    logger.Log(1, L"ERRORLEVEL=%u", error);
  }
  return error;
}
