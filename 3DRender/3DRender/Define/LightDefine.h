#ifndef __LIGHTDEFINE_H
#define __LIGHTDEFINE_H
#include "../Define/GeometryDefine.h"

const int LIGHT_STATE_ON = 0;
const int LIGHT_STATE_OFF = 1;

//点光源
const int LIGHT_TYPE_POINT = 0;
const int LIGHT_TYPE_DIRECTION = 1;
const int LIGHT_TYPE_SPOT = 2;
const int LIGHT_TYPE_AMBIENT = 3;

//环境光
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
		double r = material.ka;
		double g = material.ka;
		double b = material.ka;
		//判断是否溢出
		r = r > 1.0 ? 1.0 : r;
		g = g > 1.0 ? 1.0 : g;
		b = b > 1.0 ? 1.0 : b;
		return Color(color.r *r, color.g *g, color.b *b);
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
		double r = 0;
		double g = 0;
		double b = 0;

		//计算散射光
		Vector3D l = position- vertex.vertex;		
		double d = CalculateVector3DLength(l);
		VectorNormalize(l);
		double dp = VectorDot(vertex.normal, l);
		//光衰减系数
		double atten = kc + kl * d + kq *d * d;

		if (dp > 0)
		{
			r += material.kd * dp / atten;
			g += material.kd * dp / atten;
			b += material.kd * dp / atten;
		}

		//计算镜面高光
		l = vertex.vertex - position;
		double x = l.x - 2 * VectorDot(l, vertex.normal) * vertex.normal.x;
		double y = l.y - 2 * VectorDot(l, vertex.normal) * vertex.normal.y;
		double z = l.z - 2 * VectorDot(l, vertex.normal) * vertex.normal.z;
		Vector3D rVector = { x,y,z,1 };
		Vector3D v = objectPos - vertex.vertex;
		VectorNormalize(rVector);
		VectorNormalize(v);
		double is =  pow(VectorDot(rVector, v), material.shininess);
		if (is > 0)
		{
			r += material.ks * is / atten;
			g += material.ks * is / atten;
			b += material.ks * is / atten;
		}

		//判断是否溢出
		r = r > 1.0 ? 1.0 : r;
		g = g > 1.0 ? 1.0 : g;
		b = b > 1.0 ? 1.0 : b;
		return Color(color.r *r, color.g *g, color.b *b);
	}
};

//方向光源
class DirectionLight :public Light
{
public:
	Vector3D direction;

	Color CalculateColor(const Vertex3D &vertex, const Material &material, const Point3D &objectPos)
	{
		double r = 0;
		double g = 0;
		double b = 0;

		//计算散射光
		Vector3D l;
		l.x = -direction.x;
		l.y = -direction.y;
		l.z = -direction.z;
		VectorNormalize(l);
		double dp = VectorDot(vertex.normal, direction);
		//光衰减系数
		double atten = 1;

		if (dp > 0)
		{
			r += material.kd * dp / atten;
			g += material.kd * dp / atten;
			b += material.kd * dp / atten;
		}

		//计算镜面高光
		l = direction;
		double x = l.x - 2 * VectorDot(l, vertex.normal) * vertex.normal.x;
		double y = l.y - 2 * VectorDot(l, vertex.normal) * vertex.normal.y;
		double z = l.z - 2 * VectorDot(l, vertex.normal) * vertex.normal.z;
		Vector3D rVector = { x, y, z, 1 };
		Vector3D v = objectPos - vertex.vertex;
		VectorNormalize(rVector);
		VectorNormalize(v);
		double is = pow(VectorDot(rVector, v), material.shininess);
		if (is > 0)
		{
			r += material.ks * is / atten;
			g += material.ks * is / atten;
			b += material.ks * is / atten;
		}

		//判断是否溢出
		r = r > 1.0 ? 1.0 : r;
		g = g > 1.0 ? 1.0 : g;
		b = b > 1.0 ? 1.0 : b;
		return Color(color.r *r, color.g *g, color.b *b);
	}
};

