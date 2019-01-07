// Win32Project.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Win32Project.h"
#include "Tool.h"

void getLastErrorTest();
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void CreateButton(HWND hwnd);
void CreateMainWindow();

HINSTANCE hAppInstance;

//hInstance imageBase
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	hAppInstance = hInstance;
	//DbgPrintf("hInstance:%x, %x\n", hInstance, hInstance->unused);
	//getLastErrorTest();

	CreateMainWindow();


	return 0;
}

void CreateMainWindow()
{
	//窗口的类名
	TCHAR className[] = TEXT("My First Window");

	//创建窗口类的对象
	WNDCLASS wndclass = { 0 };						//一定要先将所有值赋值
	wndclass.hbrBackground = (HBRUSH)COLOR_MENU;	//窗口的背景色
	wndclass.lpfnWndProc = WindowProc;				//窗口的过程函数
	wndclass.lpszClassName= className;				//窗口类的名字
	wndclass.hInstance = hAppInstance;					//定义窗口类的应用程序的实例句柄

	//注册窗口类
	RegisterClass(&wndclass);

	//创建窗口
	HWND hwnd = CreateWindow(
		className,					//类名
		TEXT("我的第一个窗口"),		//窗口标题
		WS_OVERLAPPEDWINDOW,		//窗口外观样式
		10,							//相对于父窗口的X坐标
		10,							//相对于父窗口的Y坐标
		600,						//窗口的宽度
		300,						//窗口的高度
		NULL,						//父窗口句柄
		NULL,						//菜单句柄
		hAppInstance,					//当前应用程序的句柄
		NULL);						//附加数据

	//是否创建成功
	if(hwnd == NULL)
		return ;

	//显示窗口
	ShowWindow(hwnd, SW_SHOW);

	CreateButton(hwnd);
	//DbgPrintf("hwnd:%x\n", hwnd->unused);

	//消息循环
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		//DbgPrintf("message type:%x\n", msg.message);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return ;
}


LRESULT CALLBACK WindowProc(IN HWND hwnd, IN UINT uMsg, IN WPARAM wParam, IN LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CREATE:
		{
			DbgPrintf("WM_CREATE %d %d\n", wParam, lParam);
			CREATESTRUCT* createst = (LPCREATESTRUCT)lParam;
			DbgPrintf("CREATESTRUCT %s\n", createst->lpszClass);
			return 0;
		}
	case WM_MOVE:
		{
			DbgPrintf("WM_MOVE %d %d\n", wParam, lParam);
			POINTS point = MAKEPOINTS(lParam);
			DbgPrintf("X Y %d %d\n", point.x, point.y);
			return 0;
		}
	case WM_SIZE:
		{
			DbgPrintf("WM_SIZE %d %d\n", wParam, lParam);
			int newWidth = (int)(short)LOWORD(lParam);
			int newHeight = (int)(short)HIWORD(lParam);
			DbgPrintf("WM_SIZE %d %d\n", newWidth, newHeight);
			return 0;
		}
	case WM_DESTROY:
		{
			DbgPrintf("WM_DESTROY %d, %d\n");
			PostQuitMessage(0);
			return 0;
		}
	case WM_KEYUP:
		{
			DbgPrintf("WM_KEYUP %d %d\n", wParam, lParam);
			return 0;
		}
	case WM_KEYDOWN:
		{
			DbgPrintf("WM_KKEYDOWN %d %d\n", wParam, lParam);
			return 0;
		}
	case WM_LBUTTONDOWN:
		{
			DbgPrintf("WM_LBUTTONDOWN %d %d\n", wParam, lParam);
			POINTS point MAKEPOINTS(lParam);
			DbgPrintf("WM_LBUTTONDOWN %d %d\n", point.x, point.y);
			return 0;
		}
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case 1001:
				MessageBox(hwnd, TEXT("Hello Button 1"), "Demo", MB_OK);
				return 0;
			case 1002:
				MessageBox(hwnd, TEXT("Hello Button 2"), "Demo", MB_OK);
				return 0;
			case 1003:
				MessageBox(hwnd, TEXT("Hello Button 3"), "Demo", MB_OK);
				return 0;

			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CreateButton(HWND hwnd)
{
	HWND hwndPushButton;
	HWND hwndCheckBox;
	HWND hwndRadio;

	hwndPushButton = CreateWindow(
		TEXT("button"),
		TEXT("普通按钮"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,
		10, 10,
		80, 20,
		hwnd,
		(HMENU)1001,
		hAppInstance,
		NULL
		);

	hwndCheckBox = CreateWindow(
			TEXT("button"),
			TEXT("复选框"),
			WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_AUTOCHECKBOX,
			10, 40,
			80, 20,
			hwnd,
			(HMENU)1002,
			hAppInstance,
			NULL
		);

	hwndRadio = CreateWindow(
			TEXT("button"),
			TEXT("单选按钮"),
			WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			10, 70,
			80, 20,
			hwnd,
			(HMENU)1003,
			hAppInstance,
			NULL
			);

	TCHAR szBuffer[0x20];
	GetClassName(hwndPushButton, szBuffer, 0x20);

	WNDCLASS wc;
	GetClassInfo(hAppInstance, szBuffer, &wc);
	DbgPrintf("className------>%s\n", wc.lpszClassName);
	DbgPrintf("------>%x\n", wc.lpfnWndProc);

}

void getLastErrorTest()
{
	TCHAR szContent[] = TEXT("内容");
	TCHAR szTitle[] = TEXT("标题");
	MessageBox((HWND)10, szContent, szTitle, MB_OK);
	DWORD errorCode = GetLastError();
}