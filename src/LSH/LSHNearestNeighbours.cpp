#include "LSHNearestNeighbours.hpp"
#include "Profiler.hpp"

using namespace std;

LSHNearestNeighbours::LSHNearestNeighbours(std::list<Point *> &input_data, size_t table_size, uint32_t k, uint32_t w, uint32_t L)
: table_size(table_size), L(L)
{
    // set the number of hash tables
    this->hts.resize(L);
    int32_t dim = input_data.front()->getDims();

    // set the # buckets  in each one of the hashtables
    for (int i = 0; i < L; i++) {
        this->hts[i].reserve(table_size);
        this->gs.push_back(LSHHashing(k, w, dim));
    }
    // initialize hashtables according to input data
    init_with_input(input_data);
}

LSHNearestNeighbours::~LSHNearestNeighbours() {}

void LSHNearestNeighbours::init_with_input(list<Point *>& input_data) {
    #ifdef VERBOSE
    profiler_start();
    cout << "Init LSH hashtables..." << endl;
    #endif 

    for (Point *p : input_data) {
        for (int i = 0; i < this->L; i++) {
            // get the appropriate hashing mechanism and hashtable
            HT &ht = this->hts[i];
            LSHHashing &g = this->gs[i];

            // get the ID and the bucket id
            uint32_t id = g(*p);
            uint32_t bucket_id = id % this->table_size;

            // create the datapoint and insert it in the bucket
            ht[bucket_id][id].push_front(p);
        }
    }
    #ifdef VERBOSE
    profiler_stop();
    cout << "Done in " << profiler_get_duration() << "ms" << endl;
    #endif
}

void LSHNearestNeighbours::__lsh_nn_step__(const Point &q, Point *p, uint N, set<Neighbour, my_less>& neighbours, double &dist_b, double R, bool is_range_search) {
    double dist = q.dist(*p);
    if ((!is_range_search && dist - dist_b <= 0) || (is_range_search && dist - R <= 0)) {
        // add the point in the neighbours set
        neighbours.insert(make_pair(p, dist));

        // update dist b with the N-th dist in the neighbours sorted set
        if (!is_range_search && neighbours.size() >= N) {
            set<Neighbour>::iterator it = neighbours.begin();
            advance(it, N - 1);
            dist_b = it->second;
        }
    }
}

std::list<Neighbour>* LSHNearestNeighbours::__k_nearest_neighbours(const Point& q, uint32_t N, double R, bool is_range_search) {
    double dist_b = numeric_limits<double>::max();

    // list to keep k-nearest points  
    set<Neighbour, my_less> neighbours;
    // list to keep points that do not have the same id as q (for later check maybe)
    list<list<Point *> *> rest;
    for (int i = 0; i < L; i++) {
        HT &ht = this->hts[i];
        LSHHashing &g = this->gs[i];

        // get the q point's id
        uint32_t id = g(q);
        
        // find the bucket 
        uint32_t bucket_id = id % this->table_size;
        
        // sanity check
        if(ht.count(bucket_id) == 0) continue;

        // get the bucket and from that get the list of all points with same LSH ID
        list<Point *> &bucket = ht[bucket_id][id];
        
        for (auto data : bucket)    
            __lsh_nn_step__(q, data, N, neighbours, dist_b, R, is_range_search);    

        if (is_range_search) {
            // remember to insert the lists of other points of the same bucket in the rest-list
            for (auto i = ht[bucket_id].begin(); i != ht[bucket_id].end(); i++) {
                if (i->first == id) continue;

                rest.push_back(&i->second);
            }  
        }
    }

    // in case we are done with all points of the same ID 
    // but we have not yet found k points we search the rest of the buckets
    if (is_range_search) {
        // insert all elements from the rest-list in the neighbours set
        for (auto i = rest.begin(); i != rest.end(); i++) {
            for (auto neighbour : **i) {
                __lsh_nn_step__(q, neighbour, N, neighbours, dist_b, R, is_range_search);
            }
        }
    }


    list<Neighbour>* NNs = new list<Neighbour>();
    // create the kNN list to return
    set<Neighbour>::iterator it = neighbours.begin();
    int k = N;
    while ((is_range_search || k-- > 0) && neighbours.size() > 0 && it != neighbours.end()) {                
        NNs->push_back(*it);
        it++;
    }
    return NNs;
}

Neighbour LSHNearestNeighbours::nearestNeighbour(const Point &q) {
    return kNearestNeighbours(q, 1)->size() ? kNearestNeighbours(q, 1)->front()
                                            : make_pair(nullptr, -1);
}

std::list<Neighbour>* LSHNearestNeighbours::kNearestNeighbours(const Point& q, uint N) {
    return this->__k_nearest_neighbours(q, N);
}

std::list<Neighbour>* LSHNearestNeighbours::nearestNeighbours_w_rangeSearch(const Point& q, double R) {
    return this->__k_nearest_neighbours(q, 0, R, true);
}




