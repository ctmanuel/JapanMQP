#ifndef SplineInverter_H
#define SplineInverter_H

#include <vector>
#include <memory>

#include "vector3D.h"
class Spline;
class SplineSampleTree;

class SplineInverter
{
public:
	SplineInverter(const std::shared_ptr<Spline> &spline, int samplesPerT = 10);
	~SplineInverter();

    double findClosestT(const SplineVector3D &queryPoint) const;

private: //data
    std::shared_ptr<Spline> spline;

    //distance in t between samples
    double sampleStep;

	//when we find a T whose abs(distance slope) is less than this tolerance, we return
    double slopeTolerance;

    //inner class used to provide an abstraction between the spline inverter and nanoflann
    std::unique_ptr<SplineSampleTree> sampleTree;
};

#endif // SplineInverter_H
