#pragma once

#include "Point.hpp"
#include "Utilities.hpp"
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
    Curve(std::string _id = "<unknown>", CurveDistMetric _curveDist = FrechetDist, std::vector<Point> _curve = std::vector<Point>());
    ~Curve();

    // getters
    std::string getId(void);

    unsigned long complexity() const; 
    int dimensions() const;
    std::vector<Point> getCurvePoints() const;
};
