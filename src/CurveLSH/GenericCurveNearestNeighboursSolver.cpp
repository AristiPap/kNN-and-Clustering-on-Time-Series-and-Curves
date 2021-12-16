#include "GenericLSHSolver.hpp"
#include <set>

LSHSolver::LSHSolver(std::list<Curve *> &dataset, uint32_t curve_L,
                     double curve_delta, uint32_t curve_d, int flag,
                     string storage_type, uint32_t _L, uint32_t k, uint32_t dd,
                     uint32_t M, uint32_t probes, int32_t maxCurveLength)
    : CurveNearestNeighboursSolver(dataset, curve_L, curve_delta, curve_d) {
    this->dataset_transformed.clear();
    cout << "delta: " << _curve_delta << endl;
    // set up the proper ammount of Discrete lsh hash objs and vector solvers
    for (auto i = 0; i < curve_L; i++) {
        insert_in_grid_storage(dataset, storage_type, _L, k, dd, M, probes, flag, maxCurveLength);
    }
}

LSHSolver::~LSHSolver() {
    for (auto solver : solvers) {
        delete solver;
    }

    for (auto trans_dataset : dataset_transformed)
        for (auto dp : trans_dataset) 
            delete dp;
}

// Find N nearest neighbours of q. Returns list of <Curve *, dist from q>.
std::list<CurveNeighbour> *LSHSolver::kNearestNeighbours(Curve &q, uint N) {
    // we will search all L hash tables
    // save the resulted curves into a neighbours set and return top k

    set<pair<Curve *, double>, curve_compare> neighbours;

    for (auto i = 0; i < this->_curve_L; i++) {
        HashingCurve &h = *this->grid_hashes[i];
        NearestNeighboursSolver *solver = this->solvers[i];

        // get the snapped curve in concatenated form
        Point *x = h(q);
        // get the k nearest neighbours
        list<Neighbour> *cur_neighbours = solver->kNearestNeighbours(*x, N);
    
        for (auto n_i = cur_neighbours->begin(); n_i != cur_neighbours->end(); n_i++) {
            neighbours.insert(make_pair(n_i->first->getCurve(), n_i->second));
        }

        delete cur_neighbours;
    }

    list<CurveNeighbour> *closest = new list<CurveNeighbour>();
    int k_counter = 1;
    for (auto it_2 : neighbours) {
        if (k_counter > N) break;
        closest->push_back(make_pair(it_2.first, it_2.second));
        k_counter++;
    }

    return closest;
}

std::list<CurveNeighbour> *LSHSolver::nearestNeighbours_w_rangeSearch(Curve &q, double R) {
    // we will search all L hash tables
    // save the resulted curves into a neighbours set and return top k

    list<CurveNeighbour> *neighbours = new list<CurveNeighbour>();

    for (auto i = 0; i < this->_curve_L; i++) {
        HashingCurve &h = *this->grid_hashes[i];
        NearestNeighboursSolver *solver = this->solvers[i];

        // get the snapped curve in concatenated form
        Point *x = h(q);
        // get the k nearest neighbours
        list<Neighbour> *cur_neighbours = solver->nearestNeighbours_w_rangeSearch(*x, R);

        for (auto n_i = cur_neighbours->begin(); n_i != cur_neighbours->end();
             n_i++) {
            neighbours->push_back(make_pair(n_i->first->getCurve(), n_i->second));
        }

        delete cur_neighbours;
    }


    return neighbours;
}

void LSHSolver::insert_in_grid_storage(std::list<Curve *> &dataset,
                                       std::string storage_type, uint32_t _L,
                                       uint32_t k, uint32_t dd, uint32_t M,
                                       uint32_t probes, int flag,
                                       int32_t maxCurveLength) {

    // set up max_curve length based on user preferences
    int32_t max_curve_len = maxCurveLength == -1 ? dataset.front()->complexity() : maxCurveLength;

    // create a curve hashing mechanism
    if(flag == 0)
        this->grid_hashes.push_back(new DLSHHashingCurve(1, 1, dataset.front()->dimensions(), this->_curve_delta, max_curve_len));

    //Continuous LSH hashing Curves
    else if(flag == 1)
        this->grid_hashes.push_back(new CLSHHashingCurve(1, 1, dataset.front()->dimensions(), this->_curve_delta, max_curve_len));
    
    // transform all data
    transform_dataset(dataset, *this->grid_hashes.back());

    // estimate w for given curve storage solver
    uint32_t w = LSHHashing::estimate_w(this->dataset_transformed.back());

    // create solver and initialize given the snapped curves for the specific grid hash
    if (storage_type == "LSH")
        this->solvers.push_back(new LSHNearestNeighbours(
            this->dataset_transformed.back(), max(this->dataset_transformed.back().size()/8, (unsigned long)1), k, w, _L)
            );
    else if (storage_type == "Hypercube")
        this->solvers.push_back(
            new HyperCube(w, this->dataset_transformed.back().back()->getDims(),
                          dd, probes, M, this->dataset_transformed.back()));
    else {
        cerr << "The standard convolution hashing storage solver is not implemented. Try 'LSH' or 'Hypercube'" << endl;
    }
}

void LSHSolver::transform_dataset(list<Curve*>& dataset, HashingCurve& h) {
    // add a new transformed dataset list 
    this->dataset_transformed.push_back(list<Point *>());
    list<Point *>& dt = this->dataset_transformed.back();
    
    assert(dt.size() == 0); // sanity check
    
    for (auto dp_i = dataset.begin(); dp_i != dataset.end(); dp_i++) {
        // hash the curve
        Point * concated_grid_curve = h(**dp_i);
        // add the point/concated snapped curve into the transformed dataset
        dt.push_back(concated_grid_curve); 
    }

}