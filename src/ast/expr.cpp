#include "ast_real/ast/expr.hpp"

void Expr::printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr)const{}

std::string Expr::getId()const{
    throw (std::runtime_error("Error: GetId() not implemented for this class"));
}

bool Expr::isAddr()const{return false;};
