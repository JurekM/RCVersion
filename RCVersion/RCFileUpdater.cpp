#include "stdafx.h"
#include "RCFileUpdater.h"
#include "AutoHClose.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
RCFileUpdater::RCFileUpdater(ILogger &rlogger)
   : bufferData(nullptr)
   , bufferSize(0)
   , bufferChars(0)
   , errorCode(0)
   , majorVersion(0)
   , minorVersion(0)
   , buildNumber(0)
   , revision(0)
   , isUnicode(false)
   , verbose(false)
   , logger(rlogger)
{
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
RCFileUpdater::~RCFileUpdater(void)
{
   Release();
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCFileUpdater::Error(DWORD code, const wchar_t* message)
{
   errorCode = code;
   errorMessage = message;
   SetLastError(errorCode);
   logger.Log(message);
   return false;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCFileUpdater::Error(DWORD code, const wchar_t* format, const wchar_t* value)
{
   wchar_t buffer[1024] = { 0 };
   _snwprintf_s(buffer, _TRUNCATE, format, value);
   return Error(code, buffer);
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void RCFileUpdater::Release()
{
   delete[] bufferData;
   bufferChars = 0;
   bufferData = nullptr;
   bufferSize = 0;
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCFileUpdater::Allocate(size_t bytes)
{
   size_t needBytes = bytes + 256;
   if (bufferSize < needBytes)
   {
      Release();
      try {
         bufferData = new char[needBytes];
      }
      catch (...)
      {
         bufferData = nullptr;
      }
      if (!bufferData)
         return Error(ERROR_OUTOFMEMORY, L"*** RCFileUpdater::Allocate: Cannot allocate memory buffer.");
      bufferSize = needBytes;
   }

   ZeroMemory(bufferData, bufferSize);
   return true;
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCFileUpdater::Load()
{
   if (inputFile.empty())
      return Error(ERROR_INVALID_PARAMETER, L"*** RCFileUpdater::Load: Missing input file name.");

   return Load(inputFile.c_str());
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCFileUpdater::Load(const wchar_t* path)
{
   if (!path || !*path)
      return Error(ERROR_INVALID_PARAMETER, L"*** RCFileUpdater::Load: Input file path must not be empty");

   HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, 0, nullptr);
   if (INVALID_HANDLE_VALUE == hFile)
      return Error(GetLastError(), L"*** RCFileUpdater::Load: Cannot open input file", path);

   AutoHClose ahc(hFile);

   ULARGE_INTEGER li = { 0 };
   li.LowPart = GetFileSize(hFile, &li.HighPart);
   if (0 != li.HighPart || (INT_MAX - 2) <= li.LowPart)
      return Error(ERROR_FILE_CORRUPT, L"*** RCFileUpdater::Load: File too large", path);

   if (!Allocate(li.LowPart))
      return false;

   DWORD bytes = 0;
   BOOL ok = ReadFile(hFile, bufferData, DWORD(bufferSize), &bytes, nullptr);
   if (!ok)
      return Error(GetLastError(), L"*** RCFileUpdater::Load: Cannot read input file", path);

   isUnicode = 0xFF == bufferData[0] && 0xFE == bufferData[1];

   if (!isUnicode)
   {
      int flags = IS_TEXT_UNICODE_UNICODE_MASK;
      isUnicode = 0 != IsTextUnicode(bufferData, min(bytes, 256), &flags);
   }

   bufferChars = 0;
   return true;
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCFileUpdater::Store(const char* data)
{
   size_t length = strlen(data);

   if (!Allocate(length))
      return false;

   strncpy_s(bufferData, bufferSize, data, _TRUNCATE);
   bufferChars = length;
   isUnicode = false;

   return true;
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCFileUpdater::Save()
{
   if (outputFile.empty())
      return Error(ERROR_INVALID_PARAMETER, L"*** RCFileUpdater::Load: Missing output file name.");

   return Save(outputFile.c_str());
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCFileUpdater::Save(const wchar_t* path)
{
   if (!path || !*path)
      return Error(ERROR_INVALID_PARAMETER, L"*** RCFileUpdater::Save: Missing output file name.");

   HANDLE hFile = CreateFile(outputFile.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, CREATE_ALWAYS, 0, nullptr);
   if (INVALID_HANDLE_VALUE == hFile)
      return Error(GetLastError(), L"*** RCFileUpdater::Save: Cannot open output file", outputFile.c_str());

   AutoHClose ahc(hFile);

   DWORD lengthBytes = DWORD(isUnicode ? sizeof(wchar_t)*wcslen(reinterpret_cast<wchar_t*>(bufferData)) : strlen(bufferData));

   DWORD bytes = 0;
   BOOL ok = WriteFile(hFile, bufferData, lengthBytes, &bytes, nullptr);
   if (!ok)
      return Error(GetLastError(), L"*** RCFileUpdater::Save: Cannot write output file", outputFile.c_str());

   return true;
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCFileUpdater::UpdateFile()
{
   if (verbose)
      Message(L"Reading [%s]", inputFile.c_str());

   if (!Load())
      return false;

   int count = UpdateVersions();

   if (!count)
   {
      Error(ERROR_FILE_CORRUPT, L"*** No versions found in file [%s].", inputFile.c_str());
      return false;
   }

   if (verbose)
      Message(L"Writing [%s]", outputFile.c_str());

   if (!Save())
      return false;

   return true;
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCFileUpdater::MatchSkip(char* &line, const char* text, bool allowComma)
{
   line = LTrim(line, " \t");

   // text is null: no check
   if (0 == text)
      return true;

   // , text is empty: any value
   size_t length = strlen(text);
   if (0 == length)
   {
      line = LTrim(SkipTextItem(line), " \t");
      return true;
   }

   // text not empty: must match
   if (0 != _strnicmp(line, text, length))
      return false;

   // next must be space (or comma) or this is not a match
   char* ptr = line + length;

   if (!isspace(*ptr) && (!allowComma || ',' != *ptr))
      return false;

   line = allowComma ? LTrim(ptr, " ,\t") : LTrim(ptr, " \t");
   return true;
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
char* RCFileUpdater::FindVersionItem(char* line, const char* name, const char* value)
{
   //size_t nlength = name ? strlen(name) : 0;
   //size_t vlength = value ? strlen(value) : 0;

   for (; line && *line; line = NextLine(line))
   {
      // The first line/string on the line
      line = LTrim(line);

      // Skip comment lines
      if ('/' == line[0] && '/' == line[1])
         continue;

      // match and skip name
      if (!MatchSkip(line, name, false))
         continue;

      // match and skip value
      if (!MatchSkip(line, value, true))
         continue;

      // line now points to the next non-blank (or newline) after name/value match
      return line;
   }

   return nullptr;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
int RCFileUpdater::UpdateVersions() const
{
   if (!bufferData || !bufferChars)
      return -1;

   char* start = FindVersionItem(bufferData, "", "VERSIONINFO");
   if (!start || !*start)
      return 0;

   int count = 0;

   count += Update(start, bufferSize - (start - bufferData), "FILEVERSION", nullptr, "FILEVERSION     ");
   count += Update(start, bufferSize - (start - bufferData), "PRODUCTVERSION", nullptr, "PRODUCTVERSION  ");
   count += Update(start, bufferSize - (start - bufferData), "VALUE", "\"FileVersion\"", "\"FileVersion\"   ");
   count += Update(start, bufferSize - (start - bufferData), "VALUE", "\"ProductVersion\"", "\"ProductVersion\"");

   return count;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
int RCFileUpdater::Update(char* text, size_t chars, const char* name, const char* value, const char* title) const
{
   int count = 0;

   char* version = FindVersionItem(text, name, value);
   if (!version && !*version)
      return 0;

   if ('"' == *version)
      ++version;

   return UpdateVersionNumber(version, chars - (version - text), majorVersion, minorVersion, buildNumber, revision, title);
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
int RCFileUpdater::UpdateVersionNumber(char* version, size_t availableChars, int major, int minor, int build, int revision, const char* message) const
{
   bool increment = major < 0 && minor < 0 && build < 0 && revision < 0;

   char* front = LTrim(version, " \t");

   if (!isdigit(*front))
      return 0;

   char* tail = nullptr;
   long val = strtol(front, &tail, 10);
   major = (major < 0) ? val : major;

   front = LTrim(tail, " .,\t");
   if (front == tail)
      return 0;

   if (!isdigit(*front))
      return 0;
   val = strtol(front, &tail, 10);
   minor = (minor < 0) ? val : minor;

   front = LTrim(tail, " .,\t");
   if (front == tail)
      return 0;

   if (!isdigit(*front))
      return 0;
   val = strtol(front, &tail, 10);
   build = (build < 0) ? val : build;

   front = LTrim(tail, " .,\t");
   if (front == tail)
      return 0;

   if (!isdigit(*front))
      return 0;
   val = strtoul(front, &tail, 10);
   revision = (revision < 0) ? val : revision;

   tail = LTrim(tail, " \t");

   if (increment)
      ++build;

   char newVersion[64] = { 0 };
   _snprintf_s(newVersion, _TRUNCATE, "%d, %d, %d, %d", major, minor, build, revision);

   char oldVersion[64] = { 0 };
   strncpy_s(oldVersion, version, tail - version);

   if (verbose)
      Message(L"Updating %hs from [%hs] to [%hs]", message, oldVersion, newVersion);

   if (!Replace(version, tail - version, newVersion, availableChars))
      return 0;

   return 1;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCFileUpdater::Replace(char* oldText, size_t oldLength, const char* newText, size_t bufferLength)
{
   // Find the tail of the buffer
   char* tail = oldText + oldLength;
   size_t tailLength = strlen(tail);

   // Invalid parameters: lengths do not make sense
   if (bufferLength <= oldLength + tailLength)
      return false;

   // Find the new location of the tail of the buffer
   size_t newLength = newText ? strlen(newText) : 0;
   char* newTail = oldText + newLength;

   // New text does not fit in the buffer
   if (bufferLength <= newLength + tailLength)
      return false;

   // Move the tail to the new location
   if (tail != newTail)
      MoveMemory(newTail, tail, tailLength + 1);

   // Copy the new data ino the destination
   if (0 < newLength)
      MoveMemory(oldText, newText, newLength);

   return true;
}



/*
VS_VERSION_INFO VERSIONINFO
 FILEVERSION 1,0,0,1
 PRODUCTVERSION 1,0,0,1
 FILEFLAGSMASK 0x17L
#ifdef _DEBUG
 FILEFLAGS 0x1L
#else
 FILEFLAGS 0x0L
#endif
 FILEOS 0x4L
 FILETYPE 0x1L
 FILESUBTYPE 0x0L
BEGIN
   BLOCK "StringFileInfo"
   BEGIN
      BLOCK "040904b0"
      BEGIN
         VALUE "CompanyName", "Maze Computer Communications, Inc."
         VALUE "FileDescription", "RegChange - Find and replace strings in the registry."
         VALUE "FileVersion", "1, 0, 0, 1"
         VALUE "InternalName", "RegChange"
         VALUE "LegalCopyright", "Copyright (C) 2002 Maze Computer Communications, Inc."
         VALUE "OriginalFilename", "RegChang.exe"
         VALUE "ProductName", " RegChang Application"
         VALUE "ProductVersion", "1, 0, 0, 1"
      END
   END
   BLOCK "VarFileInfo"
   BEGIN
      VALUE "Translation", 0x409, 1200
   END
END
*/