#include "ClusterSolver.hpp"
#include <list>
#include <set>

using namespace std;

// define a self-compare
struct compare_sums {
    bool operator()(const pair<Point*, double>& lhs, const pair<Point*, double>& rhs){
        return lhs.second - rhs.second < 0   ? true
               : lhs.second - rhs.second > 0 ? false
                                         : (lhs.first==nullptr || rhs.first == nullptr || strcmp(lhs.first->getId().c_str(),rhs.first->getId().c_str()) < 0); //TODO: WARNING: POSSIBLE SEGFAULT IF ESTIMATED WRONG
    }
};

KMeans_pp_Solver::KMeans_pp_Solver(list<Point*>& dataset, AssignmentStep __assignment_step, int K)
:KMeans_Solver(K),dataset(dataset), assignment_step(__assignment_step) {}

KMeans_pp_Solver::~KMeans_pp_Solver() {}

// method to clear up the centroids
void KMeans_pp_Solver::clear_centroids(void) {
    // clear the centroid structs vector
    // the list of datapoints has no reason to be cleaned 
    // since the pointers will be freed outside this scope

    this->centroids.clear();
    this->K = 0;
}

uint32_t KMeans_pp_Solver::get_size() const{
    return this->dataset.size();
}

// static double get_D_i(const vector<Point>& C, Point &point) {
//     double D_i = numeric_limits<double>::max();
//     for (auto c : C) {
//         if (c.getId() == point.getId()) return -1.0; // make sure that we don't count self points 
        
//         D_i = min(D_i, point.dist(c));
//     }

//     return D_i;
// }

// routine to choose new centroid based on partial sum probabilities
static Point* choose_new_centroid(const unordered_map<Point*, double>& D, const double D_max, const uint32_t n, const uint32_t t) {
    // create the partial sums, P(r) = sum(D(i)) for i = 1, ... r, where r = 1,
    // ..., n-t
    double P_r = 0.0;
    set<pair<Point*, double>, compare_sums> partial_sums;
    // partial sum P(0) = 0, no need to insert it
    
    for (auto D_i : D) {
        if (D_i.second == -1) continue;

        double p = D_i.second / D_max;
        P_r += p * p;  // estimate sum{ D(i)^2} for i = 1..r, normalized by
                       // dividing with D(i). Just update P(r-1) in P_r variable
        // insert with the proper r-th Point
        partial_sums.insert(make_pair(D_i.first, P_r));
    }

    // choose x uniformly in [0, P(n-t)]
    double x = get_random_num(P_r);

    // get the P(r) that is greater or equal to x
    auto possible_c = partial_sums.upper_bound(make_pair(nullptr, x));
    return possible_c->first;
}

// initialize centroids with kmeans++ method
void KMeans_pp_Solver::init_centroids(int K) {
    #ifdef VERBOSE
    profiler_start();
    cout << "Starting Kmeans++ init-clusters..." << endl;
    #endif
    // before anything start clear the centroid vector
    this->clear_centroids();
    this->K = K;

    // temp centroid list
    vector<Point> C;
    uint32_t n = this->dataset.size();

    // first choose a random centroid and add it in the centroid-list 
    uint32_t random_id = (uint32_t)get_random_num(this->dataset.size());
    auto data_it = this->dataset.begin();
    advance(data_it, random_id);
    
    C.push_back(*(*data_it));


    unordered_map<Point *, double> D;
    D.reserve(2*this->dataset.size());
    D[(*data_it)] = -1;

    // init the rest of the centroids
    for (int t = 1; t < K; t++) {
        // get the min distance of all non centroid points
        
        // for all datapoints estimate the mid dist between all known centroids, D(i), i = 1, ..., n-t
        double D_max = 0.0;
        for (auto datapoint: this->dataset) {  
            double prev_D_i = D.find(datapoint) != D.end() ? D[datapoint] : numeric_limits<double>::max();
            if (prev_D_i != -1){
                // update D(i)
                double D_i = min(datapoint->dist(C.back()), prev_D_i); // returns -1 only if datapoint is a centroid
                D[datapoint] = D_i;

                // update max D(i)
                D_max = max(D_max, D_i);
            }
        }

        // choose new centroid based on squared probabilities of partial sums
        Point *new_c = choose_new_centroid(D, D_max, n, t);
        // insert the new centroid in the list 
        C.push_back(*new_c);
        D[new_c] = -1;
    }

    // set up the centroids
    for (uint32_t i = 0; i < C.size(); i++ ) this->centroids.push_back(make_pair(C[i], Datapoints()));

    #ifdef VERBOSE
    profiler_stop();
    cout << "Done in " << profiler_get_duration() << "ms" << endl;
    #endif
}

// update centroids after an assginment step
void KMeans_pp_Solver::update_centroids_step(void) {
    // context -> the cluster points are newly assigned. The centroids of each cluster are not updated yet
    // we will estimate the median of all points
    
    for (uint32_t i = 0; i < this->centroids.size(); i++) {
        auto c = this->centroids[i];
        Point new_c(c.first.getId(), c.first.getDims(), this->dataset.front()->getDistMetric());
        Datapoints& cluster_points = c.second;
        
        if (cluster_points.size() == 0) continue;

        // calculate 1/T * sum(v) for v in cluster_points
        double T = 1.0 * cluster_points.size();

        // remember to unmark any points as you use them to calculate new centroid
        for (auto datapoint : cluster_points) {
            // use the point to recalibrate centroid
            new_c += *(datapoint.first);
        }
        new_c /= T;
        
        // at this point the new_c is equal to the new clusters centroid, c = 1/T * sum(v) for all v in current cluster
        // update centroid
        this->centroids[i].first.setCoordinates(new_c.getCoordinates());
    }
}

// perform a step of k means ++
uint32_t KMeans_pp_Solver::_k_means_step(void) {
    // first execute the assignment step
    uint32_t points_changed = this->assignment_step(this->centroids, this->dataset);
    // then update the centroids, (remember to unmark the datapoints for the next assignment step)
    this->update_centroids_step();
    return points_changed;
}

// Perform a clustering with K means:
// iterate at most iter_max times
// if less that min_changed_elements change cluster then break loop
std::vector<Centroid>* KMeans_pp_Solver::get_k_clusters(int iter_max, uint32_t min_changed_elements) {
    // initiate the centroids
    this->init_centroids(K);

    #ifdef VERBOSE
    cout << "Init Clusters:" << endl;
    for (int i = 0; i < K; i++) cout << this->centroids[i].first << endl;
    #endif
    int iter = 0;
    uint32_t points_changed = min_changed_elements;
    // loop for at most iter_max times
    while (iter++ < iter_max && min_changed_elements <= points_changed) {
        points_changed = this->_k_means_step();
    }

    // after iter_max iterations if any points are unassigned then assign them brute force style
    if (unassigned_points.size())
        for (auto p : unassigned_points) {
            insert_in_closest_center(p, centroids);
        }

    // create the new vector-list of clusters
    auto* clusters = new vector<Centroid>(this->centroids);

    return clusters;
}