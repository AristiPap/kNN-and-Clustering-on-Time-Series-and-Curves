#include "ClusterSolverCurves.hpp"
#include "GenericLSHSolver.hpp"
#include "ReverseAssignment.hpp"
#include "LloydTempate.h"

uint32_t reverse_assignment_lsh_curves(vector<CurveCentroid> &centroids,
                                list<Curve *> &dataset) {

    // query dataset is centroids, input dataset is the points in the cluster
    // TODO: change the defaults

    static  LSHSolver solver(dataset, 
    KMeans_Solver::lsh_L, 
    CLUSTERING_DELTA, 
    dataset.front()->dimensions(), 
    0, 
    "LSH", 
    1, 
    KMeans_Solver::lsh_k, 
    KMeans_Solver::hc_k, 
    KMeans_Solver::hc_M, 
    KMeans_Solver::hc_probes, 
    dataset.front()->complexity()*2 - 1);
    
    return __reverse_assignment__<Curve, CurveCentroid, LSHSolver>(centroids, dataset, solver, R_MAX, unassigned_curves);
}

uint32_t Lloyd (std::vector<CurveCentroid> &centroids, std::list<Curve *> &dataset){
    return __Lloyd__(centroids,dataset);
}