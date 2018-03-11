#ifndef EXPR_HPP
#define EXPR_HPP

#include <string>
#include <iostream>

class Expr;

#include "ast_real/ast/node.hpp"

class Frame;

class Expr: public Node{
public:
    virtual void printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr, Type type)const;
    //virtual bool isAddr()const;
};

#endif
