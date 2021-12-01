#include "Utilities.hpp"

using namespace std;

std::random_device rd;
std::mt19937 gen(rd());

// function to get a normal random vector
std::vector<double> get_random_vec(int dim) {
    normal_distribution<double> normal_dist(0.0, 1.0); // standard normal distribution
    vector<double> vec;  // random vector prototype

    for (int i = 0; i < dim; i++)
        vec.push_back(normal_dist(gen));  // sample from normal distribution and
                                      // push the results in the vector

    return vec;
}

uint32_t HammingDistance(uint32_t a, uint32_t b, int dd) {
    uint32_t mask = 0x1;
    uint32_t dist = 0;
    for (int i = 0; i < dd; i++) {
        dist += ((a & mask) != (b & mask) ? 1 : 0);
        a >>= 1;
        b >>= 1;
    }
    
    return  dist;
}

float get_random_num(int max) {
    // get random int from uniform distribution
    std::uniform_real_distribution<> uniform_dist(0.0, (float)max);
    float t = uniform_dist(gen);
    return (float)t;
}

int32_t get_random_num(int32_t min, int32_t max) {
    // get random int from uniform distribution
    std::uniform_int_distribution<int32_t> uniform_dist(min, max);
    int32_t t = uniform_dist(gen);
    return t;
}

bool my_less::operator()(const pair<Point *, double>& l, const pair<Point *, double>& r) {
    return l.second - r.second < 0   ? true
        : l.second - r.second > 0 ? false
            : l.first->getId().size() - r.first->getId().size() < 0;
}
