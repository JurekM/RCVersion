#pragma once
#include "ILogger.h"

class RCFileHandler
{
protected:
   ILogger &logger;

   unsigned linesLogged;
   unsigned linesErrors;
public:
   RCFileHandler(ILogger &rlogger);
   virtual ~RCFileHandler();

   void Log(const wchar_t* format, ...);
   bool Error(DWORD error, const wchar_t* format, ...);

   bool LoadFile(const wchar_t* path, size_t padding, void* &buffer, size_t &bytes);

   bool UpdateFile(const wchar_t *inpath, const wchar_t *outpath, int major, int minor, int build, int revision);

   bool UpdateBuffer(char* buffer, size_t chars, int major, int minor, int build, int revision);
   bool UpdateBuffer(wchar_t* buffer, size_t chars, int major, int minor, int build, int revision);

   static const wchar_t* NN(const wchar_t* ptr) { return ptr ? ptr : L"(null)"; }
};

