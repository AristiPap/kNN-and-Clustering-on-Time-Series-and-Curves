#include <iostream>
#include <string>

#include "ArgumentParser.hpp"
#include "Evaluator.hpp"
#include "FileHandler.hpp"
#include "LSHNearestNeighbours.hpp"
#include "hyper_cube.hpp"
#include "GenericLSHSolver.hpp"

using namespace std;

// default sampling rate
#define F_S 0.5 

#define DEFAULT_K_LSH 4
#define DEFAULT_L 5
#define DEFAULT_HP 14

#define DEFAULT_M 10
#define DEFAULT_PROBES 2

#define DEFAULT_N 1

// globals needed for lsh
string infile_name;
string query_file_name;
string outfile_name;
u_int32_t k = -1, L = DEFAULT_L, N = DEFAULT_N, M = DEFAULT_M, probes = DEFAULT_PROBES;
string algorithm;
CurveDistMetric metric = FrechetDistDiscrete;

double delta = 0.0;
bool set_if = false, set_of = false, set_qf = false, set_alg = false, set_metric = false;

void set_var(string arg_name, string arg_val) {
    if (arg_name == "-i") {
        // input file
        infile_name = arg_val;
        set_if = true;
    } else if (arg_name == "-o") {
        // output file
        outfile_name = arg_val;
        set_of = true;
    } else if (arg_name == "-q") {
        // query file
        query_file_name = arg_val;
        set_qf = true;
    } else if (arg_name == "-algorithm") {
        algorithm = arg_val;
        set_alg = true;
    } else if (arg_name == "-metric") {
        if (arg_val == "discrete")
            metric = FrechetDistDiscrete;
        else if (arg_val == "continuous")
            metric = FrechetDistContinuous;
        else {
            cerr << "Defaulting metric for frechet algorithm: Discrete case." << endl;
            metric = FrechetDistDiscrete;
        }
        set_metric = true;
    } else {
        // set up hyper parameters
        uint32_t val = strtoul(arg_val.c_str(), nullptr, 10);
        if (arg_name == "-k")
            k = val;
        else if (arg_name == "-L")
            L = val;
        else if (arg_name == "-N")
            N = val;
        else if (arg_name == "-M")
            M = val;
        else if (arg_name == "-probes")
            probes = val;
        else if (arg_name == "-delta")
            delta = val;
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
    if (!set_qf) {
        cout << "Query File: ";
        cin >> query_file_name;
    }
    if (!set_of) {
        cout << "Output File Name: ";
        cin >> outfile_name;
    }

    if (!set_alg) {
        cout << "Algorithm: [LSH|Hypercube|Frechet]";
        cin >> algorithm;
        if (algorithm == "Frechet") {
            string ans;
            cout << "Metric for Frechet: [discrete|continuous]";
            cin >> outfile_name;
            if (ans == "discrete")
                metric = FrechetDistDiscrete;
            else if (ans == "continuous")
                metric = FrechetDistDiscrete;
            else {
                cerr
                    << "Defaulting metric for frechet algorithm: Discrete case."
                    << endl;
                metric = FrechetDistDiscrete;
            }
        }
    }
}

static void reset_params() {
    infile_name = "";
    query_file_name = "";
    outfile_name = "";
    algorithm = "";
    metric = FrechetDistDiscrete;

    // reset the bools
    set_alg = set_metric = set_if = set_of = set_qf = false;
}

static void demo_lsh(list<Point *>* dataset) {
    Evaluator evaluator;
    // create the solver and evaluate the algorithm
    uint32_t w = LSHHashing::estimate_w(*dataset);
    LSHNearestNeighbours solver{*dataset, dataset->size() / 8, k, w, L};
    evaluator.evaluate_from_file(*dataset, "LSH", solver, query_file_name, outfile_name, N, 0); // last argument @R is 0 since we dont care about it
}

static void demo_hypercube(list<Point *> *dataset) {
    Evaluator evaluator;
    // create the solver and evaluate the algorithm
    uint32_t dims = dataset->front()->getDims();
    uint32_t w = LSHHashing::estimate_w(*dataset);
    HyperCube solver{w, dims, k, probes, M, *dataset};
    evaluator.evaluate_from_file(*dataset, "HP", solver, query_file_name, outfile_name, N, 0);
}

static void demo_frechet(list<Curve *> *dataset, double f_sample) {
    Evaluator evaluator;
    FileHandler fh(L2_norm, metric, f_sample);
    fh.OpenFile(query_file_name);
    list<Curve *> * query_list = fh.create_dbCurves();
    fh.CloseFile();

    // estimate delta TODO:
    if (delta == 0)
        delta = HashingCurve::estimate_delta(*dataset, *query_list);

    LSHSolver solver(*dataset, L, delta, dataset->front()->dimensions(),
                     int(metric == FrechetDistContinuous), "LSH", 1, 1);

    evaluator.evaluate_from_file(*dataset, *query_list, algorithm, solver, outfile_name, N);

    fh.cleardb();

}

int main(int argc, char **argv) {
    // create an argument parser and parse arguments
    string categorical[] = {"-i", "-o", "-q", "-algorithm", "-metric"};
    string num_args[] = {"-k", "-L", "-R", "-N", "-M", "-probes", "-delta"};
    string flags[] = {};
    ArgumentParser arg_parser(12, categorical, 5, num_args, 7, flags, 0, set_var);
    arg_parser.parse_args(argc, argv);
    
    // TODO: Find the sampling rate from the name of the query file and pass it in
    // the file handler
    double f_sample = F_S;

    FileHandler file_handler_train(L2_norm, metric, f_sample);  // create points with L2 points
    bool end = false;
    bool start_process;
    DataList *dataset = nullptr;
    list<Curve *> *dataset_c = nullptr;
    bool reset = true;

    // loop until the user ends the process with specific input
    while (!end) {
        // set up variables
        set_up();
        cout << "Ready to run " << algorithm << "-Solver with arguments: {k: " << k
             << ", L: " << L << ", N: " << N << ", M: " << M << ", probes: "<< probes << \
             ", delta: " << delta << ", metric: " << (metric == FrechetDistDiscrete ? "discrete" : "continuous") << "} for" << endl
             << "Input File: " << infile_name << endl
             << "Query File: " << query_file_name << endl
             << "Output File: " << outfile_name << endl;
        start_process = ask("Initiate evaluation?");
        if (start_process) {
            // get dataset

            if (reset) {
                file_handler_train.OpenFile(infile_name);
                if (algorithm == "Frechet") 
                dataset_c = file_handler_train.create_dbCurves();
                else 
                    dataset = file_handler_train.create_dbPoints();
                file_handler_train.CloseFile();
            }
            if (algorithm == "LSH") {
                demo_lsh(dataset);

            } else if (algorithm == "Hypercube") {
                demo_hypercube(dataset);
            } else if (algorithm == "Frechet") {
                demo_frechet(dataset_c, f_sample);
            } else {
                cerr << "Algorithm: " << algorithm
                     << " is not a valid algorithm. Try again." << endl;
            }
        }

        // check if the user wants to exit program
        end = ask("Exit?");
        // if we are to continue, reset the parameters
        if (!end) {
            if (!ask("Re-run with same file paths?")) {
                reset_params();
                file_handler_train.cleardb();
                reset = true;
            } else
                reset = false;
        }
    }
    file_handler_train.cleardb();
}