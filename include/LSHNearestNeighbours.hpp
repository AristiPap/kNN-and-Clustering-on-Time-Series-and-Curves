#pragma once

#include "Hashing.hpp"
#include "Utilities.hpp"
#include "Point.hpp"
#include "NearestNeighboursSolver.hpp"

typedef std::unordered_map<uint32_t, HashBucket> HT;  // hashtables are unordered_maps of buckets


class LSHNearestNeighbours: public NearestNeighboursSolver {
   private:
    vector<HT> hts;
    vector<LSHHashing> gs;
    uint32_t table_size;
    int L;
    
    // Function to initialize the structures and parse input points list
    void init_with_input(std::list<Point *> &input_data);
    
    // Function to interpret a step in lsh kNN function.
    // Works for both kNN and  range search.
    // Updates basic variables and conditionals for the main algorithm
    void __lsh_nn_step__(const Point &q, Point *p, uint N, set<Neighbour, my_less> &neighbours, double &dist_b, double R, bool is_range_search);
    
    // Main kNN, range search algorithm
    // Implements the algorithm's logic and provide the interface with a generic implementation
    // of all 3 algorithms.
    std::list<Neighbour> *__k_nearest_neighbours(const Point &q, uint N, double R = 1.0, bool is_range_search = false);

   public:
    LSHNearestNeighbours(std::list<Point *>& input_data, size_t table_size, uint32_t k=4, uint32_t w=6, uint32_t L=10);
    ~LSHNearestNeighbours();

    // Find the nearest neighbour of point q. Returns a pair of <point id, distance from q>.
    Neighbour nearestNeighbour(const Point &q) final;
    // Find N nearest neighbours of q. Returns list of <point id, dist from q>.
    std::list<Neighbour> *kNearestNeighbours(const Point &q, uint N=1) final;
    // Find all enighbours in range R. Returns list of <point id, dist from q>.
    std::list<Neighbour> *nearestNeighbours_w_rangeSearch(const Point &q, double R=10000.0) final;
};