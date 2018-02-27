#ifndef DIRECT_DECLARATOR_HPP
#define DIRECT_DECLARATOR_HPP

#include <vector>
#include <iostream>
#include <string>

#include "ast/stringNode.hpp"

class ParameterDeclaration;

class DirectDeclarator: public StringNode{
private:
    StrPtr s1;
    const std::vector<const ParameterDeclaration*>* v1;
    StrPtr s2;

public:
    DirectDeclarator(StrPtr _id, StrPtr _s1, const std::vector<const ParameterDeclaration*>* _v1, StrPtr _s2);
        
    virtual void print(std::ostream& dst)const override;
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override;
    
    virtual void printMips(std::ostream& dst, Frame* framePtr)const override;
    
    std::string getId()const;
    
    const std::vector<const ParameterDeclaration*>* getParameterList()const;
    
};

#endif
