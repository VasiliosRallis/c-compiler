#include "ast_real/ast/stringNode.hpp"

#include <cctype>
#include <cassert>

#include "ast_real/compiler/frame.hpp"

extern std::vector<std::unordered_map<std::string, int> > enum_lib;

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
            return Type::FLOAT; //Used to be double here
        }
    
    }else if(isalpha(id->front()) || id->front() == '_'){
        if(enum_lib.back().find(*id) != enum_lib.back().end()){
            return Type::INT;
        
        }else{
            return framePtr->loadType(*id);
        }
        
    }else{assert(0);}
}

bool StringNode::isIdentifier()const{
    if(isalpha(id->front()) || id->front() == '_') return true;
    else return false;
}

double StringNode::eval()const{
    Type myType = getType(NULL);
    
    if(myType == Type::INT || myType == Type::FLOAT){   //Used to be double here
        return std::stod(*id);
        
    }else if(myType == Type::CHAR){
        int ascii = (*id)[1];
        return ascii;
    
    //This should never happen    
    }else{assert(0);}
}

std::string StringNode::getString()const{
    if(getType(NULL) != Type::STRING){
        assert(0);
        
    }else{return *id;}
}
