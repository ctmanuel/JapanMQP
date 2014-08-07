#ifndef NATURALSPLINE_H
#define NATURALSPLINE_H

#include "spline_library/spline.h"

#include <unordered_map>

class NaturalSpline : public Spline
{
    //constructors
    public:
        NaturalSpline(const std::vector<SplineVector3D> &points, bool includeEndpoints, double alpha = 0.0);
    protected:
        //you're only allowed to create one of these without point data if a subclass is providing the point data
        NaturalSpline();

    //methods
    public:
        virtual SplineVector3D getPosition(double x) const override;
        virtual InterpolatedPT getTangent(double x) const override;
        virtual InterpolatedPTC getCurvature(double x) const override;
        virtual InterpolatedPTCW getWiggle(double x) const override;

        virtual double getT(int index) const override;
        virtual double getMaxT(void) const override;

        virtual const std::vector<SplineVector3D> &getPoints(void) const override;

        virtual bool isLooping(void) const override;

    protected:

        struct InterpolationData;

        inline SplineVector3D computePosition(double t, const InterpolationData &segment) const;
        inline SplineVector3D computeTangent(double t, const InterpolationData &segment) const;
        inline SplineVector3D computeCurvature(double t, const InterpolationData &segment) const;
        inline SplineVector3D computeWiggle(const InterpolationData &segment) const;

        int getSegmentIndex(double x) const;

    //data
    protected:
        //a vector containing pre-computed datasets, one per segment
        //there will be lots of duplication of data here,
        //but precomputing this really speeds up the interpolation
        int numSegments;
        std::vector<InterpolationData> segmentData;

        double maxT;

        //original point data
        std::vector<SplineVector3D> points;

        //map from index to t value. it's a map and not an array so we can store negative indexes
        std::unordered_map<int,double> indexToT;
};

struct NaturalSpline::InterpolationData
{
    //for natural splines, the "point data" is coefficcients for a single cubic function
    SplineVector3D a, b, c, d;

    //t values
    double t0;
    double t1;

    //reciprocal of distance in T between t0 and t1
    double tDistanceInverse;

    //padding to make sure we're aligned to 16 bytes
    double padding;
};

inline SplineVector3D NaturalSpline::computePosition(double t, const InterpolationData &segment) const
{
    return segment.a + t * (segment.b + t * (segment.c + t * segment.d));
}

inline SplineVector3D NaturalSpline::computeTangent(double t, const InterpolationData &segment) const
{
    //compute the derivative of the position function
    return segment.b + t * (2 * segment.c + (3 * t) * segment.d);
}

inline SplineVector3D NaturalSpline::computeCurvature(double t, const InterpolationData &segment) const
{
    //compute the 2nd derivative of the position function
    return 2 * segment.c + (6 * t) * segment.d;
}

inline SplineVector3D NaturalSpline::computeWiggle(const InterpolationData &segment) const
{
    //compute the 3rd derivative of the position function
    return 6 * segment.d;
}

#endif // NATURALSPLINE_H
