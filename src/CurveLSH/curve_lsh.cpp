#include "CurveHashing.hpp"

int main(void) {
    Curve c1{};
    Curve c2{};
    
    c1.AddToCurve(new Point("-1", vector<double>({0, 1}), L1_norm));
    c1.AddToCurve(new Point("-1", vector<double>({1, 4}), L1_norm));
    
    c2.AddToCurve(new Point("-1", vector<double>({0, 3}), L1_norm));
    c2.AddToCurve(new Point("-1", vector<double>({0, 9}), L1_norm));
    
    DLSHHashingCurve h(2, 1, 1, 2, 2);
    
    cout << *h(&c1) << endl;
}