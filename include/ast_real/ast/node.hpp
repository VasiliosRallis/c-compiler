#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>

#include "compiler/frame.hpp"

class Node{
public:
    virtual void print(std::ostream& dst) const = 0;
    virtual void printPy(std::ostream& dst, int depth = 0)const;
    virtual void printMips(std::ostream& dst, Frame* framePtr = NULL)const;
    
    virtual int getLength()const;
};

#endif
