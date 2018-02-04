#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>

class VarType
    : public Expression
{
private:
    std::string id;
public:
    VarType(const std::string &_id)
        : id(_id)
    {}

    const std::string getId() const
    { return id; }

    virtual void print(std::ostream &dst) const override
    {
        dst<<id;
    }
  
};

class VarDeclr
    : public Expression
{
private:
	const Expression* type;    
	std::string id;
    
public:
    VarDeclr(const Expression* _type, const std::string &_id )
        : type(_type), id(_id) 
    {}

    const std::string getId() const
    { return id; }

    virtual void print(std::ostream &dst) const override
    {
	
        type->print(dst);
	dst << " ";
	dst << id;
	dst << ";";
    }
  
};

class DeclrList
    : public Expression
{
private:
	const Expression* declrlist;    
	const Expression* declr;
    
public:
    DeclrList(const Expression* _declrlist, const Expression* _declr )
        : declrlist(_declrlist), declr(_declr) 
    {}


    virtual void print(std::ostream &dst) const override
    {
	
        declrlist->print(dst);
	declr ->print(dst);
	
    }

};

class Block
    : public Expression
{
private:
	const Expression* declrlist;    
    
public:
    Block(const Expression* _declrlist )
        : declrlist(_declrlist) 
    {}


    virtual void print(std::ostream &dst) const override
    {
	dst << "{";
        declrlist->print(dst);
	dst << "}";
	
    }

};

class FunctionDef
    : public Expression
{
private:
	const Expression* type;
	std::string id;    
	const Expression* block;
    
public:
    FunctionDef(const Expression* _type, std::string _id, const Expression* _block )
        : type(_type), id(_id), block(_block) 
    {}


    virtual void print(std::ostream &dst) const override
    {
	type->print(dst);
        dst << " " << id << "()";
	block ->print(dst);
	
    }

};





#endif
