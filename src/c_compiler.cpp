#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

std::vector<std::string> g_variables;

//This vector will contain assembly that must be printed at the end
//but can't since we are printing in "real-time"
std::vector<std::string> endPrint;

#include "ast_real/ast/node.hpp"

std::unordered_map<std::string, Type> g_mips_var;

//Bill: We are probably going to have to store the type of the arguments to the
//function as well. Might have to use another vector 
//(We have to know the type so we know exaclty how to pass them)

std::unordered_map<std::string, std::vector<Type> > function_type;
std::unordered_map<std::string, std::vector<Type> > function_decl;

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
        
        else if(std::string(argv[1]) == "-S"){
            NodePtr ast = parseAST(argv[2]);
            std::ofstream output(argv[4]);
            ast->printMips(output, NULL);
            
            //Print the endPrint vector
            for(auto i = endPrint.begin(); i != endPrint.end(); ++i){
                output << *i;
            }
        
        }
        else{
            std::cout << "ERROR: Flag missing" << std::endl;
        }
    }
    else {
        std::cout << "ERROR: Missing Arguments, require 4 arguments" << std::endl;
    }

    return 0;
}
