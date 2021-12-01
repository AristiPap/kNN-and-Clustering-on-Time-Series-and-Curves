#pragma once
#include <iostream>

namespace cluster_global_variables {
const bool complete = false;
const std::string method = "Classic";
}  // namespace cluster_global_varialbes

typedef void (*VariableSetter) (std::string arg_name, std::string arg_val);

void lsh_set_variable(std::string arg_name, std::string arg_val);
void cube_set_variable(std::string arg_name, std::string arg_val);
void cluster_set_variable(std::string arg_name, std::string arg_val);