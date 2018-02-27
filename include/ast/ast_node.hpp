#ifndef ast_expression_hpp
#define ast_expression_hpp

#include <iostream>
#include <vector>
#include <string>

#include "ast_real/ast/node.hpp"
#include "ast_real/compiler/frame.hpp"

extern std::vector<std::string> g_variables;

typedef std::vector<NodePtr>* VectorPtr;
typedef const std::string* StrPtr;

class Expr: public Node{
public:
    virtual void printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr = NULL)const {}
    virtual void printMipsENegate(std::ostream& dst, const std::string& destName, Frame* framePtr = NULL)const {}
    virtual std::string getId()const{
        return std::string("Error: GetId() not implemented for this class");
    }
};
class Statement: public Node{};

#endif
