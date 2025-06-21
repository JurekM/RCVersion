#pragma once
#include <string>
#include <vector>
#include "MessageBuffer.h"
#include "ILogger.h"

template <class CharT>
const CharT** GetKeywordTable() { return nullptr; }

template <>
inline const wchar_t** GetKeywordTable<wchar_t>()
{
   const static wchar_t *keywords[] =
   {
      L"-PRODUCTVERSION",
      L"-FILEVERSION",
      L" FILEFLAGSMASK",
      L" FILEFLAGS",
      L" FILEOS",
      L" FILETYPE",
      L" FILESUBTYPE",
      L" BEGIN",
      L" BLOCK",
      L" END",
      L"+VALUE",
      L"\"FileVersion\"",
      L"\"ProductVersion\"",
      nullptr
   };
   return keywords;
}

template <>
inline const char** GetKeywordTable<char>()
{
   const static char *keywords[] =
   {
      "-PRODUCTVERSION",
      "-FILEVERSION",
      " FILEFLAGSMASK",
      " FILEFLAGS",
      " FILEOS",
      " FILETYPE",
      " FILESUBTYPE",
      " BEGIN",
      " BLOCK",
      " END",
      "+VALUE",
      "\"FileVersion\"",
      "\"ProductVersion\"",
      nullptr
   };
   return keywords;
}

template<class CharT, class TraitsT = std::char_traits<CharT>>
class RCUpdater
{
public:
   ILogger &logger;
   int  verbosity;
   bool debug;
   unsigned error;

   RCUpdater(ILogger &rlogger)
      : logger(rlogger)
      , verbosity(1)
      , debug(false)
      , error(0)
   {
   }

   virtual ~RCUpdater()
   {
   }

   // Left-trim chaff characters
   static CharT* LTrim(CharT*psz, const CharT*chaff)
   {
      while (*psz)
      {
         bool found{false};
         for (const CharT* c = chaff; *c && !found; ++c)
         {
            if (*psz == *c)
               found = true;
         }
         if (!found)
            break;
         ++psz;
      }
      return psz;
   }

   // Left-skip to a chaff character
   static CharT* LSkipTo(CharT*psz, const CharT*chaff)
   {
      while (*psz)
      {
         for (const CharT* c = chaff; *c; ++c)
         {
            if (*psz == *c)
               return psz;
         }
         ++psz;
      }
      return psz;
   }

   static char* strfind(char* where, const char* what)
   {
      return strstr(where, what);
   }

   static wchar_t* strfind(wchar_t* where, const wchar_t* what)
   {
      return wcsstr(where, what);
   }

   static CharT* NextLine(CharT*line)
   {
      while (*line && '\n' != *line)
         ++line;
      return *line ? line + 1 : line;
   }

   static CharT* SkipComment(CharT*psz)
   {
      static const CharT space[] = { ' ', '\t', 0 };
      psz = LTrim(psz, space);

      if ('/' == psz[0] && '/' == psz[1])
         return NextLine(psz);

      if ('/' == psz[0] && '*' == psz[1])
      {
         static const CharT endcomment[] = { '*', '/', 0 };
         CharT *next = strfind(psz + 2, endcomment);
         return (nullptr == next) ? psz : LTrim(next + 2, space);
      }

      return psz;
   }

   static CharT* SkipAllComments(CharT*psz)
   {
      static const CharT white[] = { ' ', '\t', '\r', '\n', 0 };
      CharT* next = SkipComment(psz);
      while (psz != next)
      {
         psz = LTrim(next, white);
         next = SkipComment(psz);
      }
      return psz;
   }

   static bool str2int(char* &psz, int& value, const char* chaff)
   {
      if (!isdigit(uint8_t(*psz)))
         return false;
      value = strtol(psz, &psz, 10);
      if (chaff && *chaff)
         psz = LTrim(psz, chaff);
      return true;
   }

   static bool str2int(wchar_t* &psz, int& value, const wchar_t* chaff)
   {
      if (!iswdigit(*psz))
         return false;
      value = wcstol(psz, &psz, 10);
      if (chaff && *chaff)
         psz = LTrim(psz, chaff);
      return true;
   }

