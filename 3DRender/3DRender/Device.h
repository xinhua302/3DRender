/*
*渲染装置
*/
#ifndef DEVICE_H
#define DEVICE_H
#include "3DMath.h"
#include "Camera.h"
#include "Define/GeometryDefine.h"
#include "Define/LightDefine.h"

const int RENDER_STATE_WIREFRAME = 1;	// 渲染线框
const int RENDER_STATE_COLOR = 2;		// 渲染颜色
const int RENDER_STATE_TEXTURE = 4;		// 渲染纹理
class Device
{
public:
	void Init()
	{
		lightList[lightCount++] = CreateLight(LIGHT_TYPE_POINT);
	}
	//渲染主函数
	void Render()
	{
		Clear();
		LocalToWorld();
		RemoveBackfaceTriangle();
		CalculateLight();
		WorldToCamera();
		Projection();
		ViewTransform();
		RenderObject();
	}
	void Clear()
	{
		finalTriangleCount = 0;
		memset(frameBuffer, 99, sizeof(UINT)* width * height);
		memset(depthBuffer, 0, sizeof(double)* width * height);
	}
public:
	Device(UVNCamera *camera, int width, int height, void *frameBuffer, int renderState, UINT foreground, UINT background)
	{
		Init(camera, width, height, frameBuffer, renderState, foreground, background);
	}
	~Device()
	{
		//Destroy();
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
		double k = (double)dy / (double)dx;
		double b = y1 - k*x1;
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
	void DrawTriangle(const Point2D &p1, const Point2D &p2, const Point2D &p3, UINT color, int mode)
	{
		//if (RENDER_STATE_WIREFRAME == mode)
		//{
			DrawLine(p1, p2, color);
			DrawLine(p2, p3, color);
			DrawLine(p3, p1, color);
		//}
		//else if (RENDER_STATE_COLOR == mode)
		//{
		//	//是否是平底或平顶三角形
		//	if (p1.y == p2.y)
		//	{
		//		if (p3.y > p2.y)
		//		{
		//			FillBottomTriangle(p1, p2, p3, color);
		//		}
		//		else if (p3.y < p2.y)
		//		{
		//			FillTopTriangle(p1, p2, p3, color);
		//		}
		//	}
		//	else if (p1.y == p3.y)
		//	{
		//		if (p2.y > p1.y)
		//		{
		//			FillBottomTriangle(p1, p2, p3, color);
		//		}
		//		else if (p2.y < p1.y)
		//		{
		//			FillTopTriangle(p1, p2, p3, color);
		//		}
		//	}
		//	else if (p2.y == p3.y)
		//	{
		//		if (p3.y > p2.y)
		//		{
		//			FillBottomTriangle(p1, p2, p3, color);
		//		}
		//		else if (p3.y < p2.y)
		//		{
		//			FillTopTriangle(p1, p2, p3, color);
		//		}
		//	}
		//	//不是平底也不是平顶,需要拆分
		//	else
		//	{
		//		//从上到下排列顶点
		//		Point2D pList[] = { p1, p2, p3 };
		//		SelectionSort(pList, 3);
		//		//(y - b) / k
		//		double k = (pList[0].y - pList[2].y) * 1.0f / (pList[0].x - pList[2].x);
		//		double b = pList[2].y - k*pList[2].x;
		//		int CenterPointX = (int)((pList[1].y - b) / k);
		//		Point2D centerPoint = { CenterPointX, pList[1].y };
		//		FillTopTriangle(pList[0], centerPoint, pList[1], color);
		//		FillBottomTriangle(centerPoint, pList[1], pList[2], color);
		//	}
		//}
	}
	void DrawTriangle(const Triangle &triangle, UINT color, int mode)
	{
		Point2D p1 = { (int)triangle.GetVertex(0).vertex.x, (int)triangle.GetVertex(0).vertex.y };
		Point2D p2 = { (int)triangle.GetVertex(1).vertex.x, (int)triangle.GetVertex(1).vertex.y };
		Point2D p3 = { (int)triangle.GetVertex(2).vertex.x, (int)triangle.GetVertex(2).vertex.y };
		DrawTriangle(p1, p2, p3, color, mode);
	}

	//将物体加入渲染列表
	void AddObjectList(GameObject *objecet)
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
			for (int j = 0; j < objecetList[i]->triangleCount; j++)
			{
				if (objecetList[i]->triangleList[j].state == TRIANGLE_BACKFACE)
				{
					continue;
				}
				objecetList[i]->triangleList[j].vertexList = objecetList[i]->transVertexList;
				FinalTriange &t = finalTriangleList[finalTriangleCount++];
				t.material = objecetList[i]->triangleList[j].material;
				for (int k = 0; k < 3; k++)
				{
					t.vertex[k].x = objecetList[i]->triangleList[j].GetVertex(k).vertex.x;
					t.vertex[k].y = objecetList[i]->triangleList[j].GetVertex(k).vertex.y;
					t.vertex[k].z = objecetList[i]->triangleList[j].GetVertex(k).vertex.z;
					t.uv[k].x = objecetList[i]->triangleList[j].uv[k].x;
					t.uv[k].y = objecetList[i]->triangleList[j].uv[k].y;
					t.color[k] = objecetList[i]->triangleList[j].GetVertex(k).color;
				}
			}
		}
		//for (int i = 0; i < finalTriangleCount; i++)
		//{
		//	Point2D p1 = { finalTriangleList[i].vertex[0].x, finalTriangleList[i].vertex[0].y };
		//	Point2D p2 = { finalTriangleList[i].vertex[1].x, finalTriangleList[i].vertex[1].y };
		//	Point2D p3 = { finalTriangleList[i].vertex[2].x, finalTriangleList[i].vertex[2].y };
		//	DrawTriangle(p1, p2, p3, 0x0000FFFF, RENDER_STATE_WIREFRAME);
		//}


