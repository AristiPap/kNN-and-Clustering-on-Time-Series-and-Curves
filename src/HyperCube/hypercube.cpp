#include "hyper_cube.hpp"

using namespace std;

HyperCube::HyperCube(uint32_t w, uint32_t d, uint32_t dd, uint32_t probes, uint32_t M,
                     std::list<Point *> &input_data)
      :h(dd, dd, w, d),
      dd(dd),
      w(w),
      d(d),
      num_fs(dd),
      probes(probes),
      num_points(M){
      
    hd_max = dd;
    for (auto _dp : input_data) {
        insert(_dp);
    }
    // Save dt to the dataset vector
    dataset = &input_data;
}

HyperCube::~HyperCube() {}

void HyperCube::insert(Point *p) {
    // Insert it(its pointer) on every hashtable, according to h hashes
    uint32_t hashkey = h(*p);  
    cube[hashkey].push_back(make_pair(hashkey, p));
}

// return vertices of Hamming distance == hd

set<uint32_t> HyperCube::nearVertices(uint32_t num, uint32_t key, int length, int hd, set<uint32_t>&visited) {
    set<uint32_t> s;
    // if (hd == 0) {
    if (HammingDistance(num, key, length) == hd && cube.find(num) != cube.end()) {
        s.insert(num);
        return s;
    }
    visited.insert(num);
    // }
    // Loop over the bits of num
    uint32_t mask = 1, num1;
    for (int i = 0; i < length; i++) {
        // Mask to get one bit
        num1 = num ^ mask;
        if (visited.find(num1) != visited.end()) continue;
        set<uint32_t> s_deeper = nearVertices(num1, key, length, hd, visited);
        // Unite s with s_deeper. s_deeper will contain the elements having
        // distance hd from the base element
        s.insert(s_deeper.begin(), s_deeper.end());
        mask <<= 1;
    }
    return s;
}


std::list<Neighbour> *HyperCube::nearestNeighbours(const Point &p, uint32_t k, double min_dist, bool is_range_search) {
    uint32_t hashkey = this->h(p);

    set<Neighbour, my_less> neighbours;  // <id, dist from p>

    // Compute a set of vertices with hamming distance < hd
    // Loop over the points that were hashed in the same vertice, or in a
    // vertice with a certain hamming distance.
    int pointscount = 0;
    int verticescount = 0;

    int hd = 0;
    // Loop over vertices of hamming distance == hd (starting at 0)
    while (pointscount <= num_points && verticescount <= probes && hd <= hd_max) {
        set<uint32_t>v;
        set<uint32_t> s = nearVertices(hashkey, hashkey, dd, hd, v);
        // For every vertix in hamming distance hd, check the points
        if (s.size()){
            for (auto vertice_id : s) {
                // if (HammingDistance(vertice_id, hashkey, dd) != hd) continue;
                if (verticescount > probes) break;

                mapIt it = this->cube.find(vertice_id);

                Bucket &bucket = it->second;
                // Loop over the points of this vertice
                for (auto it1 : bucket) {
                    if (pointscount > num_points) break;

                    // Compute the distance between the two points
                    double dist = p.dist(*it1.second);

                    if (dist <= min_dist) {
                        neighbours.insert(make_pair(it1.second, dist));
                        // update mdist
                        if (!is_range_search && neighbours.size() > k) {
                            set<Neighbour>::iterator it2 = neighbours.begin();
                            advance(it2, k - 1);
                            min_dist = it2->second;
                        }
                    }
                    pointscount++;
                }
                verticescount++;
            }
        }
        hd++;
    }
    
    list<Neighbour> *closest = new list<Neighbour>();
    int k_counter = 1;
    for (auto it_2 : neighbours) {
        if (!is_range_search && k_counter > k) break;
        closest->push_back(make_pair(it_2.first, it_2.second));
        k_counter++;
    }
    
    return closest;
}

Neighbour HyperCube::nearestNeighbour(const Point &q) {
    return kNearestNeighbours(q, 1)->size() ? kNearestNeighbours(q, 1)->front()
                                            : Neighbour(nullptr, -1);
}

std::list<Neighbour> *HyperCube::kNearestNeighbours(const Point &q, uint N) {
    return this->nearestNeighbours(q, N);
}

std::list<Neighbour> *HyperCube::nearestNeighbours_w_rangeSearch(const Point &q, double R) {
    return this->nearestNeighbours(q, 0, R, true);
}

// int main() {
//     Point v1 = {1, {1.0, 0.0, 1.0}};
//     Point v2 = {2, {10.0, 5.0, 1.0}};
//     Point v3 = {3, {9.5, 5.0, 1.0}};
//     Point v4 = {4, {1.0, 0.5, 2.0}};

//     list<Point *> l;
//     l.emplace_front(new Point(v1));
//     l.emplace_front(new Point(v2));
//     l.emplace_front(new Point(v3));
//     l.emplace_front(new Point(v4));

//     HyperCube hp_solver(3, 3, 10, 1, 10, 10, 1.0, l);
//     // vector<Point *> ll;
//     // ll = hypercube_solver
//     list<pair<uint32_t, double>>*
//     ll(hp_solver.nearestNeighboursBruteForce(Point(10, {9.3, 5.0, 1.0}), 2));
//     // cout << ll.first << " " << ll.second << endl;
//     for (auto ret : *ll)
//        cout << ret.first << " - " << ret.second << endl;
// }