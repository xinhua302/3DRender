/*
*渲染装置
*/
#ifndef DEVICE_H
#define DEVICE_H
#include "3DMath.h"

const int RENDER_STATE_WIREFRAME = 1;	// 渲染线框
const int RENDER_STATE_COLOR = 2;		// 渲染纹理

class Device
{
public:
	int width;
	int height;
	UINT *frameBuffer;		//图像缓存
	int renderState;
	UINT background;		//背景颜色
	UINT foreground;		//线框颜色

public:
	Device(int width, int height, void *frameBuffer, int renderState, UINT foreground, UINT background)
	{
		Init(width, height, frameBuffer, renderState, foreground, background);
	}
	~Device()
	{
		
	}
	void Init(int width, int height, void *frameBuffer, int renderState, UINT foreground, UINT background)
	{
		this->width = width;
		this->height = height;
		this->frameBuffer = (UINT*)frameBuffer;
		this->renderState = renderState;
		this->foreground = foreground;
		this->background = background;
	}
	void Destroy()
	{
		if (frameBuffer != nullptr)
		{
			delete frameBuffer;
			frameBuffer = nullptr;
		}
	}
	void Clear()
	{
		memset(frameBuffer, 0, sizeof(UINT)* width * height);
	}
	//画点
	void DrawPoint(int x, int y, UINT color)
	{
		frameBuffer[x + y * width] = color;
	}
	//画线
	void DrawLine(int x1, int y1, int x2, int y2, UINT color)
	{
		int dy = y2 - y1;
		int dx = x2 - x1;
		int x = x1;
		int y = y1;
		if (dx == 0)
		{
			if (y <= y2)
			{
				while (y <= y2)
				{
					DrawPoint(x, y++, color);
				}
			}
			else
			{
				while (y > y2)
				{
					DrawPoint(x, y--, color);
				}
			}
			return;
		}
		float k = (float)dy / (float)dx;
		float b = y1 - k*x1;
		if (abs(k) < 1)
		{
			if (x <= x2)
			{
				while (x <= x2)
				{
					DrawPoint(x, y, color);
					y = (int)(k * ++x + b + 0.5);
				}
			}
			else
			{
				while (x > x2)
				{
					DrawPoint(x, y, color);
					y = (int)(k * --x + b + 0.5);
				}
			}
		}
		else
		{
			if (y <= y2)
			{
				while (y <= y2)
				{
					DrawPoint(x, y, color);
					x = (int)((++y - b) / k + 0.5);
				}
			}
			else
			{
				while (y > y2)
				{
					DrawPoint(x, y, color);
					x = (int)((--y - b) / k + 0.5);
				}
			}
		}
	}
	void DrawLine(const Point2D &p1, const Point2D &p2, UINT color)
	{
		DrawLine(p1.x, p1.y, p2.x, p2.y, color);
	}
	//画三角形
	void DrawTriangle(const Point2D &p1, const Point2D &p2, const Point2D &p3, UINT color)
	{	
		if (RENDER_STATE_WIREFRAME == renderState)
		{
			DrawLine(p1, p2, color);
			DrawLine(p2, p3, color);
			DrawLine(p3, p1, color);
		}
		else if (RENDER_STATE_COLOR == renderState)
		{
			//是否是平底或平顶三角形
			if (p1.y == p2.y)
			{
				if (p3.y > p2.y)
				{
					FillBottomTriangle(p1, p2, p3, color);
				}
				else if (p3.y < p2.y)
				{
					FillTopTriangle(p1, p2, p3, color);
				}
			}
			else if (p1.y == p3.y)
			{
				if (p2.y > p1.y)
				{
					FillBottomTriangle(p1, p2, p3, color);
				}
				else if (p2.y < p1.y)
				{
					FillTopTriangle(p1, p2, p3, color);
				}
			}
			else if (p2.y == p3.y)
			{
				if (p3.y > p2.y)
				{
					FillBottomTriangle(p1, p2, p3, color);
				}
				else if (p3.y < p2.y)
				{
					FillTopTriangle(p1, p2, p3, color);
				}
			}
			//不是平底也不是平顶,需要拆分
			else
			{
				//从上到下排列顶点
				Point2D pList[] = { p1, p2, p3 };
				SelectionSort(pList, 3);
				//(y - b) / k
				float k = (pList[0].y - pList[2].y) * 1.0f / (pList[0].x - pList[2].x);
				float b = pList[2].y - k*pList[2].x;
				int CenterPointX = (int)((pList[1].y - b) / k + 0.5);
				Point2D centerPoint = { CenterPointX, pList[1].y };
				FillTopTriangle(pList[0], centerPoint, pList[1], color);
				FillBottomTriangle(centerPoint, pList[1], pList[2], color);
			}
		}
	}

	//填充平顶三角形
	void FillTopTriangle(const Point2D &p1, const Point2D &p2, const Point2D &p3, UINT color)
	{
		Point2D top, left, right;
		if (p1.y == p2.y)
		{
			top = p3;
			left = p1;
			right = p2;
		}
		else if (p2.y == p3.y)
		{
			top = p1;
			left = p2;
			right = p3;
		}
		else
		{
			top = p2;
			left = p1;
			right = p3;
		}
		if (left.x > right.x)
		{
			Swap(left, right);
		}
		float leftDxDivDy = ((float)top.x - (float)left.x) / ((float)top.y - (float)left.y);
		float rightDxDivDy = ((float)right.x - (float)top.x) / ((float)right.y - (float)top.y);
		float xleft = (float)left.x;
		float xRight = (float)right.x;
		int y = left.y;
		while (y >= top.y)
		{
			DrawLine((int)(xleft + 0.5), y, (int)(xRight+0.5), y, color);
			y--;
			xleft -= leftDxDivDy;
			xRight -= rightDxDivDy;
		}
	}

	//填充平底三角形
	void FillBottomTriangle(const Point2D &p1, const Point2D &p2, const Point2D &p3, UINT color)
	{
		Point2D bottom, left, right;
		if (p1.y == p2.y)
		{
			bottom = p3;
			left = p1;
			right = p2;
		}
		else if (p2.y == p3.y)
		{
			bottom = p1;
			left = p2;
			right = p3;
		}
		else
		{
			bottom = p2;
			left = p1;
			right = p3;
		}
		if (left.x > right.x)
		{
			Swap(left, right);
		}

		float leftDxDivDy = ((float)bottom.x - (float)left.x) / ((float)bottom.y - (float)left.y);
		float rightDxDivDy = ((float)right.x - (float)bottom.x) / ((float)right.y - (float)bottom.y);
		float xleft = (float)left.x;
		float xRight = (float)right.x;
		int y = left.y;
		while (y <= bottom.y)
		{
			DrawLine((int)(xleft + 0.5), y, (int)(xRight + 0.5), y, color);
			y++;
			xleft += leftDxDivDy;
			xRight += rightDxDivDy;
		}
	}

	//渲染主函数
	void Render()
	{
		Clear();
		Point2D p1 = { 30, 250 };
		Point2D p2 = { 110, 350 };
		Point2D p3 = { 200, 220 };
		DrawTriangle(p1, p2, p3, foreground);
		DrawTriangle(p1, p2, p3, foreground);
	}
};


#endif
