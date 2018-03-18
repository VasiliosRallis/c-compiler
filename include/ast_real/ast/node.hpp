#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>
#include <vector>
#include <cassert>

class Node;
class Frame;

typedef const Node* NodePtr;
typedef std::vector<NodePtr>* VectorPtr;
typedef const std::string* StrPtr;

extern NodePtr parseAST(const char* file);

enum class Type{
    ANYTHING,
    INT,
    FLOAT,
    //DOUBLE,
    VOID,
    CHAR,
    STRING,
    INT_ADDR,
    FLOAT_ADDR,
    //DOUBLE_ADDR,
    VOID_ADDR,
    CHAR_ADDR,
    STRING_ADDR
};

bool isAddr(const Type type);

Type typeToAddr(const Type type);

Type addrToType(const Type type);

class Node{
public:
    virtual ~Node(){};
    virtual void print(std::ostream& dst) const = 0;
    virtual void printPy(std::ostream& dst, int depth = 0)const;
    
    //This only reason printMips() is given a Type argument is so that the Declaration class can pass
    //the type to the DirectDeclarator class or the InitDeclarator class
    virtual void printMips(std::ostream& dst, Frame* framePtr, Type type = Type::ANYTHING)const;
    
    //Functions that just throw exception
    virtual void printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr, Type type)const;
    virtual int getLength()const;
    virtual void addGlobalMips(std::ostream& dst)const;
    virtual std::string getId()const;
    virtual Type getType(const Frame* framePtr)const;
    virtual bool isIdentifier()const;
    virtual void printGMips(std::ostream& dst, Type type)const;
    virtual double eval()const;
    virtual bool isPointer()const;
    virtual bool isDereference()const;
    virtual std::string getString()const;
};


#endif
