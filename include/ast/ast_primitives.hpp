#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

extern std::vector<std::string> g_variables;

class InitDeclarator : public Node {
private:
    NodePtr directDeclarator;    
    NodePtr asgnExpr;
public:
    InitDeclarator(NodePtr _directDeclarator, NodePtr _asgnExpr)
        : directDeclarator(_directDeclarator), asgnExpr(_asgnExpr) {}
    
    virtual void print(std::ostream& dst) const override{
        directDeclarator->print(dst);
	    dst << "=";      
        asgnExpr->print(dst);
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{
        directDeclarator->printPy(dst, depth);
        dst << "=";
        asgnExpr->printPy(dst);
    }
    
    void addGlobal()const{
        static_cast<const StringNode*>(directDeclarator)->addGlobal();
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
            if(dynamic_cast<const StringNode*>(initdeclrList->at(i))){
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
        for(int i(0); i < initdeclrList->size(); ++i){
            if(dynamic_cast<const StringNode*>(initdeclrList->at(i))){
                dynamic_cast<const StringNode*>(initdeclrList->at(i))->addGlobal();
            }
            else if(dynamic_cast<const InitDeclarator*>(initdeclrList->at(i))){
               dynamic_cast<const InitDeclarator*>(initdeclrList->at(i))->addGlobal();
            }
        }
        
    }      
};


class DirectDeclarator: public Node{
private:
    NodePtr n1;
    StrPtr s1;
    VectorPtr v1;
    StrPtr s2;

public:
    DirectDeclarator(NodePtr _n1, StrPtr _s1, VectorPtr _v1, StrPtr _s2)
        :n1(_n1), s1(_s1), v1(_v1), s2(_s2){}
        
    virtual void print(std::ostream& dst)const override{
        n1->print(dst);
        dst << *s1;
        if(v1 != NULL){
            for(int i(0); i < v1->size(); ++i){
                v1->at(i)->printPy(dst);
                if(i < v1->size() - 1) dst << ",";
            }
        }
        dst << *s2;
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override{
        print(dst);
    }
};

class ParameterDeclaration: public Node{
private:
    NodePtr n1;
    NodePtr n2;

public:
    ParameterDeclaration(NodePtr _n1, NodePtr _n2)
        :n1(_n1), n2(_n2){}
    
    virtual void print(std::ostream& dst)const override{
        n1->print(dst);
        dst << " ";
        if(n2 != NULL) n2->print(dst);
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override{
        n2->print(dst);
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
};

class FunctionDef: public Node{
private:
	VectorPtr declrSpecList;
	NodePtr directDeclarator;   
	NodePtr block;
    
public:
    FunctionDef(VectorPtr _declrSpecList, NodePtr _directDeclarator, NodePtr _block)
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
        static_cast<const Block*>(block)->printPyG(dst,depth+1);
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
