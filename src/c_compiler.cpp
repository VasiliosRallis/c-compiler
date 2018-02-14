#include "ast.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

int g_depth(0);
std::vector<std::string> g_var;

int main(int argc, char* argv[]){
    
    if(argc == 5){
        if(std::string(argv[1]) == "--translate"){
            NodePtr ast = parseAST(argv[2]);
            std::ofstream output(argv[4]);
            ast->printPy(output);
        }
        else if(std::string(argv[1]) == "--parse"){
            NodePtr ast = parseAST(argv[2]);
            std::ofstream output(argv[4]);
            ast->print(output);
        }
        else{
            std::cout << "ERROR: Flag missing" << std::endl;
        }
    }
    
    return 0;
}
