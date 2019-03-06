#include <stdio.h>
#include <windows.h>

#include "resource.h"


HWND hLblTotal;
HWND hArrLbl[3];
HANDLE hEvent;


INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
	return 0;
}

DWORD WINAPI ThreadProc2(LPVOID lpParameter)
{
	DWORD index = (DWORD)lpParameter;
	HWND hLbl = hArrLbl[index];
	WCHAR wstrBuff[10] = {0};
	WaitForSingleObject(hEvent, INFINITE);
	GetWindowText(hLblTotal, wstrBuff, 10);
	SetWindowText(hLbl, wstrBuff);
	Sleep(1000);
	SetEvent(hEvent);
	return 0;
}


DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	//默认安全属性  手动设置未通知状态（TRUE)  初始状态未通知(FALSE)  名字
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	SetWindowText(hLblTotal, L"1000");

	HANDLE hArrThread[3];
	hArrThread[0] = ::CreateThread(NULL, 0, ThreadProc2, (LPVOID)0, 0, NULL);
	hArrThread[1] = ::CreateThread(NULL, 0, ThreadProc2, (LPVOID)1, 0, NULL);
	hArrThread[2] = ::CreateThread(NULL, 0, ThreadProc2, (LPVOID)2, 0, NULL);

	Sleep(1000);
	SetEvent(hEvent);

	WaitForMultipleObjects(3, hArrThread, TRUE, INFINITE);
	CloseHandle(hArrThread[0]);
	CloseHandle(hArrThread[1]);
	CloseHandle(hArrThread[2]);
	CloseHandle(hEvent);


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
		SetWindowText(hLblTotal, TEXT("0"));
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