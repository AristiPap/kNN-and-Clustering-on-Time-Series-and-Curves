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

HashingCurve::HashingCurve(int32_t dim, int32_t w, int32_t k):dim(dim), w(w), k(k){
   
    double num = generateNumber(0,dim);

    //for(int j=0; j<L; j++){
    //    vector<double> t;
        for (int i=0; i<dim; i++)
           this->t.push_back(num);
        
       // Grid g_j(delta,dim,t);
       // grids.push_back(g_j);
    //}
}

DLSHHashingCurve::DLSHHashingCurve(int32_t k, int32_t w, int32_t dim)
:HashingCurve(k, w, dim){}

DLSHHashingCurve::~DLSHHashingCurve() {}

Point * DLSHHashingCurve::operator()(Curve *curve) {
    
    //create grid curve
    Curve* hashedCurve = curveHashing(*curve);
    //convert hashed curve to point
    Point *p = vectorCurveToPoint(hashedCurve,curve);
    //add padding
    p->padding(dim);

    return p;
}

static uint32_t calculate_h(int dim,int M, int w, Point *p){
    double *s = new double[dim];
    int *m = new int[dim];
    
    for (int i=0; i<dim; i++)
        s[i] = generateNumber(0,dim);
    
    
    for (int i=2; i<dim; i++)
        m[i] = (m[i-1]*m[1]) % M;
    
    vector<int> a;
    // Compute the a_i's
    // a_i = floor( (x_i - s_i)/w )
    int counter = 0;
    for (auto it:p->getCoordinates()){
        if(counter >= dim)
            break;
        a.push_back(floor((it - s[counter])/(double)w)); 
        counter ++;
    }   
    // h(p) = [ a_(d-1) + m a_(d-2) + ... + m^(d-1) a_0 ] mod M
    uint32_t hp = 0;
    for (int i=0; i<dim; i++){
        hp += mod(mod(a.at(dim-i-1),M)*m[i],M);
        hp = mod(hp,M);
    }
    return hp;
}


// returns the distance between two points
static double euclid_dist(double x1, double x2){
    return sqrt(pow(x2 - x1, 2));
}


//indeed curve hashing
Curve* HashingCurve::curveHashing(const Curve &curve){
    
    // cpoints -> get number of coordinates of curve parameter
    int cpoints = curve.getCurvePoints().size(); 
    
    //iterate through the points of the curve
    // divide point with delta and the quotient will show to which bucket of the grid it belongs.
    // find the minimum from the top of the bottom of the bucket
    //===============================================================================================
    
    double x;
    double previousMinX = numeric_limits<double>::min();;   // stores minX of the previous point of curve
    Point *aPoint;
    Curve* hashedCurve = new Curve();   // creating a vector to store all hashed points
    
    for(auto it:curve.getCurvePoints()){
        Point *p1 = &it;
        x = p1->getCoordinate(0);
        int StartOfBucket = (x-t.at(0))/delta;      
        
        double start  = StartOfBucket * delta; //start of bucket
        double end  = (StartOfBucket+1) * delta; //end of bucket
    
        double min_dist = start;
        
        
        if (euclid_dist(x, min_dist) > euclid_dist(x,end))
            min_dist = end;
            
        if (min_dist == previousMinX){
            previousMinX = min_dist;
            continue;
        }
        else{
            
            Point *temp_Point = new Point("-1",this->dim,this->distMetric);
            temp_Point->addCoordinate(min_dist);  
            hashedCurve->AddToCurve(temp_Point);
            previousMinX = min_dist;
        }
            
    }
    
    return hashedCurve;   
}

Point* HashingCurve::vectorCurveToPoint(Curve* hashedCurve, Curve *origin){
    // create new point to represent vector of curve with the same id as the curve
    Point* newPoint;
    newPoint = new Point(origin->getId(),this->dim,this->distMetric); 
    
    int coordinatesCounter=0;
    for(auto it:hashedCurve->getCurvePoints())
        newPoint->addCoordinate(it.getCoordinate(0));
    
    newPoint->setInitial(origin);
    
    return newPoint;
}