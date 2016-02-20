#pragma once
#include <string>
#include <windows.h>
#include <stdarg.h>

class MessageBuffer
{
public:

   std::wstring buffer;

   MessageBuffer()
   {
   }

   virtual ~MessageBuffer()
   {
   }

   void clear()
   {
      buffer.clear();
   }

   const wchar_t* message() const
   {
      return buffer.c_str();
   }

   void append(const wchar_t *text)
   {
      buffer.append(text);
   }

   void append(const char* text)
   {
      wchar_t line[1024] = { 0 };
      MultiByteToWideChar(CP_UTF8, 0, text, int(strlen(text)), line, _countof(line) - 1);
      append(line);
   }

   void format(const wchar_t* format, ...)
   {
      wchar_t line[1024] = { 0 };
      va_list vList;
      va_start(vList, format);
      _vsnwprintf_s(line, _TRUNCATE, format, vList);
      append(line);
   }
};

