#pragma once

#include <bitset>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include "Utilities.hpp"
#include "Point.hpp"
#include "Curves.hpp"

#ifndef NN
#include "ClusterSolverCurves.hpp"
#include "ClusterSolver.hpp"
#endif
using namespace std;

//Assignments
#define Lloyds_assignment 1
#define LSH_frechet 2
#define LSH_vector 3
#define Hypercube 4

//Update
#define MeanVector 1
#define MeanCurve 2

class FileHandler{

    private:
        ifstream in;
        string inputPath;
        std::list<Point *> *db;
        std::list<Curve *> *curve_db;
        DistanceMetric distMetric;
        CurveDistMetric curveDistMetric;
        double f_sample;
    public:
        FileHandler(DistanceMetric distMetric, CurveDistMetric curveDistMetric, double f_sample);
        ~FileHandler();
        int OpenFile(string inputFile);
        void CloseFile();
        std::list<Point *> * create_dbPoints();
        std::list<Curve *> * create_dbCurves();
        void cleardb(void);
        
        

        static void print_to_file(
            ofstream &out, const Point &p, string method,
            std::list<pair<Point *, double>> *neighbors,
            std::list<pair<Point *, double>> *rneighbors,
            std::list<pair<Point *, double>> *brute_neighbors, int k,
            double time, double brute_time);
        static void print_to_file(
            ofstream &out, const Curve &p, string method,
            std::list<pair<Curve *, double>> *neighbors,
            std::list<pair<Curve *, double>> *brute_neighbors, int k,
            double time, double brute_time);

        #ifndef NN
            static void print_to_file(ofstream &out,int k,double time,std::string method,std::vector<Centroid> &centroids,std::vector<double> * silhouettes, bool complete,bool silhouette);
            static void print_to_file(ofstream &out,int k,double time,std::string method,std::vector<CurveCentroid> &centroids,std::vector<double> * silhouettes, bool complete, bool silhouette);        
        #endif
};        