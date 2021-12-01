#include "Hashing.hpp"
#include <unordered_map>
#include <list>

using namespace std;

Hashing::Hashing(int32_t k, int32_t w, int32_t dim)
: k(k), w(w), dim(dim){
    init_v();  // set up all random vectors
    init_t();  // set up all random displacements
}

Hashing::~Hashing() {}

void Hashing::init_v() {
    for (int i = 0; i < k; i++) vs.push_back(Point(-1, get_random_vec(this->dim), L1_norm)); // insert v_i, metric is irrelevant
}

void Hashing::init_t() {
    for (int i = 0; i < k; i++) ts.push_back(get_random_num(w)); // insert t_i
}


// LSHHashing Implementation

LSHHashing::LSHHashing(int32_t k, int32_t w, int32_t dim)
:Hashing(k, w, dim){
    #ifdef VERBOSE
    cout << "LSH - func: k=" << k << ", w=" << w << ", dim=" << dim << endl;  
    #endif
    init_r();  // set up all random weights for convolution
}

LSHHashing::~LSHHashing() {}

void LSHHashing::init_r() {
    for (int i = 0; i < k; i++) rs.push_back(get_random_num(0, INT32_MAX)); // insert r_i
}


uint32_t LSHHashing::operator()(const Point& p) {
    int64_t id = 0;

    for (int i = 0; i < k; i++) {
        // calculate hash value h_i
        int64_t h_i = (int64_t)( (p * vs[i] + ts[i]) / (float)w );
        id = mod( mod(id, BIGINT) + mod(rs[i] * h_i,  BIGINT) , BIGINT);
    }
    assert(id >= 0); // sanity check
    assert(((int64_t)((uint32_t)id)) == id); // make sure that id can be set in 32 bit number

    // return g(.) % M
    return (uint32_t)id;
}

// estimate w based on a dataset
uint32_t LSHHashing::estimate_w(list<Point *> &dataset) {

    double avg_dist = 0.0;
    uint size = dataset.size();

    for (int i = 0; i < 1000; i++) {
        //sample 1000 distances
        list<Point *>::iterator it1 = dataset.begin(), it2 = dataset.begin();
        uint i1=0, i2=0;
        
        // sample until we get a coin flip right 
        while (rand() % 2 || i1 == i2){
            i1 = ((uint)rand()) % size;
            i2 = ((uint)rand()) % size;
        }
        // get the list nodes
        advance(it1, i1);
        advance(it2, i2);

        // calculate dist and update min/max distances
        double dist = (*it1)->dist(*(*it2));
        avg_dist += dist;

    }
    avg_dist = avg_dist / 1000.0;
    return max((uint32_t)(avg_dist), (uint32_t)2); // if the number is to small then get 6
}


uint32_t HyperCubeHashing::operator()(const Point& p) {
    
    int64_t id=0;
    for (int i=0; i<dd; i++){
        uint32_t gp = ( (p * this->vs[i] + this->ts[i]) / (float)w ); //hash(p);  // 

        // Look if h_i(p) exists as id in f_i(). If not, insert it
        if (f[i].count(gp) == 0){
            f[i][gp] = rand()%2 == 1; // rand()%2 is uniform
        }
        // Push back the f() bit to the id
        id = id<<1;
        id = id | f[i][gp];
    }

    assert(id >= 0);
    assert(((int64_t)((uint32_t)id)) == id);

    return (uint32_t)id;
}

HyperCubeHashing::HyperCubeHashing(int dd, int32_t k, int32_t w, int32_t dim):
Hashing(k, w, dim), dd(dd){
    // Create k sub-hashfunctions
    //cout<<"Checks"<<k<<w<<dim<<dd<<endl;
    assert(dd <= 32);
    for (int i=0; i<dd; i++){
        // Create the i-th f() and save it to the vector
        unordered_map<uint32_t,bool> f_i;
        f.push_back(f_i);
    }
}

HyperCubeHashing::~HyperCubeHashing(){}

//test main
// int main(void) {
//     vector<double> l = {1.0, 1.0, 1.0};
//     vector<double> ll = {1.1, 1.0, 1.0};
//     Point v(0, l);
//     Point vv(0, ll);

//     HyperCubeHashing h;
// }    
// // test main
// int main(void) {
//     vector<double> l = {1.0, 1.0, 1.0};
//     vector<double> ll = {1.1, 1.0, 1.0};
//     Point v(0, l);
//     Point vv(0, ll);

//     LSHHashing h;

//     cout << h(v) << endl << h(vv) << endl << v * vv <<  endl; 
// }
