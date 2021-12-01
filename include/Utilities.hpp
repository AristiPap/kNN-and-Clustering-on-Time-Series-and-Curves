#pragma once

#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include <assert.h>

#include "Point.hpp"

// get vector which values are acquired from N(0,1)
std::vector<double> get_random_vec(int dim);

// get a random float number from [0, w), acquired uniformly
float get_random_num(int max);

// get a random int number from [0, w), acquired uniformly
int32_t get_random_num(int32_t min, int32_t max);

// vector multiplication
uint32_t HammingDistance(uint32_t a, uint32_t b, int dd);
double vec_mul(std::vector<double> a, std::vector<double> b);
struct my_less{
    bool operator()(const std::pair<Point *,double>& l, const std::pair<Point *, double>& r);
};

constexpr int64_t mod(int64_t a, int64_t b) { return (a % b + b) % b; }
