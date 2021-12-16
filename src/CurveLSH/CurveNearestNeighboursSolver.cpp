#include "CurveNearestNeighbours.hpp"

CurveNearestNeighboursSolver::CurveNearestNeighboursSolver(std::list<Curve *> &dataset, uint32_t curve_L, double curve_delta, uint32_t curve_d) 
: _curve_L{curve_L}, _curve_delta{curve_delta}, _curve_d{curve_d}, dataset{dataset}
{}

CurveNearestNeighboursSolver::~CurveNearestNeighboursSolver() {}
