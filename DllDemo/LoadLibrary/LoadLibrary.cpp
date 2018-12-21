// LoadLibrary.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	HMODULE hMod = ::LoadLibrary(L"CreateDllDemo2.dll");
	if(hMod)
	{
		typedef int (*FUNC)(void);
		FUNC fPtr = (FUNC)GetProcAddress(hMod, "fnCreateDllDemo2");
		printf("%d\n", fPtr());
		FreeLibrary(hMod);
	}
	else
	{
		MessageBox(NULL, L"No this dll", L"Error Mark", MB_OK);
	}

	getchar();
	return 0;
}

