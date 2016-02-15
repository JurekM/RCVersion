#pragma once

class ILogger
{
public:
   virtual ~ILogger() {}
   virtual void Log(const wchar_t* message) = 0;
};
