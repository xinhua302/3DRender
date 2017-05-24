#ifndef __COLORDEFINE_H
#define __COLORDEFINE_H
#include "../3DMath.h"

struct Color
{
	double r;
	double g;
	double b;
	double alpha;

	Color()
	{

	}

	Color(UINT color)
	{
		r = double(color >> 16) / 255;
		g = double(color >> 8) / 255;
		b = double(color) / 255;
		alpha = double(color >> 24) / 255;
	}

	UINT Get32()
	{
		return (UINT(r * 255) << 16) + (UINT(g * 255) << 8) + (UINT(b * 255));
	}
};
#endif