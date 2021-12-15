#include "Utilities.hpp"
#include <list>

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


bool curve_compare::operator()(const pair<Curve *, double>& l, const pair<Curve *, double>& r) {
    return l.second - r.second < 0   ? true
        : l.second - r.second > 0 ? false
            : l.first->getId().size() - r.first->getId().size() < 0;
}


Curve *getMeanCurve(list<pair<const Point *, const Point *>> &optimal_traversal){
    
    Curve *c = new Curve();
    for(auto it:optimal_traversal){
        Point p = *it.first + *it.second;
        p /= 2;
        c->AddToCurve(&p);
    }

    return c;
}

Curve *getMeanCurve(Curve *c1, Curve *c2){
    assert(c1 && c2);

    Curve *c = nullptr;
    Frechet::backtrace = true; // set backtracing to true
    // find frechet dist between 2 curves

    c1->dist(*c2);
    // get mean curve
    c = getMeanCurve(Frechet::optimal_traversal);

    // turn backtracing off
    Frechet::backtrace = false;
    
    return c;
}

// got a bug
Curve *getMeanCurve(vector<Curve *>& CurveTree){
    Curve *c = nullptr;
    int step = 1;
    
    while(step <= CurveTree.size()- 1){
        size_t i = 0;
        size_t j = 0;
        for(i = 0; i < CurveTree.size(); i += step*2){
            j = i + step;

            if (j >= CurveTree.size()) break;
            auto c1 = CurveTree[i];
            auto c2 = CurveTree[j];
            
            c = getMeanCurve(c1,c2);
            
            if (step > 1) {
                delete CurveTree[i];
                delete CurveTree[j];
            }

            CurveTree[i] = c;
            CurveTree[j] = NULL;
        }
        step *= 2;
    }
    cout << CurveTree.front()->dimensions() << endl;
    return CurveTree.front();
}

