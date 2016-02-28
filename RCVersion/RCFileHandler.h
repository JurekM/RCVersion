#pragma once
#include "Logger.h"

class RCFileHandler
{
protected:
   ILogger &ilogger;
   Logger logger;
   bool verbose;
   unsigned error;

public:
   RCFileHandler(ILogger &rlogger);
   virtual ~RCFileHandler();

   unsigned Error() const { return error; }
   bool Verbose() const { return verbose; }
   void Verbose(bool value) { verbose = value; }

   bool LoadFile(const wchar_t* path, size_t padding, void* &buffer, size_t &bytes);
   bool SaveFile(const wchar_t* path, void* buffer, size_t bytes);

   bool UpdateFile(const wchar_t *inpath, const wchar_t *outpath, int major, int minor, int build, int revision);

   unsigned UpdateBuffer(char* buffer, size_t chars, int major, int minor, int build, int revision) const;
   unsigned UpdateBuffer(wchar_t* buffer, size_t chars, int major, int minor, int build, int revision) const;

   static const wchar_t* NN(const wchar_t* ptr) { return ptr ? ptr : L"(null)"; }
};

