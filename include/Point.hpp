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

using namespace std;

// Default point distance metric
double L1_norm(const std::vector<double>& p1, const std::vector<double>& p2);
double L2_norm(const std::vector<double>& p1, const std::vector<double>& p2);

typedef double (*DistanceMetric)(const vector<double>&, const vector<double>&);

class Point{
    private:
        DistanceMetric distMetric;
        uint32_t id;
        int dims; 
        vector<double> coords;
        bool marked;
        /* used in clustering */
        int cluster_id; //closest centroid_id 
        int s_cluster_id; //second closest centroid_id
    public:
        //constructor
        Point(DistanceMetric distMetric);
        Point(int id, int dims, DistanceMetric distMetric);
        Point(int id, vector<double> p, DistanceMetric distMetric);
        //setters
        void setId(uint32_t id);
        void setDims(int dim_num);
        void setMarked(bool flg);
        void setCoordinate(double x, int i);
        void addCoordinate(double x);
        void setCoordinates(const std::vector<double> v);
        void setCluster(int cluster_id);
        void setSecCluster(int s_cluster_id);
        //getters
        DistanceMetric getDistMetric() const;
        uint32_t getId() const;
        int getDims() const;
        bool getMarked() const;
        int getCluster() const;
        int getSecCluster() const;
        double getCoordinate(int i) const;
        vector<double> getCoordinates() const;

        //utility-functions
        // estimate distance from another point using L2-norm
        double dist(Point p) const;

        // operator overloads
        double operator*(const Point& p) const;
        void operator+=(const Point &p);
        void operator/=(const double n);
        void operator*=(const double n);

};

// overload to print a point as a vector
ostream& operator<<(ostream& os, const Point& p);


