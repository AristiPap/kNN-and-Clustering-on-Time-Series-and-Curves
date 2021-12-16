#pragma once

#include <iostream>
#include <list>

#include "CurveHashing.hpp"
#include "Utilities.hpp"

using CurveNeighbour = std::pair<Curve *, double>;

class CurveNearestNeighboursSolver {
   protected:
    uint32_t _curve_L;
    double _curve_delta;
    uint32_t _curve_d;
    std::list<Curve *> &dataset;
    std::list<std::list<Point *>> dataset_transformed; // dataset after snapping and filtering

   public:
    CurveNearestNeighboursSolver(std::list<Curve *> &dataset, uint32_t curve_L,
                                 double curve_delta, uint32_t curve_d);
    virtual ~CurveNearestNeighboursSolver();

    // Find N nearest neighbours of q. Returns list of <Curve *, dist from q>.
    virtual std::list<CurveNeighbour> *kNearestNeighbours(Curve &q,
                                                          uint N = 1) = 0;
    // Find N nearest neighbours of q. Returns list of <Curve *, dist from q>.
    virtual std::list<CurveNeighbour> *nearestNeighbours_w_rangeSearch(Curve &q, double R) = 0;
};
