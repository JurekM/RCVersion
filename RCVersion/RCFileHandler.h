#pragma once
#include "Logger.h"

class RCFileHandler
{
protected:
   ILogger &ilogger;
   Logger logger;
   bool verbose;

   unsigned linesLogged;
   unsigned linesErrors;
public:
   RCFileHandler(ILogger &rlogger);
   virtual ~RCFileHandler();

   bool LoadFile(const wchar_t* path, size_t padding, void* &buffer, size_t &bytes);

   bool UpdateFile(const wchar_t *inpath, const wchar_t *outpath, int major, int minor, int build, int revision);

   unsigned UpdateBuffer(char* buffer, size_t chars, int major, int minor, int build, int revision);
   unsigned UpdateBuffer(wchar_t* buffer, size_t chars, int major, int minor, int build, int revision);

   static const wchar_t* NN(const wchar_t* ptr) { return ptr ? ptr : L"(null)"; }
};

