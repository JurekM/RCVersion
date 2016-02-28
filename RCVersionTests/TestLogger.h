#pragma once
#include <string>
#include <ILogger.h>


class TestLogger : public ILogger
{
public:
   std::wstring messages;
   void Log(const wchar_t* message) override
   {
      messages.append(message);
      messages.append(L"\r\n");
   }
};
