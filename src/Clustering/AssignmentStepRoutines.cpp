#include "ClusterSolver.hpp"
#include "LSHNearestNeighbours.hpp"
#include "hyper_cube.hpp"
// Assumptions:
// 1. The centroids are recalibrated BUT the assignment list is the same from the last kmeans-step 
// (the one used for re-calibration)
// 2. The points are ALL UNMARKED. Remember to mark them each time you assign them in a cluster

// init radius is min(dist between centroids)/2
static double init_R(vector<Centroid> &centroids) {
    double min_dist = numeric_limits<double>::max();

    for (uint32_t i = 0; i < centroids.size(); i++)
        for (uint32_t j = i + 1; j < centroids.size(); j++)
            min_dist = min(min_dist, centroids[i].first.dist(centroids[j].first));
    
    return min_dist/2.0;
}

static void insert_into_cluster(Point *p, double dist, int centroid_id, vector<Centroid> &centroids, uint32_t &points_changed) {
    assert(centroid_id < centroids.size());

    // get p's previous and under-check cluster-centroid
    Centroid *old_c = p->getCluster() >= 0 ? &(centroids[p->getCluster()]) : nullptr;
    Centroid *new_c = &(centroids[centroid_id]);

    // check if the point is marked
    // if P is marked,
    // then we need to decide in which cluster to assign it.
    // Assign it to the new cluster only if the distance of new centroid is smaller than the assigned one
    // otherwise we will let the point at its assigned cluster
    if (p->getMarked() && old_c != nullptr && dist - old_c->second[p] >= 0) {
        new_c = old_c;
    }

    // do something only in case the previous and the newly assigned clusters are different
    if (new_c != old_c) {
        points_changed++;
        // in case its the first point assignment for this loop then assign it
        if (p->getMarked() == false) unassigned_points.erase(p);
        if (old_c != nullptr) old_c->second.erase(p);
        new_c->second[p] = dist;
        p->setMarked(true);
        p->setCluster(centroid_id);
    }
}

// Actual implementation of reverse assignment utilizing a Nearest Neighbour Solver class (either LSH or Hypercube will do).
uint32_t __reverse_assignment__(vector<Centroid> &centroids, list<Point *> &dataset, NearestNeighboursSolver &solver) {
    static bool start = true;
    // insert all items in an unassigned table
    if (start) {
        unassigned_points.insert(dataset.begin(), dataset.end());
        start = false;
    }

    // initialize R for range search
    static double R = init_R(centroids);
    // number of points that changed cluster
    uint32_t points_changes = 0;

    // perform range search with q-point each one of the centroids 
    for (auto i = 0; i < centroids.size(); i++) {
        // perform the range query, with center the current centroid
        list<Neighbour> *in_range = solver.nearestNeighbours_w_rangeSearch(centroids[i].first, R);
            
        // insert all the neighbours in the centroid
        for (auto neighbour_pair : *in_range) {
            Point *p = neighbour_pair.first;
            double dist = neighbour_pair.second;
            // insert the point into c. If p was in a different centroid before change the changes counter accordingly
            insert_into_cluster(p, dist, i, centroids, points_changes);
        }

        delete in_range; // free the list, but jsut the list not the inner pointers
    }
        
    // at the end of the loop double the search radius
    R *= 2.0;
    if (R - R_MAX > 0) {
        for (auto p = unassigned_points.begin(); p != unassigned_points.end(); p++) {
            insert_in_closest_center(*p, centroids);
            unassigned_points.erase(*p);
        }
    }
    
    #ifdef VERBOSE
    cout << "points_changes: " << points_changes << ", unassigned points: " << unassigned_points.size() << ", R = " << R << endl;
    #endif 

    return points_changes + unassigned_points.size();
}

uint32_t reverse_assignment_lsh(vector<Centroid> &centroids, list<Point *> &dataset) {
    // create lsh solver 
    static uint32_t w = LSHHashing::estimate_w(dataset);
    
    static LSHNearestNeighbours solver(dataset, dataset.size()/8, KMeans_pp_Solver::lsh_k, w, KMeans_pp_Solver::lsh_L);

    return __reverse_assignment__(centroids, dataset, solver);
}

uint32_t reverse_assignment_hypercube(vector<Centroid> &centroids, list<Point *> &dataset) {
    // create lsh solver 
    static uint32_t w = LSHHashing::estimate_w(dataset);
    static HyperCube solver(w, dataset.front()->getDims(), KMeans_pp_Solver::hc_k, KMeans_pp_Solver::hc_probes, KMeans_pp_Solver::hc_M, dataset);
    
    return __reverse_assignment__(centroids, dataset, solver);
}