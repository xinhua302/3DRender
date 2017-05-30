#ifndef __MATERIALDEFINE_H
#define __MATERIALDEFINE_H
#include "../3DMath.h"
#include "Define/ColorDefine.h"

struct Material
{
	int id;
	int state;
	int attr;
	char name[64];
	double ka;			//环境光反射系数
	double kd;			//漫反射光反射系数
	double ks;			//镜面光反射系数
	double shininess;	//高光指数
	UINT *texBuffer;	//纹理图
	double width;
	double height;
	Color color;
	Color ra, rd, rs;

	Material(UINT *tex, double width, double height)
	{
		this->texBuffer = tex;
		this->width = width;
		this->height = height;
	}
};
#endif