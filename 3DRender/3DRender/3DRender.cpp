// 3DRender.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "3DRender.h"
#include "Device.h"
#include "Camera.h"
#include "Define/GeometryDefine.h"


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
void Input(WPARAM wParam);

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
	if (!InitInstance(hInstance, nCmdShow))
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
		Sleep(33);

	}
	RenderEnd();

	return (int)msg.wParam;
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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY3DRENDER));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

	case WM_KEYDOWN:        //键盘按键被按下
		Input(wParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

///*----------------------------------------Render------------------------------------------*/
//视频缓存
UINT Buffer[SCREEN_WIDTH*SCREEN_HEIGHT];
//渲染装置
Device *device;
//相机
UVNCamera *camera;

GameObject *object0;

void RenderMain()
{
	device->Render();
	device->RenderTexture(TextureBuffer, TEXTURE_WIDTH, TEXTURE_HEIGHT);
	Display();
	Sleep(33);


}

void RenderInit()
{
	Vertex3D *vertexList = (Vertex3D*)malloc(sizeof(Vertex3D)* 8);
	vertexList[0].vertex = { 1, -1, 1, 1 };
	vertexList[0].color = Color(0x00FF0000);
	vertexList[0].normal = { 0.0, 0.0, 0.0 };

	vertexList[1].vertex = { -1, -1, 1, 1 };
	vertexList[1].color = Color(0x0000FF00);
	vertexList[1].normal = { 0.0, 0.0, 0.0 };;

	vertexList[2].vertex = { -1, 1, 1, 1 };
	vertexList[2].color = Color(0x000000FF);
	vertexList[2].normal = { 0.0, 0.0, 0.0 };

	vertexList[3].vertex = { 1, 1, 1, 1 };
	vertexList[3].color = Color(0x00FF0000);
	vertexList[3].normal = { 0.0, 0.0, 0.0 };

	vertexList[4].vertex = { 1, -1, -1, 1 };
	vertexList[4].color = Color(0x0000FF00);
	vertexList[4].normal = { 0.0, 0.0, 0.0 };

	vertexList[5].vertex = { -1, -1, -1, 1 };
	vertexList[5].color = Color(0x000000FF);
	vertexList[5].normal = { 0.0, 0.0, 0.0 };

	vertexList[6].vertex = { -1, 1, -1, 1 };
	vertexList[6].color = Color(0x00FF0000);
	vertexList[6].normal = { 0.0, 0.0, 0.0 };

	vertexList[7].vertex = { 1, 1, -1, 1 };
	vertexList[7].color = Color(0x0000FF00);
	vertexList[7].normal = { 0.0, 0.0, 0.0 };

	Triangle *triangleList = (Triangle*)malloc(sizeof(Triangle)* 12);
	triangleList[0].id = 0;
	triangleList[0].vertexList = &vertexList[0];
	triangleList[0].index[0] = 0;
	triangleList[0].index[1] = 1;
	triangleList[0].index[2] = 2;
	triangleList[0].uv[0] = { 0.0, 1.0 };
	triangleList[0].uv[1] = { 0.0, 0.0 };
	triangleList[0].uv[2] = { 1.0, 0.0 };

	triangleList[1].id = 1;
	triangleList[1].vertexList = &vertexList[1];
	triangleList[1].index[0] = 2;
	triangleList[1].index[1] = 3;
	triangleList[1].index[2] = 0;
	triangleList[1].uv[0] = { 1.0, 0.0 };
	triangleList[1].uv[1] = { 1.0, 1.0 };
	triangleList[1].uv[2] = { 0.0, 1.0 };

	triangleList[2].id = 2;
	triangleList[2].vertexList = &vertexList[2];
	triangleList[2].index[0] = 6;
	triangleList[2].index[1] = 5;
	triangleList[2].index[2] = 4;
	triangleList[2].uv[0] = { 0.0, 1.0 };
	triangleList[2].uv[1] = { 0.0, 0.0 };
	triangleList[2].uv[2] = { 1.0, 0.0 };

	triangleList[3].id = 3;
	triangleList[3].vertexList = &vertexList[3];
	triangleList[3].index[0] = 4;
	triangleList[3].index[1] = 7;
	triangleList[3].index[2] = 6;
	triangleList[3].uv[0] = { 1.0, 0.0 };
	triangleList[3].uv[1] = { 1.0, 1.0 };
	triangleList[3].uv[2] = { 0.0, 1.0 };

	triangleList[4].id = 4;
	triangleList[4].vertexList = &vertexList[4];
	triangleList[4].index[0] = 7;
	triangleList[4].index[1] = 4;
	triangleList[4].index[2] = 0;
	triangleList[4].uv[0] = { 0.0, 1.0 };
	triangleList[4].uv[1] = { 0.0, 0.0 };
	triangleList[4].uv[2] = { 1.0, 0.0 };

	triangleList[5].id = 5;
	triangleList[5].vertexList = &vertexList[5];
	triangleList[5].index[0] = 0;
	triangleList[5].index[1] = 3;
	triangleList[5].index[2] = 7;
	triangleList[5].uv[0] = { 1.0, 0.0 };
	triangleList[5].uv[1] = { 1.0, 1.0 };
	triangleList[5].uv[2] = { 0.0, 1.0 };

	triangleList[6].id = 6;
	triangleList[6].vertexList = &vertexList[6];
	triangleList[6].index[0] = 1;
	triangleList[6].index[1] = 5;
	triangleList[6].index[2] = 6;
	triangleList[6].uv[0] = { 0.0, 1.0 };
	triangleList[6].uv[1] = { 0.0, 0.0 };
	triangleList[6].uv[2] = { 1.0, 0.0 };

	triangleList[7].id = 7;
	triangleList[7].vertexList = &vertexList[7];
	triangleList[7].index[0] = 6;
	triangleList[7].index[1] = 2;
	triangleList[7].index[2] = 1;
	triangleList[7].uv[0] = { 1.0, 0.0 };
	triangleList[7].uv[1] = { 1.0, 1.0 };
	triangleList[7].uv[2] = { 0.0, 1.0 };

	triangleList[8].id = 8;
	triangleList[8].vertexList = &vertexList[8];
	triangleList[8].index[0] = 5;
	triangleList[8].index[1] = 1;
	triangleList[8].index[2] = 0;
	triangleList[8].uv[0] = { 0.0, 1.0 };
	triangleList[8].uv[1] = { 0.0, 0.0 };
	triangleList[8].uv[2] = { 1.0, 0.0 };

	triangleList[9].id = 9;
	triangleList[9].vertexList = &vertexList[9];
	triangleList[9].index[0] = 0;
	triangleList[9].index[1] = 4;
	triangleList[9].index[2] = 5;
	triangleList[9].uv[0] = { 1.0, 0.0 };
	triangleList[9].uv[1] = { 1.0, 1.0 };
	triangleList[9].uv[2] = { 0.0, 1.0 };

	triangleList[10].id = 10;
	triangleList[10].vertexList = &vertexList[10];
	triangleList[10].index[0] = 3;
	triangleList[10].index[1] = 2;
	triangleList[10].index[2] = 6;
	triangleList[10].uv[0] = { 0.0, 1.0 };
	triangleList[10].uv[1] = { 0.0, 0.0 };
	triangleList[10].uv[2] = { 1.0, 0.0 };

	triangleList[11].id = 11;
	triangleList[11].vertexList = &vertexList[11];
	triangleList[11].index[0] = 6;
	triangleList[11].index[1] = 7;
	triangleList[11].index[2] = 3;
	triangleList[11].uv[0] = { 1.0, 0.0 };
	triangleList[11].uv[1] = { 1.0, 1.0 };
	triangleList[11].uv[2] = { 0.0, 1.0 };


	char *name = "object0";
	Point3D objectPosition = { 2.0, 0.0, 4, 1 };
	object0 = new GameObject(objectPosition, 0, 0, name, 8, vertexList, 12, triangleList);
	object0->state = RENDER_STATE_WIREFRAME;
	//载入纹理图
	hTextureBitmap = (HBITMAP)LoadImage(hInst, L"102.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	//获取纹理的像素
	GetDIBits(screen_hdc, hTextureBitmap, 0, TEXTURE_HEIGHT, TextureBuffer, (BITMAPINFO*)&binfoTex, DIB_RGB_COLORS);
	Material *m = new Material(TextureBuffer, TEXTURE_WIDTH, TEXTURE_HEIGHT);
	for (int i = 0; i < 12; i++)
	{
		triangleList[i].material = m;
	}

	Point3D camerPos = {0.0, 0.0f, 0, 1 };
	Vector3D v = { 0, 1, 0.0, 0 };
	camera = new UVNCamera(camerPos, {0,0,1}, v, 2, 4, 90, 1, SCREEN_WIDTH * 1.0f / SCREEN_HEIGHT);

	device = new Device(camera, SCREEN_WIDTH, SCREEN_HEIGHT, Buffer, RENDER_STATE_WIREFRAME, 0xFFFFFFFF, 0xFFFF0000);
	device->AddObjectList(object0);
}

void RenderEnd()
{
	device->ClearObjectList();
	//delete object1;
	//delete object2;
	delete device;
	delete camera;
	delete object0;
}

void Display()
{
	////将颜色数据打印到屏幕上，这下面两个函数每帧都得调用  
	SetDIBits(screen_hdc, hCompatibleBitmap, 0, SCREEN_HEIGHT, Buffer, (BITMAPINFO*)&binfo, DIB_RGB_COLORS);
	BitBlt(screen_hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hCompatibleDC, 0, 0, SRCCOPY);
}

void Input(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_UP:            //方向键上
		if (camera != nullptr)
		{
			Point3D pos = camera->GetPosition();
			camera->SetPosition(pos.x, pos.y + 0.1f, pos.z);
		}
		break;
	case VK_DOWN:        //方向键下
		if (camera != nullptr)
		{
			Point3D pos = camera->GetPosition();
			camera->SetPosition(pos.x, pos.y - 0.1f, pos.z);
		}
		break;
	case VK_LEFT:        //方向键左
		if (camera != nullptr)
		{
			Point3D pos = camera->GetPosition();
			camera->SetPosition(pos.x - 0.1f, pos.y, pos.z);
		}
		break;
	case VK_RIGHT:        //方向键右
		if (camera != nullptr)
		{
			Point3D pos = camera->GetPosition();
			camera->SetPosition(pos.x + 0.1f, pos.y, pos.z);
		}
		break;
	case VK_NUMPAD4:
		if (camera != nullptr)
		{
			Point3D pos = camera->GetPosition();
			camera->RotateHorizontal(-0.25f);
		}
		break;
	case VK_NUMPAD6:
		if (camera != nullptr)
		{
			Point3D pos = camera->GetPosition();
			camera->RotateHorizontal(0.25f);
		}
		break;
	}
}