#ifndef __GEOMETRYDEFINE_H
#define __GEOMETRYDEFINE_H
#include "../3DMath.h"
#include "../Define/MaterialDefine.h"
#include "../Define/ColorDefine.h"

struct Vertex3D
{
	Vector3D vertex;	//顶点
	Vector3D normal;	//法线
	Color color;		//顶点颜色
	Vector2D uv;		//纹理坐标
};

struct Triangle
{
	int id;
	int state;
	int attr;
	Vertex3D* vertexList;	//顶点列表
	int index[3];			//索引
	Material *material;		//材质
};

struct GameObject
{
	int id;
	int state;
	int attr;
	char *name;		
	int state;		//状态

	int vertexCount;
	Vertex3D *localVertexList;
	Vertex3D *transVertexList;
	Triangle *triangleList;
};
#endif