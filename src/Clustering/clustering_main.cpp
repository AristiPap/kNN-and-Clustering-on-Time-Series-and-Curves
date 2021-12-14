#include <string>
#include <iostream>

#include "Profiler.hpp"
#include "ClusterSolver.hpp"
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
string method;
u_int32_t k=DEFAULT_K, L=DEFAULT_L, N=DEFAULT_N;
double R=DEFAULT_R;
bool set_if=false, set_of=false, set_conf=false, complete = false;

/*void set_var(string arg_name, string arg_val) {
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
    } else if (arg_name == "-m") {
        method = arg_val;
    } else if (arg_name == "-complete") {
        complete = true;
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
}

static void reset_params() {
    infile_name = "";
    conf_file_name = "";
    outfile_name = "";

    // reset the bools
    set_if = set_of = set_conf = false;
}*/

int main(int argc, char **argv) {
    cout << "Success"<<endl;
    // create an argument parser and parse arguments
    /*string categorical[] = {"-i", "-c", "-o", "-m"};
    string num_args[] = {};
    string flags[] = {"-complete"};
    ArgumentParser arg_parser(5, categorical, 4, num_args, 0, flags, 1, set_var);
    arg_parser.parse_args(argc, argv);


    Evaluator evaluator;
    FileHandler file_handler(L2_norm);
    bool end = false;
    bool start_process;
        // set up variables
    set_up();
    cout << "Ready to run Cluster-Solver with arguments: {m: " << method << ", complete: " << (complete==true ? "true" : "false") << "}" << endl\
    << "Input File: " << infile_name << endl\
    << "Configuration File: " << conf_file_name << endl\
    << "Output File: " << outfile_name << endl;  
    start_process = ask("Initiate evaluation?");
    if (start_process) {
        // get dataset
        file_handler.OpenFile(infile_name);
        DataList *dataset = file_handler.create_dbPoints();
        file_handler.CloseFile();
            
        // create the solver and evaluate the algorithm
        KMeans_pp_Solver::parse_config_file(conf_file_name);
        AssignmentStep assignment_step;
        if (method == "Classic") assignment_step = Lloyd;
        else if (method == "LSH") assignment_step = reverse_assignment_lsh;
        else if (method == "Hypercube") assignment_step = reverse_assignment_hypercube;
        KMeans_pp_Solver solver(*dataset, assignment_step, KMeans_pp_Solver::K);
        evaluator.evaluate_from_file(*dataset, method, solver, outfile_name, complete);
    }

    file_handler.cleardb();*/
}

// /*#include "ClusterSolver.hpp"

// int main(int argc, char **argv) {
//     list<Point *>l;

//     l.push_back(new Point(1, {0.5, 8.1}));
//     l.push_back(new Point(2, {1.1, 2.1}));
//     l.push_back(new Point(3, {2.1, 2.2}));
//     l.push_back(new Point(4, {1.9, 2.0}));
//     l.push_back(new Point(5, {0.9, 9.2}));

//     KMeans_pp_Solver solver(l, Lloyd, 2);

//     vector<Centroid> *C = solver.get_k_clusters(2, 10);

//     for (auto c : *C) {
//         cout << "Cluster: " << c.first << " with elements: " << endl;
//         for (auto p : c.second) cout << *p.first << endl;
//     }
    
//     // double avg = evaluate_w_silhouette(cout,*C,l); 
//     // cout << "DIST: " << avg << endl;

// }