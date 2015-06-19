#ifndef POISSON_H_
#define POISSON_H_

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

#endif // POISSON_H_