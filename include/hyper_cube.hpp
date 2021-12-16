#pragma once
#include <random>
#include <vector>
#include <set>
#include <unordered_map>
#include <list>
#include <bitset>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>

#include "Hashing.hpp"
#include "Utilities.hpp"
#include "NearestNeighboursSolver.hpp"

using namespace std;


typedef unordered_map<uint32_t, Bucket>::iterator mapIt;

class HyperCube : public NearestNeighboursSolver{
    private:
        HyperCubeHashing h;
        std::list<Point *> * dataset;
        //vector<Point*> dataset;
        unordered_map<uint32_t, Bucket> cube; // The hashtable
        uint32_t dd;
        uint32_t w;
        uint32_t d;
        uint32_t num_fs;  // Number of f_i's   
        uint32_t probes; // Max number of near vertices examined
        uint32_t num_points; // Max number of points examined
        uint32_t hd_max;

        // find the near vertices based on h_distance
        set<uint32_t> nearVertices(uint32_t num, uint32_t hashkey, int length, int hd, std::set<uint32_t>& visited);

        void insert(Point *p);
        // use this function as a nearest neighbours solver interface for the inner class methods
        std::list<Neighbour>* nearestNeighbours(const Point &p, uint32_t k, double min_dist=std::numeric_limits<double>::max(), bool is_range_search=false);
    public:
        HyperCube(uint32_t w, uint32_t d, uint32_t dd, uint32_t probes, uint32_t M, std::list<Point *> &input_data);
        virtual ~HyperCube();


    // Find the nearest neighbour of point q. Returns a pair of <point id, distance from q>.
    Neighbour nearestNeighbour(const Point &q) final;
    // Find N nearest neighbours of q. Returns list of <point id, dist from q>.
    std::list<Neighbour> *kNearestNeighbours(const Point &q, uint N=1) final;
    // Find all enighbours in range R. Returns list of <point id, dist from q>.
    std::list<Neighbour> *nearestNeighbours_w_rangeSearch(const Point &q, double R=10000.0) final;
};