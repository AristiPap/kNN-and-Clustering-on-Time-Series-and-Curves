#include <set>

#include "Evaluator.hpp"
#include "FileHandler.hpp"
#include "Profiler.hpp"

using namespace std;

static double true_approx_time_ratio_sum = 0.0;
static double acc_sum = 0.0;
static double approx_true_dist_ratio_sum = 0.0;
static double approx_true_dist_ratio_max = 0.0;

Evaluator::Evaluator() {}

Evaluator::~Evaluator() {}

// evaluate algorithm performance from queries of a given file
void Evaluator::evaluate_from_file(const DataList& dataset, string method_name, NearestNeighboursSolver& solver, string query_file, string out_file, const uint32_t N, const double R) {
    // try to open query file
    FileHandler file_handler(dataset.front()->getDistMetric());
    if (file_handler.OpenFile(query_file) != 0) {
        cerr << "Could not open: '" <<  query_file << "'";
        return;
    }    
    // get the query points
    DataList *queries = file_handler.create_dbPoints();

    // try to open the file 
    ofstream out_file_stream;
    out_file_stream.open(out_file, ios::out);

    if (!out_file_stream) {cerr << "Problem opening '" << out_file << endl; return;}

    // evaluate all queries 
    for (Point *q: *queries) {
        // evaluate each query point separetely
        this->evaluate(dataset, *q, method_name, solver, out_file_stream, N, R);
    }
    file_handler.CloseFile();
    file_handler.cleardb();

    #ifdef VERBOSE 
    out_file_stream << "\nTotal Stats: " << endl;
    out_file_stream << "Average per point accuracy: " << acc_sum/queries->size() << endl;
    out_file_stream << "Average brute_KNN/approx_KNN time ratio: " << true_approx_time_ratio_sum/queries->size() << endl;
    out_file_stream << "Average approx_KNN dist / true_KNN dist error: " << approx_true_dist_ratio_sum/queries->size() - 1 << endl;
    out_file_stream << "Max approx_KNN dist / true_KNN dist: " << approx_true_dist_ratio_max << endl; 
    #endif
}

void Evaluator::evaluate(const DataList& dataset, const Point& q, string method_name, NearestNeighboursSolver& solver, ofstream &out_file, const uint32_t N, const double R) {
    // get approximate nearest neighbours
    profiler_init();
    profiler_start();
    
    Neighbours *res = solver.kNearestNeighbours(q, N);
    
    profiler_stop();
    double knn_dur = profiler_get_duration();
      
    // get in range neighbours
    Neighbours *in_range = solver.nearestNeighbours_w_rangeSearch(q, R);
    
   
    // get true nearest neighbours
    profiler_init();
    profiler_start();
    
    Neighbours *_true = this->nearestNeighboursBruteForce(dataset, q, N);

    profiler_stop();
    double real_dur = profiler_get_duration();

    FileHandler::print_to_file(out_file, q, method_name, res, in_range, _true, N, knn_dur, real_dur);

    #ifdef VERBOSE
    acc_sum += this->get_accuracy(*_true, *res);
    true_approx_time_ratio_sum += real_dur / knn_dur;
    this->dist_metrics(*_true, *res);
    #endif

    delete in_range;
    delete res;
    delete _true;

}

Neighbours* Evaluator::nearestNeighboursBruteForce(const DataList &dataset, const Point& p, int k) {
    set<Neighbour, my_less> EuclidSorted;
    Neighbours* closest = new list<Neighbour>();

    for (auto it1 : dataset) {
        double dist = p.dist(*it1);
        EuclidSorted.insert(make_pair(it1, dist));
    }

    int counter = 0;
    for (auto it2 : EuclidSorted) {
        closest->push_back(make_pair(it2.first, it2.second));
        counter++;
        if (counter == k) break;
    }

    return closest;
}

// estimate accuracy of algorithm based on true results (ground truth)
double Evaluator::get_accuracy(Neighbours &__true, Neighbours &_res) {
    // estimate how many datapoints did the algorithm lost. 
    // Count how many points of _true exist in _res
    set<Neighbour> _true;
    for (auto p: __true) _true.insert(make_pair(p.first, p.second));

    uint32_t n = _true.size();
    u_int32_t found = 0;
    for (auto p : _res) {
        found += (_true.find(p) != _true.end()); // if we found p, then we add one in found counter
    }

    // return accuracy of the algorithm (properly cast)
    return (double)( ((double)found) / (double)n );
}

void Evaluator::dist_metrics(Neighbours &__true, Neighbours &_res) {
    auto it_true = __true.begin();
    auto it_res = _res.begin();

    double appr_true_dist_sum = 0.0;
    int n = 0;
    while (it_true != __true.end() && it_res != _res.end()) {
        n++;
        appr_true_dist_sum += it_res->second / it_true->second;
        approx_true_dist_ratio_max = max(approx_true_dist_ratio_max, it_res->second / it_true->second);    
        it_true++;
        it_res++;
    }
    if (n) approx_true_dist_ratio_sum += appr_true_dist_sum / n;
}

#ifndef NN
// CLustering evaluator

// evaluate algorithm performance from queries of a given file
void Evaluator::evaluate_from_file(const DataList& dataset, std::string method_name, KMeans_pp_Solver& solver, std::string out_file, bool complete) {
    profiler_init();
    profiler_start();
    cout << "Creating Clusters..." << endl;
    auto clusters = solver.get_k_clusters(ITER_MAX, MIN_UPDATES);
    cout << "Done." << endl;
    profiler_stop();
    double dt = profiler_get_duration() / 1000.0;

    #ifdef VERBOSE
    profiler_start();
    cout << "Evaluating Clustering..." << endl;
    #endif
    
    auto shillouette_eval = evaluate_w_silhouette(*clusters, dataset);
    #ifdef VERBOSE
    profiler_stop();
    cout << "Done in " << profiler_get_duration() << endl;
    #endif
    
    FileHandler f(dataset.front()->getDistMetric());
    ofstream f_out(out_file);
    f.print_to_file(f_out, clusters->size(), dt, method_name, *clusters, shillouette_eval, complete);

    delete clusters;
    delete shillouette_eval;
}

#endif