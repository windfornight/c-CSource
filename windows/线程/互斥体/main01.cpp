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


DWORD WINAPI ThreadProc1(LPVOID lpParameter)
{
	for(int i = 0; i < 5; i++)
	{
		OutputDebugStringF("++++++++\n");
		Sleep(1000);
		//ExitThread(3);
	}
	return 0;
}

DWORD WINAPI ThreadProc2(LPVOID lpParameter)
{
	for(int i = 0; i < 3; i++)
	{
		OutputDebugStringF("-------------\n");
		Sleep(1000);
	}
	return 0;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	/*
	HANDLE hArray[2];

	HANDLE hThread1 = ::CreateThread(NULL, 0, ThreadProc1, NULL, 0, NULL);
	//DWORD dwCode = ::WaitForSingleObject(hThread1, INFINITE);

	HANDLE hThread2 = ::CreateThread(NULL, 0, ThreadProc2, NULL, 0, NULL);
	hArray[0] = hThread1;
	hArray[1] = hThread2;

	DWORD dwCode = ::WaitForMultipleObjects(2, hArray, TRUE, INFINITE);

	MessageBox(0, 0, 0, 0);
	*/
	
	HANDLE g_hMutex = CreateMutex(NULL, false, L"XYZ");

	return 0;
}