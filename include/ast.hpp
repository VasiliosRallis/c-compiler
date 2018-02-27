#ifndef ast_hpp
#define ast_hpp

#include "ast/ast_node.hpp"
#include "ast/ast_primitives.hpp"
#include "ast/ast_statement.hpp"
#include "ast/ast_expr.hpp"
#include "ast_real/compiler/argTranslator.hpp"
#include "ast_real/compiler/frame.hpp"

extern NodePtr parseAST(const char* file);

#endif
