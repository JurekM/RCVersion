#pragma once
#include "Logger.h"
#include <vector>

class RCFileHandler
{
protected:
   ILogger &ilogger;
   Logger logger;
   unsigned error;

   enum LOG_LEVEL {logError=0, logMinimum=1, logNormal=2, logInfo=3, logDetail=5, logVerbose=9};

public:
   RCFileHandler(ILogger &rlogger);
   virtual ~RCFileHandler();

   unsigned Error() const { return error; }
   int Verbosity() const { return logger.Verbosity(); }
   void Verbosity(int value) { logger.Verbosity(value); }

   bool LoadFile(const wchar_t* path, size_t padding, std::vector<unsigned char>& buffer);
   bool SaveFile(const wchar_t* path, void* buffer, size_t bytes);

   bool UpdateFile(const wchar_t *inpath, const wchar_t *outpath, int major, int minor, int build, int revision);

   unsigned UpdateBuffer(char* buffer, size_t chars, int major, int minor, int build, int revision) const;
   unsigned UpdateBuffer(wchar_t* buffer, size_t chars, int major, int minor, int build, int revision) const;

   static const wchar_t* NN(const wchar_t* ptr) { return ptr ? ptr : L"(null)"; }
};

