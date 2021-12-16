#include "Curves.hpp"
#include "frechet.hpp"
#include <assert.h>


Curve::Curve(string _id, CurveDistMetric _curveDist, vector<Point> _curve) 
:   id{_id}, 
    curveDist{_curveDist}, 
    curve{_curve}
{
    dim = curve.size() ? curve.front().getDims() : 0;
    _complexity = curve.size();
    marked = false;
    cluster_id = -1;
    s_cluster_id = -1;
}

Curve::~Curve(){}

void Curve::setId(string id) { this->id = id; }

string Curve::getId() const { return id; }

curve_size_t Curve::complexity() const {
    return this->curve.empty() ? 0 : this->curve.size(); 
}

int Curve::dimensions() const {
    return dim;
}

const std::vector<Point>& Curve::getCurvePoints() const{
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

void Curve::setMarked(bool flg){
    this->marked = flg;
}

void Curve::setCluster(int cluster_id){
    this->cluster_id = cluster_id;
}

void Curve::setSecCluster(int scluster_id){
    this->s_cluster_id = scluster_id;
}

void Curve::setPoints(const Curve * c) {
    assert(this->dimensions() == c->dimensions());
    this->curve = c->curve;
    this->_complexity = this->curve.size();
}

int Curve::getCluster() const {
  return this->cluster_id;
}

int Curve::getSecCluster() const {
  return this->s_cluster_id;
}

bool Curve::getMarked() const {
  return this->marked;
}

// metrics to estimate curve distance
double FrechetDistDiscrete(const Curve& c1, const Curve& c2) {
    return Frechet::Discrete::distance(c1, c2).value;
}

double FrechetDistContinuous(const Curve& c1, const Curve& c2) {
    return Frechet::Continuous::distance(c1, c2).value;
}