		for (int i = 0; i < finalTriangleCount; i++)
		{
			DrawTriangle(finalTriangleList[i]);
		}
	}


	void DrawTriangle(FinalTriange &t)
	{
		//是否是平底或平顶三角形
		if (abs(t.vertex[0].y - t.vertex[1].y) <0.00001)
		{
			if (t.vertex[2].y >t.vertex[1].y)
			{
				BottomTriangle(t);
			}
			else if (t.vertex[2].y < t.vertex[1].y)
			{
				TopTriangle(t);
			}
		}
		else if (abs(t.vertex[0].y - t.vertex[2].y) < 0.00001)
		{
			if (t.vertex[1].y > t.vertex[0].y )
			{
				BottomTriangle(t);
			}
			else if (t.vertex[1].y < t.vertex[0].y )
			{
				TopTriangle(t);
			}
		}
		else if (abs(t.vertex[1].y - t.vertex[2].y) < 0.00001)
		{
			if (t.vertex[0].y > t.vertex[1].y)
			{
				BottomTriangle(t);
			}
			else if (t.vertex[0].y < t.vertex[1].y)
			{
				TopTriangle(t);
			}
		}
		//不是平底也不是平顶,需要拆分
		else
		{
			//从上到下排列顶点
			t.TopToBottomSort();
			//(y - b) / k
			double k = (t.vertex[0].y - t.vertex[2].y) * 1.0f / (t.vertex[0].x - t.vertex[2].x);
			double m = t.vertex[2].y - k*t.vertex[2].x;
			double CenterPointX = (t.vertex[1].y - m) / k;

			if ((t.vertex[0].x - t.vertex[2].x) == 0)
			{
				CenterPointX = t.vertex[0].x;
			}

			double reciprocalZ = GetInterpValue(t.vertex[0].x, t.vertex[0].y, 1 / t.vertex[0].z,
				t.vertex[1].x, t.vertex[1].y, 1 / t.vertex[1].z,
				t.vertex[2].x, t.vertex[2].y, 1 / t.vertex[2].z,
				CenterPointX, t.vertex[1].y);

			double r = GetInterpValue(t.vertex[0].x, t.vertex[0].y, t.color[0].r,
				t.vertex[1].x, t.vertex[1].y, t.color[1].r,
				t.vertex[2].x, t.vertex[2].y, t.color[2].r,
				CenterPointX, t.vertex[1].y);

			double g = GetInterpValue(t.vertex[0].x, t.vertex[0].y, t.color[0].g,
				t.vertex[1].x, t.vertex[1].y, t.color[1].g,
				t.vertex[2].x, t.vertex[2].y, t.color[2].g,
				CenterPointX, t.vertex[1].y);

			double b = GetInterpValue(t.vertex[0].x, t.vertex[0].y, t.color[0].b,
				t.vertex[1].x, t.vertex[1].y, t.color[1].b,
				t.vertex[2].x, t.vertex[2].y, t.color[2].b,
				CenterPointX, t.vertex[1].y);

			double u = GetInterpValue(t.vertex[0].x, t.vertex[0].y, t.uv[0].x / t.vertex[0].z,
				t.vertex[1].x, t.vertex[1].y, t.uv[1].x / t.vertex[1].z,
				t.vertex[2].x, t.vertex[2].y, t.uv[2].x / t.vertex[2].z,
				CenterPointX, t.vertex[1].y) / reciprocalZ;

			double v = GetInterpValue(t.vertex[0].x, t.vertex[0].y, t.uv[0].y / t.vertex[0].z,
				t.vertex[1].x, t.vertex[1].y, t.uv[1].y / t.vertex[1].z,
				t.vertex[2].x, t.vertex[2].y, t.uv[2].y / t.vertex[2].z,
				CenterPointX, t.vertex[1].y) / reciprocalZ;
			

			Point3D centerPoint = { CenterPointX, t.vertex[1].y, 1 / reciprocalZ };

			FinalTriange bottom(t.vertex[0], centerPoint, t.vertex[1], t.uv[0], { u, v }, t.uv[1], t.color[0], Color(r, g, b), t.color[1], t.material);
			FinalTriange top(centerPoint, t.vertex[1], t.vertex[2], { u, v }, t.uv[1], t.uv[2], Color(r, g, b), t.color[1], t.color[2], t.material);

			TopTriangle(top);
			BottomTriangle(bottom);
		}
	}

	void TopTriangle(const FinalTriange &t)
	{
		Vertex3D top, left, right;
		Vector2D topUV, leftUV, rightUV;
		Color topColor, leftColor, rightColor;
		if ((int)t.vertex[0].y == (int)t.vertex[1].y)
		{
			top.vertex = t.vertex[2];
			topUV = t.uv[2];
			topColor = t.color[2];
			left.vertex = t.vertex[0];
			leftUV = t.uv[0];
			leftColor = t.color[0];
			right.vertex = t.vertex[1];
			rightUV = t.uv[1];
			rightColor = t.color[1];
		}
		else if ((int)t.vertex[1].y == (int)t.vertex[2].y)
		{
			top.vertex = t.vertex[0];
			left.vertex = t.vertex[1];
			right.vertex = t.vertex[2];
			topUV = t.uv[0];
			leftUV = t.uv[1];
			rightUV = t.uv[2];
			topColor = t.color[0];
			leftColor = t.color[1];
			rightColor = t.color[2];
		}
		else
		{
			top.vertex = t.vertex[1];
			left.vertex = t.vertex[0];
			right.vertex = t.vertex[2];
			topUV = t.uv[1];
			leftUV = t.uv[0];
			rightUV = t.uv[2];
			topColor = t.color[1];
			leftColor = t.color[0];
			rightColor = t.color[2];

		}
		if (left.vertex.x > right.vertex.x)
		{
			Swap(left, right);
			Swap(leftUV, rightUV);
			Swap(leftColor, rightColor);
		}
		double leftDxDivDy = (top.vertex.x - left.vertex.x) / (top.vertex.y - left.vertex.y);
		double rightDxDivDy = (right.vertex.x - top.vertex.x) / (right.vertex.y - top.vertex.y);
		double xleft = left.vertex.x;
		double xRight = right.vertex.x;
		int y = left.vertex.y;

		//1/Z
		double leftDxDivDyReciprocalZ = ((1 / top.vertex.z) - (1 / left.vertex.z)) / (top.vertex.y - left.vertex.y);
		double rightDxDivDyReciprocalZ = ((1 / right.vertex.z) - (1 / top.vertex.z)) / (right.vertex.y - top.vertex.y);
		double xLeftReciprocalZ = 1 / left.vertex.z;
		double xRightReciprocalZ = 1 / right.vertex.z;

		//U
		double leftDxDivDyColorU = (topUV.x / top.vertex.z - leftUV.x / left.vertex.z) / (top.vertex.y - left.vertex.y);
		double rightDxDivDyColorU = (rightUV.x / right.vertex.z - topUV.x / top.vertex.z) / (right.vertex.y - top.vertex.y);
		double xLeftColorU = leftUV.x / left.vertex.z;
		double xRightColorU = rightUV.x / right.vertex.z;
		//V
		double leftDxDivDyColorV = (topUV.y / top.vertex.z - leftUV.y / left.vertex.z) / (top.vertex.y - left.vertex.y);
		double rightDxDivDyColorV = (rightUV.y / right.vertex.z - topUV.y / top.vertex.z) / (right.vertex.y - top.vertex.y);
		double xLeftColorV = leftUV.y / left.vertex.z;
		double xRightColorV = rightUV.y / right.vertex.z;

		//R
		double leftDxDivDyColorR = (topColor.r - leftColor.r) / (top.vertex.y - left.vertex.y);
		double rightDxDivDyColorR = (rightColor.r - topColor.r) / (right.vertex.y - top.vertex.y);
		double xLeftColorR = leftColor.r ;
		double xRightColorR = rightColor.r;

		//G
		double leftDxDivDyColorG = (topColor.g - leftColor.g) / (top.vertex.y - left.vertex.y);
		double rightDxDivDyColorG = (rightColor.g - topColor.g) / (right.vertex.y - top.vertex.y);
		double xLeftColorG = leftColor.g;
		double xRightColorG = rightColor.g;

		//B
		double leftDxDivDyColorB = (topColor.b - leftColor.b) / (top.vertex.y - left.vertex.y);
		double rightDxDivDyColorB = (rightColor.b - topColor.b) / (right.vertex.y - top.vertex.y);
		double xLeftColorB = leftColor.b;
		double xRightColorB = rightColor.b;



		double texWidth = t.material->width;
		double texHeight = t.material->height;
		UINT *texBuffer = t.material->texBuffer;

		while (y >= top.vertex.y)
		{
			double dxReciprocalZ = (xRightReciprocalZ - xLeftReciprocalZ) / (xRight - xleft);
			double reciprocalZ = xLeftReciprocalZ;

			double dxColorU = (xRightColorU - xLeftColorU) / (xRight - xleft);
			double ColorStarU = xLeftColorU;

			double dxColorV = (xRightColorV - xLeftColorV) / (xRight - xleft);
			double ColorStarV = xLeftColorV;

			double dxColorR = (xRightColorR - xLeftColorR) / (xRight - xleft);
			double ColorStarR = xLeftColorR;

			double dxColorG = (xRightColorG - xLeftColorG) / (xRight - xleft);
			double ColorStarG = xLeftColorG;

			double dxColorB = (xRightColorB - xLeftColorB) / (xRight - xleft);
			double ColorStarB = xLeftColorB;

			for (int i = (int)(xleft); i <= (int)(xRight); i++)
			{

				double u = (ColorStarU / reciprocalZ) * (texWidth - 1);
				double v = (ColorStarV / reciprocalZ) * (texHeight - 1);

				double r = ColorStarR;
				double g = ColorStarG;
				double b = ColorStarB;

				if (TestZ(i, y, reciprocalZ))
				{
					Color color = Color(texBuffer[(UINT)u + (UINT)v * (UINT)texWidth]);
					color.r *= r;
					color.g *= g;
					color.b *= b;
					DrawPoint(i, y, color.Get32());
				}

				ColorStarU += dxColorU;
				ColorStarV += dxColorV;
				reciprocalZ += dxReciprocalZ;
				ColorStarR += dxColorR;
				ColorStarG += dxColorG;
				ColorStarB += dxColorB;
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

			xLeftColorR -= leftDxDivDyColorR;
			xLeftColorG -= leftDxDivDyColorG;
			xLeftColorB -= leftDxDivDyColorB;

			xRightColorR -= rightDxDivDyColorR;
			xRightColorG -= rightDxDivDyColorG;
			xRightColorB -= rightDxDivDyColorB;
		}
	}
		void BottomTriangle(const FinalTriange &t)
		{
			Vertex3D bottom, left, right;
			Vector2D bottomUV, leftUV, rightUV;
			Color bottomColor, leftColor, rightColor;
			if ((int)t.vertex[0].y == (int)t.vertex[1].y)
			{
				bottom.vertex = t.vertex[2];
				bottomUV = t.uv[2];
				left.vertex = t.vertex[0];
				leftUV = t.uv[0];
				right.vertex = t.vertex[1];
				rightUV = t.uv[1];
				bottomColor = t.color[2];
				leftColor = t.color[0];
				rightColor = t.color[1];
			}
			else if ((int)t.vertex[1].y == (int)t.vertex[2].y)
			{
				bottom.vertex = t.vertex[0];
				left.vertex = t.vertex[1];
				right.vertex = t.vertex[2];
				bottomUV = t.uv[0];
				leftUV = t.uv[1];
				rightUV = t.uv[2];
				bottomColor = t.color[0];
				leftColor = t.color[1];
				rightColor = t.color[2];
			}
			else
			{
				bottom.vertex = t.vertex[1];
				left.vertex = t.vertex[0];
				right.vertex = t.vertex[2];
				bottomUV = t.uv[1];
				leftUV = t.uv[0];
				rightUV = t.uv[2];
				bottomColor = t.color[1];
				leftColor = t.color[0];
				rightColor = t.color[2];
			}
			if (left.vertex.x > right.vertex.x)
			{
				Swap(left, right);
				Swap(leftUV, rightUV);
				Swap(leftColor, rightColor);
			}
			double leftDxDivDy = (bottom.vertex.x - left.vertex.x) / (bottom.vertex.y - left.vertex.y);
			double rightDxDivDy = (right.vertex.x - bottom.vertex.x) / (right.vertex.y - bottom.vertex.y);
			double xleft = left.vertex.x;
			double xRight = right.vertex.x;
			int y = left.vertex.y;

			//1/Z
			double leftDxDivDyReciprocalZ = ((1 / bottom.vertex.z) - (1 / left.vertex.z)) / (bottom.vertex.y - left.vertex.y);
			double rightDxDivDyReciprocalZ = ((1 / right.vertex.z) - (1 / bottom.vertex.z)) / (right.vertex.y - bottom.vertex.y);
			double xLeftReciprocalZ = 1 / left.vertex.z;
			double xRightReciprocalZ = 1 / right.vertex.z;

			//U
			double leftDxDivDyColorU = (bottomUV.x / bottom.vertex.z - leftUV.x / left.vertex.z) / (bottom.vertex.y - left.vertex.y);
			double rightDxDivDyColorU = (rightUV.x / right.vertex.z - bottomUV.x / bottom.vertex.z) / (right.vertex.y - bottom.vertex.y);
			double xLeftColorU = leftUV.x / left.vertex.z;
			double xRightColorU = rightUV.x / right.vertex.z;
			//V
			double leftDxDivDyColorV = (bottomUV.y / bottom.vertex.z - leftUV.y / left.vertex.z) / (bottom.vertex.y - left.vertex.y);
			double rightDxDivDyColorV = (rightUV.y / right.vertex.z - bottomUV.y / bottom.vertex.z) / (right.vertex.y - bottom.vertex.y);
			double xLeftColorV = leftUV.y / left.vertex.z;
			double xRightColorV = rightUV.y / right.vertex.z;

			//R
			double leftDxDivDyColorR = (bottomColor.r - leftColor.r) / (bottom.vertex.y - left.vertex.y);
			double rightDxDivDyColorR = (rightColor.r - bottomColor.r) / (right.vertex.y - bottom.vertex.y);
			double xLeftColorR = leftColor.r;
			double xRightColorR = rightColor.r;

			//G
			double leftDxDivDyColorG = (bottomColor.g - leftColor.g) / (bottom.vertex.y - left.vertex.y);
			double rightDxDivDyColorG = (rightColor.g - bottomColor.g) / (right.vertex.y - bottom.vertex.y);
			double xLeftColorG = leftColor.g;
			double xRightColorG = rightColor.g;

			//B
			double leftDxDivDyColorB = (bottomColor.b - leftColor.b) / (bottom.vertex.y - left.vertex.y);
			double rightDxDivDyColorB = (rightColor.b - bottomColor.b) / (right.vertex.y - bottom.vertex.y);
			double xLeftColorB = leftColor.b;
			double xRightColorB = rightColor.b;

			double texWidth = t.material->width;
			double texHeight = t.material->height;
			UINT *texBuffer = t.material->texBuffer;

			while (y <= bottom.vertex.y)
			{
				double dxReciprocalZ = (xRightReciprocalZ - xLeftReciprocalZ) / (xRight - xleft);
				double reciprocalZ = xLeftReciprocalZ;

				double dxColorU = (xRightColorU - xLeftColorU) / (xRight - xleft);
				double ColorStarU = xLeftColorU;

				double dxColorV = (xRightColorV - xLeftColorV) / (xRight - xleft);
				double ColorStarV = xLeftColorV;

				double dxColorR = (xRightColorR - xLeftColorR) / (xRight - xleft);
				double ColorStarR = xLeftColorR;

				double dxColorG = (xRightColorG - xLeftColorG) / (xRight - xleft);
				double ColorStarG = xLeftColorG;

				double dxColorB = (xRightColorB - xLeftColorB) / (xRight - xleft);
				double ColorStarB = xLeftColorB;
				for (int i = (int)(xleft); i <= (int)(xRight); i++)
				{

					double u = (ColorStarU / reciprocalZ) * (texWidth - 1);
					double v = (ColorStarV / reciprocalZ) * (texHeight - 1);
					double r = ColorStarR;
					double g = ColorStarG;
					double b = ColorStarB;
					if (TestZ(i, y, reciprocalZ))
					{
						Color color = Color(texBuffer[(UINT)u + (UINT)v * (UINT)texWidth]);
						color.r *= r;
						color.g *= g;
						color.b *= b;
						DrawPoint(i, y, color.Get32());
					}

					ColorStarU += dxColorU;
					ColorStarV += dxColorV;
					reciprocalZ += dxReciprocalZ;
					ColorStarR += dxColorR;
					ColorStarG += dxColorG;
					ColorStarB += dxColorB;
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

				xLeftColorR += leftDxDivDyColorR;
				xLeftColorG += leftDxDivDyColorG;
				xLeftColorB += leftDxDivDyColorB;

				xRightColorR += rightDxDivDyColorR;
				xRightColorG += rightDxDivDyColorG;
				xRightColorB += rightDxDivDyColorB;
			}
		}

	//清空渲染列表
	void ClearObjectList()
	{
		objectListCount = 0;
		finalTriangleCount = 0;
	}

	//相机变换ss
	void WorldToCamera()
	{
		for (int i = 0; i < objectListCount; i++)
		{
			for (int j = 0; j < objecetList[i]->vertexCount; j++)
			{
				objecetList[i]->transVertexList[j].vertex = camera->WolrdToCamera(objecetList[i]->transVertexList[j].vertex);
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
				if (true)
				{
				
					Vector3D temp1 = objecetList[i]->triangleList[j].GetVertex(1).vertex;
					Vector3D temp2 = objecetList[i]->triangleList[j].GetVertex(0).vertex;
					Vector3D v1 = temp1 - temp2;
					Vector3D temp3 = objecetList[i]->triangleList[j].GetVertex(0).vertex;
					Vector3D temp4 = objecetList[i]->triangleList[j].GetVertex(2).vertex;
					Vector3D v2 = temp3 - temp4;

				//法线
				Vector3D normal;
				VectorCross(normal, v1, v2);

				for (int k = 0; k < 3; k ++)
				{
					Vector3D theNormal = objecetList[i]->triangleList[j].GetVertex(k).normal;
					Vector3D n = theNormal + normal;
					int index = objecetList[i]->triangleList[j].index[k];
					objecetList[i]->triangleList[j].vertexList[index].normal.x = n.x;
					objecetList[i]->triangleList[j].vertexList[index].normal.y = n.y;
					objecetList[i]->triangleList[j].vertexList[index].normal.z = n.z;
				}

				Vector3D direction = camera->GetPosition() - objecetList[i]->triangleList[j].GetVertex(0).vertex;

				double dot = VectorDot(direction, normal);

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
	}
	//世界变换
	void LocalToWorld()
	{
		for (int i = 0; i < objectListCount; i++)
		{
			if (objecetList[i]->transVertexList != nullptr)
			{
				free(objecetList[i]->transVertexList);
				objecetList[i]->transVertexList = nullptr;
			}
			objecetList[i]->transVertexList = (Vertex3D*)malloc(sizeof(Vertex3D)* objecetList[i]->vertexCount);
			memcpy(objecetList[i]->transVertexList, objecetList[i]->localVertexList, sizeof(Vertex3D)* objecetList[i]->vertexCount);

			Matrix4X4 translation = { 1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				objecetList[i]->position.x, objecetList[i]->position.y, objecetList[i]->position.z, 1 };

			for (int j = 0; j < objecetList[i]->vertexCount; j++)
			{
				MatrixApply(objecetList[i]->transVertexList[j].vertex,
					objecetList[i]->localVertexList[j].vertex, translation);
			}
			for (int j = 0; j < objecetList[i]->triangleCount; j++)
			{
				objecetList[i]->triangleList[j].vertexList = objecetList[i]->transVertexList;
			}
		}
	}

	//投影变换
	void Projection()
	{
		for (int i = 0; i < objectListCount; i++)
		{
			for (int j = 0; j < objecetList[i]->vertexCount; j++)
			{
				objecetList[i]->transVertexList[j].vertex.x = objecetList[i]->transVertexList[j].vertex.x / objecetList[i]->transVertexList[j].vertex.z * camera->GetViewDistance();
				objecetList[i]->transVertexList[j].vertex.y = objecetList[i]->transVertexList[j].vertex.y / objecetList[i]->transVertexList[j].vertex.z * camera->GetViewDistance();
			}
		}
	}

	//视图变换
	void ViewTransform()
	{
		for (int i = 0; i < objectListCount; i++)
		{
			for (int j = 0; j < objecetList[i]->vertexCount; j++)
			{

				double t = (objecetList[i]->transVertexList[j].vertex.x + camera->GetViewWidth() / 2) / camera->GetViewWidth();
				objecetList[i]->transVertexList[j].vertex.x = Interp(0.0f, (double)width, t);
				t = (objecetList[i]->transVertexList[j].vertex.y + camera->GetViewHeight() / 2) / camera->GetViewHeight();
					//windows编程中Y轴要反转
				objecetList[i]->transVertexList[j].vertex.y = (double)height - Interp(0.0f, (double)height, t);
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
		depthBuffer = (double*)malloc(sizeof(double)*width * height);
		this->renderState = renderState;
		this->foreground = foreground;
		this->background = background;
		this->clipMinX = 0;
		this->clipMinY = 0;
		this->clipMaxX = width;	
		this->clipMaxY = height;
		objectListCount = 0;
		finalTriangleCount = 0;
		lightCount = 0;
	}
	void Destroy()
	{
		if (frameBuffer != nullptr)
		{
			delete frameBuffer;
			frameBuffer = nullptr;
		}

		if (depthBuffer != nullptr)
		{
			free(depthBuffer);
		}

		for (int i = 0; i < lightCount; i++)
		{
			delete lightList[i];
		}
		lightCount = 0;
	}

	bool TestZ(double x, double y, double z)
	{
		if (x < clipMinX || x >= clipMaxX || y < clipMinY || y >= clipMaxY)
		{
			return false;
		}
		double curPixelDepth = depthBuffer[(UINT)x + width * (UINT)y];
		if (curPixelDepth < z)
		{
			depthBuffer[(UINT)x + width * (UINT)y] = z;
			return true;
		}
		else
		{
			return false;
		}
	}
	
	//创建光源
	Light *CreateLight(int type)
	{
		if (type == LIGHT_TYPE_POINT)
		{
			PointLight *pointLight = new PointLight();
			pointLight->color = Color(0xFFFFFFFF);
			pointLight->position = Vector3D{ 5.0, 0.0, 0.0 };
			pointLight->kc = 0.1;
			pointLight->kl = 0.1;
			pointLight->kq = 0.1;
			return pointLight;
		}
		else if (type == LIGHT_TYPE_DIRECTION)
		{
			return new DirectionLight();
		}
		else
		{
			return new SpotLight();
		}
	}

	void CalculateLight()
	{
		for (int i = 0; i < objectListCount; i++)
		{
			for (int j = 0; j < objecetList[i]->vertexCount; j++)
			{
				VectorNormalize(objecetList[i]->transVertexList[j].normal);
				Color color = Color(0);
				for (int k = 0; k < lightCount; k++)
				{
					Color theColor = lightList[k]->CalculateColor(objecetList[i]->transVertexList[j],
						*objecetList[i]->triangleList[0].material,
						camera->GetPosition());
					color = color + theColor;
				}
				objecetList[i]->transVertexList[j].color.r *= color.r;
				objecetList[i]->transVertexList[j].color.g *= color.g;
				objecetList[i]->transVertexList[j].color.b *= color.b;
			}

		}
	}

private:
	int width;
	int height;
	UINT *frameBuffer;		//图像缓存
	double *depthBuffer;		//深度缓存
	int renderState;
	UINT background;		//背景颜色
	UINT foreground;		//线框颜色
	double clipMaxX;
	double clipMaxY;
	double clipMinX;
	double clipMinY;

	UVNCamera *camera;		//相机

	int objectListCount;
	GameObject *objecetList[64];			//渲染物体列表

	int finalTriangleCount;
	FinalTriange finalTriangleList[64];

	int lightCount;
	Light *lightList[64];
};


#endif
