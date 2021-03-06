#ifndef DIRECT_DECLARATOR_HPP
#define DIRECT_DECLARATOR_HPP   

#include <vector>
#include <iostream>
#include <string>

class DirectDeclarator;

#include "ast_real/compiler/frame.hpp"
#include "ast_real/ast/stringNode.hpp"
#include "ast_real/ast/parameterDeclaration.hpp"

class Expr;

class DirectDeclarator: public StringNode{
private:
    StrPtr s1;
    const std::vector<const ParameterDeclaration*>* v1;
    StrPtr s2;
    const Expr* expr;
    
public:
    DirectDeclarator(StrPtr _id, StrPtr _s1, const std::vector<const ParameterDeclaration*>* _v1, StrPtr _s2, const Expr* expr);
        
    virtual void print(std::ostream& dst)const override;
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override;
    
    virtual void printMips(std::ostream& dst, Frame* framePtr, Type type = Type::ANYTHING)const override;
    
    std::string getId()const;
    
    const std::vector<const ParameterDeclaration*>* getParameterList()const;
    
    void printGMips(std::ostream& dst, Type type)const override;
    
    bool isPointer()const override;
    
    void insertFTypes(const Type returnType)const;
    
    int getSize()const;
};

#endif
