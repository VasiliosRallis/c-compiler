#include "ast_real/ast/node.hpp"

#include <stdexcept>
#include <typeinfo>
#include <iostream>

void Node::printPy(std::ostream& dst, int depth)const{}

int Node::getLength()const{
    throw std::runtime_error("Called getLength() on class: " + std::string(typeid(*this).name()));
    return -1;
}

void Node::printMips(std::ostream& dst, Frame* framePtr, Type type)const{
    throw std::runtime_error("Called printMips() on class: " + std::string(typeid(*this).name()));
}

void Node::addGlobalMips(std::ostream& dst)const{
    throw std::runtime_error("Called addGlobalMips() on class: " + std::string(typeid(*this).name()));
}

std::string Node::getId()const{
    throw std::runtime_error("Called getId() on class: " + std::string(typeid(*this).name()));
}

void Node::printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr, Type type)const{
    throw std::runtime_error("Called printMipsE() on class: " + std::string(typeid(*this).name()));
}

Type Node::getType(const Frame* framePtr)const{
    throw std::runtime_error("Called getType() on class: " + std::string(typeid(*this).name()));
}

bool Node::isIdentifier()const{
    throw std::runtime_error("Called isIdentifier() on class: " + std::string(typeid(*this).name()));
}

void Node::printGMips(std::ostream& dst, Type type)const{
    throw std::runtime_error("Called printGMips() on class: " + std::string(typeid(*this).name()));
}

double Node::eval()const{
    throw("Called eval() on class: " + std::string(typeid(*this).name()));
}

bool Node::isPointer()const{
    throw("Called isAddr() on class: " + std::string(typeid(*this).name()));
}


bool isAddr(const Type type){
    if(type == Type::INT_ADDR || type == Type::FLOAT_ADDR || type == Type::DOUBLE_ADDR || type == Type::VOID_ADDR || type == Type::CHAR_ADDR || type == Type::STRING_ADDR)
        return true;
    else
        return false;
}

Type typeToAddr(const Type type){
    if(type == Type::INT) return Type::INT_ADDR;
    else if(type == Type::FLOAT) return Type::FLOAT_ADDR;
    else if(type == Type::DOUBLE) return Type::DOUBLE_ADDR;
    else if(type == Type::VOID) return Type::VOID_ADDR;
    else if(type == Type::CHAR) return Type::CHAR_ADDR;
    else if(type == Type::STRING) return Type::STRING_ADDR;
    else{
        std::cerr << "Type: " << (int)type << std::endl;
        assert(0);
    }
}

Type addrToType(const Type type){
    if(type == Type::INT_ADDR) return Type::INT;
    else if(type == Type::FLOAT_ADDR) return Type::FLOAT;
    else if(type == Type::DOUBLE_ADDR) return Type::DOUBLE;
    else if(type == Type::VOID_ADDR) return Type::VOID;
    else if(type == Type::CHAR_ADDR) return Type::CHAR;
    else if(type == Type::STRING_ADDR) return Type::STRING;
    else assert(0);
}
