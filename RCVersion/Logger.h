#pragma once
#include "ILogger.h"
#include <windows.h>
#include <stdarg.h>
#include <stdio.h>

class Logger
{
protected:
   ILogger &logger;
   int verbosity;

public:
   Logger(ILogger &rlogger) : logger(rlogger), verbosity(1) { }
   virtual ~Logger() { }

   int Verbosity() const { return verbosity; }
   void Verbosity(int value) { verbosity = value; }

   void Log(int level, const wchar_t* format, ...) const
   {
      if (verbosity < level)
         return;

      va_list vList;
      va_start(vList, format);

      wchar_t line[1024]{};
      _vsnwprintf_s(line, _TRUNCATE, format, vList);

      logger.Log(line);
   }

   bool Error(DWORD error, const wchar_t* format, ...) const
   {
      va_list vList;
      va_start(vList, format);

      wchar_t line[1024]{};
      _vsnwprintf_s(line, _TRUNCATE, format, vList);
      logger.Log(line);

      _snwprintf_s(line, _TRUNCATE, L"ERROR %u", error);
      logger.Log(line);

      SetLastError(error);
      return false;
   }
};