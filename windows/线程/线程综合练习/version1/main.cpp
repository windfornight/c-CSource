#include <stdio.h>
#include <windows.h>

#include "resource.h"

HWND hLabelMain;
HWND hLabelBuffA;
HWND hLabelBuffB;
HWND hLabelArr[4];

CRITICAL_SECTION csA;
CRITICAL_SECTION csB;

HANDLE hSemaphoreFull;  //信号量，缓冲区有资源
HANDLE hSemaphoreEmpty;  //新号量，缓冲区有空闲

INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);	
	return 0;
}


BOOL IsEditEmpty(HWND hwndEdit)
{
	WCHAR wstrBuff[5] = {0};
	GetWindowText(hwndEdit, wstrBuff, 5);
	return wstrBuff[0] == L'0';
}

DWORD WINAPI ThreadProcWrite(LPVOID lpParameter)
{
	WCHAR wstrBuff[100] = {};
	GetWindowText(hLabelMain, wstrBuff, 100);
	int index = 0;
	WCHAR wchar = wstrBuff[index];
	WCHAR wstr[2] = {0};
	while(wchar)
	{
		WaitForSingleObject(hSemaphoreEmpty, INFINITE);
		if(IsEditEmpty(hLabelBuffA))
		{
			EnterCriticalSection(&csA);
			if(IsEditEmpty(hLabelBuffA))
			{
				wstr[0] = wchar;
				SetWindowText(hLabelBuffA, wstr);
				wchar = wstrBuff[++index];
	
			}
			LeaveCriticalSection(&csA);	
		}else if(IsEditEmpty(hLabelBuffB))
		{
			EnterCriticalSection(&csB);
			if(IsEditEmpty(hLabelBuffB))
			{
				wstr[0] = wchar;
				SetWindowText(hLabelBuffB, wstr);
				wchar = wstrBuff[++index];
			}
			LeaveCriticalSection(&csB);
		}
		Sleep(1000);
		ReleaseSemaphore(hSemaphoreFull, 1, NULL);
	}
	
	return 0;
}

VOID addWindowText(HWND hLabel, WCHAR* wstr)
{
	WCHAR wstrBuff[100] = {0};
	GetWindowText(hLabel, wstrBuff, 100);
	wcscat(wstrBuff, wstr);
	SetWindowText(hLabel, wstrBuff);
}

DWORD WINAPI ThreadProcRead(LPVOID lpParameter)
{
	DWORD index = (DWORD)lpParameter;
	HWND hEdit = hLabelArr[index];
	WCHAR wstr[2] = {0};
	DWORD dwExitCode;
	while(true)
	{
		dwExitCode = WaitForSingleObject(hSemaphoreFull, 10000);  //注意退出线程
		if (dwExitCode == 0x102)
		{
			//ReleaseSemaphore(hSemaphoreFull, 1, NULL);
			return -1;
		}
		memset(wstr, 0, 4);
		if(!IsEditEmpty(hLabelBuffA))  //好像并不能并行
		{
			EnterCriticalSection(&csA);
			if (!IsEditEmpty(hLabelBuffA))
			{
				GetWindowText(hLabelBuffA, wstr, 2);
				SetWindowText(hLabelBuffA, L"0");
				addWindowText(hEdit, wstr);
			}
			LeaveCriticalSection(&csA);
			
		}else if(!IsEditEmpty(hLabelBuffB))
		{
			EnterCriticalSection(&csB);
			if(!IsEditEmpty(hLabelBuffB))
			{
				GetWindowText(hLabelBuffB, wstr, 2);
				SetWindowText(hLabelBuffB, L"0");
				addWindowText(hEdit, wstr);
			}
			LeaveCriticalSection(&csB);
			
		}
		ReleaseSemaphore(hSemaphoreEmpty, 1, NULL);
		Sleep(1000);
	}
	return 0;
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	InitializeCriticalSection(&csA);
	InitializeCriticalSection(&csB);
	hSemaphoreEmpty = CreateSemaphore(NULL, 2, 2, NULL);
	hSemaphoreFull = CreateSemaphore(NULL, 0, 2, NULL);

	HANDLE hThreadArr[5];
	hThreadArr[0] = CreateThread(NULL, 0, ThreadProcWrite, NULL, 0, NULL);
	hThreadArr[1] = CreateThread(NULL, 0, ThreadProcRead, (LPVOID)0, 0, NULL);
	hThreadArr[2] = CreateThread(NULL, 0, ThreadProcRead, (LPVOID)1, 0, NULL);
	hThreadArr[3] = CreateThread(NULL, 0, ThreadProcRead, (LPVOID)2, 0, NULL);
	hThreadArr[4] = CreateThread(NULL, 0, ThreadProcRead, (LPVOID)3, 0, NULL);

	

	WaitForMultipleObjects(5, hThreadArr, TRUE, INFINITE);

	for(int index = 0; index < 5; ++index)
	{
		CloseHandle(hThreadArr[index]);
	}

	DeleteCriticalSection(&csA);
	DeleteCriticalSection(&csB);
	CloseHandle(hSemaphoreFull);
	CloseHandle(hSemaphoreEmpty);

	return 0;
}


INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		hLabelMain = GetDlgItem(hwndDlg, IDC_EDIT_MAIN);
		hLabelBuffA = GetDlgItem(hwndDlg, IDC_EDIT_BUFF_A);
		hLabelBuffB = GetDlgItem(hwndDlg, IDC_EDIT_BUFF_B);
		hLabelArr[0] = GetDlgItem(hwndDlg, IDC_EDIT_A);
		hLabelArr[1] = GetDlgItem(hwndDlg, IDC_EDIT_B);
		hLabelArr[2] = GetDlgItem(hwndDlg, IDC_EDIT_C);
		hLabelArr[3] = GetDlgItem(hwndDlg, IDC_EDIT_D);
		SetWindowText(hLabelMain, L"ABCDEFGHIJKLMN");
		SetWindowText(hLabelBuffA, L"0");
		SetWindowText(hLabelBuffB, L"0");
		for(int index = 0; index < 4; ++index)
		{
			SetWindowText(hLabelArr[index], L"");
		}
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BUTTON_START:
			{
				HANDLE thread = ::CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
				::CloseHandle(thread);
			}
			return TRUE;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		return TRUE;
	}
	return FALSE;
}

