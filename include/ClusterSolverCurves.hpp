#pragma once

#include "Utilities.hpp"
#include "CurveNearestNeighbours.hpp"
#include "Curves.hpp"
#include <unordered_map>

#define ITER_MAX 300
#define MIN_UPDATES 5
#define R_MAX 30000.0

// static struct to help reverse assignment
static set<Curve *> unassigned_points;

typedef std::pair<int, double> ClosestCentroid;
typedef std::pair<std::string ,std::pair<int, double>> Silhouette_type; // point_id,(cluster_id,distance)
typedef std::unordered_map<Curve *, double> CurveDatapoints;
typedef std::pair<Curve, CurveDatapoints> CurveCentroid; // CurveCentroid is a Curve (CurveCentroid interpretation) and a list of assign cluster points
typedef uint32_t (*AssignmentStep) (std::vector<CurveCentroid>& centroids, std::list<Curve *>& dataset);
typedef void (*UpdateStep) (std::vector<CurveCentroid>& centroids);

class KMeans_pp_Solver_Curves {
private:
    std::list<Curve *>& dataset;
    std::vector<CurveCentroid> centroids; // list of centroids
    AssignmentStep assignment_step; // The function that decides the clustering assignment
    UpdateStep update_step;
    // clear the centroids
    void clear_centroids(void);

    // initialize centroids
    void init_centroids(int K);

    // update centroids after an assginment step
    void update_centroids_step(void);

    // perform a step of k means ++
    uint32_t _k_means_step(void);
    
    uint32_t get_size() const;

public:
    // parse config_file
    static void parse_config_file(std::string file_name);
    
    static int K; // number of clusters
    static int hc_M, hc_probes, hc_k, lsh_L, lsh_k ;
    
    KMeans_pp_Solver_Curves(std::list<Curve *>& dataset, AssignmentStep __assignment_step,UpdateStep __update_step, int K);
    ~KMeans_pp_Solver_Curves();

    // Perform a clustering with K means:
    // iterate at most iter_max times
    // if less that min_changed_elements change cluster then break loop
    std::vector<CurveCentroid>* get_k_clusters(int iter_max, uint32_t min_changed_elements);
    static void evaluate_w_shilouette(std::vector<CurveCentroid> clusters); // TODO: change
};

//Lloyd function and its helper function to find closest centroids
ClosestCentroid find_closest_center(Curve &q, std::vector<CurveCentroid> &centroids);
uint32_t Lloyd (std::vector<CurveCentroid> &centroids, std::list<Curve *> &dataset);


// Actual implementation of reverse assignment utilizing a Nearest Neighbour Solver class (either LSH or Hypercube will do).
uint32_t __reverse_assignment__(std::vector<CurveCentroid> &centroids, std::list<Curve *> &dataset, NearestNeighboursSolver &solver);

// TODO: define it
uint32_t reverse_assignment_curve_lsh(std::vector<CurveCentroid> &centroids, std::list<Curve *> &dataset);


//get average for each CurveCentroid
vector<double> * evaluate_w_silhouette_per_centroid(std::vector<CurveCentroid> &centroids, const std::list<Curve *> &dataset);
//get average in total 
vector<double> *  evaluate_w_silhouette(std::vector<CurveCentroid>& clusters, const std::list<Curve *> &dataset);
// overload to print silhouette as a vector
ostream& operator<<(ostream& os, vector<double> silhouettes);

void insert_in_closest_center(Curve *q, vector<CurveCentroid> &centroids);