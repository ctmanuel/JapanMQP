#include "vector3D.h"

#include <cmath>

SplineVector3D SplineVector3D::normalized() const
{
    // Need some extra precision if the length is very small.
    double lengthSquared =	xp * xp +
							yp * yp +
							zp * zp;
    if (lengthSquared < .00000001)
		return SplineVector3D();
	else
	{
		double invLength = 1 / sqrt(lengthSquared);
        return SplineVector3D(xp * invLength, yp * invLength, zp * invLength);
	}
}

double SplineVector3D::dotProduct(const SplineVector3D& v1, const SplineVector3D& v2)
{
    return v1.xp * v2.xp + v1.yp * v2.yp + v1.zp * v2.zp;
}

double SplineVector3D::length() const
{
    return sqrt(xp * xp + yp * yp + zp * zp);
}

double SplineVector3D::lengthSquared() const
{
    return xp * xp + yp * yp + zp * zp;
}