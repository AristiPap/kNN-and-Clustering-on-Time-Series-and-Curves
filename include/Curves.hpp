#pragma once

#include "Point.hpp"
#include "Utilities.hpp"
#include <iostream> 
#include <string>
#include <sstream>
#include <algorithm>

typedef Point TimeSeries;
typedef double (*CurveDistMetric)(const Curve&, const Curve&);

double FrechetDistDiscrete(const Curve& c1, const Curve& c2);
double FrechetDistContinuous(const Curve& c1, const Curve& c2);

class Curve
{
private:
    std::string id; 
    CurveDistMetric curveDist;
    std::vector<Point> curve;
    parameter_t _complexity;
    parameter_t dim;
    bool marked;
    /* used in clustering */
    int cluster_id; //closest centroid_id 
    int s_cluster_id; //second closest centroid_id
public:
    Curve(std::string _id = "<unknown>", CurveDistMetric _curveDist = FrechetDistDiscrete, std::vector<Point> _curve = std::vector<Point>());
    ~Curve();

    unsigned long complexity() const; 
    int dimensions() const;
    const std::vector<Point>& getCurvePoints() const;
    void setId(std::string id);
    void setMarked(bool flg);
    void setCluster(int cluster_id);
    void setSecCluster(int scluster_id);
    std::string getId() const;
    int getCluster() const;
    int getSecCluster() const;
    bool getMarked() const;
    void AddToCurve(Point* p);
    void setPoints(const Curve * c);
    // estimate the distance of 2 curves, given the dist metric by the user
    double dist(Curve& curve) const;
};
