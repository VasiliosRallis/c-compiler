#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <cassert>
#include <unordered_map>
#include <utility>

#include "ast_real/ast/node.hpp"
#include "ast_real/ast/expr.hpp"
#include "ast_real/compiler/frame.hpp"
#include "ast_real/ast/directDeclarator.hpp"
#include "ast_real/ast/initDeclarator.hpp"

extern std::vector<std::string> g_variables;
extern std::unordered_map<std::string, Type> g_mips_var;
extern std::unordered_map<std::string, Type> function_type;
extern std::vector<std::unordered_map<std::string, int>> enum_lib;

class DeclSpecifier: public StringNode{
private:
    const std::vector<std::pair<StrPtr, NodePtr>*>* enum_list;
    
public:
    DeclSpecifier(StrPtr _id, const std::vector<std::pair<StrPtr, NodePtr>* >* _enum_list = NULL)
        :StringNode(_id), enum_list(_enum_list){}
  
    Type getType()const{
        if(*id == "int"){return Type::INT;}
        else if(*id == "float"){return Type::FLOAT;}
        //else if(*id == "double"){return Type::DOUBLE;}
        else if(*id == "void"){return Type::VOID;}
        else if(*id == "char"){return Type::CHAR;}
        else if(*id == "enum"){
            if(enum_list != NULL){
                std::string id;
                int val;
                for(auto i = enum_list->begin(); i != enum_list->end(); ++i){
                    id = (*((*i)->first));
                    
                    //Generate the value of the enumerator
                    if((*i)->second != NULL){
                        val = (int)(*i)->second->eval();
                    
                    }else{
                        if(i == enum_list->begin()){val = 0;}
                        else{++val;}
                    
                    }
                    
                    if(enum_lib.back().find(id) != enum_lib.back().end()){
                        enum_lib.back().erase(id);
                        enum_lib.back().emplace(id, val);
                    
                    }else{
                        enum_lib.back().emplace(id, val);
                        std::cerr << "Added: " << id << " " << val << std::endl;
                        
                    }
                }
            }
            return Type::INT;
        }            
        else{assert(0);}
    }
};

class Declaration : public Node {
private:
    const std::vector<const DeclSpecifier*>* declrspecList;    
    VectorPtr initdeclrList;
public:
    Declaration(const std::vector<const DeclSpecifier*>* _declrspecList, VectorPtr _initdeclrList)
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
        //Get the type of the variable we are trying to declare;
        //Type type = declrspecList->at(0)->getType();
        
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
    
    void printMips(std::ostream& dst, Frame* framePtr, Type type = Type::ANYTHING)const override{
    Type my_type = declrspecList->at(0)->getType();
        if(framePtr == NULL){
            printGMips(dst, type);
            
        }else{
            if(initdeclrList != NULL){
                for(int i(0); i < initdeclrList->size(); ++i){
                    if(initdeclrList->at(i)->isPointer()){
                        initdeclrList->at(i)->printMips(dst, framePtr, typeToAddr(my_type));
                    }else{
                        initdeclrList->at(i)->printMips(dst, framePtr, my_type);
                    }
                    if (i < initdeclrList->size() - 1) dst << "\n";
                }
            }
        }
    }
    
    virtual void printGMips(std::ostream& dst, Type type)const override{
        Type my_type = declrspecList->at(0)->getType();
        if(initdeclrList != NULL){
            for(int i(0); i < initdeclrList->size(); ++i){
                if(initdeclrList->at(i)->isPointer()){
                    initdeclrList->at(i)->printGMips(dst, typeToAddr(my_type));
                
                }else{
                    initdeclrList->at(i)->printGMips(dst, my_type);
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
    
    virtual void printMips(std::ostream& dst, Frame* framePtr, Type type = Type::ANYTHING)const override{
            if(declrList != NULL){
                for(int i(0); i < declrList->size(); ++i){
                    declrList->at(i)->printMips(dst, framePtr);
                    if(i < declrList->size() - 1) dst << "\n";
                }
            }
            if(statementList != NULL){
                for(int i(0); i < statementList->size(); ++i){
                    if(dynamic_cast<const Block*>(statementList->at(i))){
                        framePtr->newScope();
                        statementList->at(i)->printMips(dst, framePtr);
                        framePtr->deleteScope();
                    }else{
                        statementList->at(i)->printMips(dst, framePtr);
                    }
                    if(i < statementList->size() - 1) dst << "\n";
                }
            }
    }
};

class FunctionDef: public Node{
private:
	std::vector<const DeclSpecifier*>* declrSpecList;
	const DirectDeclarator* directDeclarator;   
	const Block* block;
    
public:
    FunctionDef(std::vector<const DeclSpecifier*>* _declrSpecList, const DirectDeclarator* _directDeclarator, const Block* _block)
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
    
    virtual void printMips(std::ostream& dst, Frame* framePtr, Type type = Type::ANYTHING)const override{
        //Get the return type of the function
        Type returnType = declrSpecList->at(0)->getType();
        //Add the function types (i.e. return type and argument types) to the global map
        directDeclarator->insertFTypes(returnType);

        dst << "\t.set noreorder\n";        
        dst << "\t.text\n";
        dst << "\t.align 2 \n";
        dst << "\t.globl ";
        dst << directDeclarator->getId() << "\n";
        dst << directDeclarator->getId() << ":" << std::endl;
        
        Frame frame(dst, directDeclarator);
        
        //This is effectively stored twice but it's fine
        frame.storeType("return", declrSpecList->at(0)->getType());
        
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
        
    virtual void print(std::ostream& dst)const override{
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
    
    virtual void printMips(std::ostream& dst, Frame* framePtr, Type type = Type::ANYTHING)const override{
        if(dynamic_cast<const Declaration*>(program)){
            program->printGMips(dst, Type::ANYTHING);
            
        }else{
            program->printMips(dst, NULL);
            
        }
        if(dynamic_cast<const Declaration*>(basicProgram)){
            basicProgram->printGMips(dst, Type::ANYTHING);
            
        }else{
            basicProgram->printMips(dst, NULL);
        }
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
