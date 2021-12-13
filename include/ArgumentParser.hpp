#pragma once

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

typedef void (*VariableSetter) (std::string arg_name, std::string arg_val);


class ArgumentParser {
    private:
    
    int args_num;
    std::vector<std::string> categorical, numerical_args, flag_args;
    VariableSetter set_var;

    bool is_valid(std::string arg);
    bool is_categorical(std::string arg);
    bool is_flag(std::string arg);

    public:
    
    ArgumentParser(int args_num, std::string *categorical, int comp_n,
                   std::string *numerical_args, int num_n,
                   std::string *flag_args, int flag_n, VariableSetter set_var);
    ~ArgumentParser();
    void parse_args(int argc, char **argv);
};