#pragma once

#include "Utilities.hpp"
#include "Point.hpp"
#include <random>
#include <vector>
#include <set>
#include <list>
#include <unordered_map>
#define BIGINT 4294967291
// Basic Hashing interface
class Hashing {
   protected:
    // static const long long int M = ;  // 2^32 - 5 (large prime)
    int32_t k;
    int32_t w;
    int32_t dim;

    // how to estimate h
    std::vector<Point> vs; // random vector, where each value is from N(0, 1)
    std::vector<float> ts;  // uniform distribution from [0,w]

    // basic init functions
    void init_t();
    void init_v();

   public:
   // every hashing subclass must implement the () operator overloading to 
   // enable hashing activity
    virtual uint32_t operator()(const Point& p)  = 0;
    Hashing(int32_t k, int32_t w, int32_t dim);
    virtual ~Hashing();
};

// Hashing class for LSH algorithm
class LSHHashing : Hashing {
   private:
    std::vector<int32_t> rs;  // uniform distribution from [0,inf)

    void init_r();

   public:
    uint32_t operator()(const Point& p) ;
    LSHHashing(int32_t k, int32_t w, int32_t dim);
    virtual ~LSHHashing();
    static uint32_t estimate_w(std::list<Point*>& dataset);
};

class HyperCubeHashing : Hashing{
    private:
        vector<unordered_map<uint32_t,bool>> f; // f_1(), f_2(), ..., f_dd()
        //LSHHashing is the class of the sub hash functions of LSH
        // vector<LSHHashing> h; // h_1(), h_2(), ..., h_k() //TODO: remove this later on since these are AMPLIFIED G functions
        int dd; // The new dimension that the point is projected to (dd<32)
    public:
        uint32_t operator()(const Point& p);
        HyperCubeHashing(int dd, int32_t k, int32_t w, int32_t dim);
        virtual ~HyperCubeHashing();
};