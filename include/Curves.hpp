#pragma once

#include "Point.hpp"
#include "Utilities.hpp"
#include <iostream> 
#include <string>
#include <sstream>
#include<algorithm>

typedef Point TimeSeries;
typedef double (*CurveDistMetric)(const Curve&, const Curve&);

double FrechetDistDiscrete(const Curve& c1, const Curve& c2);

class Curve
{
private:
    std::string id; 
    CurveDistMetric curveDist;
    std::vector<Point> curve;
    parameter_t _complexity;
    parameter_t dim;
public:
    Curve(std::string _id = "<unknown>", CurveDistMetric _curveDist = FrechetDistDiscrete, std::vector<Point> _curve = std::vector<Point>());
    ~Curve();

    unsigned long complexity() const; 
    int dimensions() const;
    std::vector<Point> getCurvePoints() const;
    void setId(std::string id);
    std::string getId() const;
    void AddToCurve(Point* p);

    // estimate the distance of 2 curves, given the dist metric by the user
    double dist(Curve& curve) const;

    // transform the current continuous curve into a discrete curve and return a pointer to it
    Curve *filter(void);
};
