#include <iostream>
#include "Point.hpp"
#include <assert.h>



Point::Point(DistanceMetric distMetric):distMetric(distMetric), dims(0), marked(false){}

Point::Point(string id, int dims, DistanceMetric distMetric) 
: distMetric(distMetric), id(id), dims(dims),marked(false), cluster_id(-1) {
    coords.resize(dims);
}

Point::Point(string id, vector<double> p, DistanceMetric distMetric)
    : distMetric(distMetric), id(id), dims(p.size()), coords(p), marked(false), cluster_id(-1) {}

//setters
void Point::setId(string id){
    this->id = id;
}

void Point::setDims(int dim_num){
    this->dims = dim_num;
}

void Point::setMarked(bool flg){
    this->marked = flg;
}

void Point::setCluster(int cluster_id){
    this->cluster_id = cluster_id;
}

void Point::setSecCluster(int scluster_id){
    this->s_cluster_id = scluster_id;
}

void Point::setCoordinate(double x, int i){
    this->coords[i] = x;
}

void Point::addCoordinate(double x) {
    (this->dims)++;
    this->coords.push_back(x);
}

//getters

DistanceMetric Point::getDistMetric() const {
    return this->distMetric;
}

string Point::getId() const {
    return id; 
}

int Point::getDims() const {
    return dims;
}

bool Point::getMarked() const {
    return marked;
}

int Point::getCluster() const {
  return this->cluster_id;
}

int Point::getSecCluster() const {
  return this->s_cluster_id;
}

double Point::getCoordinate(int i) const {
    return coords.at(i);
}

vector<double> Point::getCoordinates() const {
    return coords;
}

// dot product
double Point::operator*(const Point &p) const {
    assert(this->coords.size() == p.coords.size());

    double dot = 0.0;
    
    for (uint32_t i = 0; i < this->coords.size(); i++) dot += this->coords[i] * p.getCoordinate(i);

    return dot;
}

// vector addition
void Point::operator+=(const Point &p) {
    assert(this->coords.size() == p.coords.size());
    for (uint32_t i = 0; i < this->coords.size(); i++) this->coords[i] += p.coords[i];
}

// divide with scalar (element wise)
void Point::operator/=(const double n) {
    assert(n != 0);
    for (uint32_t i = 0; i < this->coords.size(); i++) this->coords[i] /= n;
}

// multiply with scalar (element wise)
void Point::operator*=(const double n) {
    for (uint32_t i = 0; i < this->coords.size(); i++) this->coords[i] *= n;
}

void Point::setCoordinates(const vector<double> v) {
    assert(this->coords.size() == v.size());
    for (uint32_t i = 0; i < this->coords.size(); i++) this->coords[i] = v[i];
}

double Point::dist(Point p) const {
    return this->distMetric(coords, p.getCoordinates());
}

ostream& operator<<(ostream& os, const Point& p) {
    os << "[";
    uint32_t dims = p.getDims();
    for (uint32_t i = 0; i < dims; i++)
        os << p.getCoordinate(i) << (i < dims-1 ? " " : "");
    os << "]";
    return os;
}

// Default point distance metrics

double L2_norm(const std::vector<double>& p1, const std::vector<double>& p2) {
    assert(p1.size() == p2.size());

    double dist = 0.0;
    for (uint32_t i = 0; i < p1.size(); i++)
        dist += (p1[i] - p2[i]) * (p1[i] - p2[i]);

    return sqrt(dist);
}

double L1_norm(const std::vector<double>& p1, const std::vector<double>& p2) {
    assert(p1.size() == p2.size());

    double dist = 0.0;
    for (uint32_t i = 0; i < p1.size(); i++)
        dist += abs(p1[i] - p2[i]);
    return dist;
}
