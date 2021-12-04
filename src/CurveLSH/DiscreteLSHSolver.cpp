#include "DiscreteLSHSolver.hpp"

DiscreteLSHSolver::DiscreteLSHSolver(std::list<Curve *> &dataset,
                                     uint32_t curve_L, uint32_t curve_delta,
                                     uint32_t curve_d, string storage_type,
                                     uint32_t _L, uint32_t k, uint32_t dd,
                                     uint32_t M, uint32_t probes)
    : CurveNearestNeighboursSolver(dataset, curve_L, curve_delta, curve_d) {
    this->dataset_transformed.clear();

    // set up the proper ammount of Discrete lsh hash objs and vector solvers
    for (auto i = 0; i < curve_L; i++) {
        insert_in_grid_storage(dataset, storage_type, _L, k, dd, M, probes);
    }
}

DiscreteLSHSolver::~DiscreteLSHSolver() {
    for (auto solver : solvers) {
        delete solver;
    }

    for (auto trans_dataset : dataset_transformed)
        for (auto dp : trans_dataset) 
            delete dp;
}

// Find N nearest neighbours of q. Returns list of <Curve *, dist from q>.
std::list<CurveNeighbour> *DiscreteLSHSolver::kNearestNeighbours(const Curve &q,
                                                                 uint N) {}

// Find all neighbours in range R. Returns list of <Curve *, dist from q>.
std::list<CurveNeighbour> *DiscreteLSHSolver::nearestNeighbours_w_rangeSearch(
    const Curve &q, double R) {}

void DiscreteLSHSolver::insert_in_grid_storage(std::list<Curve *> &dataset,
                                               std::string storage_type,
                                               uint32_t _L, uint32_t k,
                                               uint32_t dd, uint32_t M,
                                               uint32_t probes) 
{
    // create a curve hashing mechanism
    this->grid_hashes.push_back(DLSHHashingCurve(1, 1, dataset.front()->dimensions(), this->_curve_delta));
    
    // transform all data
    transform_dataset(dataset, this->grid_hashes.back());

    // estimate w for given curve storage solver
    uint32_t w = LSHHashing::estimate_w(this->dataset_transformed.back());

    // create solver
    if (storage_type == "LSH")
        this->solvers.push_back(new LSHNearestNeighbours(
            this->dataset_transformed.back(), this->dataset_transformed.back().size()/8, k, w, _L)
            );
    else if (storage_type == "Hypercube")
        this->solvers.push_back(
            new HyperCube(w, this->dataset_transformed.back().back()->getDims(),
                          dd, probes, M, this->dataset_transformed.back()));
    else {
        cerr << "The standard convolution hashing storage solver is not implemented. Try 'LSH' or 'Hypercube'" << endl;
    }
}
