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
    return _delta;
}

HashingCurve::HashingCurve(double delta, int32_t dim, int32_t w, int32_t k, int32_t max_curve_len):delta(delta), dim(dim), w(w), k(k), max_curve_len(max_curve_len){
   

    for (int i=0; i<dim; i++) {
        double num = generateNumber(0,delta);
        this->t.push_back(num);
        cout << num << " ";
    }
    cout << endl;
}

DLSHHashingCurve::DLSHHashingCurve(double delta, int32_t k, int32_t w, int32_t dim,int32_t max_curve_len)
:HashingCurve(delta, dim, w, k, max_curve_len){}

DLSHHashingCurve::~DLSHHashingCurve() {}

Point * DLSHHashingCurve::operator()(Curve *curve) {
    
    //create grid curve
    Curve* hashedCurve = curveHashing(*curve);
    //convert hashed curve to point
    Point *p = squeeze(hashedCurve,curve);
    //add padding
    p->padding(dim*max_curve_len);

    return p;
}

Curve* HashingCurve::curveHashing(const Curve &curve){

    // iterate through the points of the input curve
    // implement hash 
    //add new point to hashed curve
    
    Point * previousMinPoint = NULL;   // stores previous min point
    Curve* gridCurve = new Curve();   // creating a vector to store all grid points
    
    for(auto it:curve.getCurvePoints()){
        Point *p1 = &it;
        vector <double> x = p1->getCoordinates();
        
        Point *hash = new Point("-1",0,this->distMetric);
        int i = 0;
        for(auto it2 : x){
            hash->addCoordinate(floor(it2/delta + 0.5) * delta + t.at(i));
            #ifdef VERBOSE
            cout << "floor("<<it2/delta << "+ 1/2) * " << delta << " + " << t.at(i) << " = " << floor(it2/delta + 0.5) * delta + t.at(i) << endl;
            #endif
            
            i++;
        }
        
        //avoid duplicates
        if (hash == previousMinPoint){
            previousMinPoint = hash;
            continue;
        }
        
        gridCurve->AddToCurve(hash);
        
    }   
    
    return gridCurve;   
}

Point* HashingCurve::squeeze(Curve* gridCurve, Curve *origin){
    // create new point to represent vector of curve with the same id as the curve
    Point* newPoint = new Point(origin->getId(),0,this->distMetric); 
    
    for(auto it:gridCurve->getCurvePoints())
        for(int i = 0; i< it.getDims(); i++)
            newPoint->addCoordinate(it.getCoordinate(i));
    
    newPoint->setInitial(origin);
    
    return newPoint;
}