#ifndef STRING_NODE_HPP
#define STRING_NODE_HPP

#include <string>
#include <vector>

class StringNode;

#include "ast_real/ast/node.hpp"

extern std::vector<std::string> g_variables;
typedef const std::string* StrPtr;

class StringNode: public Node{
protected:
    const std::string* id;
    
public:
    StringNode(const std::string* _id);
        
    virtual void print(std::ostream& dst)const override;
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override;
    
    void addGlobal()const;
    
    std::string getId()const;
    
    virtual Type getType(const Frame* framePtr)const override;
    
    virtual bool isIdentifier()const override;
    
    double eval()const override;
};

#endif
