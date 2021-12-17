#include <string>
#include <iostream>

#include "Profiler.hpp"
#include "ClusterSolver.hpp"
#include "ClusterSolverCurves.hpp"
#include "FileHandler.hpp"
#include "Evaluator.hpp"
#include "ArgumentParser.hpp"

using namespace std;

#define DEFAULT_K 4
#define DEFAULT_L 5
#define DEFAULT_N 1
#define DEFAULT_R 10000

// globals needed for lsh
string infile_name;
string conf_file_name;
string outfile_name;
string update_method;
string assignment_method;
u_int32_t k=DEFAULT_K, L=DEFAULT_L, N=DEFAULT_N;
double R=DEFAULT_R;
bool set_if=false, set_of=false, set_conf=false, complete = false, silhouette = false;
AssignmentStep vector_assignment_step = nullptr;
CurveAssignmentStep curve_assignment_step = nullptr;

void set_var(string arg_name, string arg_val) {
    if (arg_name == "-i") {
        // input file
        infile_name = arg_val;
        set_if = true;
    } else if (arg_name == "-o") {
        //output file
        outfile_name = arg_val;
        set_of = true;
    } else if (arg_name == "-c") {
        // configuration file
        conf_file_name = arg_val;
        set_conf = true;
    } else if (arg_name == "-update") {
        update_method = arg_val;
    } else if (arg_name == "-assignment") {
        assignment_method = arg_val;
    } else if (arg_name == "-complete") {
        complete = true;
    } else if (arg_name == "-silhouette") {
        silhouette = true;
    }
}

// retunr 0 for n, 1 for y, -1 for error
static int get_answer(string ans) {
    bool end = ans == "y";
    return (!end && ans != "n") ? -1 : end; 
}

// returns true if user wants to exit
static bool ask(string msg) {
    int ret = -1;    
    cout << msg << " [y/n]: ";
    do {
        string ans;
        cin >> ans;
        ret = get_answer(ans);
    } while (ret == -1);

    return (ret == 1);
}

// set up file paths
static void set_up(void) {
    if (!set_if) {
        cout << "Input Dataset File: ";
        cin >> infile_name;
    }
    if (!set_conf) {
        cout << "Configuration File: ";
        cin >> conf_file_name;
    }
    if (!set_of) {
        cout << "Output File Name: ";
        cin >> outfile_name;
    }

    // set up assignment and update steps
    if (update_method == "Mean_Vector") {
        if (assignment_method == "Classic")
            vector_assignment_step = Lloyd;
        else if (assignment_method == "LSH")
            vector_assignment_step = reverse_assignment_lsh;
        else if (assignment_method == "Hypercube")
            vector_assignment_step = reverse_assignment_hypercube;
        else {
            cerr << "You cannot use assignment step '" << assignment_method << "' with Mean_Vector update method" << endl;
            exit(1); 
        }
    } else if (update_method == "Mean_Curve") {
        if (assignment_method == "Classic")
            curve_assignment_step = Lloyd;
        else if (assignment_method == "LSH_Frechet") 
            curve_assignment_step = reverse_assignment_lsh_curves;
        else {
            cerr << "You cannot use assignment step '" << assignment_method << "' with Mean_Curve update method" << endl;
            exit(1);
        }
    }
}

void demo_vector_clustering() {
    Evaluator evaluator;

    FileHandler file_handler(L2_norm, FrechetDistDiscrete);

    // get dataset
    file_handler.OpenFile(infile_name);
    DataList *dataset = nullptr;

    dataset = file_handler.create_dbPoints();

    file_handler.CloseFile();
    KMeans_Solver::parse_config_file(conf_file_name);
    KMeans_pp_Solver solver(*dataset, vector_assignment_step, KMeans_Solver::K);
    string i = "0";
    if (assignment_method == "Classic") i = "1";
    else if (assignment_method == "LSH") i ="2";
    else if (assignment_method == "Hypercube") i = "3";
    // create the solver and evaluate the algorithm
    evaluator.evaluate_from_file(*dataset, "A" + i + "U1", solver, outfile_name, complete, silhouette);

    file_handler.cleardb();
}


void demo_curve_clustering() {    
    Evaluator evaluator;

    FileHandler file_handler(L2_norm, FrechetDistDiscrete);

    // get dataset
    DataListCurve *dataset = nullptr;
    file_handler.OpenFile(infile_name);
    dataset = file_handler.create_dbCurves();
    file_handler.CloseFile();

    KMeans_Solver::parse_config_file(conf_file_name);
    
    KMeans_pp_Solver_Curves solver(*dataset, curve_assignment_step, KMeans_Solver::K);
    string i = "0";
    if (assignment_method == "Classic")
        i = "1";
    else if (assignment_method == "LSH_Frechet")
        i = "2";

    _pad_num_ = 0;
    for (auto c : *dataset)
        if (rand() % 2)
            for (auto p : c->getCurvePoints())
                for (auto coord : p.getCoordinates())
                    _pad_num_ = max(_pad_num_, floor((coord - CLUSTERING_DELTA) / CLUSTERING_DELTA) * CLUSTERING_DELTA + CLUSTERING_DELTA + 100);

    // create the solver and evaluate the algorithm
    evaluator.evaluate_from_file(*dataset, "A" + i + "U2", solver, outfile_name, complete, silhouette);

    file_handler.cleardb();
}

int main(int argc, char **argv) {
    // create an argument parser and parse arguments
    string categorical[] = {"-i", "-c", "-o", "-update", "-assignment"};
    string num_args[] = {};
    string flags[] = {"-complete", "-silhouette"};
    ArgumentParser arg_parser(7, categorical, 5, num_args, 0, flags, 2, set_var);
    arg_parser.parse_args(argc, argv);

    bool start_process;
    // set up variables
    set_up();
    cout << "Ready to run Cluster-Solver with arguments: {update: " << update_method << ", assignment: " << assignment_method << ", complete: " << (complete==true ? "true" : "false") << "}" << endl\
    << "Input File: " << infile_name << endl\
    << "Configuration File: " << conf_file_name << endl\
    << "Output File: " << outfile_name << endl;  
    start_process = ask("Initiate evaluation?");
    if (start_process) {
        if (update_method == "Mean_Vector")
            demo_vector_clustering();
        else if (update_method == "Mean_Curve")
            demo_curve_clustering();
        else {
            cerr << "Error in update method: '" << update_method << "'"<< endl;
            exit(1);
        }
    }

}