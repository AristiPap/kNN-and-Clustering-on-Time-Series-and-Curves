#pragma once

#include "Point.hpp"
#include <iostream> 
#include <string>
#include <sstream>
#include<algorithm>

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
    unsigned long complexity() const; 
    int dimensions() const;
    std::vector<Point> getCurvePoints() const;
    void setId(std::string id);
    std::string getId() const;
    void AddToCurve(Point* p);
    ~Curve();
};
