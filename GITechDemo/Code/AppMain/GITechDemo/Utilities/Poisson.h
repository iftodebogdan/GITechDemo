/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   Poisson.h
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

#ifndef POISSON_H
#define POISSON_H

#include <vector>

struct sPoint
{
    sPoint()
        : x(0)
        , y(0)
        , m_Valid(false)
    {}
    sPoint(float X, float Y)
        : x(X)
        , y(Y)
        , m_Valid(true)
    {}
    float x;
    float y;
    bool m_Valid;
    //
    bool IsInRectangle() const
    {
        return x >= 0 && y >= 0 && x <= 1 && y <= 1;
    }
    //
    bool IsInCircle() const
    {
        float fx = x - 0.5f;
        float fy = y - 0.5f;
        return (fx*fx + fy*fy) <= 0.25f;
    }
};

std::vector<sPoint> GeneratePoissonPoints(float MinDist, int NewPointsCount, size_t NumPoints);

#endif // POISSON_H
