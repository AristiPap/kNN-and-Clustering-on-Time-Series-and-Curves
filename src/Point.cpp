#include <iostream>
#include "Point.hpp"
#include <assert.h>
#include "Curves.hpp"


Point::Point(DistanceMetric distMetric):distMetric(distMetric), dims(0), marked(false), initial(NULL){}

Point::Point(string id, int dims, DistanceMetric distMetric) 
: distMetric(distMetric), id(id), dims(dims),marked(false), cluster_id(-1), initial(NULL) {
    coords.resize(dims);
}

Point::Point(string id, vector<double> p, DistanceMetric distMetric)
    : distMetric(distMetric), id(id), dims(p.size()), coords(p), marked(false), cluster_id(-1), initial(NULL) {}

Point::Point(const Point& p)
    : distMetric(p.distMetric),
      id("copy"+p.id),
      dims(p.dims),
      coords(p.coords),
      marked(false),
      cluster_id(-1),
      initial(p.initial) {}

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

void Point::setInitial(Curve *curve){
    this->initial = curve;
}

void Point::padding(int new_d){
    int space = new_d-this->getDims();
    for (int i=0; i < space ; i++)
        this->addCoordinate(0);
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

Curve *Point::getCurve(void) const {
    return this->initial;
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
    return this->distMetric(*this, p);
}

ostream& operator<<(ostream& os, const Point& p) {
    os << "[";
    uint32_t dims = p.getDims();
    for (uint32_t i = 0; i < dims; i++)
        os << p.getCoordinate(i) << (i < dims-1 ? " " : "");
    os << "]";
    return os;
}

Point Point::operator-(const Point p) const{
    assert(this->coords.size() == p.coords.size());

    Point sub{"<"+id+" - "+p.id+">", 0, this->distMetric};

    for (uint32_t i = 0; i < this->coords.size(); i++)
        sub.addCoordinate(this->coords[i] - p.getCoordinate(i));

    return sub;
}

Point Point::operator+(const Point p) const{
    assert(this->coords.size() == p.coords.size());

    Point sub{"<"+id+" + "+p.id+">", 0, this->distMetric};

    for (uint32_t i = 0; i < this->coords.size(); i++)
        sub.addCoordinate(this->coords[i] + p.getCoordinate(i));

    return sub;
}

// add-on from Fred module
distance_t Point::length_sqr() const {
    distance_t result = 0;

    for (dimensions_t i = 0; i < this->coords.size(); ++i) {
        result += this->coords[i] * this->coords[i];
    }
    return result;
}

distance_t Point::dist_sqr(const Point &point) const {
    distance_t result = 0, temp;
    #pragma omp simd private(temp) reduction(+: result)
    for (dimensions_t i = 0; i < this->dims; i++){
        temp = this->coords[i] - point.getCoordinate(i);//operator[](i) - point[i];
        result += temp * temp;
    }
    return result;
}

distance_t Point::line_segment_dist_sqr(const Point &p1, const Point &p2) const {
    const Vector u = p2 - p1;
    parameter_t projection_param = (*this - p1) * u / (u * u);
    if (projection_param < parameter_t(0)) projection_param = parameter_t(0);
    else if (projection_param > parameter_t(1)) projection_param = parameter_t(1);
    Point projection(u);
    //const Point projection = p1 + u * projection_param;
    for(int i=0; i<u.getCoordinates().size(); i++)
        projection.getCoordinates()[i] = u.getCoordinate(i) * projection_param;
    
    projection += p1;
    return projection.dist_sqr(*this);
}

Interval Point::ball_intersection_interval(const distance_t distance_sqr, const Point &line_start, const Point &line_end) const {
        const Point u = line_end-line_start, v = *this - line_start;
        const parameter_t ulen_sqr = u.length_sqr(), vlen_sqr = v.length_sqr();
        
        if (near_eq(ulen_sqr, parameter_t(0))) {
            if (vlen_sqr <= distance_sqr) return Interval(parameter_t(0), parameter_t(1));
            else return Interval();
        }
                
        const parameter_t p =  -2. * ((u * v) / ulen_sqr), q = vlen_sqr / ulen_sqr - distance_sqr / ulen_sqr;
        
        const parameter_t phalf_sqr = p * p / 4., discriminant = phalf_sqr - q;
        
        if (discriminant < 0) return Interval();
        
        const parameter_t discriminant_sqrt = std::sqrt(discriminant);
        
        const parameter_t minus_p_h = - p / 2., r1 = minus_p_h + discriminant_sqrt, r2 = minus_p_h - discriminant_sqrt;
        const parameter_t lambda1 = std::min(r1, r2), lambda2 = std::max(r1, r2);
                
        return Interval(std::max(parameter_t(0), lambda1), std::min(parameter_t(1), lambda2));
    }




// Default point distance metrics

double L2_norm(const Point& _p1, const Point& _p2) {
    auto p1 = _p1.getCoordinates();
    auto p2 = _p2.getCoordinates();

    assert(p1.size() == p2.size());

    double dist = 0.0;
    for (uint32_t i = 0; i < p1.size(); i++)
        dist += (p1[i] - p2[i]) * (p1[i] - p2[i]);

    return sqrt(dist);
}

double L1_norm(const Point& _p1, const Point& _p2) {
    auto p1 = _p1.getCoordinates();
    auto p2 = _p2.getCoordinates();

    assert(p1.size() == p2.size());

    double dist = 0.0;
    for (uint32_t i = 0; i < p1.size(); i++)
        dist += abs(p1[i] - p2[i]);
    return dist;
}

double InitialCurveDF(const Point& p1, const Point& p2) {
    return p1.getCurve()->dist(*p2.getCurve());
}