#pragma once

#include "CurveNearestNeighbours.hpp"
#include "LSHNearestNeighbours.hpp"
#include "hyper_cube.hpp"
//#include "Hashing.hpp"
#include "CurveHashing.hpp"

class LSHSolver: public CurveNearestNeighboursSolver{
private:
    std::vector<HashingCurve *> grid_hashes; // use the grid snappers, h's, to transform the curve into a vector 
    std::vector<NearestNeighboursSolver *> solvers; // use one different solver for each h to estimate proximity between the grid solvers

    void insert_in_grid_storage(std::list<Curve *>& dataset,std::string storage_type, uint32_t _L, uint32_t k, uint32_t dd, uint32_t M, uint32_t probes, int flag);
    void transform_dataset(std::list<Curve *>&, HashingCurve &h);

public:
    LSHSolver(std::list<Curve *> &dataset, uint32_t curve_L, uint32_t curve_delta, uint32_t curve_d, int flag, std::string storage_type="LSH", uint32_t _L=5, uint32_t k=4, uint32_t dd=14, uint32_t M=10, uint32_t probes=2);
    ~LSHSolver();
    // Find N nearest neighbours of q. Returns list of <Curve *, dist from q>.
    std::list<CurveNeighbour> *kNearestNeighbours(Curve &q, uint N = 1) final;
};


