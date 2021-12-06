#include <string>
#include <iostream>

#include "FileHandler.hpp"
#include "Evaluator.hpp"
#include "LSHNearestNeighbours.hpp"
#include "ArgumentParser.hpp"
using namespace std;

#define DEFAULT_K 4
#define DEFAULT_L 5
#define DEFAULT_N 1
#define DEFAULT_R 10000

// globals needed for lsh
string infile_name;
string query_file_name;
string outfile_name;
u_int32_t k=DEFAULT_K, L=DEFAULT_L, N=DEFAULT_N;
double R=DEFAULT_R;
bool set_if=false, set_of=false, set_qf=false;

void set_var(string arg_name, string arg_val) {
    if (arg_name == "-i") {
        // input file
        infile_name = arg_val;
        set_if = true;
    } else if (arg_name == "-o") {
        //output file
        outfile_name = arg_val;
        set_of = true;
    } else if (arg_name == "-q") {
        // query file
        query_file_name = arg_val;
        set_qf = true;
    } else {
        // set up hyper parameters
        uint32_t val = strtoul(arg_val.c_str(), nullptr, 10);
        if (arg_name == "-k")
            k = val;
        else if (arg_name == "-L")
            L = val;
        else if (arg_name == "-N")
            N = val;
        else if (arg_name == "-R")
            R = strtod(arg_val.c_str(), nullptr);
        
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
}

static void reset_params() {
    infile_name = "";
    query_file_name = "";
    outfile_name = "";

    // reset the bools
    set_if = set_of = set_qf = false;
}

int main(int argc, char **argv) {
    // create an argument parser and parse arguments
    string categorical[] = {"-i", "-o", "-q"};
    string num_args[] = {"-k", "-L", "-R", "-N"};
    string flags[] = {};
    ArgumentParser arg_parser(7, categorical, 3, num_args, 4, flags, 0, set_var);
    arg_parser.parse_args(argc, argv);

    // TODO: Find the sampling rate from the name of the query file and pass it in the file handler
    #define F_S 0.5

    Evaluator evaluator;
    FileHandler file_handler(L2_norm, F_S); // create points with L2 points
    bool end = false;
    bool start_process;
    DataList *dataset = nullptr;
    bool reset = true;
    while (!end) {
        // set up variables
        set_up();
        cout << "Ready to run LSH-Solver with arguments: {k: " << k << ", L: " << L \
            << ", N: " << N << ", R: " << R << "} for" << endl\
            << "Input File: " << infile_name << endl\
            << "Query File: " << query_file_name << endl\
            << "Output File: " << outfile_name << endl;  
        start_process = ask("Initiate evaluation?");
        if (start_process) {
            // get dataset
            
            if (reset) {
                file_handler.OpenFile(infile_name);
                dataset = file_handler.create_dbPoints();
                file_handler.CloseFile();
            }
            // create the solver and evaluate the algorithm
            uint32_t w = LSHHashing::estimate_w(*dataset);
            LSHNearestNeighbours solver{*dataset, dataset->size()/8, k, w, L};
            evaluator.evaluate_from_file(*dataset, "LSH", solver, query_file_name, outfile_name, N, R);
        }

        // check if the user wants to exit program
        end = ask("Exit?"); 
        // if we are to continue, reset the parameters
        if (!end){
            if (!ask("Re-run with same file paths?")){
                reset_params();
                file_handler.cleardb();
                reset = true;
            } else 
                reset = false;
        }  
    }
    file_handler.cleardb();
}