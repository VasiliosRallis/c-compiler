#ifndef PARAMETER_DECLARATION_HPP
#define PARAMETER_DECLARATION_HPP

#include <iostream>
#include <string>

class ParameterDeclaration;

#include "ast_real/ast/node.hpp"

class DeclSpecifier;
class DirectDeclarator;

class ParameterDeclaration: public Node{
private:
    const DeclSpecifier* n1;
    const DirectDeclarator* n2;

public:
    ParameterDeclaration(const DeclSpecifier* _n1, const DirectDeclarator* _n2);
    
    virtual void print(std::ostream& dst)const override;
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override;
    
    virtual std::string getId()const;
    
    virtual Type getType(const Frame* framePtr)const override; 
};

#endif
