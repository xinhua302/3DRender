#ifndef __COLORDEFINE_H
#define __COLORDEFINE_H
#include "../3DMath.h"

struct Color
{
	double r;
	double g;
	double b;
	double alpha;

	UINT Get32()
	{
		return (UINT(r * 255) << 16) + (UINT(g * 255) << 8) + (UINT(b * 255));
	}
};
#endif