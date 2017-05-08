/*
*几何体定义
*/
#ifndef __GEOMETRYDEFINE_H
#define __GEOMETRYDEFINE_H
#include "3DMath.h"

const int TRIANGLE_BACKFACE = 1;
const int TRIANGLE_CLIPPED = 2;

//顶点
struct Vertex3D
{
	Point3D oldPos;
	Point3D newPos;
	Point2D uv;
	UINT color;

	UINT GetRGB()
	{
		UINT rgb =  color & ((1 << 24) - 1);
		return rgb;
	}

	float GetR()
	{
		UINT c = color & 0x00FF0000;
		UINT value = c >> 16;
		return (float)value;
	}

	float GetG()
	{
		return (color & 0x0000FF00) >> 8;
	}

	float GetB()
	{
		return color & 0x000000FF;
	}
};

//三角形
struct Triangle
{
	Vertex3D vertex[3];
	UINT *texBuffer;
	int texWidth;
	int texHeight;
	int state;

	Triangle(Point3D p0, Point3D p1, Point3D p2, UINT color0, UINT color1, UINT color2, bool isNew = false)
	{
		if (isNew)
		{
			vertex[0].newPos = p0;
			vertex[1].newPos = p1;
			vertex[2].newPos = p2;
		}
		else
		{
			vertex[0].oldPos = p0;
			vertex[1].oldPos = p1;
			vertex[2].oldPos = p2;
		}
		
		vertex[0].color = color0;
		vertex[1].color = color1;
		vertex[2].color = color2;
	}

	Triangle(Point3D p0, Point3D p1, Point3D p2, Point2D uv0, Point2D uv1, Point2D uv2, UINT *texBuffer, int texWidth, int texHeight, bool isNew = false)
	{
		if (isNew)
		{
			vertex[0].newPos = p0;
			vertex[1].newPos = p1;
			vertex[2].newPos = p2;
		}
		else
		{
			vertex[0].oldPos = p0;
			vertex[1].oldPos = p1;
			vertex[2].oldPos = p2;
		}

		vertex[0].uv = uv0;
		vertex[1].uv = uv1;
		vertex[2].uv = uv2;

		this->texBuffer = texBuffer;
		this->texWidth = texWidth;
		this->texHeight = texHeight;
	}

	void SortTopToBottom()
	{
		const int count = 3;
		for (int i = 0; i < count - 1; i++)
		{
			int index = i;
			for (int j = index; j < count; j++)
			{
				if (vertex[j].newPos.y > vertex[index].newPos.y)
				{
					continue;
				}
				index = j;
			}

			if (index != i)
			{
				Swap(vertex[i], vertex[index]);
			}
		}
	}
};

//物体
struct Objecet
{
	Point3D position;	//坐标
	int triangleCount;	//三角形数量
	Triangle *triangleList;
	int state;

	Objecet(Point3D pos, int triangleCount, Triangle *list, int state) : position(pos), triangleCount(triangleCount)
	{
		this->triangleList = list;
		this->state = state;
	}
};
#endif 

