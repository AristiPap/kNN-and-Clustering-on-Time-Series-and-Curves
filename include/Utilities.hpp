#pragma once

#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include <assert.h>
#include <set>

#include "Point.hpp"
#include "Curves.hpp"
#include "frechet.hpp"
// get vector which values are acquired from N(0,1)
std::vector<double> get_random_vec(int dim);

// get a random float number from [0, w), acquired uniformly
float get_random_num(int max);

// get a random int number from [0, w), acquired uniformly
int32_t get_random_num(int32_t min, int32_t max);

// vector multiplication
uint32_t HammingDistance(uint32_t a, uint32_t b, int dd);
double vec_mul(std::vector<double> a, std::vector<double> b);

//overloads for get mean curve at different hierarchies

// returns a curve where each point is the mean distance between 2 points of a pair from the initial list
Curve *getMeanCurve(std::list<std::pair<const Point *, const Point *>> &optimal_traversal);
// returns the mean distance between 2 
Curve *getMeanCurve(Curve *c1, Curve *c2);
//returns the mean curve from a complete balanced binary tree of curves 
Curve getMeanCurve(vector<Curve>& CurveTree, int max_curve_len);
// compare point, dist-from-query tuples
struct my_less{
    bool operator()(const std::pair<Point *,double>& l, const std::pair<Point *, double>& r);
};

// compare (curve, frechet-dist from query) tuples 
struct curve_compare {
    bool operator()(const std::pair<Curve*, double>& l, const std::pair<Curve*, double>& r);
};


constexpr int64_t mod(int64_t a, int64_t b) { return (a % b + b) % b; }