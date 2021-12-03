#pragma once

#include "Curves.hpp"
#include "Point.hpp"
#include "Utilities.hpp"
#include 

class DiscreteLSHSolver
{
private:
    
public:
    DiscreteLSHSolver(std::list<Point *>& input_data, size_t table_size, uint32_t k=4, uint32_t w=6, uint32_t L);
    ~DiscreteLSHSolver();
};
