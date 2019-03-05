#include <stdio.h>
#include <windows.h>

void __cdecl OutputDebugStringF(const char *format, ...)  
{  
	va_list vlArgs;  
	char    *strBuffer = (char*)GlobalAlloc(GPTR, 4096);  
	va_start(vlArgs, format);  
	_vsnprintf(strBuffer, 4096 - 1, format, vlArgs);  
	va_end(vlArgs);  
	strcat(strBuffer, "\n");  
	OutputDebugStringA(strBuffer);  
	GlobalFree(strBuffer);  
	return;  
}  


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	HANDLE g_hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"XYZ");
	WaitForSingleObject(g_hMutex, INFINITE);

	while(true)
	{
		OutputDebugStringF("---------22222222------------\n");
		Sleep(1000);
	}

	ReleaseMutex(g_hMutex);
	return 0;
}