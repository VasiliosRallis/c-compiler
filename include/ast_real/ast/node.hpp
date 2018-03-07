#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>
#include <vector>

class Node;
class Frame;

typedef const Node* NodePtr;
typedef std::vector<NodePtr>* VectorPtr;
typedef const std::string* StrPtr;

extern NodePtr parseAST(const char* file);

enum class Type{
    NOTHING,
    INT,
    FLOAT,
    DOUBLE
};

class Node{
public:
    virtual ~Node(){};
    virtual void print(std::ostream& dst) const = 0;
    virtual void printPy(std::ostream& dst, int depth = 0)const;
    virtual void printMips(std::ostream& dst, Frame* framePtr = NULL    )const;
    
    virtual int getLength()const;
};


#endif
