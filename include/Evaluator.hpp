#pragma once
#include <list>


#include "ClusterSolverCurves.hpp"
#include "ClusterSolver.hpp"

#include "NearestNeighboursSolver.hpp"
#include "CurveNearestNeighbours.hpp"

typedef std::list<Point*> DataList;
typedef std::list<Curve*> DataListCurve;
typedef std::list<Neighbour> Neighbours;  // list of neighbours

class Evaluator {
   private:
    /* data */
    void print_to_fstream(ofstream stream, string msg);
    uint32_t k, w, L, probes, M;
    double R;

    // use brute force O(n*logn) algorithm to calculate nearest neighbours
    Neighbours* nearestNeighboursBruteForce(const DataList &dataset, const Point& p, int k);
    list<CurveNeighbour>* nearestNeighboursBruteForce(const list<Curve *> &dataset, const Curve& c, int k);


   public:
    Evaluator();
    ~Evaluator();

    // evaluate algorithm performance from queries of a given file
    void evaluate_from_file(
        const DataList& dataset,         // dataset
        std::string method_name,
        NearestNeighboursSolver& solver, // NN solver
        std::string query_file,          // file with query points
        std::string out_file = "stdout", // default fout is stdout
        const uint32_t N = 1,            // number of neighbours to find
        const double R = -1.0,           // default is no range search, change to R>=0 to range search
        double f_sample = 1.0
    );

    void evaluate(
        const DataList& dataset,         // dataset
        const Point& q,                  // query point
        std::string method_name,
        NearestNeighboursSolver& solver, // solver (LSH, HP)
        ofstream &out_file,               // name of the out_file, default stdout
        const uint32_t N = 1,            // number of neighbours to find
        const double R = -1.0           // R for range search. If R>= 0 then we activate range_search
    );

    // evaluate algorithm performance from queries of a given file
    //void evaluate_from_file(const DataList& dataset, std::string method_name, KMeans_pp_Solver_Curves& solver, std::string out_file, bool complete);
    void evaluate_from_file(const DataListCurve& dataset, std::string method_name, KMeans_pp_Solver_Curves& solver, std::string out_file, bool complete);

    // add-ons for curve evaluation
    void evaluate_from_file(
        const list<Curve *>& dataset,   // the training dataset
        const list<Curve *>& queries,   // the query dataset
        std::string method_name,        // the method name: [Vector-like | Discrete/Continuous LSH] w\ LSH/Hypercube storage
        CurveNearestNeighboursSolver& solver,
        std::string out_file,           // name of output file
        const uint32_t N=1              // number of top nearest neighbours to search for
    );

    // test a specific curve
    void evaluate(const list<Curve*>& dataset, Curve &q, std::string method_name, CurveNearestNeighboursSolver& solver, ofstream& out_file_stream, uint32_t N);
};

template <class T>
static void dist_metrics(list<pair<T, double>>& __true, list<pair<T, double>>& _res);

// evaluate accuracy of NN-RS results
template <class T>
double get_accuracy(list<pair<T, double>>& __true, list<pair<T, double>>& _res);