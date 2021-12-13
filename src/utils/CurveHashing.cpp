#include "CurveHashing.hpp"
#include <unordered_map>
#include <list>

using namespace std;

HashingCurve::~HashingCurve() {}

//https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
double generateNumber(double const& lower,double const& upper){

    random_device rd; //seed
    mt19937 gen(rd());
    uniform_real_distribution<> dis(lower,upper);
    uniform_real_distribution<>  distr(lower, upper);

    return distr(gen);

}

double HashingCurve :: estimate_delta(std::list<Curve*>& dataset_input, std::list<Curve*>& dataset_query){
    assert(dataset_input.size() > 1);
    assert(dataset_query.size() > 1);
    
    int32_t _dim = dataset_input.front()->dimensions();
    
    //m1 is the average of the complexities of the input curve dataset
    double m1 = 0;
    int counter_input = 0;
    for(auto it:dataset_input)
        counter_input += it->complexity();
    m1 = counter_input / dataset_input.size();
    //m2 is the average of the complexities of the query curve dataset
    double m2 = 0;
    int counter_query = 0;
    for(auto it2:dataset_query)
        counter_query += it2->complexity();
    m2 = counter_query / dataset_query.size();
    
    double _delta = 4 * _dim * min(m1,m2);
    return _delta/1000;
}

HashingCurve::HashingCurve(int32_t dim, int32_t w, int32_t k, double delta, int32_t max_curve_len):delta(delta), dim(dim), w(w), k(k), max_curve_len(max_curve_len){

    for (int i=0; i<dim; i++) {
        double num = generateNumber(0,delta);
        this->t.push_back(num);
    }
}

DLSHHashingCurve::DLSHHashingCurve(int32_t k, int32_t w, int32_t dim,double delta, int32_t max_curve_len)
:HashingCurve(dim, w, k, delta, max_curve_len){}

DLSHHashingCurve::~DLSHHashingCurve() {}

Point * DLSHHashingCurve::operator()(Curve &curve) {
    
    //create grid curve
    Curve* hashedCurve = curveHashing(curve);
    //convert hashed curve to point
    Point *p = squeeze(*hashedCurve, &curve);
    //add padding
    p->padding(dim*max_curve_len);

    return p;
}

Curve* HashingCurve::curveHashing(Curve &curve){
    // iterate through the points of the input curve
    // implement hash 
    //add new point to hashed curve
    
    Point * previousMinPoint = NULL;   // stores previous min point
    Curve* gridCurve = new Curve();   // creating a vector to store all grid points
    
    for(auto it:curve.getCurvePoints()){
        Point *p1 = &it;
        vector <double> x = p1->getCoordinates();
        
        Point *hash = new Point("<snapped-point>",0,this->distMetric);
        int i = 0;
        for(auto it2 : x){
            hash->addCoordinate(floor(it2/delta + 0.5) * delta + t.at(i));
            
            i++;
        }
        
        //avoid duplicates
        if (previousMinPoint && hash->getCoordinates() == previousMinPoint->getCoordinates()){
            continue;
        } else 
            previousMinPoint = hash;
        
        gridCurve->AddToCurve(hash);
        
    }   
    
    return gridCurve;   
}

Point* HashingCurve::squeeze(Curve& gridCurve, Curve *origin){
    // create new point to represent vector of curve with the same id as the curve
    Point* newPoint = new Point(origin->getId(), 0, InitialCurveDF); 
    
    for(auto it:gridCurve.getCurvePoints())
        for(int i = 0; i< it.getDims(); i++)
            newPoint->addCoordinate(it.getCoordinate(i));
    
    newPoint->setInitial(origin);
    
    return newPoint;
}


// Continuous LSH implementation
CLSHHashingCurve::CLSHHashingCurve(int32_t k, int32_t w, int32_t dim, double delta,int32_t max_curve_len) 
: HashingCurve(dim, w, k, delta, max_curve_len)
{} 

CLSHHashingCurve::~CLSHHashingCurve(){}

Point* CLSHHashingCurve::operator()(Curve& curve) {
    // First filter the continuous curve
    Curve *filtered_curve = this->filter(curve);

    // snap the curve onto the grid
    Curve *grid_curve = this->curveHashing(curve);
    delete filtered_curve;

    // keep only the sequences of min and maxes
    auto curve_points = grid_curve->getCurvePoints();
    Curve min_max_sequence_curve = Curve("<min-max-sequence>-"+curve.getId(), FrechetDistContinuous, {curve_points.front()}); 
    for (auto p_i = curve_points.begin()+1; p_i+1 != curve_points.end(); p_i++) {
        // add the point in i only if p_i \not-in (min(p_{i-1}, p_{i+1}), max(p_{i-1}, p_{i+1}))
        double v = p_i->getCoordinate(1);
        double prev_v = (p_i-1)->getCoordinate(1);
        double next_v = (p_i+1)->getCoordinate(1);
        double min_v = min(prev_v, next_v);
        double max_v = max(prev_v, next_v);
        if (v - min_v < 0 || max_v - v < 0) {
            // the point with value v is a local maxima/minima
            // so add it in the curve
            min_max_sequence_curve.AddToCurve(&(*p_i));
        }
    }

    //add the last point of the curve
    min_max_sequence_curve.AddToCurve(&(curve_points.back()));
    delete grid_curve;

    // squeeze the point and return the vector x with padding
    Point* p = squeeze(min_max_sequence_curve, &curve);
    // add padding
    p->padding(dim * max_curve_len);

    return p;
}

// // function to filter ONLY continues curves on 2D-plane
Curve* CLSHHashingCurve::filter(Curve &c) {
    assert(c.complexity() > 1);
    assert(c.dimensions() > 0);

    // if the curve has less that 3 points then there is no reason to filter
    if (c.complexity() < 3) return new Curve(c);

    auto curve_points = c.getCurvePoints();
    auto a_i = curve_points.begin(), b_i = curve_points.begin(), c_i = curve_points.begin();
    b_i++; c_i++; c_i++;

    // initiate the new curve
    Curve *filtered_curve = new Curve("<filtered>"+c.getId(), FrechetDistContinuous, {*a_i});

    // Pass all the curves-points, do:
    while (a_i != curve_points.end() && b_i != curve_points.end() && c_i != curve_points.end()) {
        // 1. Check if |a-b| < e and |b-c| < e, for given points where a, b, c are the
        //  values of the time series in specific timestamps
        double a = (*a_i).getCoordinate(1); // get y coordinate: T(t_i), t_i is a timestamp
        double b = (*b_i).getCoordinate(1); // get y coordinate: T(t_i), t_i is a timestamp
        double c = (*c_i).getCoordinate(1); // get y coordinate: T(t_i), t_i is a timestamp
        if (abs(a-b) <= EPSILON && abs(b-c) <= EPSILON) {
            // we eliminate the point in b_i
            // by proceeding to the next points for b and c
            b_i++;
            c_i++;
        } else {
            // the point in b must be in the curve since it cannot be filtered
            filtered_curve->AddToCurve(&(*b_i));
            // 2. merge the points if prev-condition is true and set a_i = b_i,
            // b_i = b_i + 1, c_i = c_i + 1, and restart the process
            a_i = b_i;
            b_i++;
            c_i++;
        }
    }
    
    // 3. at the end of the procedure we have the last point in the curve at c_i. Add it into the filtered curve
    filtered_curve->AddToCurve(&curve_points.back());

    // 4. return the new curve
    return filtered_curve;
}