#include <stdio.h>
#include <windows.h>

#include "resource.h"

HWND hLblTotal;
HWND hLblA;
HWND hLblB;
HWND hLblC;
CRITICAL_SECTION cs;
HANDLE hMutex;


INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	InitializeCriticalSection(&cs);
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);	
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

int GetEditNumber(HWND hWNDEdit)
{
	WCHAR wstrBuff[5] = {0};
	char strBuff[10] = {0};
	GetWindowText(hWNDEdit, wstrBuff, 5);
	W2C(strBuff, wstrBuff);
	int number = 0;
	sscanf(strBuff, "%d", &number);
	return number;
}

void SetEditText(HWND hWNDEdit, int deltaNumber)
{
	WCHAR wstrBuff[5] = {0};
	char strBuff[10] = {0};
	int number = 0;
	GetWindowText(hWNDEdit, wstrBuff, 5);
	W2C(strBuff, wstrBuff);
	sscanf(strBuff, "%d", &number);
	number += deltaNumber;
	memset(strBuff, 0, 10);
	sprintf(strBuff, "%d", number);
	C2W(wstrBuff, strBuff);
	SetWindowText(hWNDEdit, wstrBuff);
}


DWORD WINAPI ThreadProcA(LPVOID lpParamter)
{
	int leftCnt = 0;
	while(true)
	{
		//EnterCriticalSection(&cs);
		WaitForSingleObject(hMutex, INFINITE);
		leftCnt = GetEditNumber(hLblTotal);
		if(leftCnt >= 5)
		{
			SetEditText(hLblTotal, -5);
			SetEditText(hLblA, 5);
		}
		//LeaveCriticalSection(&cs);
		ReleaseMutex(hMutex);
		if (leftCnt <= 0)
		{
			break;
		}
		Sleep(2);
	}
	return 0;
}


DWORD WINAPI ThreadProcB(LPVOID lpParamter)
{
	int leftCnt = 0;
	while(true)
	{
		//EnterCriticalSection(&cs);
		WaitForSingleObject(hMutex, INFINITE);
		leftCnt = GetEditNumber(hLblTotal);
		if(leftCnt >= 5)
		{
			SetEditText(hLblTotal, -5);
			SetEditText(hLblB, 5);
		}
		//LeaveCriticalSection(&cs);
		ReleaseMutex(hMutex);
		if (leftCnt <= 0)
		{
			break;
		}
		Sleep(2);
	}
	return 0;
}

DWORD WINAPI ThreadProcC(LPVOID lpParamter)
{
	int leftCnt = 0;
	while(true)
	{
		//EnterCriticalSection(&cs);
		WaitForSingleObject(hMutex, INFINITE);
		leftCnt = GetEditNumber(hLblTotal);
		if(leftCnt >= 5)
		{
			SetEditText(hLblTotal, -5);
			SetEditText(hLblC, 5);
		}
		//LeaveCriticalSection(&cs);
		ReleaseMutex(hMutex);
		if (leftCnt <= 0)
		{
			break;
		}
		Sleep(2);
	}
	return 0;
}



DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	hMutex = CreateMutex(NULL, false, NULL);
	HANDLE threadA = ::CreateThread(NULL, 0, ThreadProcA, NULL, 0, NULL);
	HANDLE threadB = ::CreateThread(NULL, 0, ThreadProcB, NULL, 0, NULL);
	HANDLE threadC = ::CreateThread(NULL, 0, ThreadProcC, NULL, 0, NULL);
	//CloseHandle(threadA);
	//CloseHandle(threadB);
	//CloseHandle(threadC);

	HANDLE hArray[3] = {threadA, threadB, threadC};
	WaitForMultipleObjects(3, hArray, true, INFINITE);
	CloseHandle(hMutex);
	hMutex = NULL;
	
	return 0;
}


INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		hLblTotal = GetDlgItem(hwndDlg, IDC_EDIT_TOTAL);
		hLblA = GetDlgItem(hwndDlg, IDC_EDIT_A);
		hLblB = GetDlgItem(hwndDlg, IDC_EDIT_B);
		hLblC = GetDlgItem(hwndDlg, IDC_EDIT_C);
		SetWindowText(hLblTotal, TEXT("1000"));
		SetWindowText(hLblA, TEXT("0"));
		SetWindowText(hLblB, TEXT("0"));
		SetWindowText(hLblC, TEXT("0"));
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BUTTON_GO:
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
