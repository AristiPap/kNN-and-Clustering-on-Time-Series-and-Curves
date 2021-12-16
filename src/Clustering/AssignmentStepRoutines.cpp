#include "ClusterSolver.hpp"
#include "LSHNearestNeighbours.hpp"
#include "hyper_cube.hpp"
#include "ReverseAssignment.hpp"
#include  "LloydTempate.h"

// Assumptions:
// 1. The centroids are recalibrated BUT the assignment list is the same from the last kmeans-step 
// (the one used for re-calibration)
// 2. The points are ALL UNMARKED. Remember to mark them each time you assign them in a cluster

uint32_t reverse_assignment_lsh(vector<Centroid> &centroids, list<Point *> &dataset) {
    // create lsh solver 
    static uint32_t w = LSHHashing::estimate_w(dataset);
    
    static LSHNearestNeighbours solver(dataset, dataset.size()/8, KMeans_pp_Solver::lsh_k, w, KMeans_pp_Solver::lsh_L);

    return __reverse_assignment__(centroids, dataset, solver, R_MAX, unassigned_points);
}

uint32_t reverse_assignment_hypercube(vector<Centroid> &centroids, list<Point *> &dataset) {
    // create lsh solver 
    static uint32_t w = LSHHashing::estimate_w(dataset);
    static HyperCube solver(w, dataset.front()->getDims(), KMeans_pp_Solver::hc_k, KMeans_pp_Solver::hc_probes, KMeans_pp_Solver::hc_M, dataset);
    
    return __reverse_assignment__(centroids, dataset, solver, R_MAX, unassigned_points);
}

uint32_t Lloyd (std::vector<Centroid> &centroids, std::list<Point *> &dataset){
    return __Lloyd__(centroids,dataset);
}