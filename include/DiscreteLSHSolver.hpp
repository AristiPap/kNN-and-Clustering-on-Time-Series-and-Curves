#pragma once

#include "Curves.hpp"
#include "Point.hpp"
#include "Utilities.hpp"
#include <iostream>
#include <iostream> 

class CurveNearestNeighboursSolver
{
private:
    uint32_t _curve_L, _vector_L;
    uint32_t _vector_w;
    uint32_t _vector_k;
    uint32_t _curve_delta;
    uint32_t _curve_d;
public:
    CurveNearestNeighboursSolver(size_t table_size, uint32_t curve_L, uint32_t curve_delta, uint32_t curve_d, uint32_t vector_k=4, uint32_t vector_w=500, uint32_t vector_L=5);
    ~CurveNearestNeighboursSolver();
};


class DiscreteLSHSolver: public CurveNearestNeighboursSolver{
private:
    
    
public:
    DiscreteLSHSolver(std::list<Point *>& dataset, size_t table_size, uint32_t curve_L, uint32_t curve_delta, uint32_t curve_d, uint32_t vector_k, uint32_t vector_w, uint32_t vector_L);
    ~DiscreteLSHSolver();
};
