#include "GenericClusterSolver.hpp"

// intialize static members of Kmeans solver
int KMeans_Solver::hc_k = 3;
int KMeans_Solver::hc_M = 1400;
int KMeans_Solver::hc_probes = 14;
int KMeans_Solver::lsh_k = 4;
int KMeans_Solver::lsh_L = 6;
int KMeans_Solver::K = 0;

KMeans_Solver::KMeans_Solver(int K){
    KMeans_Solver::K = K;
}

KMeans_Solver::~KMeans_Solver(){}

void KMeans_Solver::parse_config_file(std::string file_name){
    ifstream in(file_name);
    if (!in) {
        cerr << "Cannot open the input file " << file_name << endl;
        exit(1);
    }
    
    string str = "";
    int *params[] = {
        &KMeans_Solver::K,
        &KMeans_Solver::lsh_L,
        &KMeans_Solver::lsh_k,
        &KMeans_Solver::hc_M,
        &KMeans_Solver::hc_k,
        &KMeans_Solver::hc_probes
    };
    
    for (int i = 0; i < 6; i++) {
        try {
            getline(in, str, '\n');
            if (!str.empty() && str.back() == '\r') str.erase(str.size() - 1);
            uint32_t pos = str.find(":");
            *params[i] = atoi(str.substr(pos+1).c_str());
            
        } catch(exception e) {
            cerr << "Error parsing config file." << endl;
            exit(1);
        }
    }
}

ostream& operator<<(ostream& os, vector<double> silhouettes) {
    os << "[";
    uint32_t dims = silhouettes.size();

    for (uint32_t i = 0; i < dims; i++)
        os << silhouettes[i] << (i < dims - 1 ? ", " : "");
    os << "]" << endl;
    return os;
}