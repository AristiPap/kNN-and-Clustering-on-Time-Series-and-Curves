#include "FileHandler.hpp"

FileHandler::FileHandler(DistanceMetric distMetric, double f_sample){
    this->db = nullptr;
    this->distMetric = distMetric;
    this->f_sample = f_sample;
};
FileHandler::~FileHandler(){};

int FileHandler:: OpenFile(string inputFile){
    this->inputPath = inputFile;
    
    in.open(inputFile,std::ios_base::in);
    
    if (!in){
        cerr << "Cannot open the input file " << inputFile <<  endl;
        exit(1);
    }
    
    return 0;
}

void FileHandler::CloseFile(){
    in.close();
}

std::list<Point *> *FileHandler::create_dbPoints(){
    int T = (int)(1/this->f_sample);
    int sample_i = 0;
    string str,token;
    string id;
    vector<double> coords;

    this->db = new list<Point *> ();
    
    //start with 1 point to see if file is empty or not
    if(getline(in,str,'\n')){
        if ( !str.empty() && str.back() == '\r' ) 
            str.erase( str.size() - 1 );
        istringstream ss(str);
        ss >> token;
        id = token;
        
        while( ss >> token )
            // sample with the given sampling rate
                coords.push_back(stod(token));
                
        if (coords.size() != 120)
            cout << "coords size: " << coords.size() << " " << id << endl;

        Point p1(id, coords, this->distMetric);
        this->db->push_back(new Point(p1));
        coords.clear();
    }else{
        cerr << "Input file " << this->inputPath << " is empty." << endl;
        return nullptr;
    }  
    //read rest of the file
    while(getline(in,str,'\n')){
        sample_i = 0;
        if ( !str.empty() && str.back() == '\r' ) 
            str.erase( str.size() - 1 );
        istringstream ss(str);
        ss >> token;
        id = token;
        while( ss >> token )
            // sample with the given sampling rate
                coords.push_back(stod(token));

        if (coords.size() != 120)
            cout << "coords size: " << coords.size() << " " << id << endl;
        Point p(id, coords, this->distMetric);
        this->db->push_back(new Point(p));
        coords.clear();
    }
    
    //return list of points    
    return this->db;
}

std::list<Curve *> *FileHandler::create_dbCurves(){
    
    this->curve_db = new list<Curve *> ();
    int _ratio = int(1 / this->f_sample);
    string str;
    //get curve
    while (getline(in,str,'\n')){
        if ( !str.empty() && str.back() == '\r' ) 
            str.erase( str.size() - 1 );
         
        stringstream linestream(str);
        string id;
        string word;
        //find how many tabs exist in line read. tabs == points 
        size_t tabs = count(str.begin(), str.end(),'\t'); 
        //get id
        getline(linestream, id, '\t');
        Curve *tmp_Curve = new Curve();
        tmp_Curve->setId(id); 
        //cout<<"id is:"<<id<<" and ratio" << _ratio <<endl;
        
        double y;
        int _id = 1;
        
        for (int i=0; i< tabs; i++){   
            getline(linestream, word, '\t'); 
            //cast string value to double
            y = std::stod(word.c_str());

            Point *p = new Point(id,0,this->distMetric);               
            //cout << "x and y: " << _id << "," << y << endl;
            
            //add x coordinate -> number of time column
            p->addCoordinate(double(_id));                   
            
            //add y coordinate -> value of xij
            p->addCoordinate(y);                      
            
            tmp_Curve->AddToCurve(p);
            _id ++;
            
            //move to the prev of the next point of sample
            if(_ratio > 1){
                for(int j=0; j < _ratio - 1; j++){
                    getline(linestream, word, '\t');
                    i++;
                    _id++;
                } 
            }           
            
        }
        this->curve_db->push_back(tmp_Curve);   
    }
    return this->curve_db;
}

void FileHandler::print_to_file(ofstream &out,const Point &p,string method,std::list<pair<Point *, double>>* neighbors,std::list<pair<Point *, double>>* rneighbors,std::list<pair<Point *, double>>* brute_neighbors,int k,double time, double brute_time)
{    
    if (!out) {
        cerr << "Cannot write to of stream. Exiting..." << endl;
        return;
    }
    
    out << "Query: " << p.getId() << endl;
    int counter =1;
    auto it2 = brute_neighbors->begin();
    for(auto it1 : *neighbors){
        out << "Nearest neighbour-"<< counter++ <<": "<< it1.first->getId() << endl;
        out << "distance"<<method<<": "<<it1.second << endl;
        out << "distanceTrue: " << it2->second << endl;
        advance(it2,1);
    }
    
    out << "t"<<method<<": "<< time << " ms" << endl;
    out << "tTrue: " << brute_time << " ms" << endl;
    
    // r-near neighbors:
    if ((*rneighbors).size()>0){
        out << "R-near neighbours:" << endl;
        for(auto it3 : *rneighbors)
            out << it3.first->getId() <<endl;
    }
    out << endl;
}


// add on for the second assignment
void FileHandler::print_to_file(ofstream &out, const Curve &p, string method,
                          std::list<pair<Curve *, double>> *neighbors,
                          std::list<pair<Curve *, double>> *brute_neighbors,
                          int k, double time, double brute_time) 
{
    if (!out) {
        cerr << "Cannot write to of stream. Exiting..." << endl;
        return;
    }

    out << "Query: " << p.getId() << endl;
    int counter = 1;
    auto it2 = brute_neighbors->begin();
    for (auto it1 : *neighbors) {
        out << "Nearest neighbour-" << counter++ << ": " << it1.first->getId()
            << endl;
        out << "distance" << method << ": " << it1.second << endl;
        out << "distanceTrue: " << it2->second << endl;
        advance(it2, 1);
    }

    out << "t" << method << ": " << time << " ms" << endl;
    out << "tTrue: " << brute_time << " ms" << endl;
}

#ifndef NN
void FileHandler::print_to_file(ofstream &out,int k,double time,string func,std::vector<Centroid> &centroids,std::vector<double> * silhouettes, bool complete){
    
    if (!out) {
        cerr << "Cannot write to of stream. Exiting..." << endl;
        return;
    }
    
    out << "Algorithm: " << func << endl;
    
    for(int i=0; i<k; i++){
        out << "CLUSTER-" << i << " {" ;
        //print table of centroids coordinates
        out << centroids[i].second.size() << ", "; 
        out<<(out,centroids[i].first) << " }" << endl;
    }
    out << "clustering_time:" << time << "s" <<  endl;
    //print vector of silhouettes for each centroid in dataset and the total average
    out<< "Silhouette:";
    out << *silhouettes;
    
    if(complete){
        out<<endl;
        for(int i=0; i<k; i++){ 
            out << "CLUSTER-" << i << " {" ;
            //print table of centroids coordinates
            out << centroids[i].second.size() << ", "; 
            out<<(out,centroids[i].first);
            for(auto it : centroids[i].second)
                out<<(out,it.first->getId()) << " ";
            
            out << " }" << endl;
        }
    }
    
    out << endl;
}
#endif

void FileHandler::cleardb(void) {
    
    if(this->db == nullptr)
        return;
    for (auto p : *this->db) {
        delete p;
    }
    
    delete this->db;
    this->db = nullptr;
}