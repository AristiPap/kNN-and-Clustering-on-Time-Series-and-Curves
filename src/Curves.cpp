#include "Curves.hpp"
#include "frechet.hpp"

void Curve::setId(string id){
    this->id = id;
}

string Curve::getId() const {
    return id; 
}
Curve::Curve(string _id, CurveDistMetric _curveDist, vector<Point> _curve) 
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

void Curve::AddToCurve(Point* p){
    assert(p->getDims() == this->dim);
    this->_complexity++;
    curve.push_back(*p);
}


double FrechetDistDiscrete(const Curve& c1, const Curve& c2) {
    return Frechet::Discrete::distance(c1, c2).value;
}