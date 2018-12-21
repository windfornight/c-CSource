// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBox(NULL, L"Dll process attach!", L"I love this mark!", MB_OK);
		break;
	case DLL_THREAD_ATTACH:
		MessageBox(NULL, L"Dll thread attach!", L"I love this mark!", MB_OK);
		break;
	case DLL_THREAD_DETACH:
		MessageBox(NULL, L"Dll thread detach!", L"I love this mark!", MB_OK);
		break;
	case DLL_PROCESS_DETACH:
		MessageBox(NULL, L"Dll process detach!", L"I love this mark!", MB_OK);
		break;
	}
	return TRUE;
}

