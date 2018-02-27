#ifndef ast_hpp
#define ast_hpp

#include "ast/ast_node.hpp"
#include "ast/ast_primitives.hpp"
#include "ast/ast_statement.hpp"
#include "ast/ast_expr.hpp"
#include "compiler/frame.hpp"
#include "compiler/argTranslator.hpp"

extern NodePtr parseAST(const char* file);

#endif
