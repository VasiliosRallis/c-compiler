#include "ast.hpp"

int main()
{
    NodePtr ast = parseAST();

    ast->print(std::cout);
    
    std::cout << std::endl;

    return 0;
}
