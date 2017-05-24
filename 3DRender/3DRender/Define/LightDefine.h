#ifndef __LIGHTDEFINE_H
#define __LIGHTDEFINE_H
#include "../Define/GeometryDefine.h"

class Light
{
public:
	int id;
	int state;
	int attr;
	Point3D position;
	Color color;

	//计算光强
	virtual Color CalculateColor(Vertex3D &vertex, Material &material) = 0;
};

//点光源
class PointLight :public Light
{
	//衰减系数
	double kc;
	double kl;
	double kq;

	Color CalculateColor(Vertex3D &vertex, Material &material)
	{

	}
};

//方向光源
class DirectionLight :public Light
{
	Vector3D direction;

	Color CalculateColor(Vertex3D &vertex, Material &material)
	{

	}
};

//聚光灯
class SpotLight :public Light
{
	//衰减系数
	double kc;
	double kl;
	double kq;

	Vector3D direction;
	double inAngle;		//内锥角度
	double outAngle;	//外锥角度

	Color CalculateColor(Vertex3D &vertex, Material &material)
	{

	}
};
#endif