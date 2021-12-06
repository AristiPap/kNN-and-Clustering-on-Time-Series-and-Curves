#include "Curves.hpp"

Curve::Curve(){}

void Curve::setId(string id){
    this->id = id;
}

string Curve::getId() const {
    return id; 
}

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
    curve.push_back(*p);
}