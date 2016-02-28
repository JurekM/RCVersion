#pragma once
#include "ILogger.h"
#include <windows.h>
#include <stdarg.h>
#include <stdio.h>

class Logger
{
protected:
   ILogger &logger;

public:
   Logger(ILogger &rlogger) : logger(rlogger) { }
   virtual ~Logger() { }

   void Log(const wchar_t* format, ...) const
   {
      va_list vList;
      va_start(vList, format);

      wchar_t line[1024] = { 0 };
      _vsnwprintf_s(line, _TRUNCATE, format, vList);

      logger.Log(line);
   }

   bool Error(DWORD error, const wchar_t* format, ...) const
   {
      va_list vList;
      va_start(vList, format);

      wchar_t line[1024] = { 0 };
      _vsnwprintf_s(line, _TRUNCATE, format, vList);
      logger.Log(line);

      _snwprintf_s(line, _TRUNCATE, L"ERROR %u", error);
      logger.Log(line);

      SetLastError(error);
      return false;
   }
};