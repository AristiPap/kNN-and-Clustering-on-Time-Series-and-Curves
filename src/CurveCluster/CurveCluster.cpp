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
                                         : (lhs.first==nullptr || rhs.first == nullptr || strcmp(lhs.first->getId().c_str(),rhs.first->getId().c_str())); //TODO: WARNING: POSSIBLE SEGFAULT IF ESTIMATED WRONG
    }
};


KMeans_pp_Solver_Curves::KMeans_pp_Solver_Curves(list<Curve*>& dataset, AssignmentStep __assignment_step,UpdateStep __update_step, int K)
:KMeans_Solver(K),dataset(dataset), assignment_step(__assignment_step),update_step(__update_step) {}

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

// static double get_D_i(const vector<Point>& C, Point &point) {
//     double D_i = numeric_limits<double>::max();
//     for (auto c : C) {
//         if (c.getId() == point.getId()) return -1.0; // make sure that we don't count self points 
        
//         D_i = min(D_i, point.dist(c));
//     }

//     return D_i;
// }

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
        Curve * new_c = nullptr;
        CurveDatapoints& cluster_points = c.second;
        
        if (cluster_points.size() == 0) continue;

        // remember to unmark any points as you use them to calculate new centroid
        vector<Curve *> Curvetree;
        for(auto it:cluster_points){
            Curvetree.push_back(it.first);
        }
        new_c = getMeanCurve(Curvetree);
        
        this->centroids[i].first.setPoints(new_c);
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

