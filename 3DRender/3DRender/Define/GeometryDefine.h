#ifndef __GEOMETRYDEFINE_H
#define __GEOMETRYDEFINE_H
#include "../3DMath.h"
#include "../Define/MaterialDefine.h"
#include "../Define/ColorDefine.h"
#include <string>

const int TRIANGLE_FOREFACE = 0;
const int TRIANGLE_BACKFACE = 1;
const int TRIANGLE_CLIPPED = 2;

struct FinalTriange
{
	Point3D vertex[3];
	Point2D uv[3];
	Color color[3];
	Material *material;		//材质

	FinalTriange()
	{

	}

	FinalTriange(Point3D p0, Point3D p1, Point3D p2, Point2D uv0, Point2D uv1, Point2D uv2, Color c0, Color c1, Color c2, Material *m)
	{
		vertex[0] = p0;
		vertex[1] = p1;
		vertex[2] = p2;

		uv[0] = uv0;
		uv[1] = uv1;
		uv[2] = uv2;

		color[0] = c0;
		color[1] = c1;
		color[2] = c2;

		material = m;
	}

	void TopToBottomSort()
	{
		int count = 3;
		for (int i = 0; i < count - 1; i++)
		{
			int index = i;
			for (int j = index; j < count; j++)
			{
				if (vertex[j].y < vertex[index].y)
				{
					continue;
				}
				index = j;
			}

			if (index != i)
			{
				Swap(vertex[i], vertex[index]);
				Swap(uv[i], uv[index]);
				Swap(color[i], color [index]);
			}
		}
	}
};

struct Vertex3D
{
	Vector3D vertex;	//顶点
	Vector3D normal;	//法线
	Color color;		//顶点颜色
};

struct Triangle
{
	int id;
	int state;
	int attr;
	Vertex3D* vertexList;	//顶点列表
	int index[3];			//索引
	Vector2D uv[3];		//纹理坐标
	Material *material;		//材质

	Vertex3D GetVertex(int i) const
	{
		return vertexList[index[i]];
	}

};

struct GameObject
{
	int id;
	int state;
	int attr;
	char *name;		
	
	Point3D position;

	int vertexCount;
	Vertex3D *localVertexList;
	Vertex3D *transVertexList;

	int triangleCount;
	Triangle *triangleList;

	GameObject(Point3D pos ,int id, int attr, char *name, int vertexCount, Vertex3D *localVertexList, int triangleCount, Triangle* triangleList)
	{
		this->position = pos;
		this->id = id;
		this->attr = attr;
		this->name = name;
		this->localVertexList = localVertexList;
		this->vertexCount = vertexCount;
		this->triangleList = triangleList;
		this->triangleCount = triangleCount;

		this->transVertexList = nullptr;
	}

	virtual ~GameObject()
	{
		if (name == nullptr)
		{
			delete name;
			name = nullptr;
		}

		if (localVertexList == nullptr)
		{
			delete localVertexList;
			localVertexList = nullptr;
		}

		if (transVertexList == nullptr)
		{
			delete transVertexList;
			transVertexList = nullptr;
		}

		if (triangleList == nullptr)
		{
			delete triangleList;
			triangleList = nullptr;
		}
		
	}
};

#endif