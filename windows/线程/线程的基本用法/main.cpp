#include <windows.h>
#include <iostream>

#include "resource.h"

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

INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR hDialog;
HINSTANCE hAppIns;
HWND labelHandler;
HANDLE hThread;
CRITICAL_SECTION cs;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	InitializeCriticalSection(&cs);
	hAppIns = hInstance;
	hDialog = DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);	
	DeleteCriticalSection(&cs);
	return 0;
}

void W2C(char* pcstr, const wchar_t* pwstr)
{
	int nlength = wcslen(pwstr);
	int nbytes = WideCharToMultiByte(0, 0, pwstr, nlength, NULL, 0, NULL, NULL);
	WideCharToMultiByte(0, 0, pwstr, nlength, pcstr, nbytes, NULL, NULL);
	pcstr[nbytes] = '\0';
}


void C2W(wchar_t *wstr, const char* str)
{
	int len = MultiByteToWideChar(CP_OEMCP, 0, str, -1, NULL, 0);
	MultiByteToWideChar(CP_OEMCP, 0, str, -1, wstr, len);
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	/*
	WCHAR wstrBuff[5] = {0};
	char strBuff[10] = {0};
	int timeNum;

	GetWindowText(labelHandler, wstrBuff, 10);
	W2C(strBuff, wstrBuff);
	sscanf(strBuff, "%d", &timeNum);

	while(timeNum)
	{
		//::MultiByteToWideChar WideCharToMultiByte
		timeNum--;
		sprintf(strBuff, "%d", timeNum);
		//OutputDebugStringF("%d, %s\n", timeNum, strBuff);
		SetWindowText(labelHandler, (LPTSTR)strBuff);
		Sleep(1000);
		GetWindowText(labelHandler, wstrBuff, 10);
		W2C(strBuff, wstrBuff);
		sscanf(strBuff, "%d", &timeNum);
	}
	*/
	WCHAR wstrBuff[5] = {0};
	char strBuff[10] = {0};
	int timeNum = 0;
	while(timeNum < 1000)
	{
		/*if (timeNum == 100)
		{
			::ExitThread(3);
		}*/
		timeNum++;
		sprintf(strBuff, "%d", timeNum);
		C2W(wstrBuff, strBuff);
		SetWindowText(labelHandler, wstrBuff);
		Sleep(200);
	}

	//释放资源

	return 0;
}


DWORD WINAPI ThreadProc1(LPVOID lpParameter)
{

	WCHAR wstrBuff[5] = {0};
	char strBuff[10] = {0};
	int timeNum = 0;
	int count = 0;

	
	while(count < 10)
	{
		memset(wstrBuff, 0, 10);
		memset(strBuff, 0, 10);
		EnterCriticalSection(&cs);
		GetWindowText(labelHandler, wstrBuff, 10);
		Sleep(100);
		W2C(strBuff, wstrBuff);
		sscanf(strBuff, "%d", &timeNum);
		timeNum++;
		sprintf(strBuff, "%d", timeNum);
		C2W(wstrBuff, strBuff);
		SetWindowText(labelHandler, wstrBuff);
		LeaveCriticalSection(&cs);
		count++;
	}
	

	//释放资源

	return 0;
}

DWORD WINAPI ThreadProc2(LPVOID lpParameter)
{

	WCHAR wstrBuff[5] = {0};
	char strBuff[10] = {0};
	int timeNum = 0;
	int count = 0;


	while(count < 10)
	{
		memset(wstrBuff, 0, 10);
		memset(strBuff, 0, 10);
		EnterCriticalSection(&cs);
		GetWindowText(labelHandler, wstrBuff, 10);
		Sleep(100);
		W2C(strBuff, wstrBuff);
		sscanf(strBuff, "%d", &timeNum);
		timeNum++;
		sprintf(strBuff, "%d", timeNum);
		C2W(wstrBuff, strBuff);
		SetWindowText(labelHandler, wstrBuff);
		LeaveCriticalSection(&cs);
		count++;
	}


	//释放资源

	return 0;
}







INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		labelHandler = GetDlgItem(hwndDlg, IDC_EDIT_LABEL);
		SetWindowText(labelHandler, TEXT("0"));
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BUTTON_START:
			hThread = ::CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
			//::CloseHandle(hThread);
			//hThread = ::CreateThread(NULL, 0, ThreadProc1, NULL, 0, NULL);
			return TRUE;
		case IDC_BUTTON_2:
			{
			//hThread = ::CreateThread(NULL, 0, ThreadProc2, NULL, 0, NULL);
			::SuspendThread(hThread);
			/*CONTEXT context;
			context.ContextFlags = CONTEXT_CONTROL;
			BOOL ok = ::GetThreadContext(hThread, &context);
			context.Eip = 0x401000;
			SetThreadContext(hThread, &context);*/
			}
			return TRUE;
		case IDC_BUTTON_3:
			::ResumeThread(hThread);
			return TRUE;
		case IDC_BUTTON_4:
			::TerminateThread(hThread, 2);
			::WaitForSingleObject(hThread, INFINITE);  //等待线程终止
			return TRUE;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		return TRUE;
	}
	return FALSE;
}
