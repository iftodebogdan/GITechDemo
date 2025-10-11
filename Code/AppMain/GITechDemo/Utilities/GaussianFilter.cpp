/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   GaussianFilter.cpp
 *      Author: Bogdan Iftode
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

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
