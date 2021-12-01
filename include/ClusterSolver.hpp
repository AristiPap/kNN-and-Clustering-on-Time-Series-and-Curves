#pragma once


#include "Utilities.hpp"
#include "NearestNeighboursSolver.hpp"
#include "Point.hpp"
#include <unordered_map>

#define ITER_MAX 300
#define MIN_UPDATES 5
#define R_MAX 30000.0

// static struct to help reverse assignment
static set<Point *> unassigned_points;

// typedef std::pair<Point *, double> Datapoint;
typedef std::pair<int, double> ClosestCentroid;
typedef std::pair<uint32_t ,std::pair<int, double>> Silhouette_type; // point_id,(cluster_id,distance)
typedef std::unordered_map<Point *, double> Datapoints;
typedef std::pair<Point, Datapoints> Centroid; // centroid is a Point (centroid interpretation) and a list of assign cluster points
typedef uint32_t (*AssignmentStep) (std::vector<Centroid>& centroids, std::list<Point *>& dataset);

class KMeans_pp_Solver {
private:
    std::list<Point *>& dataset;
    std::vector<Centroid> centroids; // list of centroids
    AssignmentStep assignment_step; // The function that decides the clustering assignment


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
    
    KMeans_pp_Solver(std::list<Point *>& dataset, AssignmentStep __assignment_step, int K);
    ~KMeans_pp_Solver();

    // Perform a clustering with K means:
    // iterate at most iter_max times
    // if less that min_changed_elements change cluster then break loop
    std::vector<Centroid>* get_k_clusters(int iter_max, uint32_t min_changed_elements);
    static void evaluate_w_shilouette(std::vector<Centroid> clusters); // TODO: change
};

//Lloyd function and its helper function to find closest centroids
ClosestCentroid find_closest_center(Point &q, std::vector<Centroid> &centroids);
uint32_t Lloyd (std::vector<Centroid> &centroids, std::list<Point *> &dataset);


// Actual implementation of reverse assignment utilizing a Nearest Neighbour Solver class (either LSH or Hypercube will do).
uint32_t __reverse_assignment__(std::vector<Centroid> &centroids, std::list<Point *> &dataset, NearestNeighboursSolver &solver);

uint32_t reverse_assignment_lsh(std::vector<Centroid> &centroids, std::list<Point *> &dataset);

uint32_t reverse_assignment_hypercube(std::vector<Centroid> &centroids, std::list<Point *> &dataset);

//get average for each centroid
vector<double> * evaluate_w_silhouette_per_centroid(std::vector<Centroid> &centroids, const std::list<Point *> &dataset);
//get average in total 
vector<double> *  evaluate_w_silhouette(std::vector<Centroid>& clusters, const std::list<Point *> &dataset);
// overload to print silhouette as a vector
ostream& operator<<(ostream& os, vector<double> silhouettes);

void insert_in_closest_center(Point *q, vector<Centroid> &centroids);