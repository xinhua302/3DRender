#ifndef __GEOMETRYDEFINE_H
#define __GEOMETRYDEFINE_H
#include "3DMath.h"

//三角形
struct Triangle
{
	Point3D oldPos[3];
	Point3D newPos[3];

	Triangle(const Point3D & p0, const Point3D & p1, const Point3D & p2)
	{
		oldPos[0] = p0;
		oldPos[1] = p1;
		oldPos[2] = p2;
	}
};

//物体
struct Objecet
{
	Point3D position;	//坐标
	int triangleCount;	//三角形数量
	Triangle *triangleList;

	Objecet(Point3D pos, int triangleCount, Triangle *list) : position(pos), triangleCount(triangleCount)
	{
		this->triangleList = list;
	}
};
#endif 

