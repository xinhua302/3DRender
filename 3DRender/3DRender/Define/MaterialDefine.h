#ifndef __MATERIALDEFINE_H
#define __MATERIALDEFINE_H
#include "../3DMath.h"

struct Material
{
	int id;
	int state;
	int attr;
	char *name;
	double ka;			//环境光反射系数
	double kd;			//漫反射光反射系数
	double ks;			//镜面光反射系数
	double shininess;	//高光指数
	UINT *texBuffer;	//纹理图
	UINT width;
	UINT height;
};
#endif