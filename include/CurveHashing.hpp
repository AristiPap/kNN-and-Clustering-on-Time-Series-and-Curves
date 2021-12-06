#pragma once

#include <list>
#include <random>
#include <set>
#include <unordered_map>
#include <vector>

#include "Point.hpp"
#include "Utilities.hpp"
#include "global_variables_namespace.hpp"
#include "Curves.hpp"

#define BIGINT 4294967291
// Basic Hashing interface
class HashingCurve {
   protected:
    int32_t dim;  // Dimension of the points of the curves
    double delta;
    int32_t w;
    int32_t k;
    vector<double> t;
    DistanceMetric distMetric;
    
   public:
    // every hashing subclass must implement the () operator overloading to
    // enable hashing activity
    virtual Point * operator()(Curve *curve) = 0;
    HashingCurve(int32_t dim, int32_t L, int32_t w, int32_t k, double delta);
    ~HashingCurve();
    Point* vectorCurveToPoint(Curve* hashedCurve, Curve *origin);
    Curve* curveHashing(const Curve &curve);
};

// Hashing class for LSH algorithm
class DLSHHashingCurve : HashingCurve {
   private:
    vector<double> t;

   public:
    Point * operator()(Curve *curve);
    DLSHHashingCurve(int32_t k, int32_t w, int32_t dim, double delta);
    ~DLSHHashingCurve();
    static uint32_t estimate_w(std::list<Curve*>& dataset);
};

class CLSHHashingCurve : HashingCurve {
   private:
    vector<double> t;
    
   public:
    Point * operator()(Curve *curve);
    CLSHHashingCurve(int32_t k, int32_t w, int32_t dim, double delta);
    ~CLSHHashingCurve();
    static uint32_t estimate_w(std::list<Curve*>& dataset);
};