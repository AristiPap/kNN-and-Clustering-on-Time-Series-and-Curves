#include "ArgumentParser.hpp"

using namespace std;


ArgumentParser::ArgumentParser(int args_num, string *categorical, int comp_n, string *numerical_args, int num_n, string *flag_args, int flag_n, VariableSetter set_var) {
	this->args_num = args_num;
	// set up argument lists
	for (int i=0; i<comp_n; i++) this->categorical.push_back(categorical[i]);
	for (int i=0; i<num_n; i++) this->numerical_args.push_back(numerical_args[i]);
	for (int i=0; i<flag_n; i++) this->flag_args.push_back(flag_args[i]);

	// set the variable setter
	this->set_var = set_var;
}

ArgumentParser::~ArgumentParser(){}

bool ArgumentParser::is_valid(string arg) {
	for (string a : this->categorical) {
		if (a == arg) return true;
	}

	for (string a : this->numerical_args) {
		if (a == arg) return true;
	}

	for (string a : this->flag_args) {
		if (a == arg) return true;
	}

	return false;
}

bool ArgumentParser::is_categorical(string arg) {
	for (string a : this->categorical) {
		if (a == arg) return true;
	}

	return false;
}


bool ArgumentParser::is_flag(string arg) {
	for (string a : this->flag_args) {
		if (a == arg) return true;
	}

	return false;
}

void ArgumentParser::parse_args(int argc, char **argv) {
	// check argument number
	if (argc > 2 * this->args_num + 1){
		cerr << "Too many arguments" << endl;
		exit(EXIT_FAILURE);
	}

	for (int i = 1; i < argc; i += 2) {
		string arg_name = string{argv[i]};
		string arg_val; 

		if (this->is_valid(arg_name)) {
			// check if its a flag
			if (this->is_flag(arg_name)) {
				i--;
				arg_val = "true";
			} else {
				arg_val = string{argv[i+1]};
			}
			this->set_var(arg_name, arg_val);
		} else {
			cerr << "Wrong argument: " << arg_name << endl;
			exit(EXIT_FAILURE);
		}
	}
}

