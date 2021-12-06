#include "Curves.hpp"

Curve::Curve(string _id = "<unknown>", CurveDistMetric _curveDist = FrechetDist, vector<Point> _curve = vector<Point>()) 
:   id{_id}, 
    curveDist{_curveDist}, 
    curve{_curve}
{}

Curve::~Curve(){}

curve_size_t Curve::complexity() const {
    return this->curve.empty() ? 0 : this->curve.size(); 
}

int Curve::dimensions() const {
    return dim;
}

std::vector<Point> Curve::getCurvePoints() const{
    return this->curve;
}

string Curve::getId(void) {
    return this->id;
}