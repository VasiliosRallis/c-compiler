#include "ast.hpp"

int main(int argc, char* argv[]){
    
    if(argc == 5){
        NodePtr ast = parseAST(argv[2]);
        ast->print(std::cout);
    }
    
    return 0;
}        
