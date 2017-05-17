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
const int TEXTURE_WIDTH = 148;
const int TEXTURE_HEIGHT = 149;

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
HDC screen_hdc;
HWND screen_hwnd;
HDC hCompatibleDC; //兼容HDC  
HBITMAP hCompatibleBitmap; //兼容BITMAP  
HBITMAP hOldBitmap; //旧的BITMAP    
HBITMAP hTextureBitmap;//纹理图
UINT TextureBuffer[TEXTURE_WIDTH * TEXTURE_HEIGHT];
BITMAPINFO binfo; //BITMAPINFO结构体  
BITMAPINFO binfoTex;

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

   memset(&binfoTex, 0, sizeof(BITMAPINFO));
   binfoTex.bmiHeader.biBitCount = BITS;      //每个像素多少位，也可直接写24(RGB)或者32(RGBA)  
   binfoTex.bmiHeader.biCompression = BI_RGB;
   binfoTex.bmiHeader.biHeight = -TEXTURE_HEIGHT;
   binfoTex.bmiHeader.biPlanes = 1;
   binfoTex.bmiHeader.biSizeImage = 0;
   binfoTex.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   binfoTex.bmiHeader.biWidth = TEXTURE_WIDTH;

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

/*----------------------------------------Render------------------------------------------*/
//视频缓存
UINT Buffer[SCREEN_WIDTH*SCREEN_HEIGHT];
//渲染装置
Device *device;
//相机
UVNCamera *camera;
//测试用的正方体
Objecet *object1;
Objecet *object2;
//8个坐标
Point3D mesh[8] = {
	{ 1, -1, 1, 1 },
	{ -1, -1, 1, 1 },
	{ -1,  1, 1, 1 },
	{ 1,  1, 1, 1 },
	{ 1, -1, -1, 1 },
	{ -1, -1, -1, 1 },
	{ -1,  1, -1, 1 },
	{ 1,  1, -1, 1 },
};

UINT color[8] = { 0x00FF0000 , 0x0000FF00 , 0x000000FF , 0x00FF0000 , 0x0000FF00 , 0x000000FF , 0x00FF0000 , 0x0000FF00 };
//12个三角形
Triangle T1(mesh[0], mesh[1], mesh[2], color[0], color[1], color[2]);
Triangle T2(mesh[2], mesh[3], mesh[1], color[2], color[3], color[1]);

Triangle T3(mesh[6], mesh[5], mesh[4], color[6], color[5], color[4]);
Triangle T4(mesh[4], mesh[7], mesh[6], color[4], color[7], color[6]);

Triangle T5(mesh[7], mesh[4], mesh[0], color[7], color[4], color[0]);
Triangle T6(mesh[0], mesh[3], mesh[7], color[0], color[3], color[7]);

Triangle T7(mesh[1], mesh[5], mesh[6], color[1], color[5], color[6]);
Triangle T8(mesh[6], mesh[2], mesh[1], color[6], color[2], color[1]);

Triangle T9(mesh[5], mesh[1], mesh[0], color[5], color[1], color[0]);
Triangle T10(mesh[0], mesh[4], mesh[5], color[0], color[4], color[5]);

Triangle T11(mesh[3], mesh[2], mesh[6], color[3], color[2], color[6]);
Triangle T12(mesh[6], mesh[7], mesh[3], color[6], color[7], color[3]);
Triangle list2[12] = { T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12 };

Point2D p0 = { 0.0f, 0.0f };
Point2D p1 = { 1.0f, 0.0f };
Point2D p2 = { 1.0f, 1.0f };
Point2D p3 = { 0.0f, 1.0f };
Triangle t1(mesh[0], mesh[1], mesh[2], p3, p0, p1, TextureBuffer, TEXTURE_WIDTH, TEXTURE_HEIGHT);
Triangle t2(mesh[2], mesh[3], mesh[1], p1, p2, p3, TextureBuffer, TEXTURE_WIDTH, TEXTURE_HEIGHT);

