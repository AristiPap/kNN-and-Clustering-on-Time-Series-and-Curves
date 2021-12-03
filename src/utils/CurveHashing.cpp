#include "CurveHashing.hpp"
#include <unordered_map>
#include <list>

using namespace std;

HashingCurve::HashingCurve(int32_t dim, int32_t L, int32_t w, int32_t k)
:dim(dim),L(L), w(w), k(k){}

HashingCurve::~HashingCurve() {}
