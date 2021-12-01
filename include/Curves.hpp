#pragma once

#include "Point.hpp"

typedef Point TimeSeries;
typedef double (*CurveDistMetric)(const Curve&, const Curve&);

class Curve
{
private:
    std::string id; 
    CurveDistMetric curveDist;
    std::vector<Point> curve;
    parameter_t _complexity;
    parameter_t dim;
public:
    Curve(/* args */);
    ~Curve();
};
