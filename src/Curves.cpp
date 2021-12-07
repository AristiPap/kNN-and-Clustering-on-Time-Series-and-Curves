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

double Curve::dist(Curve& curve) const{
    assert(curve.dimensions() == this->dimensions());

    return this->curveDist(*this, curve);
}

// function to filter the curve in case the curve is in continuous representation
Curve* Curve::filter(void) {
    assert(this->curve.size() > 1);
    assert(this->dim > 0);

    // if the curve has less that 3 points then there is no reason to filter
    if (this->curve.size() < 3) return new Curve(*this);

    // Pass all the curves-points, do:
    // 1. Check if |a-b| < e and |b-c| < e, for given points where a, b, c are the 
    //  values of the time series in specific timestamps
    // 2. merge the points if prev-condition is true and set a_i = c_i, b_i = b_i + 1, c_i = c_i + 2, repeat 
    // 3. consider changing the merging into applying an intermediate point where b  = (a+c)/2 (average value of 2)
    // 4. return the new curve
}

// metrics to estimate curve distance
double FrechetDistDiscrete(const Curve& c1, const Curve& c2) {
    return Frechet::Discrete::distance(c1, c2).value;
}
