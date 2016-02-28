#pragma once
//#include <locale>
//#include <windows.h>
//
//inline wchar_t* LTrim(wchar_t* text, const wchar_t* chaff)
//{
//   while (*text) {
//      bool bTrimmed = false;
//      for (LPCWSTR c = chaff; *c; ++c) {
//         if (*text == *c) {
//            bTrimmed = true;
//            ++text;
//            break;
//         }
//      }
//      if (!bTrimmed)
//         break;
//   }
//   return text;
//}
//
//inline char* LTrim(char* text, const char* chaff)
//{
//   while (*text) {
//      bool bTrimmed = false;
//      for (const char* c = chaff; *c; ++c) {
//         if (*text == *c) {
//            bTrimmed = true;
//            ++text;
//            break;
//         }
//      }
//      if (!bTrimmed)
//         break;
//   }
//   return text;
//}
