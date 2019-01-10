#include <iostream>

#include "windows.h"

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

#define DbgPrintf OutputDebugStringF


INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR hDialog;
HINSTANCE hAppIns;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	hAppIns = hInstance;
	int a = 100;
	DbgPrintf("=============================================");
	hDialog = DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);	

	DbgPrintf("hEditUser:%d", (int)hDialog);
	
	return 0;
}

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch(uMsg)
	{
	case WM_INITDIALOG:
		//MessageBox(NULL, TEXT("WM_INITDIALOG"), TEXT("INIT"), MB_OK);
		return TRUE;
	case WM_COMMAND:
		HWND hEditUser;
		HWND hEditAccount;
		TCHAR szBuffer[0x50] = {0};
		switch(LOWORD(wParam))
		{
		case IDC_BUTTON_OK:
			hEditUser = GetDlgItem((HWND)hAppIns, IDC_EDIT_USER);
			hEditAccount = GetDlgItem((HWND)hDialog, IDC_EDIT_ACCOUNT);
			GetWindowText(hEditUser, szBuffer, 0x50);
			DbgPrintf("hEditUser:%d, %d", (int)hEditUser, (int)hDialog);
			DbgPrintf("user name: %s", szBuffer);
			GetWindowText(hEditAccount, szBuffer, 0x50);
			DbgPrintf("user account: %s", szBuffer);
			//MessageBox(NULL, TEXT("IDC_BUTTON_OK"), TEXT("OK"), MB_OK);
			return TRUE;
		case IDC_BUTTON_CANCEL:
			//MessageBox(NULL, TEXT("IDC_BUTTON_OUT"), TEXT("OUT"), MB_OK);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


