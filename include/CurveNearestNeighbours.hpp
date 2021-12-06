#pragma once

#include <iostream>
#include <list>

#include "CurveHashing.hpp"
#include "Curves.hpp"
#include "NearestNeighboursSolver.hpp"
#include "Point.hpp"
#include "Utilities.hpp"
#include "frechet.hpp"

using CurveNeighbour = std::pair<Curve *, double>;

class CurveNearestNeighboursSolver {
   protected:
    uint32_t _curve_L;
    uint32_t _curve_delta;
    uint32_t _curve_d;
    std::list<Curve *> &dataset;
    std::list<std::list<Point *>> dataset_transformed; // dataset after snapping and filtering

   public:
    CurveNearestNeighboursSolver(std::list<Curve *> &dataset, uint32_t curve_L,
                                 uint32_t curve_delta, uint32_t curve_d);
    ~CurveNearestNeighboursSolver();

    // Find N nearest neighbours of q. Returns list of <Curve *, dist from q>.
    virtual std::list<CurveNeighbour> *kNearestNeighbours(const Curve &q,
                                                          uint N = 1) = 0;
    // Find all neighbours in range R. Returns list of <Curve *, dist from q>.
    virtual std::list<CurveNeighbour> *nearestNeighbours_w_rangeSearch(
        const Curve &q, double R = 10000.0) = 0;
};
