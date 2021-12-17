#pragma once

#include "NearestNeighboursSolver.hpp"
#include "CurveNearestNeighbours.hpp"
#include "Profiler.hpp"

#include <unordered_map>

#define ITER_MAX 300
#define MIN_UPDATES 5
#define R_MAX 30000.0

typedef std::pair<int, double> ClosestCentroid;
typedef std::pair<std::string ,std::pair<int, double>> Silhouette_type; // point_id,(cluster_id,distance)

class KMeans_Solver{

protected:

public:
    static int K; // number of clusters 
    static int hc_M, hc_probes, hc_k, lsh_L, lsh_k ;
    
    KMeans_Solver(int K);

    virtual ~KMeans_Solver();
    
    // clear the centroids
    virtual void clear_centroids(void) = 0; 

    // initialize centroids
    virtual void init_centroids(int K) = 0; 

    // update centroids after an assginment step
    virtual void update_centroids_step(void) = 0; 

    // perform a step of k means ++
    virtual uint32_t _k_means_step(void) = 0; 
    
    virtual uint32_t get_size() const = 0;
    
    static void parse_config_file(std::string file_name);
};

// overload to print silhouette as a vector
ostream& operator<<(ostream& os, vector<double> silhouettes);

template <class T, class CentroidT>
void insert_in_closest_center(T* q, vector<CentroidT>& centroids) {
    // find the distance to first centroid
    int closest_center = 0;
    double min_distance = q->dist(centroids[0].first);
    int K = centroids.size();

    // compare with the rest cluster centers
    for (int i = 1; i < K; i++) {
        double distance = q->dist(centroids[i].first);
        if (distance < min_distance) {
            closest_center = i;
            min_distance = distance;
        }
    }

    // insert it
    centroids[closest_center].second[q] = min_distance;
    q->setMarked(true);
    q->setCluster(closest_center);
}
