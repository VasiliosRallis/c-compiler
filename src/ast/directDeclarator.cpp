#include <vector>
#include <string>
#include <unordered_map>

#include "ast_real/ast/directDeclarator.hpp"

class ParameterDeclaration;

DirectDeclarator::DirectDeclarator(StrPtr _id, StrPtr _s1, const std::vector<const ParameterDeclaration*>* _v1, StrPtr _s2)
    :StringNode(_id), s1(_s1), v1(_v1), s2(_s2){}
    
void DirectDeclarator::print(std::ostream& dst)const{
    dst << *id;
    if(s1 != NULL) dst << *s1;
    if(v1 != NULL){
        for(int i(0); i < v1->size(); ++i){
            v1->at(i)->printPy(dst);
            if(i < v1->size() - 1) dst << ",";
        }
    }
    if(s2 != NULL) dst << *s2;
}

void DirectDeclarator::printPy(std::ostream& dst, int depth)const{
    for(int i(0); i < depth; ++i) dst << "\t";
    print(dst);
}

void DirectDeclarator::printMips(std::ostream& dst, Frame* framePtr, Type type)const{
    //Set the default value for an unitialized variable to zero
    framePtr->store(dst, "$zero", *id);
}

std::string DirectDeclarator::getId()const{
    return std::string(*id);
}

const std::vector<const ParameterDeclaration*>* DirectDeclarator::getParameterList()const{
    return v1;
}

void DirectDeclarator::addGlobalMips(std::ostream& dst)const{
    dst << "\t.comm\t" << *id << ",4,4" << std::endl;
}