Triangle t3(mesh[6], mesh[5], mesh[4], p3, p0, p1, TextureBuffer, TEXTURE_WIDTH, TEXTURE_HEIGHT);
Triangle t4(mesh[4], mesh[7], mesh[6], p1, p2, p3, TextureBuffer, TEXTURE_WIDTH, TEXTURE_HEIGHT);

Triangle t5(mesh[7], mesh[4], mesh[0], p3, p0, p1, TextureBuffer, TEXTURE_WIDTH, TEXTURE_HEIGHT);
Triangle t6(mesh[0], mesh[3], mesh[7], p1, p2, p3, TextureBuffer, TEXTURE_WIDTH, TEXTURE_HEIGHT);

Triangle t7(mesh[1], mesh[5], mesh[6], p3, p0, p1, TextureBuffer, TEXTURE_WIDTH, TEXTURE_HEIGHT);
Triangle t8(mesh[6], mesh[2], mesh[1], p1, p2, p3, TextureBuffer, TEXTURE_WIDTH, TEXTURE_HEIGHT);

Triangle t9(mesh[5], mesh[1], mesh[0], p3, p0, p1, TextureBuffer, TEXTURE_WIDTH, TEXTURE_HEIGHT);
Triangle t10(mesh[0], mesh[4], mesh[5], p1, p2, p3, TextureBuffer, TEXTURE_WIDTH, TEXTURE_HEIGHT);

Triangle t11(mesh[3], mesh[2], mesh[6], p3, p0, p1, TextureBuffer, TEXTURE_WIDTH, TEXTURE_HEIGHT);
Triangle t12(mesh[6], mesh[7], mesh[3], p1, p2, p3, TextureBuffer, TEXTURE_WIDTH, TEXTURE_HEIGHT);
//三角形列表
Triangle list1[12] = { t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12 };

void RenderMain()
{
	device->Render();
	device->RenderTexture(TextureBuffer, TEXTURE_WIDTH, TEXTURE_HEIGHT);
	Display();
	Sleep(33);
}

void RenderInit()
{
	//载入纹理图
	hTextureBitmap = (HBITMAP)LoadImage(hInst, L"102.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	//获取纹理的像素
	GetDIBits(screen_hdc, hTextureBitmap, 0, TEXTURE_HEIGHT, TextureBuffer, (BITMAPINFO*)&binfoTex, DIB_RGB_COLORS);

	Point3D objectPosition = { 1.9f, 0.5f, 4, 1 };

	Point3D camerPos = { -0.5, 2.0f, 0, 1 };
	Vector3D v = { 0, 1, 0.0, 0 };
	camera = new UVNCamera(camerPos, objectPosition, v, 2, 4, 90, 1, SCREEN_WIDTH * 1.0f / SCREEN_HEIGHT);

	device = new Device(camera, SCREEN_WIDTH, SCREEN_HEIGHT, Buffer, RENDER_STATE_WIREFRAME, 0xFFFFFFFF, 0xFFFF0000);

	object1 = new Objecet(objectPosition, 12, list1, RENDER_STATE_TEXTURE);
	object2 = new Objecet({ -2.0f, -1.5f, 5, 1 }, 12, list2, RENDER_STATE_COLOR);
	device->AddObjectList(object1);
	device->AddObjectList(object2);
}

void RenderEnd()
{
	device->ClearObjectList();
	delete object1;
	delete object2;
	delete device;
	delete camera;
	
}

void Display()
{
	//将颜色数据打印到屏幕上，这下面两个函数每帧都得调用  
	SetDIBits(screen_hdc, hCompatibleBitmap, 0, SCREEN_HEIGHT, Buffer, (BITMAPINFO*)&binfo, DIB_RGB_COLORS);
	BitBlt(screen_hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hCompatibleDC, 0, 0, SRCCOPY);
}