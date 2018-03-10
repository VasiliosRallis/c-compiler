#include "ast_real/ast/stringNode.hpp"

#include <cctype>
#include <cassert>

#include "ast_real/compiler/frame.hpp"

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

Type StringNode::getType(const Frame* framePtr)const{
    if(id->front() == '\'' && id->back() == '\''){
        return Type::CHAR;
    
    }else if(id->front() == '"' && id->back() == '"'){
        return Type::STRING;
    
    //If the first character is a digit and that is no '.' in the string assume it's a float    
    }else if(isdigit(id->front())){
        if(id->find('.') == std::string::npos){
            return Type::INT;
        
        }else{
            return Type::FLOAT;
        }
    
    }else if(isalpha(id->front()) || id->front() == '_'){
        return framePtr->loadType(*id);
    
    }else{assert(0);}
}

bool StringNode::isIdentifier()const{
    if(isalpha(id->front()) || id->front() == '_') return true;
    else return false;
}
