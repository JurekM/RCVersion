#include "stdafx.h"
#include "RCVersionOptions.h"


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
const wchar_t RCVersionOptions::Help[] =
L"\nSyntax: RCVersion <resource-file.rc> [<options>]"
L"\n /b:<build-number>  new build number, default: increment by one"
L"\n /m:<major-version> new major version, default: unchanged"
L"\n /n:<minor-version> new minor version, default: unchanged"
L"\n /r:<revision>      new revision number, default: 0"
L"\n /o:<output-file>   output file path, default: same as input"
L"\n /v:{0|1}           verbose, default on"
L"\n"
L"\n"
L"\nThis command locates and modifies FILEVERSION and PRODUCTVERSION resources"
L"\nin a standard Windows RC file. Resource must consist of four parts separated"
L"\nby commans or periods. The default behaviour is to increment the build number,"
L"\nkeep the major and minur version number and reset the revision to zero."
L"\nThe primary intended use of this command is in C++ project build. The '/b:'"
L"\nparameter is meant to be set to, for example, source control sequence number"
L"\nwhich provides increasing, unique version numbers. If the '/b:' parameter has"
L"\nno value or evaluates to zero, the default action is taken."
;


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
RCVersionOptions::RCVersionOptions(PRINT_PROC PrintProc)
	: errorDetected(false)
	, majorVersion(-1)
	, minorVersion(-1)
	, buildNumber(-1)
	, revision(-1)
	, verbose(true)
	, Print(PrintProc)
{
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void RCVersionOptions::Error(const wchar_t* message)
{
	errorDetected = true;
	if (0 != Print)
		Print(message);
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void RCVersionOptions::Error(const wchar_t* format, const wchar_t* value)
{
	wchar_t buffer[1024] = { 0 };
	_snwprintf_s(buffer, _TRUNCATE, format, value);
	Error(buffer);
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
int RCVersionOptions::NumericOption(const wchar_t* value)
{
	wchar_t* tail = nullptr;
	int result = wcstoul(value, &tail, 10);

	if (0 == *tail)
		return int(result);

	Error(L"*** Invalid option value: [%s]", value);
	return -1;
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
std::wstring RCVersionOptions::PathOption(const wchar_t* value)
{
	if (!value || !*value)
		return std::wstring();

	wchar_t path[1024] = { 0 };
	if (!ExpandEnvironmentStrings(value, path, _countof(path)))
		return value;

	return path;
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCVersionOptions::Parse(int argc, wchar_t* argv[])
{
	errorDetected = false;

	for (int nArg = 1; nArg < argc; ++nArg)
	{
		LPCWSTR arg = argv[nArg];

		if (!arg || !*arg)
			continue;

		if (L'/' == *arg || L'-' == *arg)
		{
			wchar_t code = towlower(arg[1]);
			bool colon = (0 != code && L':' == arg[2]);
			const wchar_t* value = colon ? &arg[3] : 0;
			//wchar_t* tail = nullptr;

			if (!value)
			{
				Error(L"*** Invalid option format: [%s]", arg);
				continue;
			}

			switch (code)
			{
			case L'm':
				if (*value)
					majorVersion = NumericOption(value);
				break;
			case L'n':
				if (*value)
					minorVersion = NumericOption(value);
				break;
			case L'b':
				if (*value)
					buildNumber = NumericOption(value);
				break;
			case L'r':
				if (*value)
					revision = NumericOption(value);
				break;
			case L'o':
				outputFile = PathOption(value);
				break;
			case L'v':
				verbose = 0 != NumericOption(value);
				break;
			default:
				Error(L"*** Unknown option: [%s]", arg);
				break;
			}
		}
		else
		{
			if (inputFile.empty())
				inputFile = PathOption(arg);
			else
				Error(L"*** Input file already defined, unexpected argument: [%s]", arg);
		}
	}

	return !errorDetected;
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCVersionOptions::Validate()
{
	if (inputFile.empty())
		Error(L"*** Missing 'input file' parameter.");
	if (outputFile.empty())
		outputFile = inputFile;

	if (errorDetected)
		return false;

	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(inputFile.c_str()))
		Error(L"*** Cannot access file [%s]", inputFile.c_str());

	return !errorDetected;
}
