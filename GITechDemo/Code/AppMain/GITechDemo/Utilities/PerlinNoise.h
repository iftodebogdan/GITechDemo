/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	PerlinNoise.h
 *		Author:	Bogdan Iftode
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#ifndef PERLIN_H_

#define PERLIN_H_

#include <stdlib.h>


#define SAMPLE_SIZE 1024

class Perlin
{
public:

	Perlin(int octaves, double freq, double amp, int seed);

	float Get(float x, float y)
	{
		return (float)Get((double)x, (double)y);
	}

	double Get(double x, double y)
	{
		double vec[2];
		vec[0] = x;
		vec[1] = y;
		return perlin_noise_2D(vec);
	};

private:
	void init_perlin(int n, double p);
	double perlin_noise_2D(double vec[2]);

	double noise1(double arg);
	double noise2(double vec[2]);
	double noise3(double vec[3]);
	void normalize2(double v[2]);
	void normalize3(double v[3]);
	void init(void);

	int   mOctaves;
	double mFrequency;
	double mAmplitude;
	int   mSeed;

	int p[SAMPLE_SIZE + SAMPLE_SIZE + 2];
	double g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];
	double g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
	double g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];
	bool  mStart;

};

#endif

