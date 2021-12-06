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
    int32_t max_curve_len;
    vector<double> t;
    DistanceMetric distMetric;
   public:
    // every hashing subclass must implement the () operator overloading to
    // enable hashing activity
    virtual Point * operator()(Curve *curve) = 0;
    HashingCurve(double delta, int32_t dim, int32_t w, int32_t k, int32_t max_curve_len);
    ~HashingCurve();
    //squeeze curve to point
    Point* squeeze(Curve* hashedCurve, Curve *origin);
    Curve* curveHashing(const Curve &curve);
    static uint32_t estimate_delta(std::list<Curve*>& dataset);

};

// Hashing class for LSH algorithm
class DLSHHashingCurve : public HashingCurve {

   public:
    Point * operator()(Curve *curve);
    DLSHHashingCurve(double delta, int32_t k, int32_t w, int32_t dim, int32_t max_curve_len);
    ~DLSHHashingCurve();
};

class CLSHHashingCurve : HashingCurve {
    
   public:
    Point * operator()(Curve *curve);
    CLSHHashingCurve(double delta, int32_t k, int32_t w, int32_t dim, int32_t max_curve_len);
    ~CLSHHashingCurve();
};