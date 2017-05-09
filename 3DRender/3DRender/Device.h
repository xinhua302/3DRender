/*
*渲染装置
*/
#ifndef DEVICE_H
#define DEVICE_H
#include "3DMath.h"
#include "Camera.h"
#include "GeometryDefine .h"

const int RENDER_STATE_WIREFRAME = 1;	// 渲染线框
const int RENDER_STATE_COLOR = 2;		// 渲染颜色
const int RENDER_STATE_TEXTURE = 4;		// 渲染纹理

class Device
{
public:
	//渲染主函数
	void Render()
	{
		Clear();
		LocalToWorld();
		WorldToCamera();
		RemoveBackfaceTriangle();
		Projection();
		ViewTransform();
		RenderObject();
		/*Triangle t({200, 400, 200}, { 400, 400, 200 }, { 300, 100, 200 }, 0x00FF0000, 0x0000FF00, 0x000000FF, true);
		DrawTriangle(t);*/
	}
	void Clear()
	{
		memset(frameBuffer, 99, sizeof(UINT)* width * height);
	}
public:
	Device(UVNCamera *camera, int width, int height, void *frameBuffer, int renderState, UINT foreground, UINT background)
	{
		Init(camera, width, height, frameBuffer, renderState, foreground, background);
	}
	~Device()
	{

	}
	//设置裁剪区域
	void SetClip(int clipMinX, int clipMinY, int clipMaxX, int clipMaxY)
	{
		this->clipMaxX = clipMaxX;
		this->clipMaxY = clipMaxY;
		this->clipMinX = clipMinX;
		this->clipMinY = clipMinY;
	}
	//画点
	void DrawPoint(int x, int y, UINT color)
	{
		if (x < clipMinX || x >= clipMaxX || y < clipMinY || y >= clipMaxY)
		{
			return;
		}
		frameBuffer[(UINT)x + (UINT)y * (UINT)width] = color;
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
					y = (int)(k * ++x + b);
				}
			}
			else
			{
				while (x > x2)
				{
					DrawPoint(x, y, color);
					y = (int)(k * --x + b);
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
					x = (int)((++y - b) / k);
				}
			}
			else
			{
				while (y > y2)
				{
					DrawPoint(x, y, color);
					x = (int)((--y - b) / k);
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
				int CenterPointX = (int)((pList[1].y - b) / k);
				Point2D centerPoint = { CenterPointX, pList[1].y };
				FillTopTriangle(pList[0], centerPoint, pList[1], color);
				FillBottomTriangle(centerPoint, pList[1], pList[2], color);
			}
		}
	}
	void DrawTriangle(const Triangle &triangle, UINT color)
	{
		Point2D p1 = { (int)triangle.vertex[0].newPos.x, (int)triangle.vertex[0].newPos.y };
		Point2D p2 = { (int)triangle.vertex[1].newPos.x, (int)triangle.vertex[1].newPos.y };
		Point2D p3 = { (int)triangle.vertex[2].newPos.x, (int)triangle.vertex[2].newPos.y };
		DrawTriangle(p1, p2, p3, color);
	}
	void DrawTriangle(const Triangle &triangle)
	{
		if (triangle.vertex[0].newPos.y == triangle.vertex[1].newPos.y)
		{
			if (triangle.vertex[2].newPos.y > triangle.vertex[1].newPos.y)
			{
				FillBottomTriangle(triangle);
			}
			else if (triangle.vertex[2].newPos.y < triangle.vertex[1].newPos.y)
			{
				FillTopTriangle(triangle);
			}
		}
		else if (triangle.vertex[0].newPos.y == triangle.vertex[2].newPos.y)
		{
			if (triangle.vertex[1].newPos.y > triangle.vertex[0].newPos.y)
			{
				FillBottomTriangle(triangle);
			}
			else if (triangle.vertex[1].newPos.y < triangle.vertex[0].newPos.y)
			{
				FillTopTriangle(triangle);
			}
		}
		else if (triangle.vertex[1].newPos.y == triangle.vertex[2].newPos.y)
		{
			if (triangle.vertex[0].newPos.y > triangle.vertex[1].newPos.y)
			{
				FillBottomTriangle(triangle);
			}
			else if (triangle.vertex[0].newPos.y < triangle.vertex[1].newPos.y)
			{
				FillTopTriangle(triangle);
			}
		}
		//不是平底也不是平顶,需要拆分
		else
		{
			//从上到下排列顶点
			Triangle t = triangle;
			t.SortTopToBottom();
			//(y - b) / k
			Linear2D linear2D(t.vertex[0].newPos.x, t.vertex[0].newPos.y, t.vertex[2].newPos.x, t.vertex[2].newPos.y);
			float centerX = linear2D.InputYGetX(t.vertex[1].newPos.y);
			Point3D centerPos = { centerX,  t.vertex[1].newPos.y , 0 };
			UINT centerColorR = GetInterpValue(t.vertex[0].newPos.x, t.vertex[0].newPos.y, t.vertex[0].GetR(),
				t.vertex[1].newPos.x, t.vertex[1].newPos.y, t.vertex[1].GetR(), 
				t.vertex[2].newPos.x, t.vertex[2].newPos.y, t.vertex[2].GetR(),
				centerX, t.vertex[1].newPos.y);
			UINT centerColorG = GetInterpValue(t.vertex[0].newPos.x, t.vertex[0].newPos.y, t.vertex[0].GetG(),
				t.vertex[1].newPos.x, t.vertex[1].newPos.y, t.vertex[1].GetG(),
				t.vertex[2].newPos.x, t.vertex[2].newPos.y, t.vertex[2].GetG(),
				centerX, t.vertex[1].newPos.y);
			UINT centerColorB = GetInterpValue(t.vertex[0].newPos.x, t.vertex[0].newPos.y, t.vertex[0].GetB(),
				t.vertex[1].newPos.x, t.vertex[1].newPos.y, t.vertex[1].GetB(),
				t.vertex[2].newPos.x, t.vertex[2].newPos.y, t.vertex[2].GetB(),
				centerX, t.vertex[1].newPos.y);
			UINT centerColor = (centerColorR << 16) + (centerColorG << 8) + (centerColorB);
			Triangle t1(t.vertex[0].newPos, centerPos, t.vertex[1].newPos, t.vertex[0].color, centerColor, t.vertex[1].color, true);
			Triangle t2(centerPos, t.vertex[1].newPos, t.vertex[2].newPos, centerColor, t.vertex[1].color, t.vertex[2].color, true);
			FillTopTriangle(t1);
			FillBottomTriangle(t2);
		}
	}

	//纹理
	void DrawTriangleByTex(const Triangle &triangle)
	{
		if (triangle.vertex[0].newPos.y == triangle.vertex[1].newPos.y)
		{
			if (triangle.vertex[2].newPos.y > triangle.vertex[1].newPos.y)
			{
				FillBottomTriangleByTex(triangle);
			}
			else if (triangle.vertex[2].newPos.y < triangle.vertex[1].newPos.y)
			{
				FillTopTriangleByTex(triangle);
			}
		}
		else if (triangle.vertex[0].newPos.y == triangle.vertex[2].newPos.y)
		{
			if (triangle.vertex[1].newPos.y > triangle.vertex[0].newPos.y)
			{
				FillBottomTriangleByTex(triangle);
			}
			else if (triangle.vertex[1].newPos.y < triangle.vertex[0].newPos.y)
			{
				FillTopTriangleByTex(triangle);
			}
		}
		else if (triangle.vertex[1].newPos.y == triangle.vertex[2].newPos.y)
		{
			if (triangle.vertex[0].newPos.y > triangle.vertex[1].newPos.y)
			{
				FillBottomTriangleByTex(triangle);
			}
			else if (triangle.vertex[0].newPos.y < triangle.vertex[1].newPos.y)
			{
				FillTopTriangleByTex(triangle);
			}
		}
		//不是平底也不是平顶,需要拆分
		else
		{
			//从上到下排列顶点
			Triangle t = triangle;
			t.SortTopToBottom();
			Linear2D linear2D(t.vertex[0].newPos.x, t.vertex[0].newPos.y, t.vertex[2].newPos.x, t.vertex[2].newPos.y);
			float centerX = linear2D.InputYGetX(t.vertex[1].newPos.y);
			Point3D centerPos = { centerX, t.vertex[1].newPos.y, 0 };
			float centerU = GetInterpValue(t.vertex[0].newPos.x, t.vertex[0].newPos.y, t.vertex[0].uv.x,
				t.vertex[1].newPos.x, t.vertex[1].newPos.y, t.vertex[1].uv.x,
				t.vertex[2].newPos.x, t.vertex[2].newPos.y, t.vertex[2].uv.x,
				centerX, t.vertex[1].newPos.y);
			float centerV = GetInterpValue(t.vertex[0].newPos.x, t.vertex[0].newPos.y, t.vertex[0].uv.y,
				t.vertex[1].newPos.x, t.vertex[1].newPos.y, t.vertex[1].uv.y,
				t.vertex[2].newPos.x, t.vertex[2].newPos.y, t.vertex[2].uv.y,
				centerX, t.vertex[1].newPos.y);

			Point2D centerUV = { centerU, centerV };

			Triangle t1(t.vertex[0].newPos, centerPos, t.vertex[1].newPos, t.vertex[0].uv, centerUV, t.vertex[1].uv, t.texBuffer, t.texWidth, t.texWidth, true);
			Triangle t2(centerPos, t.vertex[1].newPos, t.vertex[2].newPos, centerUV, t.vertex[1].uv, t.vertex[2].uv, t.texBuffer, t.texWidth, t.texWidth, true);
			FillTopTriangleByTex(t1);
			FillBottomTriangleByTex(t2);
		}
	}

	//透视矫正纹理
	void DrawTriangleByTexCorrect(const Triangle &triangle)
	{
		if (triangle.vertex[0].newPos.y == triangle.vertex[1].newPos.y)
		{
			if (triangle.vertex[2].newPos.y > triangle.vertex[1].newPos.y)
			{
				FillBottomTriangleByTexCorrect(triangle);
			}
			else if (triangle.vertex[2].newPos.y < triangle.vertex[1].newPos.y)
			{
				FillTopTriangleByTexCorrect(triangle);
			}
		}
		else if (triangle.vertex[0].newPos.y == triangle.vertex[2].newPos.y)
		{
			if (triangle.vertex[1].newPos.y > triangle.vertex[0].newPos.y)
			{
				FillBottomTriangleByTexCorrect(triangle);
			}
			else if (triangle.vertex[1].newPos.y < triangle.vertex[0].newPos.y)
			{
				FillTopTriangleByTexCorrect(triangle);
			}
		}
		else if (triangle.vertex[1].newPos.y == triangle.vertex[2].newPos.y)
		{
			if (triangle.vertex[0].newPos.y > triangle.vertex[1].newPos.y)
			{
				FillBottomTriangleByTexCorrect(triangle);
			}
			else if (triangle.vertex[0].newPos.y < triangle.vertex[1].newPos.y)
			{
				FillTopTriangleByTexCorrect(triangle);
			}
		}
		//不是平底也不是平顶,需要拆分
		else
		{
			//从上到下排列顶点
			Triangle t = triangle;
			t.SortTopToBottom();
			Linear2D linear2D(t.vertex[0].newPos.x, t.vertex[0].newPos.y, t.vertex[2].newPos.x, t.vertex[2].newPos.y);
			float centerX = linear2D.InputYGetX(t.vertex[1].newPos.y);
			float centerZ = 1 / (GetInterpValue(t.vertex[0].newPos.x, t.vertex[0].newPos.y, 1 / t.vertex[0].newPos.z,
				t.vertex[1].newPos.x, t.vertex[1].newPos.y, 1 / t.vertex[1].newPos.z,
				t.vertex[2].newPos.x, t.vertex[2].newPos.y, 1 / t.vertex[2].newPos.z,
				centerX, t.vertex[1].newPos.y));
			Point3D centerPos = { centerX, t.vertex[1].newPos.y, centerZ };
			float centerU = GetInterpValue(t.vertex[0].newPos.x, t.vertex[0].newPos.y, t.vertex[0].uv.x / t.vertex[0].newPos.z,
				t.vertex[1].newPos.x, t.vertex[1].newPos.y, t.vertex[1].uv.x / t.vertex[1].newPos.z,
				t.vertex[2].newPos.x, t.vertex[2].newPos.y, t.vertex[2].uv.x / t.vertex[2].newPos.z,
				centerX, t.vertex[1].newPos.y);
			float centerV = GetInterpValue(t.vertex[0].newPos.x, t.vertex[0].newPos.y, t.vertex[0].uv.y / t.vertex[0].newPos.z,
				t.vertex[1].newPos.x, t.vertex[1].newPos.y, t.vertex[1].uv.y / t.vertex[1].newPos.z,
				t.vertex[2].newPos.x, t.vertex[2].newPos.y, t.vertex[2].uv.y / t.vertex[2].newPos.z,
				centerX, t.vertex[1].newPos.y);

			Point2D centerUV = { centerU * centerZ, centerV * centerZ };

			Triangle t1(t.vertex[0].newPos, centerPos, t.vertex[1].newPos, t.vertex[0].uv, centerUV, t.vertex[1].uv, t.texBuffer, t.texWidth, t.texHeight, true);
			Triangle t2(centerPos, t.vertex[1].newPos, t.vertex[2].newPos, centerUV, t.vertex[1].uv, t.vertex[2].uv, t.texBuffer, t.texWidth, t.texHeight, true);
			FillTopTriangleByTexCorrect(t1);
			FillBottomTriangleByTexCorrect(t2);
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
		y--;
		while (y >= top.y)
		{
			DrawLine((int)(xleft), y, (int)(xRight)-1, y, color);
			y--;
			xleft -= leftDxDivDy;
			xRight -= rightDxDivDy;
		}
	}

	void FillTopTriangle(const Triangle &t)
	{
		Vertex3D top, left, right;
		if (t.vertex[0].newPos.y == t.vertex[1].newPos.y)
		{
			top = t.vertex[2];
			left = t.vertex[0];
			right = t.vertex[1];
		}
		else if (t.vertex[1].newPos.y == t.vertex[2].newPos.y)
		{
			top = t.vertex[0];
			left = t.vertex[1];
			right = t.vertex[2];
		}
		else
		{
			top = t.vertex[1];
			left = t.vertex[0];
			right = t.vertex[2];;
		}
		if (left.newPos.x > right.newPos.x)
		{
			Swap(left, right);
		}
		float leftDxDivDy = (top.newPos.x - left.newPos.x) / ((float)top.newPos.y - (float)left.newPos.y);
		float rightDxDivDy = ((float)right.newPos.x - (float)top.newPos.x) / ((float)right.newPos.y - (float)top.newPos.y);
		float xleft = (float)left.newPos.x;
		float xRight = (float)right.newPos.x;
		int y = left.newPos.y;

		//R
		float leftDxDivDyColorR = (top.GetR() - left.GetR()) / (top.newPos.y - left.newPos.y);
		float rightDxDivDyColorR = (right.GetR() - top.GetR()) / (right.newPos.y - top.newPos.y);
		float xLeftColorR = left.GetR();
		float xRightColorR = right.GetR();
		//G
		float leftDxDivDyColorG = (top.GetG() - left.GetG()) / (top.newPos.y - left.newPos.y);
		float rightDxDivDyColorG = (right.GetG() - top.GetG()) / (right.newPos.y - top.newPos.y);
		float xLeftColorG = left.GetG();
		float xRightColorG = right.GetG();
		//B
		float leftDxDivDyColorB = (top.GetB() - left.GetB()) / (top.newPos.y - left.newPos.y);
		float rightDxDivDyColorB = (right.GetB() - top.GetB()) / (right.newPos.y - top.newPos.y);
		float xLeftColorB = left.GetB();
		float xRightColorB = right.GetB();
		y--;
		while (y >= top.newPos.y)
		{
			float dxColorR = (xRightColorR - xLeftColorR) / (xRight - xleft);
			float ColorStarR = xLeftColorR;

			float dxColorG = (xRightColorG - xLeftColorG) / (xRight - xleft);
			float ColorStarG = xLeftColorG;

			float dxColorB = (xRightColorB - xLeftColorB) / (xRight - xleft);
			float ColorStarB = xLeftColorB;
			for (int i = (int)(xleft); i < (int)(xRight); i++)
			{
				UINT color = (((UINT)ColorStarR) << 16) + (((UINT)ColorStarG) << 8) + (((UINT)ColorStarB));
				DrawPoint(i, y, color);
				ColorStarR += dxColorR;
				ColorStarG += dxColorG;
				ColorStarB += dxColorB;
			}
			y--;
			xleft -= leftDxDivDy;
			xRight -= rightDxDivDy;

			xLeftColorR -= leftDxDivDyColorR;
			xRightColorR -= rightDxDivDyColorR;

			xLeftColorG -= leftDxDivDyColorG;
			xRightColorG -= rightDxDivDyColorG;

			xLeftColorB -= leftDxDivDyColorB;
			xRightColorB -= rightDxDivDyColorB;
		}
	}

	//纹理
	void FillTopTriangleByTex(const Triangle &t)
	{
		Vertex3D top, left, right;
		if (t.vertex[0].newPos.y == t.vertex[1].newPos.y)
		{
			top = t.vertex[2];
			left = t.vertex[0];
			right = t.vertex[1];
		}
		else if (t.vertex[1].newPos.y == t.vertex[2].newPos.y)
		{
			top = t.vertex[0];
			left = t.vertex[1];
			right = t.vertex[2];
		}
		else
		{
			top = t.vertex[1];
			left = t.vertex[0];
			right = t.vertex[2];;
		}
		if (left.newPos.x > right.newPos.x)
		{
			Swap(left, right);
		}
		float leftDxDivDy = (top.newPos.x - left.newPos.x) / ((float)top.newPos.y - (float)left.newPos.y);
		float rightDxDivDy = ((float)right.newPos.x - (float)top.newPos.x) / ((float)right.newPos.y - (float)top.newPos.y);
		float xleft = (float)left.newPos.x;
		float xRight = (float)right.newPos.x;
		int y = left.newPos.y;

		//U
		float leftDxDivDyColorU = (top.uv.x - left.uv.x) / (top.newPos.y - left.newPos.y);
		float rightDxDivDyColorU = (right.uv.x - top.uv.x) / (right.newPos.y - top.newPos.y);
		float xLeftColorU = left.uv.x;
		float xRightColorU = right.uv.x;
		//V
		float leftDxDivDyColorV = (top.uv.y - left.uv.y) / (top.newPos.y - left.newPos.y);
		float rightDxDivDyColorV = (right.uv.y - top.uv.y) / (right.newPos.y - top.newPos.y);
		float xLeftColorV = left.uv.y;
		float xRightColorV = right.uv.y;

		while (y >= top.newPos.y)
		{
			float dxColorU = (xRightColorU - xLeftColorU) / (xRight - xleft);
			float ColorStarU = xLeftColorU;

			float dxColorV = (xRightColorV - xLeftColorV) / (xRight - xleft);
			float ColorStarV = xLeftColorV;

			for (int i = (int)(xleft); i <= (int)(xRight); i++)
			{
				float u = ColorStarU * (t.texWidth - 1);
				float v = ColorStarV * (t.texHeight - 1);
				if (u < 0 || v < 0 || u >= t.texWidth || v >= t.texHeight)
				{
					int haa = 0;
				}
				UINT color = t.texBuffer[(UINT)u + (UINT)v * (UINT)t.texWidth];
				DrawPoint(i, y, color);
				ColorStarU += dxColorU;
				ColorStarV += dxColorV;
			}
			y--;
			xleft -= leftDxDivDy;
			xRight -= rightDxDivDy;

			xLeftColorU -= leftDxDivDyColorU;
			xRightColorU -= rightDxDivDyColorU;

			xLeftColorV -= leftDxDivDyColorV;
			xRightColorV -= rightDxDivDyColorV;
		}
	}

	//透视矫正纹理
	void FillTopTriangleByTexCorrect(const Triangle &t)
	{
		Vertex3D top, left, right;
		if (t.vertex[0].newPos.y == t.vertex[1].newPos.y)
		{
			top = t.vertex[2];
			left = t.vertex[0];
			right = t.vertex[1];
		}
		else if (t.vertex[1].newPos.y == t.vertex[2].newPos.y)
		{
			top = t.vertex[0];
			left = t.vertex[1];
			right = t.vertex[2];
		}
		else
		{
			top = t.vertex[1];
			left = t.vertex[0];
			right = t.vertex[2];;
		}
		if (left.newPos.x > right.newPos.x)
		{
			Swap(left, right);
		}
		float leftDxDivDy = (top.newPos.x - left.newPos.x) / ((float)top.newPos.y - (float)left.newPos.y);
		float rightDxDivDy = ((float)right.newPos.x - (float)top.newPos.x) / ((float)right.newPos.y - (float)top.newPos.y);
		float xleft = (float)left.newPos.x;
		float xRight = (float)right.newPos.x;
		int y = left.newPos.y;

		//1/Z
		float leftDxDivDyReciprocalZ = ((1 / top.newPos.z) - (1 / left.newPos.z)) / (top.newPos.y - left.newPos.y);
		float rightDxDivDyReciprocalZ = ((1 / right.newPos.z) - (1 / top.newPos.z)) / (right.newPos.y - top.newPos.y);
		float xLeftReciprocalZ = 1 / left.newPos.z;
		float xRightReciprocalZ = 1 / right.newPos.z;

		//U
		float leftDxDivDyColorU = (top.uv.x / top.newPos.z - left.uv.x / left.newPos.z) / (top.newPos.y - left.newPos.y);
		float rightDxDivDyColorU = (right.uv.x / right.newPos.z - top.uv.x / top.newPos.z) / (right.newPos.y - top.newPos.y);
		float xLeftColorU = left.uv.x / left.newPos.z;
		float xRightColorU = right.uv.x / right.newPos.z;
		//V
		float leftDxDivDyColorV = (top.uv.y / top.newPos.z - left.uv.y / left.newPos.z) / (top.newPos.y - left.newPos.y);
		float rightDxDivDyColorV = (right.uv.y / right.newPos.z - top.uv.y/ top.newPos.z) / (right.newPos.y - top.newPos.y);
		float xLeftColorV = left.uv.y / left.newPos.z;
		float xRightColorV = right.uv.y / right.newPos.z;
		y--;
		while (y >= top.newPos.y)
		{
			float dxReciprocalZ = (xRightReciprocalZ - xLeftReciprocalZ) / (xRight - xleft);
			float reciprocalZ = xLeftReciprocalZ;

			float dxColorU = (xRightColorU - xLeftColorU) / (xRight - xleft);
			float ColorStarU = xLeftColorU;

			float dxColorV = (xRightColorV - xLeftColorV) / (xRight - xleft);
			float ColorStarV = xLeftColorV;

			for (int i = (int)(xleft); i < (int)(xRight); i++)
			{
				float u = (ColorStarU / reciprocalZ) * (t.texWidth-1);
				float v = (ColorStarV / reciprocalZ) * (t.texHeight-1);
				if (u < 0 || v < 0 || u >= t.texWidth || v >= t.texHeight)
				{
					int haa = 0;
				}
				UINT color = t.texBuffer[(UINT)u + (UINT)v * (UINT)t.texWidth];
				DrawPoint(i, y, color);
				ColorStarU += dxColorU;
				ColorStarV += dxColorV;
				reciprocalZ += dxReciprocalZ;
			}
			y--;
			xleft -= leftDxDivDy;
			xRight -= rightDxDivDy;

			xLeftReciprocalZ -= leftDxDivDyReciprocalZ;
			xRightReciprocalZ -= rightDxDivDyReciprocalZ;

			xLeftColorU -= leftDxDivDyColorU;
			xRightColorU -= rightDxDivDyColorU;

			xLeftColorV -= leftDxDivDyColorV;
			xRightColorV -= rightDxDivDyColorV;
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
		while (y < bottom.y)
		{
			DrawLine((int)(xleft), y, (int)(xRight)-1, y, color);
			y++;
			xleft += leftDxDivDy;
			xRight += rightDxDivDy;
		}
	}

	void FillBottomTriangle(const Triangle &t)
	{
		Vertex3D bottom, left, right;
		if (t.vertex[0].newPos.y == t.vertex[1].newPos.y)
		{
			bottom = t.vertex[2];
			left = t.vertex[0];
			right = t.vertex[1];
		}
		else if (t.vertex[1].newPos.y == t.vertex[2].newPos.y)
		{
			bottom = t.vertex[0];
			left = t.vertex[1];
			right = t.vertex[2];
		}
		else
		{
			bottom = t.vertex[1];
			left = t.vertex[0];
			right = t.vertex[2];;
		}
		if (left.newPos.x > right.newPos.x)
		{
			Swap(left, right);
		}
		float leftDxDivDy = (bottom.newPos.x - left.newPos.x) / ((float)bottom.newPos.y - (float)left.newPos.y);
		float rightDxDivDy = ((float)right.newPos.x - (float)bottom.newPos.x) / ((float)right.newPos.y - (float)bottom.newPos.y);
		float xleft = (float)left.newPos.x;
		float xRight = (float)right.newPos.x;
		int y = left.newPos.y;

		//颜色
		//R
		float leftDxDivDyColorR= (bottom.GetR() - left.GetR()) / (bottom.newPos.y - left.newPos.y);
		float rightDxDivDyColorR = (right.GetR() - bottom.GetR()) / (right.newPos.y - bottom.newPos.y);
		float xLeftColorR = left.GetR();
		float xRightColorR = right.GetR();
		//G
		float leftDxDivDyColorG = (bottom.GetG() - left.GetG()) / (bottom.newPos.y - left.newPos.y);
		float rightDxDivDyColorG = (right.GetG() - bottom.GetG()) / (right.newPos.y - bottom.newPos.y);
		float xLeftColorG = left.GetG();
		float xRightColorG = right.GetG();
		//B
		float leftDxDivDyColorB = (bottom.GetB() - left.GetB()) / (bottom.newPos.y - left.newPos.y);
		float rightDxDivDyColorB = (right.GetB() - bottom.GetB()) / (right.newPos.y - bottom.newPos.y);
		float xLeftColorB = left.GetB();
		float xRightColorB = right.GetB();

		while (y < bottom.newPos.y)
		{
			float dxColorR = (xRightColorR - xLeftColorR) / (xRight - xleft);
			float ColorStarR = xLeftColorR;

			float dxColorG = (xRightColorG - xLeftColorG) / (xRight - xleft);
			float ColorStarG = xLeftColorG;

			float dxColorB = (xRightColorB - xLeftColorB) / (xRight - xleft);
			float ColorStarB = xLeftColorB;
			for (int i = (int)(xleft); i < (int)(xRight); i++)
			{
				UINT color = (((UINT)ColorStarR) << 16) + (((UINT)ColorStarG) << 8) + (((UINT)ColorStarB));
 				DrawPoint(i, y, color);
				ColorStarR+= dxColorR;
				ColorStarG += dxColorG;
				ColorStarB += dxColorB;
			}
			y++;
			xleft += leftDxDivDy;
			xRight += rightDxDivDy;

			xLeftColorR += leftDxDivDyColorR;
			xRightColorR += rightDxDivDyColorR;

			xLeftColorG += leftDxDivDyColorG;
			xRightColorG += rightDxDivDyColorG;

			xLeftColorB += leftDxDivDyColorB;
			xRightColorB += rightDxDivDyColorB;
		}
	}

	//纹理
	void FillBottomTriangleByTex(const Triangle &t)
	{
		Vertex3D bottom, left, right;
		if (t.vertex[0].newPos.y == t.vertex[1].newPos.y)
		{
			bottom = t.vertex[2];
			left = t.vertex[0];
			right = t.vertex[1];
		}
		else if (t.vertex[1].newPos.y == t.vertex[2].newPos.y)
		{
			bottom = t.vertex[0];
			left = t.vertex[1];
			right = t.vertex[2];
		}
		else
		{
			bottom = t.vertex[1];
			left = t.vertex[0];
			right = t.vertex[2];;
		}
		if (left.newPos.x > right.newPos.x)
		{
			Swap(left, right);
		}
		float leftDxDivDy = (bottom.newPos.x - left.newPos.x) / ((float)bottom.newPos.y - (float)left.newPos.y);
		float rightDxDivDy = ((float)right.newPos.x - (float)bottom.newPos.x) / ((float)right.newPos.y - (float)bottom.newPos.y);
		float xleft = (float)left.newPos.x;
		float xRight = (float)right.newPos.x;
		int y = left.newPos.y;

		//U
		float leftDxDivDyColorU = (bottom.uv.x - left.uv.x) / (bottom.newPos.y - left.newPos.y);
		float rightDxDivDyColorU = (right.uv.x - bottom.uv.x) / (right.newPos.y - bottom.newPos.y);
		float xLeftColorU = left.uv.x;
		float xRightColorU = right.uv.x;
		//V
		float leftDxDivDyColorV = (bottom.uv.y - left.uv.y) / (bottom.newPos.y - left.newPos.y);
		float rightDxDivDyColorV = (right.uv.y - bottom.uv.y) / (right.newPos.y - bottom.newPos.y);
		float xLeftColorV = left.uv.y;
		float xRightColorV = right.uv.y;


		while (y <= bottom.newPos.y)
		{
			float dxColorU = (xRightColorU - xLeftColorU) / (xRight - xleft);
			float ColorStarU = xLeftColorU;

			float dxColorV = (xRightColorV - xLeftColorV) / (xRight - xleft);
			float ColorStarV = xLeftColorV;
			for (int i = (int)(xleft); i <= (int)(xRight); i++)
			{
				float u = ColorStarU * (t.texWidth -1);
				float v = ColorStarV * (t.texHeight - 1);
				if (u < 0 || v < 0 || u >= t.texWidth || v >= t.texHeight)
				{
					int haa = 0;
				}
				UINT color = t.texBuffer[(UINT)u + (UINT)v * (UINT)t.texWidth];
				DrawPoint(i, y, color);
				ColorStarU += dxColorU;
				ColorStarV += dxColorV;
			}
			y++;
			xleft += leftDxDivDy;
			xRight += rightDxDivDy;

			xLeftColorU += leftDxDivDyColorU;
			xRightColorU += rightDxDivDyColorU;

			xLeftColorV += leftDxDivDyColorV;
			xRightColorV += rightDxDivDyColorV;
		}
	}

	//透视矫正纹理
	void FillBottomTriangleByTexCorrect(const Triangle &t)
	{
		Vertex3D bottom, left, right;
		if (t.vertex[0].newPos.y == t.vertex[1].newPos.y)
		{
			bottom = t.vertex[2];
			left = t.vertex[0];
			right = t.vertex[1];
		}
		else if (t.vertex[1].newPos.y == t.vertex[2].newPos.y)
		{
			bottom = t.vertex[0];
			left = t.vertex[1];
			right = t.vertex[2];
		}
		else
		{
			bottom = t.vertex[1];
			left = t.vertex[0];
			right = t.vertex[2];;
		}
		if (left.newPos.x > right.newPos.x)
		{
			Swap(left, right);
		}
		float leftDxDivDy = (bottom.newPos.x - left.newPos.x) / ((float)bottom.newPos.y - (float)left.newPos.y);
		float rightDxDivDy = ((float)right.newPos.x - (float)bottom.newPos.x) / ((float)right.newPos.y - (float)bottom.newPos.y);
		float xleft = (float)left.newPos.x;
		float xRight = (float)right.newPos.x;
		int y = left.newPos.y;

		//1/Z
		float leftDxDivDyReciprocalZ = ((1 / bottom.newPos.z) - (1 / left.newPos.z)) / (bottom.newPos.y - left.newPos.y);
		float rightDxDivDyReciprocalZ = ((1 / right.newPos.z) - (1 / bottom.newPos.z)) / (right.newPos.y - bottom.newPos.y);
		float xLeftReciprocalZ = 1 / left.newPos.z;
		float xRightReciprocalZ = 1 / right.newPos.z;

		//U
		float leftDxDivDyColorU = (bottom.uv.x / bottom.newPos.z - left.uv.x / left.newPos.z) / (bottom.newPos.y - left.newPos.y);
		float rightDxDivDyColorU = (right.uv.x / right.newPos.z - bottom.uv.x / bottom.newPos.z) / (right.newPos.y - bottom.newPos.y);
		float xLeftColorU = left.uv.x / left.newPos.z;
		float xRightColorU = right.uv.x / right.newPos.z;
		//V
		float leftDxDivDyColorV = (bottom.uv.y / bottom.newPos.z - left.uv.y / left.newPos.z) / (bottom.newPos.y - left.newPos.y);
		float rightDxDivDyColorV = (right.uv.y / right.newPos.z - bottom.uv.y / bottom.newPos.z) / (right.newPos.y - bottom.newPos.y);
		float xLeftColorV = left.uv.y / left.newPos.z;
		float xRightColorV = right.uv.y / right.newPos.z;

		while (y < bottom.newPos.y)
		{
			float dxReciprocalZ = (xRightReciprocalZ - xLeftReciprocalZ) / (xRight - xleft);
			float reciprocalZ = xLeftReciprocalZ;

			float dxColorU = (xRightColorU - xLeftColorU) / (xRight - xleft);
			float ColorStarU = xLeftColorU;

			float dxColorV = (xRightColorV - xLeftColorV) / (xRight - xleft);
			float ColorStarV = xLeftColorV;
			for (int i = (int)(xleft); i < (int)(xRight); i++)
			{
				float u = (ColorStarU / reciprocalZ) * (t.texWidth-1);
				float v = (ColorStarV / reciprocalZ) * (t.texHeight-1);
				if (u < 0 || v < 0 || u >= t.texWidth || v >= t.texHeight)
				{
					int haa = 0;
				}
				UINT color = t.texBuffer[(UINT)u + (UINT)v * (UINT)t.texWidth];
				DrawPoint(i, y, color);
				ColorStarU += dxColorU;
				ColorStarV += dxColorV;
				reciprocalZ += dxReciprocalZ;
			}
			y++;
			xleft += leftDxDivDy;
			xRight += rightDxDivDy;

			xLeftReciprocalZ += leftDxDivDyReciprocalZ;
			xRightReciprocalZ += rightDxDivDyReciprocalZ;

			xLeftColorU += leftDxDivDyColorU;
			xRightColorU += rightDxDivDyColorU;

			xLeftColorV += leftDxDivDyColorV;
			xRightColorV += rightDxDivDyColorV;
		}
	}

	//将物体加入渲染列表
	void AddObjectList(Objecet *objecet)
	{
		if (objecet == nullptr)
		{
			return;
		}
		objecetList[objectListCount++] = objecet;
	}

	//渲染物体列表
	void RenderObject()
	{
		for (int i = 0; i < objectListCount; i++)
		{
			if (objecetList[i]->state == RENDER_STATE_WIREFRAME)
			{
				for (int j = 0; j < objecetList[i]->triangleCount; j++)
				{
					if (objecetList[i]->triangleList[j].state == TRIANGLE_BACKFACE)
						continue;

					DrawTriangle(objecetList[i]->triangleList[j], foreground);
				}
			}
			else if (objecetList[i]->state == RENDER_STATE_COLOR)
			{
				for (int j = 0; j < objecetList[i]->triangleCount; j++)
				{
					if (objecetList[i]->triangleList[j].state == TRIANGLE_BACKFACE)
						continue;

					DrawTriangle(objecetList[i]->triangleList[j]);
				}
			}
			else if (objecetList[i]->state == RENDER_STATE_TEXTURE)
			{
				for (int j = 0; j < objecetList[i]->triangleCount; j++)
				{
					if (objecetList[i]->triangleList[j].state == TRIANGLE_BACKFACE)
						continue;

					DrawTriangleByTexCorrect(objecetList[i]->triangleList[j]);
					
				}
			}
			
		}
	}

	//清空渲染列表
	void ClearObjectList()
	{
		objectListCount = 0;
	}

	//相机变换ss
	void WorldToCamera()
	{
		for (int i = 0; i < objectListCount; i++)
		{
			for (int j = 0; j < objecetList[i]->triangleCount; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					objecetList[i]->triangleList[j].vertex[k].newPos = camera->WolrdToCamera(objecetList[i]->triangleList[j].vertex[k].newPos);
				}
			}
		}
	}

	//背面剔除
	void RemoveBackfaceTriangle()
	{
		for (int i = 0; i < objectListCount; i++)
		{
			for (int j = 0; j < objecetList[i]->triangleCount; j++)
			{
				Vector3D v1 = objecetList[i]->triangleList[j].vertex[1].newPos - objecetList[i]->triangleList[j].vertex[0].newPos;
				Vector3D v2 =  objecetList[i]->triangleList[j].vertex[2].newPos - objecetList[i]->triangleList[j].vertex[1].newPos;
				//法线
				Vector3D normal;
				VectorCross(normal, v1, v2);

				Vector3D direction = objecetList[i]->triangleList[j].vertex[0].newPos - camera->GetPosition();

				float dot = VectorDot(direction, normal);

				if (dot <= 0.0f)
				{
					objecetList[i]->triangleList[j].state = TRIANGLE_BACKFACE;
				}
				else
				{
					objecetList[i]->triangleList[j].state = TRIANGLE_FOREFACE;
				}
			}
		}
	}
	//世界变换
	void LocalToWorld()
	{
		for (int i = 0; i < objectListCount; i++)
		{
			Matrix4X4 translation = { 1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				objecetList[i]->position.x, objecetList[i]->position.y, objecetList[i]->position.z, 1 };

			for (int j = 0; j < objecetList[i]->triangleCount; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					MatrixApply(objecetList[i]->triangleList[j].vertex[k].newPos,
						objecetList[i]->triangleList[j].vertex[k].oldPos, translation);
				}
			}
		}
	}

	//投影变换
	void Projection()
	{
		for (int i = 0; i < objectListCount; i++)
		{
			for (int j = 0; j < objecetList[i]->triangleCount; j++)
			{
				if (objecetList[i]->triangleList[j].state == TRIANGLE_BACKFACE)
					continue;

				for (int k = 0; k < 3; k++)
				{
					objecetList[i]->triangleList[j].vertex[k].newPos.x = objecetList[i]->triangleList[j].vertex[k].newPos.x / objecetList[i]->triangleList[j].vertex[k].newPos.z * camera->GetViewDistance();
					objecetList[i]->triangleList[j].vertex[k].newPos.y = objecetList[i]->triangleList[j].vertex[k].newPos.y / objecetList[i]->triangleList[j].vertex[k].newPos.z * camera->GetViewDistance();
				}
			}
		}
	}

	//视图变换
	void ViewTransform()
	{
		for (int i = 0; i < objectListCount; i++)
		{
			for (int j = 0; j < objecetList[i]->triangleCount; j++)
			{
				if (objecetList[i]->triangleList[j].state == TRIANGLE_BACKFACE)
					continue;

				for (int k = 0; k < 3; k++)
				{
 					float t = (objecetList[i]->triangleList[j].vertex[k].newPos.x + camera->GetViewWidth() / 2) / camera->GetViewWidth();
					objecetList[i]->triangleList[j].vertex[k].newPos.x = Interp(0.0f, (float)width, t);
					t = (objecetList[i]->triangleList[j].vertex[k].newPos.y + camera->GetViewHeight() / 2) / camera->GetViewHeight();
					//windows编程中Y轴要反转
					objecetList[i]->triangleList[j].vertex[k].newPos.y = (float)height - Interp(0.0f, (float)height, t);
				}
			}
		}
	}

	void RenderTexture(UINT *texTureBuffer, int width, int height)
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				UINT color = texTureBuffer[j + width * i];
				DrawPoint(j, i, color);
			}
		}
	}

private:
	void Init(UVNCamera *camera, int width, int height, void *frameBuffer, int renderState, UINT foreground, UINT background)
	{
		this->camera = camera;
		this->width = width;
		this->height = height;
		this->frameBuffer = (UINT*)frameBuffer;
		this->renderState = renderState;
		this->foreground = foreground;
		this->background = background;
		this->clipMinX = 0;
		this->clipMinY = 0;
		this->clipMaxX = width;
		this->clipMaxY = height;
		objectListCount = 0;
	}
	void Destroy()
	{
		if (frameBuffer != nullptr)
		{
			delete frameBuffer;
			frameBuffer = nullptr;
		}
	}

	
private:
	int width;
	int height;
	UINT *frameBuffer;		//图像缓存
	int renderState;
	UINT background;		//背景颜色
	UINT foreground;		//线框颜色
	int clipMaxX;
	int clipMaxY;
	int clipMinX;
	int clipMinY;

	UVNCamera *camera;		//相机

	int objectListCount;
	Objecet *objecetList[64];			//渲染物体列表
};


#endif
