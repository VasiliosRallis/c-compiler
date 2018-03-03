#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

#include "ast_real/ast/node.hpp"
#include "ast_real/ast/expr.hpp"
#include "ast_real/compiler/frame.hpp"
#include "ast_real/ast/directDeclarator.hpp"

extern std::vector<std::string> g_variables;
extern std::vector<std::string> g_mips_var;

class InitDeclarator : public StringNode{
private:
    const Expr* asgnExpr;
public:
    InitDeclarator(StrPtr _id, const Expr* _asgnExpr)
        :StringNode(_id), asgnExpr(_asgnExpr) {}
    
    virtual void print(std::ostream& dst) const override{
        dst << *id;
	    dst << "=";      
        asgnExpr->print(dst);
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{
        for(int i(0); i < depth; ++i) dst << "\t";
        dst << *id;
        dst << "=";
        asgnExpr->printPy(dst);
    }
    
    virtual void printMips(std::ostream& dst, Frame* framePtr)const override{
        //Generate a unique name
        std::string destName = makeName();
        //Ask the expression to evaluate itself and store its value in the frame, with destName as it's identifier
        asgnExpr->printMipsE(dst, destName, framePtr);
        //Temporary store the identifier in $t1
        framePtr->load(dst, "$t0", destName);
        //Store it in the frame
        framePtr->store(dst, "$t0", *id, true);
    }
    
    void addGlobalMips(std::ostream& dst)const{
        dst << "\t.globl\t" << *id << std::endl;
        dst << "\t.data" << std::endl;
        dst << "\t.align 2" << std::endl;
        dst << "\t.size\t" << *id << ", 4" << std::endl;
        dst << *id << ":" << std::endl;
        dst << "\t.word\t";
        asgnExpr->printPy(dst);
        dst << std::endl;
        
        g_mips_var.push_back(*id);
    }
};

class Declaration : public Node {
private:
    VectorPtr declrspecList;    
    VectorPtr initdeclrList;
public:
    Declaration(VectorPtr _declrspecList, VectorPtr _initdeclrList)
        : declrspecList(_declrspecList), initdeclrList(_initdeclrList) {}
    
    virtual void print(std::ostream& dst) const override{
	    if(initdeclrList!=NULL){        
		    for(int i(0); i < declrspecList->size(); ++i){
            	    	declrspecList->at(i)->print(dst);
            	    	dst << " ";
            	}
	        
		    for(int i(0); i < initdeclrList->size(); ++i){       
            		initdeclrList->at(i)->print(dst);
			    if(i < initdeclrList->size() - 1) dst << ",";
	        	}
	    }
        dst << ";";
    }
    virtual void printPy(std::ostream& dst, int depth = 0) const override{
	// Check if its null ? Does int; have a valid translation to python?        
	    for(int i(0); i < initdeclrList->size(); ++i){
                if(dynamic_cast<const DirectDeclarator*>(initdeclrList->at(i))){
                    initdeclrList->at(i)->printPy(dst, depth);
                    dst << "=0\n";
                }
                else if(dynamic_cast<const InitDeclarator*>(initdeclrList->at(i))){
                    initdeclrList->at(i)->printPy(dst, depth);
                    dst << "\n";
                }
                else{
                    std::cerr << "Not supported yet!" << std::endl;
                }
       }
    }
    
    void addGlobal()const{
        if(initdeclrList != NULL){
            for(int i(0); i < initdeclrList->size(); ++i){
                if(dynamic_cast<const StringNode*>(initdeclrList->at(i))){
                    dynamic_cast<const StringNode*>(initdeclrList->at(i))->addGlobal();
                }
                else if(dynamic_cast<const InitDeclarator*>(initdeclrList->at(i))){
                   dynamic_cast<const InitDeclarator*>(initdeclrList->at(i))->addGlobal();
                }
            }
        }
    }
    
    void printMips(std::ostream& dst, Frame* framePtr = NULL)const override{
        if(initdeclrList != NULL){
            for(int i(0); i < initdeclrList->size(); ++i){
                initdeclrList->at(i)->printMips(dst, framePtr);
                if (i < initdeclrList->size() - 1) dst << "\n";
            }
        }
    }
    
