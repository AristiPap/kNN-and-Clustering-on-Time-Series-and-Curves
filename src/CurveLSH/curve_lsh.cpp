#include "CurveHashing.hpp"
#include <string>
#include <iostream>

#include "FileHandler.hpp"
#include "Evaluator.hpp"
#include "LSHNearestNeighbours.hpp"
#include "ArgumentParser.hpp"
using namespace std;

#define DEFAULT_K 4
#define DEFAULT_L 5
#define DEFAULT_N 1
#define DEFAULT_R 10000
typedef std::list<Curve*> DataCurveList;

int main(void) {
    Curve c1{};
    Curve c2{};
    
    c1.AddToCurve(new Point("-1", vector<double>({0, 1}), L1_norm));
    c1.AddToCurve(new Point("-1", vector<double>({1, 4}), L1_norm));
    
    c2.AddToCurve(new Point("-1", vector<double>({0, 3}), L1_norm));
    c2.AddToCurve(new Point("-1", vector<double>({0, 9}), L1_norm));
    
    DLSHHashingCurve h(2, 1, 2, 2, 2);
    
    cout << *h(&c1) << endl;
    
    #define F_S 0.5

    FileHandler file_handler(L2_norm, F_S); // create points with L2 points
    DataCurveList *dataset = nullptr;
    file_handler.OpenFile("/home/aristi/Desktop/kNN-and-Clustering-on-Time-Series-and-Curves/nasdaq2017_LQ.csv");
    dataset = file_handler.create_dbCurves();
    file_handler.CloseFile();
    cout << dataset->front()->getCurvePoints()[0]<< endl;
}