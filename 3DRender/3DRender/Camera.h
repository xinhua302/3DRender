/*
*摄像机
*/
#ifndef __CAMERA_H
#define __CAMERA_H
#include "3DMath.h"

class UVNCamera
{
public:
	Point3D GetPosition() const
 	{
		return position;
	}
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

	float GetViewWidth() const
	{
		return width;
	}

	float GetViewHeight() const
	{
		return height;
	}

	//设置相机目标
	void SetTarget(const Point3D &target, Vector3D &v)
	{
		v = v;
		//n方向
 		n = target - position;
		//n向量
		VectorCross(u, v, n);
		//重置v向量
		VectorCross(v, n, u);

		VectorNormalize(u);
		VectorNormalize(v);
		VectorNormalize(n);

		CalculateMatCamera();
	}
		

	UVNCamera(Point3D position, Vector3D target, Vector3D v, 
		float nearClip, float farClip, float fov, float viewDistance, float aspectRatio)
		:position(position), u(u), v(v),
		nearClip(nearClip), farClip(farClip), fov(fov), viewDistance(viewDistance), aspectRatio(aspectRatio)
	{
		SetTarget(target, v);
		width = (float)(viewDistance * tan(fov *  PI / 360.0f ) * 2);
		height = width / aspectRatio;
	}
	~UVNCamera()
	{
	}
private:
	//计算相机矩阵
	void CalculateMatCamera()
	{
		matCamera
			= { u.x, v.x, n.x, 0,
			u.y, v.y, n.y, 0,
			u.z, v.z, n.z, 0,
			-position.x,-position.y,-position.z,1 };
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

	float aspectRatio;	//宽高比
	float width;
	float height;
};
#endif