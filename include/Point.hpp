#pragma once
#include <vector>
#include <bitset>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <assert.h>

#include "types.hpp"
#include "interval.hpp"

// used to pad a point
extern double _pad_num_;

using namespace std;
typedef double (*DistanceMetric)(const Point&, const Point&);

// Default point distance metric
double L1_norm(const Point& p1, const Point& p2);
double L2_norm(const Point& p1, const Point& p2);

// calculate the frechet distance of the original curves between 
// 2 concatenated grid curve in Point objs
double InitialCurveDF(const Point& p1, const Point& p2);

class Point{
    private:
        DistanceMetric distMetric;
        std::string id;
        int dims; 
        vector<double> coords;
        bool marked;
        /* used in clustering */
        int cluster_id; //closest centroid_id 
        int s_cluster_id; //second closest centroid_id
        /* used in curves */
        Curve *initial;
    public:
        //constructor
        Point(DistanceMetric distMetric);
        Point(std::string id, int dims, DistanceMetric distMetric);
        Point(std::string id, vector<double> p, DistanceMetric distMetric);
        Point(const Point& p);
        //setters
        void setId(std::string id);
        void setDims(int dim_num);
        void setMarked(bool flg);
        void setCoordinate(double x, int i);
        void addCoordinate(double x);
        void setCoordinates(const std::vector<double> v);
        void setCluster(int cluster_id);
        void setSecCluster(int s_cluster_id);
        void setInitial(Curve *curve);
        void padding(int new_d);
        //getters
        DistanceMetric getDistMetric() const;
        std::string getId() const;
        int getDims() const;
        bool getMarked() const;
        int getCluster() const;
        int getSecCluster() const;
        double getCoordinate(int i) const;
        vector<double> getCoordinates() const;
        Curve *getCurve(void) const;

        //utility-functions
        // estimate distance from another point using L2-norm
        double dist(Point p) const;

        // operator overloads
        double operator*(const Point& p) const;
        void operator+=(const Point &p);
        void operator/=(const double n);
        void operator*=(const double n);
        Point operator-(const Point p) const;
        Point operator+(const Point p) const;
        // Fred add-on for frechet dist
        distance_t length_sqr() const;
        distance_t dist_sqr(const Point &point) const;
        distance_t line_segment_dist_sqr(const Point &p1, const Point &p2) const;
        Interval ball_intersection_interval(const distance_t distance_sqr, const Point& line_start, const Point& line_end) const;
};

// overload to print a point as a vector
ostream& operator<<(ostream& os, const Point& p);


