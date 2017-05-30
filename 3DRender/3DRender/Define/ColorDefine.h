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

	Color(double r, double g, double b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->alpha = 0;
	}

	Color(UINT color)
	{
		r = double((color & 0x00FF0000) >> 16) / 255;
		g = double((color & 0x0000FF00) >> 8) / 255;
		b = double(color & 0x000000FF) / 255;
		alpha = double((color & 0xFF000000) >> 24) / 255;
	}

	Color(double r, double g, double b, double a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->alpha = a;
	}

	UINT Get32()
	{
		return (UINT(r * 255) << 16) + (UINT(g * 255) << 8) + (UINT(b * 255));
	}

	Color operator*(const double x)
	{
		return Color{ this->r * x, this->g* x, this->b * x, 0 };
	}

	Color operator+(const Color &color)
	{
		return Color{ this->r + color.r, this->g + color.g, this->b + color.b, 0 };
	}
};
#endif