#pragma once

#include <list>
#include <random>
#include <set>
#include <unordered_map>
#include <vector>

#include "Point.hpp"
#include "Utilities.hpp"
#include "global_variables_namespace.hpp"

#define BIGINT 4294967291
// Basic Hashing interface
class HashingCurve {
   protected:
    int32_t dim;  // Dimension of the points of the curves
    double delta;
    int32_t L;  // ammout of grids
    int32_t w;
    int32_t k;

   public:
    // every hashing subclass must implement the () operator overloading to
    // enable hashing activity
    virtual uint32_t operator()(const Point& p) = 0;
    HashingCurve(int32_t dim, int32_t L, int32_t w, int32_t k, double delta);
    ~HashingCurve();
};

// Hashing class for LSH algorithm
class DLSHHashingCurve : HashingCurve {
   private:
    vector<double> t;

   public:
    uint32_t operator()(const Point& p);
    DLSHHashingCurve(int32_t k, int32_t w, int32_t dim, double delta);
    ~DLSHHashingCurve();
    static uint32_t estimate_w(std::list<Point*>& dataset);
};

class CLSHHashingCurve : HashingCurve {
   private:
    vector<double> t;
    
   public:
    uint32_t operator()(const Point& p);
    CLSHHashingCurve(int32_t k, int32_t w, int32_t dim, double delta);
    ~CLSHHashingCurve();
    static uint32_t estimate_w(std::list<Point*>& dataset);
};