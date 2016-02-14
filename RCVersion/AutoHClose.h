#pragma once
#include "windows.h"

class AutoHClose
{
	HANDLE handle;
public:
	AutoHClose(HANDLE h) : handle(h) { }

	virtual ~AutoHClose()
	{
		DWORD error = GetLastError();
		Close();
		SetLastError(error);
	}

	void Close()
	{
		if (nullptr != handle)
			CloseHandle(handle);
		handle = nullptr;
	}
};

