// Taken from http://www.programming-techniques.com/2013/02/gaussian-filter-generation-using-cc.html

#include "stdafx.h"

#include "GaussianFilter.h"

#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;

#define PI (3.14159265358979323846f)

void CreateGaussianFilter(float fKernel[], const unsigned int size, const float stdDev)
{
	// set standard deviation to 1.0
	float sigma = stdDev;
	float r, s = 2.f * sigma * sigma;

	// sum is for normalization
	float sum = 0.f;

	// generate size x size kernel
	unsigned int i = 0;
	for (float x = -((float)size - 1.f) / 2.f; x <= ((float)size - 1.f) / 2.f; x += 1.f, i++)
	{
		r = sqrtf(x*x + x*x);
		fKernel[i] = (exp(-(r*r) / s)) / (PI * s);
		sum += fKernel[i];
	}

	// normalize the Kernel
	for (unsigned int i = 0; i < size; i++)
		fKernel[i] /= sum;
}
