/*
*3D数学库
*/
#ifndef __3DMATH_H
#define __3DMATH_H
#include <cmath>

typedef unsigned int UINT32;
//4X4矩阵
struct Matrix4X4
{
	float m[4][4];
};

//3D向量
struct Vector3D
{
	float x, y, z, w;
};

//2D向量
struct Vector2D
{
	int x, y;

	bool operator<(const Vector2D & v) 
	{
		return (y < v.y);
	}

	bool operator>(const Vector2D & v)
	{
		return (y > v.y);
	}

};

//2D点
typedef Vector2D Point2D;

template<class T>
void Swap(T &p1, T &p2)
{
	T temp = p1;
	p1 = p2;
	p2 = temp;
}

//选择排序
template<class T>
void SelectionSort(T arr[], int count)
{	
	for (int i = 0; i < count-1; i++)
	{
		int index = i;
		for (int j = index; j < count; j++)
		{
			if (arr[j] > arr[index])
			{
				continue;
			}
			index = j;
		}

		if (index != i)
		{
			Swap(arr[i], arr[index]);
		}
	}
}

//4D点
typedef Vector3D Point3D;

//值在范围内
int MakeValueInRange(int value, int min, int max)
{
	return (value < min) ? min : ((value > max) ? max : value);
}

//计算插值 t[0, 1]
float Interp(float value1, float value2, float t)
{
	return value1 + (value2 - value1) * t;
}

//计算向量长度
float CalculateVector3DLength(const Vector3D &v)
{
	float sq = v.x * v.x + v.y * v.y + v.z * v.z;
	return sqrt(sq);
}

//向量加
void VectorAdd(Vector3D &destV, const Vector3D &x, const Vector3D &y)
{
	destV.x = x.x + y.x;
	destV.y = x.y + y.y;
	destV.z = x.z + y.z;
	destV.w = 1.0f;
}

//向量减
void VectorSub(Vector3D &destV, const Vector3D &x, const Vector3D &y)
{
	destV.x = x.x - y.x;
	destV.y = x.y - y.y;
	destV.z = x.z - y.z;
	destV.w = 1.0f;
}

//向量点乘
float VectorDot(const Vector3D &x, const Vector3D &y)
{
	return x.x * y.x + x.y * y.y + x.z * y.z;
}

//向量叉乘
void VectorCross(Vector3D &destV, const Vector3D &x, const Vector3D &y)
{
	destV.x = x.y * y.z - x.z * y.y;
	destV.y = x.z * y.x - y.z * x.x;
	destV.z = x.x * y.y - y.x * x.y;
	destV.w = 1.0f;
}

//向量插值
void VectorInterp(Vector3D &destV, const Vector3D &x, const Vector3D &y, float t)
{
	destV.x = Interp(x.x, y.x, t);
	destV.y = Interp(x.y, y.y, t);
	destV.z = Interp(x.z, y.z, t);
	destV.w = 1.0f;
}

//向量归一化
void VectorNormalize(Vector3D &v)
{
	float length = CalculateVector3DLength(v);
	if (length != 0.0f)
	{
		v.x /= length;
		v.y /= length;
		v.z /= length;
	}
}

//矩阵相加
void MatrixAdd(Matrix4X4 &destM, const Matrix4X4 &m1, const Matrix4X4 &m2)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			destM.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
}

//矩阵减
void MatrixSub(Matrix4X4 &destM, const Matrix4X4 &m1, const Matrix4X4 &m2)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			destM.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}
}

//矩阵乘法
void MatrixMul(Matrix4X4 &destM, const Matrix4X4 &m1, const Matrix4X4 &m2)
{
	for (int i = 0; i < 4; i++) 
	{
		for (int j = 0; j < 4; j++) 
		{
			destM.m[j][i] = (m1.m[j][0] * m2.m[0][i]) +
				(m1.m[j][1] * m2.m[1][i]) +
				(m1.m[j][2] * m2.m[2][i]) +
				(m1.m[j][3] * m2.m[3][i]);
		}
	}
}

//向量和矩阵相乘
void MatrixApply(Vector3D &destV, const Vector3D v, const Matrix4X4 &m)
{
	float X = v.x, Y = v.y, Z = v.z, W = v.w;
	destV.x = X * m.m[0][0] + Y * m.m[1][0] + Z * m.m[2][0] + W * m.m[3][0];
	destV.y = X * m.m[0][1] + Y * m.m[1][1] + Z * m.m[2][1] + W * m.m[3][1];
	destV.z = X * m.m[0][2] + Y * m.m[1][2] + Z * m.m[2][2] + W * m.m[3][2];
	destV.w = X * m.m[0][3] + Y * m.m[1][3] + Z * m.m[2][3] + W * m.m[3][3];
}

//矩阵缩放
void MatrixScale(Matrix4X4 &destM, const Matrix4X4 &m, float scale)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			destM.m[i][j] = m.m[i][j] * scale;
		}
	}
}

//单位矩阵
void MatrixIdentity(Matrix4X4 &destM)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i == j)
			{
				destM.m[i][j] = 1.0f;
			}
			else
			{
				destM.m[i][j] = 0.0f;
			}			
		}
	}
}

//零矩阵
void MatrixZero(Matrix4X4 &destM)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			destM.m[i][j] = 0.0f;
		}
	}
}

//平移变换
void MatrixTranslate(Matrix4X4 &m, float x, float y, float z)
{
	MatrixIdentity(m);
	m.m[3][0] = x;
	m.m[3][1] = y;
	m.m[3][2] = z;
}

//缩放变换
void MatrixScale(Matrix4X4 &m, float x, float y, float z)
{
	MatrixIdentity(m);
	m.m[0][0] = x;
	m.m[1][1] = y;
	m.m[2][2] = z;
}

//矩阵旋转
void MatrixRotate(Matrix4X4 &m, float x, float y, float z, float theta)
{
	float qsin = (float)sin(theta * 0.5f);
	float qcos = (float)cos(theta * 0.5f);
	Vector3D vec = { x, y, z, 1.0f };
	float w = qcos;
	VectorNormalize(vec);
	x = vec.x * qsin;
	y = vec.y * qsin;
	z = vec.z * qsin;
	m.m[0][0] = 1 - 2 * y * y - 2 * z * z;
	m.m[1][0] = 2 * x * y - 2 * w * z;
	m.m[2][0] = 2 * x * z + 2 * w * y;
	m.m[0][1] = 2 * x * y + 2 * w * z;
	m.m[1][1] = 1 - 2 * x * x - 2 * z * z;
	m.m[2][1] = 2 * y * z - 2 * w * x;
	m.m[0][2] = 2 * x * z - 2 * w * y;
	m.m[1][2] = 2 * y * z + 2 * w * x;
	m.m[2][2] = 1 - 2 * x * x - 2 * y * y;
	m.m[0][3] = m.m[1][3] = m.m[2][3] = 0.0f;
	m.m[3][0] = m.m[3][1] = m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;
}

struct Transform
{
	Matrix4X4 world;
	Matrix4X4 view;
	Matrix4X4 projection;
	Matrix4X4 transform;	//transform = world * view * projection
	float w, h;				//屏幕大小
};

//更新矩阵
void TransformUpdate(Transform &ts)
{
	Matrix4X4 m;
	MatrixMul(m, ts.world, ts.view);
	MatrixMul(ts.transform, m, ts.projection);
}
#endif