#include <iostream>
#include <windows.h>

CRITICAL_SECTION cs;

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
	for (int x = 0; x < 1000; x++)
	{
		EnterCriticalSection(&cs); 
		Sleep(1000);
		OutputDebugStringF("1111:%d, %x, %x\n", cs.LockCount, cs.RecursionCount, cs.OwningThread);  //
		LeaveCriticalSection(&cs);
	}
	return 0;
}

DWORD WINAPI ThreadProc2(LPVOID lpParameter)
{
	for(int x = 0; x < 1000; x++)
	{
		EnterCriticalSection(&cs);
		Sleep(1000);
		OutputDebugStringF("2222:%d, %x, %x\n", cs.LockCount, cs.RecursionCount, cs.OwningThread);
		LeaveCriticalSection(&cs);
	}
	return 0;
}


DWORD WINAPI ThreadProc3(LPVOID lpParameter)
{
	for(int x = 0; x < 1000; x++)
	{
		EnterCriticalSection(&cs);
		Sleep(1000);
		OutputDebugStringF("3333:%d, %x, %x\n", cs.LockCount, cs.RecursionCount, cs.OwningThread);
		LeaveCriticalSection(&cs);
	}
	return 0;
}


DWORD WINAPI ThreadProc4(LPVOID lpParameter)
{
	for(int x = 0; x < 1000; x++)
	{
		EnterCriticalSection(&cs);
		Sleep(1000);
		OutputDebugStringF("44444:%d, %x, %x\n", cs.LockCount, cs.RecursionCount, cs.OwningThread);
		LeaveCriticalSection(&cs);
	}
	return 0;
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	InitializeCriticalSection(&cs);  //初始化临界区

	HANDLE hThread1 = ::CreateThread(NULL, 0, ThreadProc1, NULL, 0, NULL);

	HANDLE hThread2 = ::CreateThread(NULL, 0, ThreadProc2, NULL, 0, NULL);

	//HANDLE hThread3 = ::CreateThread(NULL, 0, ThreadProc3, NULL, 0, NULL);

	//HANDLE hThread4 = ::CreateThread(NULL, 0, ThreadProc4, NULL, 0, NULL);
	
	::CloseHandle(hThread1);
	::CloseHandle(hThread2);
	//::CloseHandle(hThread3);
	//::CloseHandle(hThread4);

	Sleep(1000*60*60);
	
	DeleteCriticalSection(&cs);  //删除临界区


	return 0;
}