    void addGlobalMips(std::ostream& dst)const{
        if(initdeclrList != NULL){
            for(int i(0); i < initdeclrList->size(); ++i){
                if(dynamic_cast<const DirectDeclarator*>(initdeclrList->at(i))){
                    dynamic_cast<const DirectDeclarator*>(initdeclrList->at(i))->addGlobalMips(dst);
                }
                else if(dynamic_cast<const InitDeclarator*>(initdeclrList->at(i))){
                   dynamic_cast<const InitDeclarator*>(initdeclrList->at(i))->addGlobalMips(dst);
                }
            }
        }
    }
    
    
};

class Block: public Node{
private:
	VectorPtr declrList;
	VectorPtr statementList;    
    
public:
    Block(VectorPtr _declrList, VectorPtr _statementList)
        : declrList(_declrList), statementList(_statementList) {}
    virtual void print(std::ostream& dst) const override{
	    dst << "{";
	    if(declrList != NULL){
            for(int i(0); i < declrList->size(); ++i){
                declrList->at(i)->print(dst);
      	    }
      	}
	    if(statementList != NULL){
            for(int i(0); i < statementList->size(); ++i){
                statementList->at(i)->print(dst);
        	}
        }
	    dst << "}";
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{
        if(declrList != NULL){
            for(int i(0); i < declrList->size(); ++i){
                declrList->at(i)->printPy(dst, depth);
                dst << "\n";
            }
        }
        if(statementList != NULL){
            for(int i(0); i < statementList->size(); ++i){
                statementList->at(i)->printPy(dst, depth);
                dst << "\n";
            }
        }
    }
    
    virtual void printPyG(std::ostream& dst, int depth = 0) const{
        for(int i(0); i < g_variables.size(); ++i){
            for(int j(0); j < depth ; ++j) dst << "\t";
            dst << "global " << g_variables.at(i) << "\n";
        }
        if(declrList != NULL){
            for(int i(0); i < declrList->size(); ++i){
                declrList->at(i)->printPy(dst, depth);
                dst << "\n";
            }
        }
        if(statementList != NULL){
            for(int i(0); i < statementList->size(); ++i){
                statementList->at(i)->printPy(dst, depth);
                dst << "\n";
            }
                
        }
    }
    
    virtual void printMips(std::ostream& dst, Frame* framePtr = NULL)const override{
            if(declrList != NULL){
                for(int i(0); i < declrList->size(); ++i){
                    declrList->at(i)->printMips(dst, framePtr);
                    if(i < declrList->size() - 1) dst << "\n";
                }
            }
            if(statementList != NULL){
                for(int i(0); i < statementList->size(); ++i){
                    statementList->at(i)->printMips(dst, framePtr);
                    if(i < statementList->size() - 1) dst << "\n";
                }
            }
    }
};

class FunctionDef: public Node{
private:
	VectorPtr declrSpecList;
	const DirectDeclarator* directDeclarator;   
	const Block* block;
    
public:
    FunctionDef(VectorPtr _declrSpecList, const DirectDeclarator* _directDeclarator, const Block* _block)
        : declrSpecList(_declrSpecList), directDeclarator(_directDeclarator), block(_block) {}
        

    virtual void print(std::ostream& dst) const override{
        for(int i(0); i < declrSpecList->size(); ++i){
            declrSpecList->at(i)->print(dst);
            dst << " ";
        }
        directDeclarator->print(dst);
	    block->print(dst);
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{    
	    dst << "def ";
        directDeclarator->printPy(dst);
        dst << ":\n";
        block->printPyG(dst,depth+1);
    }
    
    virtual void printMips(std::ostream& dst, Frame* framePtr = NULL)const override{
        dst << "\t.set noreorder\n";        
        dst << "\t.text\n";
        dst << "\t.align 2 \n";
        dst << "\t.globl ";
        dst << directDeclarator->getId() << "\n";
        dst << directDeclarator->getId() << ":\n";
        
        Frame frame(dst, directDeclarator);
      
        block->printMips(dst, &frame);
        
        frame.clean(dst);
    }    
};

class Program: public Node{
private:
    NodePtr program;
    NodePtr basicProgram;

public:
    Program(NodePtr _program, NodePtr _basicProgram)
        :program(_program), basicProgram(_basicProgram){}
        
    virtual void print(std::ostream& dst) const override{
	    program->print(dst);
	    dst << " ";
	    basicProgram->print(dst);
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{
        if(dynamic_cast<const Declaration*>(program)){
            dynamic_cast<const Declaration*>(program)->addGlobal();
        }
	    program->printPy(dst);
	    dst << "\n";
	    basicProgram->printPy(dst);
	    if(dynamic_cast<const Declaration*>(basicProgram))
	        dynamic_cast<const Declaration*>(basicProgram)->addGlobal();
    }
    
    virtual void printMips(std::ostream& dst, Frame* framePtr = NULL)const override{
        if(dynamic_cast<const Declaration*>(program)){
            dynamic_cast<const Declaration*>(program)->addGlobalMips(dst);
        }else{
            program->printMips(dst);
        }
        if(dynamic_cast<const Declaration*>(basicProgram)){
            dynamic_cast<const Declaration*>(basicProgram)->addGlobalMips(dst);
        }else{
            basicProgram->printMips(dst);
        }
    }
};

class IntConst: public StringNode{
public:
    IntConst(StrPtr _id)
        :StringNode(_id){}
        
    virtual void print(std::ostream& dst) const override{
        dst << std::stoi((*id));
    }
    virtual void printPy(std::ostream& dst, int depth = 0) const override{
        dst << std::stoi((*id));
    }
    
    int getId()const{
        return std::stoi(*id);
    }
};

class Operator: public StringNode{
public:
    enum Type{
        MUL,
        ADD,
        SHIFT,
        RELATIONAL,
        EQUALITY,
        AND,
        EXCLUSIVE_OR,
        INCLUSIVE_OR,
        LOGICAL_AND,
        LOGICAL_OR,
        ASSIGN
    };  
    Operator(StrPtr _id, Operator::Type _type)
        :StringNode(_id), type(_type){}
        
    virtual void print(std::ostream& dst)const override{
        dst << *id;
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override{
        if(type == LOGICAL_AND){
            dst << " and ";
        }
        else if (type == LOGICAL_OR){
            dst <<" or " ;
        }
        else{
            dst << *id ;
        }
    }

    

protected:
   Type type;
};

#endif
