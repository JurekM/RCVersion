#pragma once

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

	typedef void(*PRINT_PROC)(const wchar_t* message);
	PRINT_PROC Print;

	RCVersionOptions(PRINT_PROC PrintProc);

	bool Parse(int argc, wchar_t* argv[]);
	bool Validate();

	void Error(const wchar_t* message);
	void Error(const wchar_t* format, const wchar_t* value);

	int NumericOption(const wchar_t* value);
	static std::wstring PathOption(const wchar_t* value);
};
