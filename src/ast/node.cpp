#include "ast_real/ast/node.hpp"

#include <stdexcept>
#include <typeinfo>

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
