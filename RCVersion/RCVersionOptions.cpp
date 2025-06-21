#include "stdafx.h"
#include "RCVersionOptions.h"


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
const wchar_t RCVersionOptions::Help[] =
L"\nSyntax: RCVersion <resource-file.rc> [<options>]"
L"\n /m:<major-version> new major version, default: unchanged"
L"\n /n:<minor-version> new minor version, default: unchanged"
L"\n /b:<build-number>  new build number, default: increment by one"
L"\n /r:<revision>      new revision number, default: unchanged"
L"\n /o:<output-file>   output file path, default: same as input"
L"\n /v:{0|1|...|9}     verbosity level, 0=lowest, 9=highest, default: 3"
L"\n"
L"\n"
L"\nThis command locates and modifies FILEVERSION and PRODUCTVERSION resources in"
L"\na standard Windows RC file. Resource must consist of four parts separated by"
L"\ncommas and/or periods. The default behaviour is to increment the build number,"
L"\nkeep the major and minor version and revision numbers."
L"\nThe primary intended use of this command is in C++ project build. The '/b:'"
L"\nparameter is meant to be set to, for example, source control sequence number"
L"\nwhich provides increasing, unique version numbers."
L"\nFile paths may contain environment variables, they will be expanded."
L"\nLicense: https://github.com/JurekM/RCVersion"
;


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
RCVersionOptions::RCVersionOptions(ILogger &rlogger)
   : errorDetected(false)
   , majorVersion(-1)
   , minorVersion(-1)
   , buildNumber(-1)
   , revision(-1)
   , verbosity(3)
   , helpOnly(false)
   , logger(rlogger)
{
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void RCVersionOptions::Error(const wchar_t* format, ...)
{
   errorDetected = true;
   va_list vList;
   va_start(vList, format);
   wchar_t buffer[1024] = { 0 };
   _vsnwprintf_s(buffer, _TRUNCATE, format, vList);
   logger.Log(buffer);
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
void RCVersionOptions::CheckVerbosity(int argc, const wchar_t* argv[])
{
   for (int n = 1; n < argc; ++n)
   {
      const wchar_t* arg = argv[n];
      if ('/' == arg[0] && 'v' == towlower(arg[1]) && ':' == arg[2] && iswdigit(arg[3]))
      {
         wchar_t* tail = nullptr;
         unsigned long result = wcstoul(&arg[4], &tail, 10);
         if (0 == *tail)
            verbosity = int(result);
      }
   }
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCVersionOptions::Parse(int argc, const wchar_t* argv[])
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

         if (L'?' == code)
         {
            helpOnly = true;
            verbosity = 0;
            return false;
         }

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
            if (*value)
               verbosity = NumericOption(value);
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
            Error(L"*** Input file already defined as: [%s], unexpected argument: [%s]", inputFile.c_str(), arg);
      }
   }

   return !errorDetected;
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCVersionOptions::Validate()
{
   if (helpOnly)
      return false;

   if (inputFile.empty())
      Error(L"*** Missing 'input file' parameter.");
   if (outputFile.empty())
      outputFile = inputFile;

   if (errorDetected)
      return false;

   return !errorDetected;
}
