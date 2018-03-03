#ifndef PARAMETER_DECLARATION_HPP
#define PARAMETER_DECLARATION_HPP

#include <iostream>
#include <string>

class ParameterDeclaration;

#include "ast_real/ast/node.hpp"
#include "ast_real/ast/directDeclarator.hpp"

class ParameterDeclaration: public Node{
private:
    NodePtr n1;
    const DirectDeclarator* n2;

public:
    ParameterDeclaration(NodePtr _n1, const DirectDeclarator* _n2);
    
    virtual void print(std::ostream& dst)const override;
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override;
    
    virtual std::string getId()const; 
};

#endif