   static bool format(char*buffer, size_t chars, int major, int minor, int build, int revision)
   {
      return 0 < _snprintf_s(buffer, chars, _TRUNCATE, "%d, %d, %d, %d", major, minor, build, revision);
   }

   static bool format(wchar_t*buffer, size_t chars, int major, int minor, int build, int revision)
   {
      return 0 < _snwprintf_s(buffer, chars, _TRUNCATE, L"%d, %d, %d, %d", major, minor, build, revision);
   }

   static bool parse(CharT*xbuffer, CharT**tail, int &major, int &minor, int &build, int &revision)
   {
      static const CharT space[] = { ' ','\t' , 0 };
      static const CharT chaff[] = { '.', ',', ' ', '\t', 0 };
      *tail = LTrim(xbuffer, space);

      if (!str2int(*tail, major, chaff))
         return false;
      if (!str2int(*tail, minor, chaff))
         return false;
      if (!str2int(*tail, build, chaff))
         return false;
      if (!str2int(*tail, revision, nullptr))
         return false;

      return true;
   }

   static bool replace(CharT *buffer, size_t totalChars, size_t oldChars, const CharT* newString)
   {
      if (totalChars <= oldChars)
         return false;
      size_t newChars = TraitsT::length(newString);
      if (oldChars < newChars)
      {
         size_t oldTail = TraitsT::length(buffer + oldChars);
         if (totalChars <= oldTail + newChars)
            return false;
         TraitsT::move(buffer + newChars, buffer + oldChars, oldTail + 1);
      }
      else if (newChars < oldChars)
      {
         TraitsT::move(buffer + newChars, buffer + oldChars, TraitsT::length(buffer + oldChars) + 1);
      }
      TraitsT::copy(buffer, newString, newChars);
      return true;
   }

   // The next line after VERSIONINFO or zero if not found
   static size_t FindStartOfVersion(CharT *buffer)
   {
      static const CharT space[] = { ' ', '\t', 0 };
      static const CharT stopper[] = { ' ', '\t', '\n', '/', 0 };
      static const CharT keyword[] = { 'V', 'E', 'R', 'S', 'I', 'O', 'N', 'I', 'N', 'F', 'O', 0 };
      size_t length = TraitsT::length(keyword);

      CharT* line = buffer;
      while (*line)
      {
         line = SkipAllComments(line);
         line = LSkipTo(line, stopper);
         line = SkipComment(line);

         // Is this the space delimited keyword we need
         bool found = 0 == TraitsT::compare(keyword, line, length);
         found = found && (uint8_t(line[length]) <= uint8_t(' ') || '/' == line[length]);
         line = NextLine(line);

         // Return the offset of the next line
         if (found)
            return line - buffer;
      }

      return 0;
   }

   void FindVersionStrings(CharT *buffer, size_t start, std::vector<size_t> &offsets)
   {
      static const CharT space[] = { ' ', '\t', 0 };
      static const CharT chaff[] = { ',',  ' ', '\t', 0 };
      static const CharT **keywords = GetKeywordTable<CharT>();

      CharT *line = buffer + start;
      while (*line)
      {
         line = SkipComment(line);

         if ('#' == line[0])
         {
            line = NextLine(line);
            continue;
         }

         bool found{false};

         for (unsigned ndx = 0; 0 != keywords[ndx]; ++ndx)
         {
            const CharT* keyword = keywords[ndx];
            const CharT code = *keyword++;
            size_t length = TraitsT::length(keyword);

            // This is a STRINGFILEINFO name, not a keyword
            if ('\"' == code)
               continue;

            if (0 != TraitsT::compare(keyword, line, length))
               continue;
            if (' ' < line[length] && '/' != line[length])
               continue;

            found = true;
            if (7 <= verbosity || '-'==code && 6 <= verbosity)
            {
               wchar_t msg[1024]{};
               _snwprintf_s(msg, _TRUNCATE, L"FOUND: [%c]:%s offset=%u", wchar_t(code), MessageBuffer(keyword).message(), unsigned(line - buffer));
               logger.Log(msg);
            }

            // FIXEDFILEINFO keyword, version follows after space
            if ('-' == code)
            {
               line = SkipComment(line + length);
               size_t offset = line - buffer;
               offsets.push_back(offset);
               break;
            }

            // STRINGFILEINFO keyword, version may follow after string name
            if ('+' == code)
            {
               line = SkipComment(line + length);

               for (unsigned vx = 0; 0 != keywords[vx]; ++vx)
               {
                  const CharT* name = keywords[vx];
                  const CharT type = *name;
                  size_t chars = TraitsT::length(name);
                  if ('\"' != type)
                     continue;
                  if (0 != TraitsT::compare(name, line, chars))
                     continue;
                  line = SkipComment(line + chars);
                  line = LTrim(line, chaff);
                  line = SkipComment(line);
                  if (*line <= CharT(' '))
                     continue;
                  if (CharT('\"') == *line)
                     ++line;

                  line = LTrim(line, space);
                  if (6 <= verbosity)
                  {
                     wchar_t msg[1024]{};
                     _snwprintf_s(msg, _TRUNCATE, L"FOUND NAME: [%s] offset=%u", MessageBuffer(name).message(), unsigned(line - buffer));
                     logger.Log(msg);
                  }
                  size_t offset = line - buffer;
                  offsets.push_back(offset);
                  break;
               }
               break;
            }

            // Expected but not used keyword
            if (' ' == code)
            {
               break;
            }

            // When we get here, we have a bad keyword table
            found = false;
         }

         if (!found)
            break;

         line = NextLine(line);
      }
   }


