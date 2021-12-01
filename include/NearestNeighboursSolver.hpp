#pragma once

#include <bitset>
#include <cmath>
#include <ctime>
#include <fstream>
#include <limits>
#include <list>
#include <random>
#include <set>
#include <unordered_map>
#include <vector>

#include "Point.hpp"

typedef std::pair<uint32_t, Point *> Data;  // <g(p)%M, point class>
typedef std::unordered_map<uint32_t, std::list<Point *>> HashBucket; // bucket that hashed points based on g(p)=ID(p)
typedef std::list<Data> Bucket; // bucket/vetrices that contain datapoints along with their LSH IDs
typedef std::pair<Point *, double> Neighbour;  // Neighbour -> pointer to point object, distance from q_point

// basic interface for Nearest Neighbour Solvers
class NearestNeighboursSolver
{
private:
    // data variables
public:
    NearestNeighboursSolver();
    ~NearestNeighboursSolver();
    // Find the nearest neighbour of point q. Returns a pair of <point id, distance from q>.
    virtual Neighbour nearestNeighbour(const Point &q) = 0;
    // Find N nearest neighbours of q. Returns list of <point id, dist from q>.
    virtual std::list<Neighbour> *kNearestNeighbours(const Point &q, uint N=1) = 0;
    // Find all enighbours in range R. Returns list of <point id, dist from q>.
    virtual std::list<Neighbour> *nearestNeighbours_w_rangeSearch(const Point &q, double R=10000.0) = 0;

};
