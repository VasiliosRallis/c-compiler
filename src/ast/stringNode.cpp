#include "ast_real/ast/stringNode.hpp"

StringNode::StringNode(const std::string* _id) 
    :id(_id){}
    
void StringNode::print(std::ostream& dst)const{
    dst << *id;
}

void StringNode::printPy(std::ostream& dst, int depth)const{
    for(int i(0); i < depth; ++i) dst << "\t";
    dst << *id;
}

void StringNode::addGlobal()const{
    std::string global = *id;
    g_variables.push_back(global);
}

std::string StringNode::getId()const{
    return std::string(*id);
}
