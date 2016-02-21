#pragma once
#include <string>
#include <vector>
#include "MessageBuffer.h"

template <class charT>
const charT** GetKeywordTable() { return nullptr; }

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

template<class charT, class traitsT = std::char_traits<charT>>
class RCUpdater
{
public:
   enum KTYPE { kSkip, kFixed, kString };

   ILogger &logger;
   bool verbose;
   bool debug;

   RCUpdater(ILogger &rlogger)
      : logger(rlogger)
      , verbose(false)
      , debug(false)
   {
   }

   virtual ~RCUpdater()
   {
   }

   // Left-trim chaff characters
   static charT* LTrim(charT*psz, const charT*chaff)
   {
      while (*psz)
      {
         bool found = false;
         for (const charT* c = chaff; *c && !found; ++c)
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
   charT* LSkipTo(charT*psz, const charT*chaff)
   {
      while (*psz)
      {
         for (const charT* c = chaff; *c; ++c)
         {
            if (*psz == *c)
               return psz;
         }
         ++psz;
      }
      return psz;
   }

   charT* NextLine(charT*line)
   {
      while (*line && '\n' != *line)
         ++line;
      return *line ? line + 1 : line;
   }

   static bool str2int(char* &psz, int& value, const char* chaff)
   {
      if (!isdigit(unsigned char(*psz)))
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

   static bool parse(charT*xbuffer, charT**tail, int &major, int &minor, int &build, int &revision)
   {
      static const charT space[] = { ' ','\t' , 0 };
      static const charT chaff[] = { '.', ',', ' ', '\t', 0 };
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

   static bool replace(charT *buffer, size_t totalChars, size_t oldChars, const charT* newString)
   {
      size_t newChars = traitsT::length(newString);
      if (oldChars < newChars)
      {
         size_t oldTail = traitsT::length(buffer + oldChars);
         if (totalChars <= oldTail + newChars)
            return false;
         traitsT::move(buffer + newChars, buffer + oldChars, oldTail + 1);
      }
      else if (newChars < oldChars)
      {
         traitsT::move(buffer + newChars, buffer + oldChars, traitsT::length(buffer + oldChars) + 1);
      }
      traitsT::copy(buffer, newString, newChars);
      return true;
   }

   // The next line after VERSIONINFO or zero if not found
   size_t FindStartOfVersion(charT *buffer)
   {
      static const charT space[] = { ' ', '\t', 0 };
      static const charT keyword[] = { 'V', 'E', 'R', 'S', 'I', 'O', 'N', 'I', 'N', 'F', 'O', 0 };
      size_t length = traitsT::length(keyword);

      charT* line = buffer;
      while (*line)
      {
         // Skip to the keyword
         line = LTrim(line, space);
         line = LSkipTo(line, space);
         line = LTrim(line, space);

         // Is this the space delimited keyword we need
         bool found = 0 == traitsT::compare(keyword, line, length);
         found = found && (unsigned char(line[length]) <= unsigned char(' '));
         line = NextLine(line);

         // Return the offset of the next line
         if (found)
            return line - buffer;
      }

      return 0;
   }

   void FindVersionStrings(charT *buffer, size_t start, std::vector<size_t> &offsets)
   {
      static const charT space[] = { ' ', '\t', 0 };
      static const charT chaff[] = { ',',  ' ', '\t', 0 };
      static const charT **keywords = GetKeywordTable<charT>();

      charT *line = buffer + start;
      while (*line)
      {
         line = LTrim(line, space);

         if ('#' == line[0] || '/' == line[0] && '/' == line[1])
         {
            line = NextLine(line);
            continue;
         }

         bool found = false;

         for (unsigned ndx = 0; 0 != keywords[ndx]; ++ndx)
         {
            const charT* keyword = keywords[ndx];
            const charT code = *keyword++;
            size_t length = traitsT::length(keyword);

            // This is a STRINGFILEINFO name, not a keyword
            if ('\"' == code)
               continue;

            if (0 != traitsT::compare(keyword, line, length))
               continue;
            if (' ' < line[length])
               continue;

            line = LTrim(line, space);
            found = true;
            wprintf(L"FOUND: [%c]:%s offset=%u\n", wchar_t(code), MessageBuffer(keyword).message(), unsigned(line - buffer));

            // FIXEDFILEINFO keyword, version follows after space
            if ('-' == code)
            {
               line = LTrim(line + length, space);
               size_t offset = line - buffer;
               offsets.push_back(offset);
               break;
            }

            // STRINGFILEINFO keyword, version may follow after string name
            if ('+' == code)
            {
               line = LSkipTo(line, space);
               line = LTrim(line, space);

               for (unsigned vx = 0; 0 != keywords[vx]; ++vx)
               {
                  const charT* name = keywords[vx];
                  const charT type = *name;
                  size_t chars = traitsT::length(name);
                  if ('\"' != type)
                     continue;
                  if (0 != traitsT::compare(name, line, chars))
                     continue;
                  line = LTrim(line + chars, chaff);
                  if (*line <= charT(' '))
                     continue;
                  if (charT('\"') == *line)
                     ++line;

                  line = LTrim(line, space);
                  wprintf(L"FOUND NAME: [%s] offset=%u\n", MessageBuffer(name).message(), unsigned(line - buffer));
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


   unsigned UpdateVersion(charT *buffer, size_t chars, int xmajor, int xminor, int xbuild, int xrevision)
   {
      size_t start = FindStartOfVersion(buffer);
      unsigned replacementsMade = 0;

      if (0 == start)
         return 0;

      std::vector<size_t> offsets;
      FindVersionStrings(buffer, start, offsets);

      if (0 == offsets.size())
         return 0;

      bool success = true;

      for (auto iter = offsets.rbegin(); offsets.rend() != iter; ++iter)
      {
         size_t offset = *iter;
         charT *tail = 0;
         int major = -1, minor = -1, build = -1, revision = -1;
         if (!parse(buffer + offset, &tail, major, minor, build, revision))
         {
            MessageBuffer msg(std::basic_string<charT>(buffer + offset, tail).c_str());
            wprintf(L"Version parsing failed for [%s] at char [%c]\n", msg.message(), wchar_t(*tail));
            success = false;
            continue;
         }

         major = (xmajor < 0) ? major : xmajor;
         minor = (xminor < 0) ? minor : xminor;
         build = (xbuild < 0) ? build + 1 : xbuild;
         revision = (xrevision < 0) ? revision : xrevision;

         charT newVersion[256] = { 0 };

         if (!format(newVersion, _countof(newVersion), major, minor, build, revision))
         {
            wprintf(L"Version formatting failed for [%d,%d,%d,%d]\n", major, minor, build, revision);
            success = false;
            continue;
         }

         if (!replace(buffer + offset, chars - offset, tail - (buffer + offset), newVersion))
         {
            MessageBuffer msg1;
            msg1.append(std::basic_string<charT>(buffer + offset, tail).c_str());
            MessageBuffer msg2;
            msg2.append(newVersion);
            wprintf(L"Version replace failed for [%s] / [%s]\n", msg1.message(), msg2.message());
            success = false;
            continue;
         }

         ++replacementsMade;
      }

      return success ? replacementsMade : 0;
   }
};
