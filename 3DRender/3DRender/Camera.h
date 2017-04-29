#ifndef __CAMERA_H
#define __CAMERA_H
#include "3DMath.h"

class UVNCamera
{
public:
	//相机变换
	Point3D WolrdToCamera(const Point3D &worldPos)
	{
		Point3D tPos;
		MatrixApply(tPos, worldPos, matCamera);
		return tPos;
	}

	float GetViewDistance() const
	{
		return viewDistance;
	}

	float GetFov() const
	{
		return fov;
	}

	UVNCamera(Point3D position, Vector3D u, Vector3D v, Vector3D n, 
		float nearClip, float farClip, float fov, float viewDistance)
		:position(position), u(u), v(v), n(n),
		nearClip(nearClip), farClip(farClip), fov(fov), viewDistance(viewDistance)
	{
		CalculateMatCamera();
	}
	~UVNCamera()
	{
	}
private:
	//计算相机矩阵
	void CalculateMatCamera()
	{
		//平移矩阵
		Matrix4X4 translation = { 1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			-position.x, -position.y, -position.z, 1 };
		//旋转矩阵
		Matrix4X4 rotation = { u.x, u.y, u.z, 0,
			v.x, v.y,v.z, 0,
			n.x, n.y, n.z, 0,
			0,0,0,1 };

		MatrixMul(matCamera, translation, rotation);
	}

private:
	Point3D position;	//相机坐标
	Matrix4X4 matCamera;//相机变换矩阵

	Vector3D u;			//u向量
	Vector3D v;			//v向量
	Vector3D n;			//n向量

	float fov;			//视广角
	float nearClip;		//近裁剪面
	float farClip;		//远裁剪面
	float viewDistance;	//视距
};
#endif