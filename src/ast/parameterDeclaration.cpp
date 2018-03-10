#include "ast_real/ast/parameterDeclaration.hpp"

#include "ast/ast_primitives.hpp"

ParameterDeclaration::ParameterDeclaration(const DeclSpecifier* _n1, const DirectDeclarator* _n2)
    :n1(_n1), n2(_n2){}

void ParameterDeclaration::print(std::ostream& dst)const{
    n1->print(dst);
    dst << " ";
    if(n2 != NULL) n2->print(dst);
}

void ParameterDeclaration::printPy(std::ostream& dst, int depth)const{
    n2->print(dst);
}

std::string ParameterDeclaration::getId()const{
    if(n2 != NULL) return n2->getId();
    else return std::string("Error: Called getId on Parameter Declaration with no ID");
}

Type ParameterDeclaration::getType(const Frame* framePtr)const{
    return n1->getType();
}
