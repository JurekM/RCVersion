#pragma once
#include "ILogger.h"


class RCVersionOptions
{
public:
	static const wchar_t Help[];

	bool errorDetected;

	int majorVersion;
	int minorVersion;
	int buildNumber;
	int revision;

   unsigned verbosity;
   bool helpOnly;

	std::wstring inputFile;
	std::wstring outputFile;

	ILogger &logger;

	RCVersionOptions(ILogger &rlogger);

	void CheckVerbosity(int argc, const wchar_t* argv[]);
	bool Parse(int argc, const wchar_t* argv[]);
	bool Validate();

	void Error(const wchar_t* format, ...);

	int NumericOption(const wchar_t* value);
	static std::wstring PathOption(const wchar_t* value);
};
