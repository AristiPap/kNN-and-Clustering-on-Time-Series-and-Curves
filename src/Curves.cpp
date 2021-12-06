#include "Curves.hpp"
#include "frechet.hpp"
#include <assert.h>

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
{
    dim = curve.size() ? curve.front().getDims() : 0;
    _complexity = curve.size();
}

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
    if (!dim) dim = p->getDims();
    
    assert(p->getDims() == this->dim);
    this->_complexity++;
    curve.push_back(*p);
}

double Curve::dist(Curve& curve) {
    assert(curve.dimensions() == this->dimensions());

    return this->curveDist(*this, curve);
}


// metrics to estimate curve distance
double FrechetDistDiscrete(const Curve& c1, const Curve& c2) {
    return Frechet::Discrete::distance(c1, c2).value;
}