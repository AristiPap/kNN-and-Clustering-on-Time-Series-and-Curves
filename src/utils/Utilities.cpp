#include "Utilities.hpp"
#include <list>

using namespace std;

std::random_device rd;
std::mt19937 gen(rd());

// function to get a normal random vector
std::vector<double> get_random_vec(int dim) {
    normal_distribution<double> normal_dist(0.0, 1.0); // standard normal distribution
    vector<double> vec;  // random vector prototype

    for (int i = 0; i < dim; i++)
        vec.push_back(normal_dist(gen));  // sample from normal distribution and
                                      // push the results in the vector

    return vec;
}

uint32_t HammingDistance(uint32_t a, uint32_t b, int dd) {
    uint32_t mask = 0x1;
    uint32_t dist = 0;
    for (int i = 0; i < dd; i++) {
        dist += ((a & mask) != (b & mask) ? 1 : 0);
        a >>= 1;
        b >>= 1;
    }
    
    return  dist;
}

float get_random_num(int max) {
    // get random int from uniform distribution
    std::uniform_real_distribution<> uniform_dist(0.0, (float)max);
    float t = uniform_dist(gen);
    return (float)t;
}

int32_t get_random_num(int32_t min, int32_t max) {
    // get random int from uniform distribution
    std::uniform_int_distribution<int32_t> uniform_dist(min, max);
    int32_t t = uniform_dist(gen);
    return t;
}

bool my_less::operator()(const pair<Point *, double>& l, const pair<Point *, double>& r) {
    return l.second - r.second < 0   ? true
        : l.second - r.second > 0 ? false
            : l.first->getId().size() - r.first->getId().size() < 0;
}


bool curve_compare::operator()(const pair<Curve *, double>& l, const pair<Curve *, double>& r) {
    return l.second - r.second < 0   ? true
        : l.second - r.second > 0 ? false
            : l.first->getId().size() - r.first->getId().size() < 0;
}


Curve *getMeanCurve(list<pair<const Point *, const Point *>> optimal_traversal){

    Curve *c = new Curve();
    for(auto it:optimal_traversal){
        Point p = *it.first + *it.second;
        p /= 2;
        c->AddToCurve(&p);
    }
    return c;
}

Curve *getMeanCurve(Curve *c1, Curve *c2){

    Curve *c = nullptr;
    Frechet::backtrace = true; // set backtracing to true
    // find frechet dist between 2 curves
    c1->dist(*c2);
    
    // get mean curve
    c = getMeanCurve(Frechet::optimal_traversal);
    
    // turn backtracing off
    Frechet::backtrace = false;
    
    return c;
}

Curve *getMeanCurve(vector<Curve *> CurveTree){

    Curve *c = nullptr;
    int step = 1;
    
    while(step <= CurveTree.size()- 1){
        // get first element
        int counter = 0;
        for(auto it1=CurveTree.begin(); it1!=CurveTree.end(); it1 +=step+1){
            auto it2 = it1 + step;
            c = getMeanCurve(*it1,*it2);
            CurveTree[counter] = c;
            if(step > 1)
                delete CurveTree[counter + step];
        
            CurveTree.at(counter + step) = NULL;
            counter += step + 1;
        }
        step *= 2;
    }
    
    return CurveTree.front();
}


// init radius is min(dist between centroids)/2
template<class CentroidT>
static double init_R(vector<CentroidT> &centroids) {
    double min_dist = numeric_limits<double>::max();

    for (uint32_t i = 0; i < centroids.size(); i++)
        for (uint32_t j = i + 1; j < centroids.size(); j++)
            min_dist = min(min_dist, centroids[i].first.dist(centroids[j].first));
    
    return min_dist/2.0;
}

template<class T, class CentroidT>
static void insert_into_cluster(T *p, double dist, int centroid_id, vector<CentroidT> &centroids, uint32_t &points_changed, set<T *>& unassigned_points) {
    assert(centroid_id < centroids.size());

    // get p's previous and under-check cluster-centroid
    CentroidT *old_c = p->getCluster() >= 0 ? &(centroids[p->getCluster()]) : nullptr;
    CentroidT *new_c = &(centroids[centroid_id]);

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
template<class T, class CentroidT, class SolverT>
uint32_t __reverse_assignment__(vector<CentroidT> &centroids, list<T *> &dataset, SolverT &solver, double R_max, set<T *>& unassigned_points) {
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
        list<pair<T *, double>> *in_range = solver.nearestNeighbours_w_rangeSearch(centroids[i].first, R);
            
        // insert all the neighbours in the centroid
        for (auto neighbour_pair : *in_range) {
            T *p = neighbour_pair.first;
            double dist = neighbour_pair.second;
            // insert the point into c. If p was in a different centroid before change the changes counter accordingly
            insert_into_cluster(p, dist, i, centroids, points_changes, unassigned_points);
        }

        delete in_range; // free the list, but jsut the list not the inner pointers
    }
        
    // at the end of the loop double the search radius
    R *= 2.0;
    if (R - R_max > 0) {
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
