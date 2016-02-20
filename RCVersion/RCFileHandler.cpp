#include "stdafx.h"
#include "RCFileHandler.h"
#include "RCUpdater.h"
#include "AutoHClose.h"
#include "AutoFree.h"

RCFileHandler::RCFileHandler(ILogger &rlogger)
   : logger(rlogger)
   , linesLogged(0)
   , linesErrors(0)
{
}


RCFileHandler::~RCFileHandler()
{
}

void RCFileHandler::Log(const wchar_t* format, ...)
{
   va_list vList;
   va_start(vList, format);

   wchar_t line[1024] = { 0 };
   _vsnwprintf_s(line, _TRUNCATE, format, vList);

   ++linesLogged;
   logger.Log(line);
}

bool RCFileHandler::Error(DWORD error, const wchar_t* format, ...)
{
   va_list vList;
   va_start(vList, format);

   wchar_t line[1024] = { 0 };
   _vsnwprintf_s(line, _TRUNCATE, format, vList);
   logger.Log(line);
   ++linesErrors;

   _snwprintf_s(line, _TRUNCATE, L"ERROR %u", error);
   logger.Log(line);
   ++linesErrors;

   SetLastError(error);
   return false;
}

bool RCFileHandler::UpdateFile(const wchar_t *inpath, const wchar_t *outpath, int major, int minor, int build, int revision)
{
   Log(L"UpdateFile(%s,%s)\n", NN(inpath), NN(outpath));

   void* buffer = nullptr;
   size_t bytes = 0;

   if (!LoadFile(inpath, 1024, buffer, bytes))
      return false;

   AutoFree af(buffer);

   int flags = IS_TEXT_UNICODE_UNICODE_MASK;
   bool isUnicode = 0 != IsTextUnicode(buffer, int(min(bytes, 256)), &flags);

   bool ok;
   if (isUnicode)
      ok = UpdateBuffer(static_cast<wchar_t*>(buffer), bytes / sizeof(wchar_t), major, minor, build, revision);
   else
      ok = UpdateBuffer(static_cast<char*>(buffer), bytes / sizeof(char), major, minor, build, revision);

   return false;
}

bool RCFileHandler::RCFileHandler::UpdateBuffer(char* buffer, size_t chars, int major, int minor, int build, int revision)
{
   Log(L"UpdateBuffer<char>(...,%u)\n", unsigned(chars));
   RCUpdater<char> updater;
   updater.UpdateVersion(buffer, chars, major, minor, build, revision);
   return false;
}

bool RCFileHandler::RCFileHandler::UpdateBuffer(wchar_t* buffer, size_t chars, int major, int minor, int build, int revision)
{
   //wchar_t (*fxKeywords)[2] =
   //{
   //   { wchar_t('P'),wchar_t('R'),wchar_t('O'),wchar_t('D'),wchar_t('U'),wchar_t('C'),wchar_t('T'),wchar_t('V'),wchar_t('E'),wchar_t('R'),wchar_t('S'),wchar_t('I'),wchar_t('O'),wchar_t('N'),0 },
   //   { wchar_t('F'),wchar_t('I'),wchar_t('L'),wchar_t('E'),wchar_t('V'),wchar_t('E'),wchar_t('R'),wchar_t('S'),wchar_t('I'),wchar_t('O'),wchar_t('N'),0 },
   //};


   Log(L"UpdateBuffer<wchar>(...,%u)\n", unsigned(chars));
   RCUpdater<wchar_t> updater;
   wchar_t kvinfo[] = L"VERSIONINFO";
   updater.UpdateVersion(buffer, chars, major, minor, build, revision);
   return false;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCFileHandler::LoadFile(const wchar_t* path, size_t padding, void* &buffer, size_t &bytes)
{
   if (!path || !*path)
      return Error(ERROR_INVALID_PARAMETER, L"*** RCFileUpdater::Load: Input file path must not be empty");

   HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, 0, nullptr);
   if (INVALID_HANDLE_VALUE == hFile)
      return Error(GetLastError(), L"*** RCFileUpdater::Load: Cannot open input file", path);

   AutoHClose ahc(hFile);

   ULARGE_INTEGER li = { 0 };
   li.LowPart = GetFileSize(hFile, &li.HighPart);
   if (0 != li.HighPart || (0x7FFFFFFF - padding) <= li.LowPart)
      return Error(ERROR_FILE_CORRUPT, L"*** RCFileUpdater::Load: File too large", path);

   bytes = li.LowPart + padding;
   buffer = malloc(bytes);
   if (!buffer)
      return Error(ERROR_OUTOFMEMORY, L"*** RCFileUpdater::Load: File too large", path);

   DWORD readBytes = 0;
   BOOL ok = ReadFile(hFile, buffer, DWORD(bytes), &readBytes, nullptr);
   if (!ok)
   {
      free(buffer);
      buffer = nullptr;
      bytes = 0;
      return Error(GetLastError(), L"*** RCFileUpdater::Load: Cannot read input file", path);
   }

   return true;
}