//聚光灯
class SpotLight :public Light
{
public:

	//衰减系数
	double kc;
	double kl;
	double kq;

	Vector3D direction;
	double inAngle;		//内锥角度
	double outAngle;	//外锥角度
	double pf;			//指数因子

	void SetInAngle(double inAngle)
	{
		this->inAngle = inAngle / 180.0 * PI;
	}

	void SetOutAngle(double outAngle)
	{
		this->outAngle = outAngle / 180.0 * PI;
	}

	Color CalculateColor(const Vertex3D &vertex, const Material &material, const Point3D &objectPos)
	{
		Vector3D l = vertex.vertex - position;
		VectorNormalize(l);
		VectorNormalize(direction);
		double dp = VectorDot(l, direction);

		double r = 0.0;
		double g = 0.0;
		double b = 0.0;
		if (dp <= 0.0)
		{
			return Color(r, g, b);
		}
		double angle = acos(dp);



		//本影
		if (angle < inAngle)
		{
			//计算散射光
			Vector3D l = position - vertex.vertex;
			double d = CalculateVector3DLength(l);
			VectorNormalize(l);
			double dp = VectorDot(vertex.normal, l);
			//光衰减系数
			double atten = kc + kl * d + kq *d * d;

			if (dp > 0)
			{
				r += material.kd * dp / atten;
				g += material.kd * dp / atten;
				b += material.kd * dp / atten;
			}

			//计算镜面高光
			l = vertex.vertex - position;
			double x = l.x - 2 * VectorDot(l, vertex.normal) * vertex.normal.x;
			double y = l.y - 2 * VectorDot(l, vertex.normal) * vertex.normal.y;
			double z = l.z - 2 * VectorDot(l, vertex.normal) * vertex.normal.z;
			Vector3D rVector = { x, y, z, 1 };
			Vector3D v = objectPos - vertex.vertex;
			VectorNormalize(rVector);
			VectorNormalize(v);
			double is = pow(VectorDot(rVector, v), material.shininess);
			if (is > 0)
			{
				r += material.ks * is / atten;
				g += material.ks * is / atten;
				b += material.ks * is / atten;
			}
		}
		//半影
		else if (angle >= inAngle && angle < outAngle)
		{
			//计算散射光
			Vector3D l = position - vertex.vertex;
			double d = CalculateVector3DLength(l);
			VectorNormalize(l);
			double dp = VectorDot(vertex.normal, l);
			//光衰减系数
			double atten = (kc + kl * d + kq *d * d);
			double pfValue = pow((cos(angle) - cos(2 * outAngle)) / (cos(2 * inAngle) - cos(2 * outAngle)), pf);
			atten = pfValue * atten;
			if (dp > 0)
			{
				r += material.kd * dp / atten;
				g += material.kd * dp / atten;
				b += material.kd * dp / atten;
			}

			//计算镜面高光
			l = vertex.vertex - position;
			double x = l.x - 2 * VectorDot(l, vertex.normal) * vertex.normal.x;
			double y = l.y - 2 * VectorDot(l, vertex.normal) * vertex.normal.y;
			double z = l.z - 2 * VectorDot(l, vertex.normal) * vertex.normal.z;
			Vector3D rVector = { x, y, z, 1 };
			Vector3D v = objectPos - vertex.vertex;
			VectorNormalize(rVector);
			VectorNormalize(v);
			double is = pow(VectorDot(rVector, v), material.shininess);
			if (is > 0)
			{
				r += material.ks * is / atten;
				g += material.ks * is / atten;
				b += material.ks * is / atten;
			}
		}

		//判断是否溢出
		r = r > 1.0 ? 1.0 : r;
		g = g > 1.0 ? 1.0 : g;
		b = b > 1.0 ? 1.0 : b;
		return Color(r, g, b);
	}
};
#endif