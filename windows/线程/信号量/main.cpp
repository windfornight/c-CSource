#include <stdio.h>
#include <windows.h>

#include "resource.h"

HWND hLblTotal;
HWND hArrLbl[3];
HANDLE hSemaphore;


INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
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

DWORD WINAPI ThreadProc2(LPVOID lpParameter)
{
	DWORD index = (DWORD)lpParameter;
	HWND hLabel = hArrLbl[index];

	WCHAR wstrBuff[10] = {0};
	DWORD dwTimer = 0;

	WaitForSingleObject(hSemaphore, INFINITE);

	while(dwTimer < 100)
	{
		Sleep(100);
		memset(wstrBuff, 0, 20);
		//SetEditText(hLblTotal, -1);
		SetEditText(hLabel, 1);
		dwTimer++;
	}

	ReleaseSemaphore(hSemaphore, 1, NULL);

	return 0;
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	hSemaphore = ::CreateSemaphore(NULL, 0, 3, NULL);

	HANDLE hArrThread[3];

	hArrThread[0] = ::CreateThread(NULL, 0, ThreadProc2, (LPVOID)0, 0, NULL);
	hArrThread[1] = ::CreateThread(NULL, 0, ThreadProc2, (LPVOID)1, 0, NULL);
	hArrThread[2] = ::CreateThread(NULL, 0, ThreadProc2, (LPVOID)2, 0, NULL);

	SetWindowText(hLblTotal, TEXT("1000"));

	//��֪ͨ״̬
	ReleaseSemaphore(hSemaphore, 2, NULL);

	WaitForMultipleObjects(3, hArrThread, TRUE, INFINITE);
	CloseHandle(hArrThread[0]);
	CloseHandle(hArrThread[1]);
	CloseHandle(hArrThread[2]);
	CloseHandle(hSemaphore);

	return 0;
}



INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		hLblTotal = GetDlgItem(hwndDlg, IDC_EDIT_MAIN);
		hArrLbl[0] = GetDlgItem(hwndDlg, IDC_EDIT_A);
		hArrLbl[1] = GetDlgItem(hwndDlg, IDC_EDIT_B);
		hArrLbl[2] = GetDlgItem(hwndDlg, IDC_EDIT_C);
		SetWindowText(hLblTotal, TEXT("1000"));
		SetWindowText(hArrLbl[0], TEXT("0"));
		SetWindowText(hArrLbl[1], TEXT("0"));
		SetWindowText(hArrLbl[2], TEXT("0"));
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