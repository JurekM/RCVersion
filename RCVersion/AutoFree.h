#pragma once
#include "windows.h"

class AutoFree
{
	void* ptr;
public:
	AutoFree(void* p) : ptr(p) { }

	virtual ~AutoFree()
	{
		DWORD error = GetLastError();
		Free();
		SetLastError(error);
	}

	void Free()
	{
		if (nullptr != ptr)
			free(ptr);
		ptr = nullptr;
	}
};

