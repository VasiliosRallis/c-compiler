#ifndef EXPR_HPP
#define EXPR_HPP

#include <string>
#include <iostream>

#include "ast_real/compiler/frame.hpp"
#include "ast_real/ast/node.hpp"

class Expr: public Node{
public:
    virtual void printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr = NULL)const;
    virtual void printMipsENegate(std::ostream& dst, const std::string& destName, Frame* framePtr = NULL)const;
    virtual std::string getId()const;
};

#endif
