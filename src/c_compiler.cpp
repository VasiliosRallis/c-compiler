#include "ast.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

int g_depth(0);
std::vector<std::string> g_variables;

int main(int argc, char* argv[]){
    
    if(argc == 5){
        if(std::string(argv[1]) == "--translate"){
            NodePtr ast = parseAST(argv[2]);
            std::ofstream output(argv[4]);
            ast->printPy(output, 0);
            
            //Standard end
            output << "\n\n# Boilerplat \n";
            output << "if __name__ == \"__main__\":\n";
            output << "\timport sys\n";
            output << "\tret=main()\n";
            output << "\tsys.exit(ret)\n";

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
