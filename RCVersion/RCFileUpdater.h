#pragma once
#include "ILogger.h"
#include "trim.h"

class RCFileUpdater
{
public:
	char*    bufferData;
	size_t   bufferSize;
	size_t   bufferChars;

	DWORD errorCode;
	std::wstring errorMessage;

	int majorVersion;
	int minorVersion;
	int buildNumber;
	int revision;

	bool verbose;

	std::wstring inputFile;
	std::wstring outputFile;

	ILogger &logger;

	void Message(const wchar_t* format, ...) const
	{
		va_list vList;
		va_start(vList, format);
		wchar_t buffer[1024] = { 0 };
		_vsnwprintf_s(buffer, _TRUNCATE, format, vList);
      logger.Log(buffer);
	}

	// Always return 'false' - allows simpler error handling code
	bool Error(DWORD code, const wchar_t* message);
	bool Error(DWORD code, const wchar_t* format, const wchar_t* value);

	bool Allocate(size_t bytes);
	void Release();

	static bool MatchSkip(char* &line, const char* text, bool allowComma);
	static char* FindVersionItem(char* bufferData, const char* name, const char* value);
	int UpdateVersionNumber(char* version, size_t availableChars, int major, int minor, int build, int revision, const char* message) const;
	int Update(char* text, size_t chars, const char* name, const char* value, const char* title) const;
	static bool Replace(char* oldText, size_t oldLength, const char* newText, size_t bufferLength);

	RCFileUpdater(ILogger &rlogger);
	~RCFileUpdater(void);

	bool Load();
	bool Load(const wchar_t* path);
	bool Store(const char* data);
	bool Save();
	bool Save(const wchar_t* path);
	int UpdateVersions() const;

	bool UpdateFile();
};
