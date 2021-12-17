#pragma once

#include "GenericClusterSolver.hpp"

#define CLUSTERING_DELTA 0.57

// static struct to help reverse assignment
static set<Curve *> unassigned_curves;


typedef std::unordered_map<Curve *, double> CurveDatapoints;
typedef std::pair<Curve, CurveDatapoints> CurveCentroid; // CurveCentroid is a Curve (CurveCentroid interpretation) and a list of assign cluster points
typedef uint32_t (*CurveAssignmentStep) (std::vector<CurveCentroid>& centroids, std::list<Curve *>& dataset);
typedef void (*CurveUpdateStep) (std::vector<CurveCentroid>& centroids);

class KMeans_pp_Solver_Curves : public KMeans_Solver {
private:
    std::list<Curve *>& dataset;
    std::vector<CurveCentroid> centroids; // list of centroids
    CurveAssignmentStep assignment_step; // The function that decides the clustering assignment
    CurveUpdateStep update_step;
    

    // clear the centroids
    void clear_centroids(void) final; 
    
    // initialize centroids
    void init_centroids(int K) final; 
    
    // update centroids after an assginment step
    void update_centroids_step(void) final; 
    
    // perform a step of k means ++
    uint32_t _k_means_step(void) final; 
        
    uint32_t get_size() const final;

public:
    KMeans_pp_Solver_Curves(std::list<Curve *>& dataset, CurveAssignmentStep __assignment_step, int K);
    virtual ~KMeans_pp_Solver_Curves();

    // Perform a clustering with K means:
    // iterate at most iter_max times
    // if less that min_changed_elements change cluster then break loop
    std::vector<CurveCentroid>* get_k_clusters(int iter_max, uint32_t min_changed_elements);
    //static void evaluate_w_shilouette(std::vector<CurveCentroid> clusters); // TODO: change
};

//Lloyd function and its helper function to find closest centroids
//ClosestCentroid find_closest_center(Curve &q, std::vector<CurveCentroid> &centroids);
uint32_t Lloyd (std::vector<CurveCentroid> &centroids, std::list<Curve *> &dataset);


uint32_t reverse_assignment_lsh_curves(std::vector<CurveCentroid> &centroids, std::list<Curve *> &dataset);

//get average for each CurveCentroid
//vector<double> * evaluate_w_silhouette_per_centroid(std::vector<CurveCentroid> &centroids, const std::list<Curve *> &dataset);

//get average in total 
//vector<double> *  evaluate_w_silhouette(std::vector<CurveCentroid>& clusters, const std::list<Curve *> &dataset);


// void insert_in_closest_center(Curve *q, vector<CurveCentroid> &centroids);