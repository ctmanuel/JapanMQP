#ifndef SPLINE_H
#define SPLINE_H

#include <map>
#include <vector>
#include "vector3D.h"

class Spline
{
public:
    struct InterpolatedPT;
    struct InterpolatedPTC;
    struct InterpolatedPTCW;

	virtual SplineVector3D getPosition(double x) const = 0;
    virtual InterpolatedPT getTangent(double x) const = 0;
    virtual InterpolatedPTC getCurvature(double x) const = 0;
    virtual InterpolatedPTCW getWiggle(double x) const = 0;

	virtual double getT(int index) const = 0;
    virtual double getMaxT(void) const = 0;

	virtual const std::vector<SplineVector3D> &getPoints(void) const = 0;
    virtual bool isLooping(void) const = 0;
};

struct Spline::InterpolatedPT
{
    SplineVector3D position;
    SplineVector3D tangent;

    InterpolatedPT(const SplineVector3D &p, const SplineVector3D &t)
        :position(p),tangent(t)
    {}
};

struct Spline::InterpolatedPTC
{
    SplineVector3D position;
    SplineVector3D tangent;
    SplineVector3D curvature;

    InterpolatedPTC(const SplineVector3D &p, const SplineVector3D &t, const SplineVector3D &c)
        :position(p),tangent(t),curvature(c)
    {}
};

struct Spline::InterpolatedPTCW
{
    SplineVector3D position;
    SplineVector3D tangent;
    SplineVector3D curvature;
    SplineVector3D wiggle;

    InterpolatedPTCW(const SplineVector3D &p, const SplineVector3D &t, const SplineVector3D &c, const SplineVector3D &w)
        :position(p),tangent(t),curvature(c), wiggle(w)
    {}
};

#endif // SPLINE_H
