#include "ClusterSolverCurves.hpp"
#include "Profiler.hpp"
#include <list>
#include <set>

using namespace std;


// define a self-compare
struct compare_sums {
    bool operator()(const pair<Curve*, double>& lhs, const pair<Curve*, double>& rhs){
        return lhs.second - rhs.second < 0   ? true
               : lhs.second - rhs.second > 0 ? false
                                         : (lhs.first==nullptr || rhs.first == nullptr || strcmp(lhs.first->getId().c_str(),rhs.first->getId().c_str()) < 0); //TODO: WARNING: POSSIBLE SEGFAULT IF ESTIMATED WRONG
    }
};


KMeans_pp_Solver_Curves::KMeans_pp_Solver_Curves(list<Curve*>& dataset, CurveAssignmentStep __assignment_step, int K)
:KMeans_Solver(K),dataset(dataset), assignment_step(__assignment_step){}

KMeans_pp_Solver_Curves::~KMeans_pp_Solver_Curves() {}

// method to clear up the centroids
void KMeans_pp_Solver_Curves::clear_centroids(void) {
    // clear the centroid structs vector
    // the list of datapoints has no reason to be cleaned 
    // since the pointers will be freed outside this scope

    this->centroids.clear();
    this->K = 0;
}

uint32_t KMeans_pp_Solver_Curves::get_size() const{
    return this->dataset.size();
}

// routine to choose new centroid based on partial sum probabilities
static Curve* choose_new_centroid(const unordered_map<Curve*, double>& D, const double D_max, const uint32_t n, const uint32_t t) {
    // create the partial sums, P(r) = sum(D(i)) for i = 1, ... r, where r = 1,
    // ..., n-t
    double P_r = 0.0;
    set<pair<Curve*, double>, compare_sums> partial_sums;
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
void KMeans_pp_Solver_Curves::init_centroids(int K) {
    #ifdef VERBOSE
    profiler_start();
    cout << "Starting Kmeans++ init-clusters..." << endl;
    #endif
    // before anything start clear the centroid vector
    this->clear_centroids();
    this->K = K;

    // temp centroid list
    vector<Curve> C;
    uint32_t n = this->dataset.size();

    // first choose a random centroid and add it in the centroid-list 
    uint32_t random_id = (uint32_t)get_random_num(this->dataset.size());
    auto data_it = this->dataset.begin();
    advance(data_it, random_id);
    
    C.push_back(*(*data_it));


    unordered_map<Curve *, double> D;
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
        Curve *new_c = choose_new_centroid(D, D_max, n, t);
        // insert the new centroid in the list 
        C.push_back(*new_c);
        D[new_c] = -1;
    }

    // set up the centroids
    for (uint32_t i = 0; i < C.size(); i++ ) this->centroids.push_back(make_pair(C[i], CurveDatapoints()));

    #ifdef VERBOSE
    profiler_stop();
    cout << "Done in " << profiler_get_duration() << "ms" << endl;
    #endif
}

// update centroids after an assginment step
void KMeans_pp_Solver_Curves::update_centroids_step(void) {
    // context -> the cluster points are newly assigned. The centroids of each cluster are not updated yet
    // we will estimate the median of all points
    
    for (uint32_t i = 0; i < this->centroids.size(); i++) {
        auto c = this->centroids[i];
        CurveDatapoints& cluster_points = c.second;
        
        if (cluster_points.size() == 0) continue;

        // remember to unmark any points as you use them to calculate new centroid
        vector<Curve> Curvetree;
        for(auto it:cluster_points){
            Curvetree.push_back(*it.first);
        }
        Curve new_c = getMeanCurve(Curvetree, dataset.front()->complexity()*2 - 1);
        #ifdef DEBUG
        cout << "Mean curve '" << this->centroids[i].first.getId() << "' " <<  this->centroids[i].first.complexity();
        #endif

        this->centroids[i].first.setPoints(&new_c);
        
        #ifdef DEBUG
        cout << " -> " << this->centroids[i].first.complexity() << endl;
        #endif
    }
}

// perform a step of k means ++
uint32_t KMeans_pp_Solver_Curves::_k_means_step(void) {
    // first execute the assignment step
    uint32_t points_changed = this->assignment_step(this->centroids, this->dataset);
    // then update the centroids, (remember to unmark the datapoints for the next assignment step)
    this->update_centroids_step();
    return points_changed;
}

// Perform a clustering with K means:
// iterate at most iter_max times
// if less that min_changed_elements change cluster then break loop
std::vector<CurveCentroid>* KMeans_pp_Solver_Curves::get_k_clusters(int iter_max, uint32_t min_changed_elements) {
    // initiate the centroids
    this->init_centroids(K);

    int iter = 0;
    uint32_t points_changed = min_changed_elements;
    // loop for at most iter_max times
    while (iter++ < iter_max && min_changed_elements <= points_changed) {
        points_changed = this->_k_means_step();
    }

    // after iter_max iterations if any points are unassigned then assign them brute force style
    if (unassigned_curves.size())
        for (auto p : unassigned_curves) {
            insert_in_closest_center(p, centroids);
        }

    // create the new vector-list of clusters
    auto* clusters = new vector<CurveCentroid>(this->centroids);

    return clusters;
}