void insert_in_closest_center(Curve *q, vector<CurveCentroid> &centroids) {
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

// Perform a clustering with K means:
// iterate at most iter_max times
// if less that min_changed_elements change cluster then break loop
std::vector<CurveCentroid>* KMeans_pp_Solver_Curves::get_k_clusters(int iter_max, uint32_t min_changed_elements) {
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
    if (unassigned_curves.size())
        for (auto p : unassigned_curves) {
            insert_in_closest_center(p, centroids);
        }

    // create the new vector-list of clusters
    auto* clusters = new vector<CurveCentroid>(this->centroids);

    return clusters;
}

// helper method used to find the closest cluster center to a data point 
ClosestCentroid find_closest_centroid(Curve& q, vector<CurveCentroid> &centroids)
{
  // find the distance to first centroid 
  int closest_center = 0;
  double min_distance = q.dist(centroids[0].first);
  int K = centroids.size();

  // compare with the rest cluster centers
  for (int i = 1; i < K; i++)
  {
    double distance = q.dist(centroids[i].first);
    if (distance < min_distance)
    {
      closest_center = i;
      min_distance = distance;
    }
  }

  return make_pair(closest_center,min_distance);
}


ClosestCentroid find_second_closest_centroid(Curve& q, vector<CurveCentroid> &centroids){
  
  int closest_center = -1, second_closest_center= -1;
  double min_dist = -1, sec_min_dist = -1;
  int K = centroids.size();
  
  // check which of the first 2 clusters are closer
  if (q.dist(centroids[0].first) < q.dist(centroids[1].first)){
    second_closest_center = 1;
    closest_center = 0;
    sec_min_dist = q.dist(centroids[1].first);
    min_dist = q.dist(centroids[0].first);
  }
  else{
    second_closest_center = 0;
    closest_center = 1;
    sec_min_dist = q.dist(centroids[0].first);
    min_dist = q.dist(centroids[1].first);
  }

  // compare with the rest of the centroids 
  for (int c = 2; c < K; c++)
  {
    // compute the distance to the current center
    double dist = q.dist(centroids[c].first);

    // check if the new center is closest or second closest 
    if (dist <= min_dist ){
      second_closest_center = closest_center;
      closest_center = c;
      sec_min_dist = min_dist;
      min_dist = dist;
    }
    else if (dist <= sec_min_dist){
      second_closest_center = c;
      sec_min_dist = dist;
    }
  }

  /* return the second closest center */
  return make_pair(second_closest_center,sec_min_dist);
}

uint32_t Lloyd (std::vector<CurveCentroid> &centroids, std::list<Curve *> &dataset){
	
    uint32_t points_updated = 0;
    // loop through data points to find the closest cluster center 
    for (auto it : dataset) {
        // find the closest centroid for training example i 
        ClosestCentroid closest_center = find_closest_centroid(*it, centroids);
        
        if (it->getCluster() == -1){
          // add data point to the cluster 
            centroids[closest_center.first].second[it] = closest_center.second;
          // mark it 
            it->setMarked(true);
            it->setCluster(closest_center.first);
            points_updated++; 
        }
        //if its assigned to a centroid then we are at the recalibration stage of the clusters
        else{
            //update distance of point to centroid
            if(it->getCluster() == closest_center.first)
                centroids[closest_center.first].second[it] = closest_center.second;
            
            else{
                //add point to new centroid
                centroids[closest_center.first].second[it] = closest_center.second;
                //keep statistics
                points_updated ++;
                
                //delete point from old centroid
                centroids[it->getCluster()].first.setMarked(false);
                centroids[it->getCluster()].second.erase(it);

                //update reverse mapping
                it->setMarked(true);
                it->setCluster(closest_center.first);
            }
        }    
    }

    cout << "points_updated: " << points_updated << endl;
    return points_updated;
}


vector<double> * evaluate_w_silhouette_per_centroid(std::vector<CurveCentroid> &centroids, const std::list<Point *> &dataset){
    
    uint32_t _size = dataset.size();
    int K = centroids.size();
    //result array that contains the silhouette function results
    vector<double> * silhouette_dist = new vector<double> ();
    
    double dist;
    silhouette_dist->resize(K, 0);
    
    // Following loops evaluate:
    // 1) second closest centroid of each point
    // 2) average distance between points of the same cluster
    // 3) calculate the average distance of the clusters point from the points of the second closest cluster
    
    for (int i = 0; i < K; i++){
        uint32_t points = 0;
        for(auto it2 : centroids[i].second){
            //get points, second closest centroid
            ClosestCentroid sec_closest_center = find_second_closest_centroid(*it2.first, centroids);
            it2.first->setSecCluster(sec_closest_center.first);
            // calcucate a
            dist = 0.0;
            for (auto it3 : centroids[i].second)
                dist += (*it2.first).dist(*it3.first);
            
            double a = dist/(1.0*centroids[i].second.size() - 1.0 );
            
            // calculate b
            dist = 0.0;
            // number of points in second closest cluster
            points = centroids[it2.first->getSecCluster()].second.size();
            for (auto it3 : centroids[it2.first->getSecCluster()].second)
                dist += (*it2.first).dist(*it3.first);

            double b = dist / (1.0 * points);

            if ((double)std::max(a,b) == 0)
                continue;
            double avg_pp = (b - a) / ((double)std::max(a,b));
            //for each points calculated silhouette distance, save also the cluster id 
            (*silhouette_dist)[i] += avg_pp; 
        }
        (*silhouette_dist)[i] = (*silhouette_dist)[i] / (centroids[i].second.size() * 1.0);
    }
    
    return silhouette_dist;
}

vector<double> *  evaluate_w_silhouette(std::vector<CurveCentroid>& clusters, const std::list<Point *> &dataset){
    
    vector<double> *silhouette_dist = evaluate_w_silhouette_per_centroid(clusters,dataset);
    int K = clusters.size();
    
    //compute the sum of all the silhouettes in order to later compute the average 
    double sum = 0.0;
    for (int i = 0; i < K; i++)
        sum += (*silhouette_dist)[i];
        
    // compute average silhouette
    double average_silhouette = ((double) sum) / (1.0*silhouette_dist->size());
    
    silhouette_dist->push_back(average_silhouette);
    
    return silhouette_dist;
}

ostream& operator<<(ostream& os, vector<double> silhouettes) {
    os << "[";
    uint32_t dims = silhouettes.size();
    
    for (uint32_t i = 0; i < dims; i++)
        os << silhouettes[i] << (i < dims-1 ? ", " : "") ;
    os << "]" << endl;
    return os;
}