   unsigned UpdateVersion(CharT *buffer, size_t chars, int xmajor, int xminor, int xbuild, int xrevision)
   {
      size_t start = FindStartOfVersion(buffer);
      unsigned replacementsMade{};

      error = ERROR_FILE_CORRUPT;
      if (0 == start)
         return 0;

      std::vector<size_t> offsets;
      FindVersionStrings(buffer, start, offsets);

      error = ERROR_FILE_CORRUPT;
      if (0 == offsets.size())
         return 0;

      error = NO_ERROR;
      bool success{true};

      for (auto iter = offsets.rbegin(); success && offsets.rend() != iter; ++iter)
      {
         size_t offset = *iter;
         CharT *tail{nullptr};
         int major{-1}, minor{-1}, build{-1}, revision{-1};
         if (!parse(buffer + offset, &tail, major, minor, build, revision))
         {
            MessageBuffer msgb(std::basic_string<CharT>(buffer + offset, tail).c_str());
            wchar_t msg[1024]{};
            _snwprintf_s(msg, _TRUNCATE, L"Version parsing failed for [%s] at char [%c]", msgb.message(), wchar_t(*tail));
            logger.Log(msg);
            error = ERROR_FILE_CORRUPT;
            success = false;
            continue;
         }

         major = (xmajor < 0) ? major : xmajor;
         minor = (xminor < 0) ? minor : xminor;
         build = (xbuild < 0) ? build + 1 : xbuild;
         revision = (xrevision < 0) ? revision : xrevision;

         CharT newVersion[256]{};

         if (!format(newVersion, _countof(newVersion), major, minor, build, revision))
         {
            wchar_t msg[1024]{};
            _snwprintf_s(msg, _TRUNCATE, L"Version formatting failed for [%d,%d,%d,%d]", major, minor, build, revision);
            logger.Log(msg);
            error = ERROR_FILE_CORRUPT;
            success = false;
            continue;
         }

         {
            MessageBuffer from(std::basic_string<CharT>(buffer + offset, tail).c_str());
            MessageBuffer to(std::basic_string<CharT>(newVersion).c_str());
            wchar_t msg[1024]{};
            _snwprintf_s(msg, _TRUNCATE, L"Replacing [%s] with [%s]", from.message(), to.message());
            if (3 <= verbosity)
               logger.Log(msg);
         }

         if (!replace(buffer + offset, chars - offset, tail - (buffer + offset), newVersion))
         {
            MessageBuffer msg1;
            msg1.append(std::basic_string<CharT>(buffer + offset, tail).c_str());
            MessageBuffer msg2;
            msg2.append(newVersion);
            wchar_t msg[1024]{};
            _snwprintf_s(msg, _TRUNCATE, L"Version replace failed for [%s] / [%s]", msg1.message(), msg2.message());
            logger.Log(msg);
            error = ERROR_INSUFFICIENT_BUFFER;
            success = false;
            continue;
         }

         ++replacementsMade;
      }

      return success ? replacementsMade : 0;
   }
};
