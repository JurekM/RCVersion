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

	bool verbose;

	std::wstring inputFile;
	std::wstring outputFile;

	ILogger &logger;

	RCVersionOptions(ILogger &rlogger);

	bool Parse(int argc, wchar_t* argv[]);
	bool Validate();

	void Log(const wchar_t* message);
	void Error(const wchar_t* format, ...);

	int NumericOption(const wchar_t* value);
	static std::wstring PathOption(const wchar_t* value);
};
