#ifndef ast_expression_hpp
#define ast_expression_hpp

#include <iostream>
#include <vector>
#include <string>

#include "ast_real/ast/node.hpp"
#include "ast_real/compiler/frame.hpp"

extern std::vector<std::string> g_variables;

typedef const std::string* StrPtr;

class Statement: public Node{};

#endif
