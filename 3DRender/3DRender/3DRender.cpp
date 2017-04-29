// 3DRender.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "3DRender.h"
#include "Device.h"
#include "Camera.h"


//屏幕宽度和高度   
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int BITS = 32;				//每个像素的位数  
const int MAX_LOADSTRING = 100;

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
HDC screen_hdc;
HWND screen_hwnd;
HDC hCompatibleDC; //兼容HDC  
HBITMAP hCompatibleBitmap; //兼容BITMAP  
HBITMAP hOldBitmap; //旧的BITMAP         
BITMAPINFO binfo; //BITMAPINFO结构体  

//视频缓存
UINT Buffer[SCREEN_WIDTH*SCREEN_HEIGHT];
//渲染装置
Device *device;
//相机
UVNCamera *camera;

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void RenderInit();
void RenderMain();
void RenderEnd();
void Display();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY3DRENDER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY3DRENDER));
	RenderInit();
	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		RenderMain();
		
	}
	RenderEnd();

	return (int) msg.wParam;
}



//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY3DRENDER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_CAPTION | WS_SYSMENU,
      CW_USEDEFAULT, 0, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   RECT  rectProgram, rectClient;
   GetWindowRect(hWnd, &rectProgram);   //获得程序窗口位于屏幕坐标
   GetClientRect(hWnd, &rectClient);      //获得客户区坐标
   //非客户区宽,高
   int nWidth = rectProgram.right - rectProgram.left - (rectClient.right - rectClient.left);
   int nHeiht = rectProgram.bottom - rectProgram.top - (rectClient.bottom - rectClient.top);
   nWidth += SCREEN_WIDTH;
   nHeiht += SCREEN_HEIGHT;
   rectProgram.right = nWidth;
   rectProgram.bottom = nHeiht;
   int showToScreenx = GetSystemMetrics(SM_CXSCREEN) / 2 - nWidth / 2;    //居中处理
   int showToScreeny = GetSystemMetrics(SM_CYSCREEN) / 2 - nHeiht / 2;
   MoveWindow(hWnd, showToScreenx, showToScreeny, rectProgram.right, rectProgram.bottom, false);

   memset(&binfo, 0, sizeof(BITMAPINFO));
   binfo.bmiHeader.biBitCount = BITS;      //每个像素多少位，也可直接写24(RGB)或者32(RGBA)  
   binfo.bmiHeader.biCompression = BI_RGB;
   binfo.bmiHeader.biHeight = -SCREEN_HEIGHT;
   binfo.bmiHeader.biPlanes = 1;
   binfo.bmiHeader.biSizeImage = 0;
   binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   binfo.bmiHeader.biWidth = SCREEN_WIDTH;

   //获取屏幕HDC  
   screen_hwnd = hWnd;
   screen_hdc = GetDC(screen_hwnd);

   //获取兼容HDC和兼容Bitmap,兼容Bitmap选入兼容HDC(每个HDC内存每时刻仅能选入一个GDI资源,GDI资源要选入HDC才能进行绘制)  
   hCompatibleDC = CreateCompatibleDC(screen_hdc);
   hCompatibleBitmap = CreateCompatibleBitmap(screen_hdc, SCREEN_WIDTH, SCREEN_HEIGHT);
   hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hCompatibleBitmap);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


void RenderMain()
{
	device->Render();
	Display();
	Sleep(33);
}

void RenderInit()
{
	Point3D camerPos = { 0, 0, 0, 1 };
	Vector3D u = { 1, 0, 0, 0 };
	Vector3D v = { 0, 1, 0, 0 };
	Vector3D n = { 0, 0, 1, 0 };
	camera = new UVNCamera(camerPos, u, v, n, 2, 4, 90, 100);
	device = new Device(camera, SCREEN_WIDTH, SCREEN_HEIGHT, Buffer, RENDER_STATE_WIREFRAME, 0xFFFFFFFF, 0xFFFF0000);
}

void RenderEnd()
{
	delete device;
	delete camera;
}

void Display()
{
	//将颜色数据打印到屏幕上，这下面两个函数每帧都得调用  
	SetDIBits(screen_hdc, hCompatibleBitmap, 0, SCREEN_HEIGHT, Buffer, (BITMAPINFO*)&binfo, DIB_RGB_COLORS);
	BitBlt(screen_hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hCompatibleDC, 0, 0, SRCCOPY);
}