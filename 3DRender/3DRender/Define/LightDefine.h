#ifndef __LIGHTDEFINE_H
#define __LIGHTDEFINE_H
#include "../Define/GeometryDefine.h"

const int LIGHT_STATE_ON = 0;
const int LIGHT_STATE_OFF = 1;

//点光源
const int LIGHT_TYPE_POINT = 0;
const int LIGHT_TYPE_DIRECTION = 1;
const int LIGHT_TYPE_SPOT = 2;

class Light
{
public:
	int id;
	int state;
	int attr;
	Point3D position;
	Color color;

	//计算光强
	virtual Color CalculateColor(const Vertex3D &vertex, const Material &material, const Point3D &objectPos)
	{
		return Color(0);
	}
};

//点光源
class PointLight :public Light
{
public:
	//衰减系数
	double kc;
	double kl;
	double kq;

	Color CalculateColor(const Vertex3D &vertex, const Material &material, const Point3D &objectPos)
	{
		Vector3D l = position- vertex.vertex;
		
		double d = CalculateVector3DLength(l);

		double dp = VectorDot(vertex.normal, l);
		double r = material.ra.r;
		double g = material.ra.g;
		double b = material.ra.b;

		if (dp > 0)
		{
			double atten = kc + kl * d + kq *d * d;
			r += material.rd.r * dp / atten;
			g += material.rd.g * dp / atten;
			b += material.rd.b * dp / atten;
		}
		VectorNormalize(l);

		double x = l.x - 2 * VectorDot(l, vertex.normal) * vertex.normal.x;
		double y = l.y - 2 * VectorDot(l, vertex.normal) * vertex.normal.y;
		double z = l.z - 2 * VectorDot(l, vertex.normal) * vertex.normal.z;
		Vector3D rVector = { x,y,z,1 };
		Vector3D v = objectPos - vertex.vertex;
		double is =  pow(VectorDot(rVector, v), material.shininess);
		if (is > 0)
		{
			r += material.rs.r * is;
			g += material.rs.g * is;
			b += material.rs.b * is;
		}

		r = r > 1.0 ? 1.0 : r;
		g = g > 1.0 ? 1.0 : g;
		b = b > 1.0 ? 1.0 : b;
		return Color(color.r *r, color.g *g, color.b *b);
	}
};

//方向光源
class DirectionLight :public Light
{
	Vector3D direction;

	Color CalculateColor(const Vertex3D &vertex, const Material &material, const Point3D &objectPos)
	{
		return Color(0);
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

	Color CalculateColor(const Vertex3D &vertex, const Material &material, const Point3D &objectPos)
	{
		return Color(0);
	}
};
#endif