#ifndef ast_expr_hpp
#define ast_expr_hpp    
#include "ast_real/ast/postfixExpr.hpp"
#include "ast_real/compiler/frame.hpp"
#include <cassert>

extern std::vector<std::string> endPrint;

class PrimaryExpr: public Expr{
private:
    NodePtr expr; //This can be everything a stringConst an intConst (might lead to future issues)

public:
    PrimaryExpr(NodePtr _expr)
        :expr(_expr){}
        
    virtual void print(std::ostream& dst)const override{
        if(dynamic_cast<const Expr*>(expr)){
            dst << "(";
            expr->print(dst);
            dst << ")";
        }else{
            expr->print(dst);
        }   
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{
        if(dynamic_cast<const Expr*>(expr)){
            dst << "(";
            expr->printPy(dst);
            dst << ")";
        }else{
            expr->printPy(dst);
        }   
    }
	
	virtual void printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr, Type type)const override{
        if(dynamic_cast<const StringNode*>(expr)){
            std::string id = expr->getId();
	        Type myType = expr->getType(framePtr);
	        
	        if(type == Type::ANYTHING) type = expr->getType(framePtr);
	        
	        if(type == Type::INT){
                if(expr->isIdentifier()){
	                if(myType == Type::INT || myType == Type::CHAR || isAddr(myType)){
	                    framePtr->load(dst, "$t0", id);
	                    framePtr->store(dst, "$t0", destName, type);
	                
	                }else if(myType == Type::FLOAT || myType == Type::DOUBLE){
	                    framePtr->load(dst, "$f0", id);
	                    TypeConv::convert(dst, type, myType, "$t0", "$f0");
	                    framePtr->store(dst, "$t0", destName, type);
	                
	                }else{assert(0);}
	                
	            }else if(myType == Type::INT){
	                dst << "li $t0, " << id << std::endl;
	                framePtr->store(dst, "$t0", destName, Type::INT);
	            
	            }else if(myType == Type::CHAR){
	                int ascii;
	                
	                //Handle escape sequence (e.g. /000 = NULL)
	                if(id[1] == '\\'){
	                    if(id[2] == 'x') ascii = std::stoi(id.substr(3), nullptr, 16);
	                        
	                    else ascii = std::stoi(id.substr(3), nullptr, 8);
	                        
	                }else{
	                    ascii = (int)id[1];
                    }
	                dst << "li $t0, " << ascii << std::endl;
	                framePtr->store(dst, "$t0", destName, Type::CHAR);
	            
	            }else{assert(0);}

	        }else if(type == Type::CHAR){
	            if(expr->isIdentifier()){
	                if(myType == Type::CHAR){
	                    framePtr->load(dst, "$t0", id);
	                    framePtr->store(dst, "$t0", destName, Type::CHAR);
	                    
	                }else{assert(0);}
	                
	                
	            }else if(myType == Type::CHAR){
	                int ascii;
	                
	                //Handle escape sequence (e.g. /000 = NULL)
	                if(id[1] == '\\'){
	                    if(id[2] == 'x') ascii = std::stoi(id.substr(3), nullptr, 16);
	                        
	                    else ascii = std::stoi(id.substr(3), nullptr, 8);
	                        
	                }else{
	                    ascii = (int)id[1];
                    }
	                dst << "li $t0, " << ascii << std::endl;
	                framePtr->store(dst, "$t0", destName, Type::CHAR);
	                
	            }else{assert(0);}
	            
	        }else if(isAddr(type)){
	            if(expr->isIdentifier()){
                    if(myType == Type::CHAR || myType == Type::INT || isAddr(myType)){	                    
	                    framePtr->load(dst, "$t0", id);
	                    framePtr->store(dst, "$t0", destName, type);
	                
	                }else{assert(0);}
	                
	            }else{
	                if(myType == Type::INT){
	                    dst << "li $t0, " << id << std::endl;
	                    framePtr->store(dst, "$t0", destName, type);
	                    
	                }else if(myType == Type::CHAR){
	                    int ascii = (int)id[1];
	                    dst << "li $t0, " << ascii << std::endl;
	                    framePtr->store(dst, "$t0", destName, type);
	                    
	                }else{assert(0);}   
	            }            
	        }else if(type == Type::DOUBLE){{assert(0);}
	                
	        }else if(type == Type::FLOAT){
	            if(expr->isIdentifier()){
	                if(myType == Type::CHAR || myType == Type::INT || isAddr(myType)){
	                    framePtr->load(dst, "$t0", id);
	                    TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f0", "$t0");
	                    framePtr->store(dst, "$f0", destName, type);
	                    
	                }else if(myType == Type::FLOAT){
                        framePtr->load(dst, "$f0", id);
                        framePtr->store(dst, "$f0", destName, type);
                        
                    }else if(myType == Type::DOUBLE){
                        framePtr->load(dst, "$f0", id);
                        TypeConv::convert(dst, Type::FLOAT, Type::DOUBLE, "$f0", "$f0");
                        framePtr->store(dst, "$f0", destName, type);
                        
                    }else{assert(0);}
                    
                }else{
                    if(myType == Type::INT || myType == Type::CHAR){
                        dst << "li $t0, " << expr->eval() << std::endl;
                        TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f0", "$t0");
                        framePtr->store(dst, "$f0", destName, type);
                    
                    //Since it is a constant, it has to be double (i.e. not float)    
                    }else if(myType == Type::DOUBLE){
                        //Set up for label of float constant
                        endPrint.push_back("\t.rdata\n");
                        endPrint.push_back("\t.align 2\n");
                        
                        //Create the label
                        std::string label = makeName("FLOAT");
                        endPrint.push_back(label + ":\n");
                        endPrint.push_back("\t.float\t" + std::to_string((float)expr->eval()) + "\n");
                        
                        //Load the float
                        dst << "lui $t0, %hi(" + label + ")\n";
                        dst << "lwc1 $f0, %lo(" + label + ")($t0)" << std::endl;
                        dst << "nop" << std::endl;
                        
                        framePtr->store(dst, "$f0", destName, type);
                        
                    }else{assert(0);}
                }        
	        }else{assert(0);}
	        
        }else if(dynamic_cast<const Expr*>(expr)){
            expr->printMipsE(dst, destName, framePtr, type);
                    
        }else{assert(0);}
        
    }
    
    double eval()const override{
        if(dynamic_cast<const StringNode*>(expr)){
            std::string id = expr->getId();
            Type myType = expr->getType(NULL);
            
            if(expr->isIdentifier()){
                //We need it to be a constant in global declarations
                assert(0);
                
            }else{ 
                if(myType == Type::INT || myType == Type::FLOAT || myType == Type::DOUBLE){
                    return std::stod(id);
                
                }else if(myType == Type::CHAR){
	                int ascii;
	                
	                //Handle escape sequence (e.g. /000 = NULL)
	                if(id[1] == '\\'){
	                    if(id[2] == 'x') ascii = std::stoi(id.substr(3), nullptr, 16);
	                        
	                    else ascii = std::stoi(id.substr(3), nullptr, 8);
	                        
	                }else{
	                    ascii = (int)id[1];
                    }
	                
	                return ascii;
                    
                }else{assert(0);}
            }
        
        }else if(dynamic_cast<const Expr*>(expr)){
            return expr->eval();
        
        }else{assert(0);}
    }
                     
    virtual std::string getId()const override{
        return expr->getId();
    }
    
    
    virtual Type getType(const Frame* framePtr)const override{
        return expr->getType(framePtr);
    }
    
    bool isIdentifier()const override{
        return expr->isIdentifier();
    }
    
};

class UnaryExpr: public Expr{
private:
    StrPtr oper;
    const Expr* postfixExpr;
public:
    UnaryExpr(StrPtr _oper, const Expr* _postfixExpr)
        :oper(_oper), postfixExpr(_postfixExpr){}
        
    virtual void print(std::ostream& dst) const override{
        dst << *oper;
        postfixExpr->print(dst);
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override{
        dst << *oper;
        postfixExpr->printPy(dst);        
    }
    
    virtual void printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr, Type type)const override{
        Type destType = postfixExpr->getType(framePtr);
        
        if(*oper == "-"){
            if(destType == Type::INT){            
                postfixExpr->printMipsE(dst, destName, framePtr, destType);
	            framePtr->load(dst, "$t0", destName);
                dst << "sub $t0, $0, $t0" << std::endl;     //Negate variable value stored in $t0

                if(type == Type ::INT){ // Thing above asked for an int we have to convert our value to an int and store into stack 
                    framePtr->store(dst, "$t0", destName, type);
                }
                else{ // Thing above asked for Float we return our value as a float
                    TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f0", "$t0");                                  
                    framePtr->store(dst, "$f0", destName, type); 
                }
            }
            else{
                postfixExpr->printMipsE(dst, destName, framePtr, destType);
                framePtr->load(dst, "$f0", destName);
                dst << "mfc1 $t0, $f0" << std :: endl;
                dst << "li $t1, 0x80000000" << std::endl; // Negation of float = Invert first bit by xoring with first bit being 1 and the rest 0
                dst << "xor $t0, $t0, $t1" << std::endl;             
                dst << "mtc1 $t0, $f0" << std::endl;

                if(type == Type ::INT){ // Thing above asked for an int we have to convert our value to an int and store into stack 
                    TypeConv::convert(dst, Type::INT, Type::FLOAT, "$t0", "$f0");
                    framePtr->store(dst, "$t0", destName, type);
                }
                else{ // Thing above asked for Float we return our value as a float                                  
                    framePtr->store(dst, "$f0", destName, type); 
                }           
            }
        }
        else if(*oper == "+"){
            if(destType == Type::INT){                
                postfixExpr->printMipsE(dst, destName, framePtr, destType);
             
                if(type == Type ::INT){ // Thing above asked for an int we have to convert our value to an int and store into stack 
                    framePtr->store(dst, "$t0", destName, type);
                }
                else{ // Thing above asked for Float we return our value as a float
                    TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f0", "$t0");                                  
                    framePtr->store(dst, "$f0", destName, type); 
                }               
            }
            else{
                postfixExpr->printMipsE(dst, destName, framePtr, destType);
                framePtr->load(dst, "$f0", destName);

                if(type == Type ::INT){ // Thing above asked for an int we have to convert our value to an int and store into stack
                    TypeConv::convert(dst, Type::INT, Type::FLOAT, "$t0", "$f0");
                    framePtr->store(dst, "$t0", destName, type);
                }
                else{ // Thing above asked for Float we return our value as a float                                  
                    framePtr->store(dst, "$f0", destName, type); 
                }
            }
        }
        else if (*oper == "!"){
            if(destType == Type::INT){
                postfixExpr->printMipsE(dst, destName, framePtr, destType);
                framePtr->load(dst, "$t0", destName);                    	
                dst << "sltu $t0, $t0, 1" << std::endl;
	            dst << "andi $t0, $t0, 0x00ff" << std::endl;
                
                if(type == Type ::INT){ // Thing above asked for an int we have to convert our value to an int and store into stack 
                    framePtr->store(dst, "$t0", destName, type);
                }
                else{ // Thing above asked for Float we return our value as a float
                    TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f0", "$t0");                                  
                    framePtr->store(dst, "$f0", destName, type); 
                }
            }
            else{
                std::string branchlabel = std::string("$" + makeName("BRANCH"));
                std::string branchlabel2 = std::string("$" + makeName("BRANCH"));

                postfixExpr->printMipsE(dst, destName, framePtr, destType);
                framePtr->load(dst, "$f0", destName);
                dst << "mfc1 $t0, $f0" << std :: endl;
                dst << "nop" << std::endl;
                
                dst << "mtc1 $0, $f2" << std::endl;     // $f2 has 0 in floating point representation NEED TO TEST for -0 case
                dst << "nop" << std::endl;
                dst << "c.eq.s $f0, $f2" << std::endl;
                dst << "nop" << std::endl;
                dst << "bc1f " << branchlabel << std::endl;
                dst << "nop" << std::endl << std::endl;
                
                dst << "li $t1, 1" << std::endl;
                dst << "mtc1 $t1, $f4" << std::endl;
                dst << "cvt.s.w $f4, $f4" << std::endl; //   $f4 has 1 in fp representation.
                dst << "mov.s $f0, $f4" << std::endl;
	            dst << "b " << branchlabel2 << std::endl;
                dst << "nop" << std::endl << std::endl;

                dst << branchlabel << ":" << std::endl;
	            dst << "mtc1 $0, $f0" << std::endl; 
	            dst << "nop" << std::endl ;
                    
                dst << branchlabel2 << ":" << std::endl;
            
                if(type == Type ::INT){ // Thing above asked for an int we have to convert our value to an int and store into stack
                    TypeConv::convert(dst, Type::INT, Type::FLOAT, "$t0", "$f0");
                    framePtr->store(dst, "$t0", destName, type);
                }
                else{ // Thing above asked for Float we return our value as a float                                  
                    framePtr->store(dst, "$f0", destName, type); 
                }       
            }
        }
        else if(*oper == "~"){
            if(destType == Type::INT){
                postfixExpr->printMipsE(dst,destName,framePtr, destType);
                framePtr->load(dst, "$t0", destName);                    	
                dst << "nor  $t0, $t0, $0" << std::endl;

                if(type == Type::INT){ // Thing above asked for an int we have to convert our value to an int and store into stack 
                    framePtr->store(dst, "$t0", destName, type);
                }
                else{ // Thing above asked for Float we return our value as a float
                    TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f0", "$t0");                                  
                    framePtr->store(dst, "$f0", destName, type);
                } 
            }
            else{
                throw std::runtime_error("Invalid Unary Operation of BITWISE NOT " + *oper + " on Non-Integer Types");
            }           
        }
        else if(*oper == "++"){
            if(destType == Type ::INT){            
                postfixExpr->printMipsE(dst, destName, framePtr, destType);
	            framePtr->load(dst, "$t0", destName);
                dst << "addi $t0, $t0, 1" << std::endl;     //Increment value before storing it back
	            
                if(type == Type::INT){
                    framePtr->store(dst, "$t0", destName, type);
                    if(dynamic_cast<const PrimaryExpr*>(postfixExpr)){
                        std::string id = postfixExpr->getId();
                        framePtr->store(dst, "$t0", id, destType);      // Should it be destType instead of type here?
                    }
                }
                else{
                    TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f0", "$t0");                                  
                    framePtr->store(dst, "$f0", destName, type);
                    if(dynamic_cast<const PrimaryExpr*>(postfixExpr)){
                        std::string id = postfixExpr->getId();
                        framePtr->store(dst, "$t0", id, destType);      // Should it be destType instead of type here?
                    } 
                }
            }

            else{
                postfixExpr->printMipsE(dst, destName, framePtr, destType);
                framePtr->load(dst, "$f0", destName);
                TypeConv::convert(dst, Type::INT, Type::FLOAT, "$t0", "$f0");
                dst << "addi $t0, $t0, 1" << std::endl;     //Increment value before storing it back
                
                if(type == Type::INT){
                    framePtr->store(dst, "$t0", destName, type);
                    if(dynamic_cast<const PrimaryExpr*>(postfixExpr)){
                        TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f0", "$t0");
                        std::string id = postfixExpr->getId();
                        framePtr->store(dst, "$f0", id, destType);      // Should it be destType instead of type here?
                    }
                }
                else{
                    TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f0", "$t0");                                  
                    framePtr->store(dst, "$f0", destName, type);
                    if(dynamic_cast<const PrimaryExpr*>(postfixExpr)){
                        TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f0", "$t0");
                        std::string id = postfixExpr->getId();
                        framePtr->store(dst, "$f0", id, destType);      // Should it be destType instead of type here?
                    } 
                }
            
            }
        }
        else if(*oper == "--"){
            if(destType == Type ::INT){            
                postfixExpr->printMipsE(dst, destName, framePtr, destType);
	            framePtr->load(dst, "$t0", destName);
                dst << "addi $t0, $t0, -1" << std::endl;     //Increment value before storing it back
	            
                if(type == Type::INT){
                    framePtr->store(dst, "$t0", destName, type);
                    if(dynamic_cast<const PrimaryExpr*>(postfixExpr)){
                        std::string id = postfixExpr->getId();
                        framePtr->store(dst, "$t0", id, destType);      // Should it be destType instead of type here?
                    }
                }
                else{
                    TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f0", "$t0");                                  
                    framePtr->store(dst, "$f0", destName, type);
                    if(dynamic_cast<const PrimaryExpr*>(postfixExpr)){
                        std::string id = postfixExpr->getId();
                        framePtr->store(dst, "$t0", id, destType);      // Should it be destType instead of type here?
                    } 
                }
            }

            else{
                postfixExpr->printMipsE(dst, destName, framePtr, destType);
                framePtr->load(dst, "$f0", destName);
                TypeConv::convert(dst, Type::INT, Type::FLOAT, "$t0", "$f0");
                dst << "addi $t0, $t0, -1" << std::endl;     //Increment value before storing it back
                
                if(type == Type::INT){
                    framePtr->store(dst, "$t0", destName, type);
                    if(dynamic_cast<const PrimaryExpr*>(postfixExpr)){
                        TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f0", "$t0");
                        std::string id = postfixExpr->getId();
                        framePtr->store(dst, "$f0", id, destType);      // Should it be destType instead of type here?
                    }
                }
                else{
                    TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f0", "$t0");                                  
                    framePtr->store(dst, "$f0", destName, type);
                    if(dynamic_cast<const PrimaryExpr*>(postfixExpr)){
                        TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f0", "$t0");
                        std::string id = postfixExpr->getId();
                        framePtr->store(dst, "$f0", id, destType);      // Should it be destType instead of type here?
                    } 
                }
            
            }            
        }
        else if(*oper == "&"){
            std::string id(postfixExpr->getId());
            framePtr->loadAddr(dst, "$t0", id);
            framePtr->store(dst, "$t0", destName, type);
            
        }else if(*oper == "*"){
            postfixExpr->printMipsE(dst, destName, framePtr, typeToAddr(type));
            framePtr->load(dst, "$t0", destName);
            
            Type exprType = postfixExpr->getType(framePtr);
            
            if(exprType == Type::INT_ADDR || exprType == Type::FLOAT_ADDR){
                dst << "lw $t0, 0($t0)" << std::endl;
                
            }else if(exprType == Type::CHAR_ADDR){
                dst << "lb $t0, 0($t0)" << std::endl;
                
            }else{assert(0);}
            
            framePtr->store(dst, "$t0", destName, type);
        }
    }
    
    //virtual bool isAddr()const override{
    //    if(*oper == "&") return true;
    //    else return false;
    //}
    
    virtual std::string getId()const override{
        return postfixExpr->getId();
    }
    
    Type getType(const Frame* framePtr)const override{
        if(*oper == "&"){; 
            if(framePtr == NULL) return typeToAddr(g_mips_var.at(getId()));
            else return typeToAddr(postfixExpr->getType(framePtr));
            
        }else if(*oper == "*"){
            if(framePtr == NULL) return addrToType(g_mips_var.at(getId()));
            else return addrToType(postfixExpr->getType(framePtr));
           
        }else if(*oper == "-" || *oper == "!" || *oper == "+" || *oper == "++" || *oper == "--" || *oper == "~"){
            return postfixExpr->getType(framePtr);
        
            
        }else{assert(0);}
    }
    
    bool isIdentifier()const override{
        return postfixExpr->isIdentifier();
    }
    
    double eval()const override{
        if(*oper == "+"){
            return postfixExpr->eval();
            
        }else if(*oper == "-"){
            return -postfixExpr->eval();
            
        }else{assert(0);}
    }
    
    bool isDereference()const{
        if(*oper == "*") return true;
        else return false;
    }
    
};

class ConditionalExpr: public Expr{
private:
    NodePtr logicalOrExpr;
    NodePtr expr;
    NodePtr conditionalExpr;

public:
    ConditionalExpr(NodePtr _logicalOrExpr, NodePtr _expr, NodePtr _conditionalExpr)
        :logicalOrExpr(_logicalOrExpr), expr(_expr), conditionalExpr(_conditionalExpr){}
        
    virtual void print(std::ostream& dst)const override{
        logicalOrExpr->print(dst);
        dst << " ? ";
        expr->print(dst);
        dst << " : ";
        conditionalExpr->print(dst);
    }
    virtual void printPy(std::ostream& dst, int depth = 0)const override{}
};

class BinaryOperation: public Expr{
private:
    const Expr* operand1;
    const Operator* oper;
    const Expr* operand2;
    
public:
    BinaryOperation(const Expr* _operand1, const Operator* _oper, const Expr* _operand2)
        :operand1(_operand1), oper(_oper), operand2(_operand2){}
        
    virtual void print(std::ostream& dst)const override{
        operand1->print(dst);
        oper->print(dst);
        operand2->print(dst);
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override{
        for(int i(0); i < depth; ++i) dst << "\t";
        operand1->printPy(dst);
        oper->printPy(dst);
        operand2->printPy(dst);
    }
    virtual void printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr, Type type)const override{
        std::string n1 = makeName("binary");
        std::string n2 = makeName("binary");
        
        bool doLater = false;
        
        Type destType;

        /** LOGICAL OPERATOR SECTION **/
        //SPECIAL CASES HERE REQUIRE BRANCHES AND NOT EVAL RHS IF LHS IS 0 in AND or 1 in OR Operation
        if (oper->getId() == "&&" || oper->getId() == "||" ) {

            //Resolve destType for && and || Operations
            if(operand1->getType(framePtr) == Type::DOUBLE || operand2->getType(framePtr) == Type::DOUBLE)
                destType = Type::DOUBLE;
            else if(operand1->getType(framePtr) == Type::FLOAT || operand2->getType(framePtr) == Type::FLOAT)
                destType = Type::FLOAT;
            else destType = Type::INT;
            
            //Only Evaluate LH operand first and see if Shortcircuited!
            if(destType == Type::INT){ 
                operand1->printMipsE(dst, n1, framePtr, destType);
                framePtr->load(dst, "$t0", n1);
            
            }else{
                operand1->printMipsE(dst, n1, framePtr, destType);
                framePtr->load(dst, "$f0", n1);
            }

            std::string b1 = std::string("$" + makeName("LOGICALB"));
            std::string b2 = std::string("$" + makeName("LOGICALB"));
            std::string b3 = std::string("$" + makeName("LOGICALB"));
            
            if(oper->getId() == "&&"){
            
                if(destType == Type::INT){            
                    dst << "beq $t0, $0, " << b1 << std::endl;
                    dst << "nop" << std::endl << std::endl;
                    //if LH operand is zero, result is definitely 0, thus branch to b1 do not evaluate RH operand

                    operand2->printMipsE(dst, n2, framePtr, destType);
                    framePtr->load(dst, "$t1", n2);
                    dst << "beq $t1, $0, " << b1 << std::endl;
                    dst << "nop" << std::endl << std::endl;

                    dst << "li $t2, 1" << std:: endl;
                    dst << "b " << b2 << std::endl;
                    dst << "nop" << std::endl << std::endl;
                    
                    dst << b1 << ":" << std::endl;
                    dst << "move $t2, $0" << std::endl;
                    dst << b2 << ":" << std::endl;                  
                }
                else { 
                    dst << "mtc1 $0, $f6" << std::endl;
	                dst << "nop" << std::endl;
                    dst << "cvt.s.w $f6, $f6" << std::endl;

                    dst << "li $t0, 1" << std::endl;
                    dst << "mtc1 $t0, $f8" << std::endl;
	                dst << "nop" << std::endl;
                    dst << "cvt.s.w $f8, $f8" << std::endl;                    
                    //$f6, $f8 now have decimal values 0 and 1 in single Float representations.                     
                        
                    dst << "c.eq.s $f0, $f6" << std::endl;
                    dst << "nop" << std::endl;
                    dst << "bc1t " << b1 << std ::endl;
                    dst << "nop" << std::endl << std::endl;
                    //if LH operand is zero, result is definitely 0, thus branch to b1 do not evaluate RH operand

                    operand2->printMipsE(dst, n2, framePtr, destType);
                    framePtr->load(dst, "$f2", n2);
                    dst << "c.eq.s $f2, $f6" << std::endl;
                    dst << "nop" << std::endl;
                    dst << "bc1t " << b1 << std ::endl;
                    dst << "nop" << std::endl << std::endl;

                    dst << "mov.s $f4, $f8" << std:: endl;
                    dst << "b " << b2 << std::endl;
                    dst << "nop" << std::endl << std::endl;
                    
                    dst << b1 << ":" << std::endl;
                    dst << "mov.s $f4, $f6" << std::endl;
                    dst << b2 << ":" << std::endl;                  
                }
            
            }
            else if(oper->getId() == "||"){
                if(destType == Type::INT){            
                    dst << "bne $t0, $0, " << b1 << std::endl;  //if LH operand is nonzero, result is definitely 1, thus branch to b1, do not evaluate RH operand
                    dst << "nop" << std::endl << std::endl;
                    
                    operand2->printMipsE(dst, n2, framePtr, destType);
                    framePtr->load(dst, "$t1", n2);             // IF LH operand 0 we Eval RH operand and store into $t1
                    dst << "beq $t1, $0, " << b2 << std::endl;  // IF RH Operand Also = 0, result is 0 branch to b2,  else goes into b1 and load into $t2 =1
                    dst << "nop" << std::endl << std::endl;
                    
                    dst << b1 << ":" << std::endl;
                    dst << "li $t2, 1" << std::endl;
                    dst << "b " << b3 << std::endl;
                    dst << "nop" << std::endl << std::endl; 
                    
                    dst << b2 << ":" << std::endl;
                    dst << "move $t2, $0" << std:: endl;
                    
                    dst << b3 << ":" << std::endl; 
                }
                else {
                    dst << "mtc1 $0, $f6" << std::endl;
	                dst << "nop" << std::endl;
                    dst << "cvt.s.w $f6, $f6" << std::endl;

                    dst << "li $t0, 1" << std::endl;
                    dst << "mtc1 $t0, $f8" << std::endl;
	                dst << "nop" << std::endl;
                    dst << "cvt.s.w $f8, $f8" << std::endl;                    
                    //$f6, $f8 now have decimal values 0 and 1 in single Float representations.
                    
                    dst << "c.eq.s $f0, $f6" << std::endl;
                    dst << "nop" << std::endl;
                    dst << "bc1f " << b1 << std::endl;  //if LH operand is nonzero, result is definitely 1, thus branch to b1, do not evaluate RH operand
                    dst << "nop" << std::endl << std::endl;
                    
                    operand2->printMipsE(dst, n2, framePtr, destType);
                    framePtr->load(dst, "$f2", n2);             // IF LH operand 0 we Eval RH operand and store into $f2
                    dst << "c.eq.s $f2, $f6" << std::endl;
                    dst << "nop" << std::endl;
                    dst << "bc1t " << b2 << std::endl;  // IF RH Operand Also = 0, result is 0 branch to b2,  else goes into b1 and load into $t2 =1
                    dst << "nop" << std::endl << std::endl;
                    
                    dst << b1 << ":" << std::endl;
                    dst << "mov.s $f4, $f8" << std::endl;
                    dst << "b " << b3 << std::endl;
                    dst << "nop" << std::endl << std::endl; 
                    
                    dst << b2 << ":" << std::endl;
                    dst << "mov.s $f4, $f6" << std:: endl;
                    
                    dst << b3 << ":" << std::endl;
                    
                }
            }
            else { assert(0);} // SHOULD NVR HAPPEN
        }
        else if(oper->getId() == "=" ){
            //We don't need to eval the first operand if we are doing a normal assignment
            // ASSUMPTION MADE HERE: If it is an assignment, destType should be evaluated based on operand 2 then downcasted/upcasted if necessary
            destType = operand2->getType(framePtr);

            //TEMPFIX : check if RHS OF assignment is an addr, if yes convert using addrtotype, TEMPFIX NEEDS TO BE RE EVALUATED            
            if(isAddr(destType)){
                destType =addrToType(destType);
            }
                
            if(destType == Type::INT){
                operand2->printMipsE(dst, n2, framePtr, destType);
                framePtr->load(dst, "$t1", n2);
                
            }else{
                operand2->printMipsE(dst, n2, framePtr, destType);
                framePtr->load(dst, "$f2", n2);
            }
        }
        else{
            //The rest of Binary operators :we have to evaluate the types based on the more general one, 
            //Assign EXPR like += , *= etc falls here too as x*=2.3 is similar to x = x*2.3 where we resolve RHS via float
            
            if(operand1->getType(framePtr) == Type::DOUBLE || operand2->getType(framePtr) == Type::DOUBLE){
                destType = Type::DOUBLE;
                
            }else if(operand1->getType(framePtr) == Type::FLOAT || operand2->getType(framePtr) == Type::FLOAT){
                destType = Type::FLOAT;
                
            }else if(isAddr(operand1->getType(framePtr)) || isAddr(operand2->getType(framePtr))){
                if(isAddr(operand1->getType(framePtr))){
                    destType = operand1->getType(framePtr);
                   
                }else{
                    destType = operand2->getType(framePtr);
                }
            
            }else{destType = Type::INT;}
            
            if(destType == Type::INT || isAddr(destType)){ 
                operand1->printMipsE(dst, n1, framePtr, destType);
                operand2->printMipsE(dst, n2, framePtr, destType);
                framePtr->load(dst, "$t0", n1);
                framePtr->load(dst, "$t1", n2);
            
            }else{
                operand1->printMipsE(dst, n1, framePtr, destType);
                operand2->printMipsE(dst, n2, framePtr, destType);
                framePtr->load(dst, "$f0", n1);
                framePtr->load(dst, "$f2", n2);
            }
        }

        /**ARITHMETIC OPERATIONS SECTION**/

        if(oper->getId() == "+"){
            if(destType == Type::INT || isAddr(destType)){
                
                //Pointer arithmetic
                if(isAddr(operand1->getType(framePtr)) && !isAddr(operand2->getType(framePtr))){
                    
                    Type elementType = addrToType(operand1->getType(framePtr));
                    
                    if(elementType == Type::INT || elementType == Type::FLOAT){
                        dst << "sll $t1, $t1, 2" << std::endl;
                        
                    }else if(elementType == Type::DOUBLE){
                        dst << "sll $t1, $t1, 3" << std::endl;
                        
                    }else if(elementType == Type::CHAR){
                        //Do nothing
                        
                    }else{assert(0);}
                    
                }else if(isAddr(operand2->getType(framePtr)) && !isAddr(operand1->getType(framePtr))){
    
                    Type elementType = addrToType(operand2->getType(framePtr));
                    
                    if(elementType == Type::INT || elementType == Type::FLOAT){
                        dst << "sll $t0, $t0, 2" << std::endl;
                        
                    }else if(elementType == Type::DOUBLE){
                        dst << "sll $t0, $t0, 3" << std::endl;
                        
                    }else if(elementType == Type::CHAR){
                        //Do nothing
                        
                    }else{assert(0);}
                    
                }else{} //Do nothing          
                
                dst << "add $t2, $t0, $t1" << std::endl;
                   
            }else{
                dst << "add.s $f4, $f0, $f2" << std::endl;
                
            }
        }
        else if(oper->getId() == "-"){
            if(destType == Type::INT){
                dst << "sub $t2, $t0, $t1" << std:: endl;
            }
            else{
                dst << "sub.s $f4, $f0, $f2" << std::endl;
            }
        }
        else if(oper->getId() == "*"){
            if(destType == Type::INT){
                dst<< "mul $t2, $t0, $t1" << std :: endl;
            }
            else{
                dst <<"mul.s $f4, $f0, $f2" << std::endl;
            }
        }
        else if (oper->getId() == "/"){
            if(destType == Type::INT){
                dst<<"div $t0, $t1" << std:: endl;
                dst<<"mflo $t2" << std::endl;
            }
            else{
                dst<<"div.s $f4, $f0, $f2" <<std::endl;
            }
        }
        else if (oper->getId() == "%"){
            if(destType == Type::INT){
                dst<<"div $t0, $t1" << std:: endl;
                dst<<"mfhi $t2" << std::endl;
            }
            else{
                throw std::runtime_error("Invalid BinaryOperation of % " + oper->getId() + " on Non-Integer Types");
            }
        }
        else if (oper->getId() == "==") {
            if(destType == Type::INT){
                dst << "xor $t2, $t0, $t1" << std::endl;
                dst << "sltu $t2, $t2, 1" << std::endl;
            }
            else{
                std::string branchlabel = std::string("$" + makeName("BRANCH"));
                dst << "li $t3, 1" << std::endl;
                dst << "mtc1 $t3, $f6" << std::endl; //Move t3 into $f6 Dest Reg is $f6
                dst << "cvt.s.w $f4, $f6" << std::endl; // $f4 has value 1
                dst << "c.eq.s $f2,$f0" << std :: endl; // if equal, flag 1 set
	            dst << "nop" << std::endl;
	            dst << "bc1t "<< branchlabel << std::endl; // branch if flag 1 set
	            dst << "nop" << std::endl;
                // IF NOT EQUAL, perform this, and set $f4 output reg to 0
                dst << "mtc1 $0, $f6" << std::endl;
                dst << "cvt.s.w $f4, $f6" << std::endl;
                dst << branchlabel << ":" << std::endl;
            
            }    
        }
        else if (oper->getId() == "!=") {
            if(destType == Type::INT){
                dst << "xor $t2, $t0, $t1" << std::endl;
                dst << "sltu $t2, $0, $t2" << std::endl;
            }
            else{
                std::string branchlabel = std::string("$" + makeName("BRANCH"));
                dst << "li $t3, 1" << std::endl;
                dst << "mtc1 $t3, $f8" << std::endl;
                dst << "cvt.s.w $f4, $f8" << std::endl; // $f4 i.e output has decimal value 1 in float representation

                dst << "c.eq.s $f2,$f0" << std :: endl; // if 2 operands not equal, flag 1 false
	            dst << "nop" << std::endl;
	            dst << "bc1f "<< branchlabel << std::endl; // branch if flag 1 false
	            dst << "nop" << std::endl;
                // IF EQUAL,Do not branch, perform this, and set $f4 output reg to 0
                dst << "mtc1 $0, $f6" << std::endl;
                dst << "cvt.s.w $f4, $f6" << std::endl;
                dst << branchlabel << ":" << std::endl;
                      
            }    
        }
        else if (oper->getId() == "<") {
            if(destType == Type ::INT){
                dst << "slt $t2, $t0, $t1" << std::endl;
            }
            else{
                std::string branchlabel = std::string("$" + makeName("BRANCH"));
                dst << "li $t3, 1" << std::endl;
                dst << "mtc1 $t3, $f8" << std::endl;
                dst << "cvt.s.w $f4, $f8" << std::endl; // $f4 i.e output has decimal value 1 in float representation

                dst << "c.lt.s $f0,$f2" << std :: endl; // if f0 < f2, flag 1 true
	            dst << "nop" << std::endl;
	            dst << "bc1t "<< branchlabel << std::endl; // branch if flag 1 true
	            dst << "nop" << std::endl;
                
                dst << "mtc1 $0, $f6" << std::endl;
                dst << "cvt.s.w $f4, $f6" << std::endl;
                dst << branchlabel << ":" << std::endl;
            }
        }
        else if (oper->getId() == "<=") {
            if(destType == Type::INT){
                dst << "slt $t2, $t0, $t1" << std::endl; // if t0<t1, t2 is 1

                dst << "xor $t3, $t0, $t1" << std::endl;
                dst << "sltu $t3, $t3, 1" << std::endl;     // if t0 == t1, t3 is 0 and slt makes t3 = 1

                dst << "or $t2, $t2, $t3" << std::endl;     // either one is true means <= is true
            }
            else{
                std::string branchlabel = std::string("$" + makeName("BRANCH"));
                dst << "li $t3, 1" << std::endl;
                dst << "mtc1 $t3, $f8" << std::endl;
                dst << "cvt.s.w $f4, $f8" << std::endl; // $f4 i.e output has decimal value 1 in float representation

                dst << "c.le.s $f0,$f2" << std :: endl; // if f0 <= f2, flag 1 true
	            dst << "nop" << std::endl;
	            dst << "bc1t "<< branchlabel << std::endl; // branch if flag 1 true
	            dst << "nop" << std::endl;
                
                dst << "mtc1 $0, $f6" << std::endl;
                dst << "cvt.s.w $f4, $f6" << std::endl;
                dst << branchlabel << ":" << std::endl;
            }
        }
        else if (oper->getId() == ">") {
            if(destType == Type::INT){
                dst << "slt $t2, $t1, $t0" << std::endl;
            }
            else{
                std::string branchlabel = std::string("$" + makeName("BRANCH"));
                dst << "li $t3, 1" << std::endl;
                dst << "mtc1 $t3, $f8" << std::endl;
                dst << "cvt.s.w $f4, $f8" << std::endl; // $f4 i.e output has decimal value 1 in float representation

                dst << "c.lt.s $f2,$f0" << std :: endl; // if f2 < f0, f0 > f2, flag 1 true
	            dst << "nop" << std::endl;
	            dst << "bc1t "<< branchlabel << std::endl; // branch if flag 1 true
	            dst << "nop" << std::endl;
                
                dst << "mtc1 $0, $f6" << std::endl;
                dst << "cvt.s.w $f4, $f6" << std::endl;
                dst << branchlabel << ":" << std::endl;
            }
        }
        else if (oper->getId() == ">=") {
            if(destType == Type::INT){
                dst << "slt $t2, $t1, $t0" << std::endl; // if t0>t1, t1<t0 and thus t2 is 1

                dst << "xor $t3, $t0, $t1" << std::endl;
                dst << "sltu $t3, $t3, 1" << std::endl;     // if t0 == t1, t3 is 0 and slt makes t3 = 1

                dst << "or $t2, $t2, $t3" << std::endl;     // either one is true means >= is true
            }
            else{
                std::string branchlabel = std::string("$" + makeName("BRANCH"));
                dst << "li $t3, 1" << std::endl;
                dst << "mtc1 $t3, $f8" << std::endl;
                dst << "cvt.s.w $f4, $f8" << std::endl; // $f4 i.e output has decimal value 1 in float representation

                dst << "c.le.s $f2,$f0" << std :: endl; // if f2<=f0, f0 >= f2, flag 1 true
	            dst << "nop" << std::endl;
	            dst << "bc1t "<< branchlabel << std::endl; // branch if flag 1 true
	            dst << "nop" << std::endl;
                
                dst << "mtc1 $0, $f6" << std::endl;
                dst << "cvt.s.w $f4, $f6" << std::endl;
                dst << branchlabel << ":" << std::endl;
            }
        }
        else if (oper->getId() == "<<") {
            if(destType == Type::INT){
                dst << "sllv $t2, $t0, $t1" << std::endl;
            }
            else{ 
                throw std::runtime_error("Called BinaryOperation of Left Shift " + oper->getId() + " on Non-Integer Types"); 
            } 
        }
        else if (oper->getId() == ">>") {
            if(destType == Type::INT){
                dst << "srav $t2, $t0, $t1" << std::endl;   // if unsigned we have to use srlv instead
            }
            else{ 
                throw std::runtime_error("Called BinaryOperation of Right Shift " + oper->getId() + " on Non-Integer Types");
            } 
        }
        
        /** BITWISE OPERATOR SECTIONS **/
        else if (oper->getId() == "&") { // Bitwise Operations do not work on double or floats
            if(destType == Type::INT){            
                dst << "and $t2, $t0, $t1" << std::endl;
            }
            else {
                throw std::runtime_error("Called BinaryOperation of Bitwise AND " + oper->getId() + " on Non-Integer Types that doesnt exist");   
            }        
        }
        else if (oper->getId() == "|") { // Bitwise Operations do not work on double or floats
            if(destType == Type::INT){            
                dst << "or $t2, $t0, $t1" << std::endl;
            }
            else {
                throw std::runtime_error("Called BinaryOperation of Bitwise OR " + oper->getId() + " on Non-Integer Types that doesnt exist");   
            }        
        }
        else if (oper->getId() == "^") { // Bitwise Operations do not work on double or floats
            if(destType == Type::INT){            
                dst << "xor $t2, $t0, $t1" << std::endl;
            }
            else {
                throw std::runtime_error("Called BinaryOperation of Bitwise XOR " + oper->getId() + " on Non-Integer Types that doesnt exist");   
            }        
        }      
        
        /** ASGN OPERATORS **/
        else if(oper->getId() == "="){
            if(dynamic_cast<const PrimaryExpr*>(operand1)){
                
                doLater = true;
                //Do nothing (this case is now handled later so that we don't repeat the code
               
                if(destType == Type::INT){
                    dst << "move $t2, $t1" << std::endl;
                }
                else{
                    dst << "mov.s $f4, $f2" << std::endl;
                }                

            }else if(dynamic_cast<const PostfixExpr*>(operand1)){
                framePtr->storeArrayElement(dst, "$t1", dynamic_cast<const PostfixExpr*>(operand1));
                dst << "move $t2, $t1" << std::endl;
           }else{
                doLater = true;
            }
        }
        //TODO: BILL we have to ASGN OPER FOR POSTFIX EXPR AS WELL ?
        else if(oper->getId() == "+="){
            doLater = true;
            if(destType == Type::INT){
                dst << "addu $t2, $t0, $t1" << std::endl;   // Addu Unsigned add used as we ignore trap if overflow, what C does
            }
            else{
                dst << "add.s $f4, $f0, $f2" << std::endl;
            }
        }
        else if(oper->getId() == "-="){
            doLater = true;
            if(destType == Type::INT){
                dst << "subu $t2, $t0, $t1" << std::endl;   // Subu Unsigned subtract used as we ignore trap if overflow, what C does
            }
            else{
                dst << "sub.s $f4, $f0, $f2" << std::endl;
            }
        }
        else if(oper->getId() == "*="){
            doLater = true;
            if(destType == Type::INT){
                dst << "mul $t2, $t0, $t1" << std::endl;   // Subu Unsigned subtract used as we ignore trap if overflow, what C does
            }
            else{
                dst << "mul.s $f4, $f0, $f2" << std::endl;
            }
        }
        else if(oper->getId() == "/="){
            doLater = true;
            if(destType == Type::INT){
                dst<<"div $t0, $t1" << std:: endl;
                dst<<"mflo $t2" << std::endl;
            }
            else{
                dst<<"div.s $f4, $f0, $f2" <<std::endl;
            }
        }
        else if(oper->getId() == "%="){
            doLater = true;
            if(destType == Type::INT){
                dst<<"div $t0, $t1" << std:: endl;
                dst<<"mfhi $t2" << std::endl;
            }
            else{
                throw std::runtime_error("Invalid BinaryOperation of Modulo " + oper->getId() + " on Non-Integer Types");
            }
        }
        else if (oper->getId() == "<<=") {
            doLater = true;
            if(destType == Type::INT){
                dst << "sllv $t2, $t0, $t1" << std::endl;
            }
            else{ 
                throw std::runtime_error("Called BinaryOperation of Left Shift " + oper->getId() + " on Non-Integer Types"); 
            } 
        }
        else if (oper->getId() == ">>=") {
            doLater = true;
            if(destType == Type::INT){
                dst << "srav $t2, $t0, $t1" << std::endl;   // if unsigned we have to use srlv instead
            }
            else{ 
                throw std::runtime_error("Called BinaryOperation of Right Shift " + oper->getId() + " on Non-Integer Types");
            } 
        }
        else if (oper->getId() == "&=") { // Bitwise Operations do not work on double or floats
            doLater = true;
            if(destType == Type::INT){            
                dst << "and $t2, $t0, $t1" << std::endl;
            }
            else {
                throw std::runtime_error("Called BinaryOperation of Bitwise AND " + oper->getId() + " on Non-Integer Types that doesnt exist");   
            }        
        }
        else if (oper->getId() == "|=") { // Bitwise Operations do not work on double or floats
            doLater = true;
            if(destType == Type::INT){            
                dst << "or $t2, $t0, $t1" << std::endl;
            }
            else {
                throw std::runtime_error("Called BinaryOperation of Bitwise OR " + oper->getId() + " on Non-Integer Types that doesnt exist");   
            }        
        }
        else if (oper->getId() == "^=") { // Bitwise Operations do not work on double or floats
            doLater = true;
            if(destType == Type::INT){            
                dst << "xor $t2, $t0, $t1" << std::endl;
            }
            else {
                throw std::runtime_error("Called BinaryOperation of Bitwise XOR " + oper->getId() + " on Non-Integer Types that doesnt exist");   
            }        
        }    
        // TODO : DO NOT ASSERT ELSE HERE! CUZ ITS POSSIBLE FOR LOGICAL OPERATORS TO END UP IN THIS ELSE (SPECIAL CASE)
        
        //Really important for when we have exprStatement (e.g. double x = 3; x = 1 + 2;)
        if(type == Type::ANYTHING) type = operand1->getType(framePtr);
        
        //We only convert if type mismatch between type(LHS) and destType(RHS) 
        if(type == Type::DOUBLE){
            if(destType == Type::FLOAT){
                TypeConv::convert(dst, Type::DOUBLE, Type::FLOAT, "$f4", "$f4");
                
            }else if(destType == Type::INT){
                TypeConv::convert(dst, Type::DOUBLE, Type::INT, "$f4", "$t2");
                
            }else if(destType == Type::DOUBLE){
                //No need to convert, kept here to not assert   
            }else{assert(0);}

            framePtr->store(dst, "$f4", destName, Type::DOUBLE);
            if(doLater) framePtr->store(dst, "$f4", operand1->getId(), Type::DOUBLE);

        }else if(type == Type::FLOAT){
            if(destType == Type::DOUBLE){
                TypeConv::convert(dst, Type::FLOAT, Type::DOUBLE, "$f4", "$f4");
                               
            }else if(destType == Type::INT){
                TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f4", "$t2");
                
            }else if(destType == Type::FLOAT){
                //No need to convert, kept here to not assert       
            }else{assert(0);}

            framePtr->store(dst, "$f4", destName, Type::FLOAT);
            if(doLater) framePtr->store(dst, "$f4", operand1->getId(), Type::FLOAT);
                    
        }else if(type == Type::INT || type == Type::CHAR){
            if(destType == Type::DOUBLE){
                TypeConv::convert(dst, Type::INT, Type::DOUBLE, "$t2", "$f4");
                                
            }else if(destType == Type::FLOAT){
                TypeConv::convert(dst, Type::INT, Type::FLOAT, "$t2", "$f4");   
                
            }else if(destType == Type::INT){
                //No need to convert, kept here to not assert              
            }else{assert(0);}

            if(type == Type::CHAR){
                    dst << "andi $t2, $t2, 0xFF" << std::endl;
            }

            framePtr->store(dst, "$t2", destName, Type::INT);
            if(doLater){
                //BILL: This solves the issue that we are having in the case of the INT ARRAY TODO 
                if(operand1->isDereference()){
                    framePtr->load(dst, "$t0", operand1->getId());
                    dst << "lw $t0, 0($t0)" << std::endl;
                    dst << "sw $t1, 0($t0)" << std::endl;
                }else{
                    framePtr->store(dst, "$t2", operand1->getId(), Type::INT);
                }
            }

        }else if(isAddr(type)){
            if(destType == Type::DOUBLE || destType == Type::FLOAT){
                //This should never happen
                assert(0);
                
            }else{
                //Don't need to convert simply transfer whatever we got as an addr
                framePtr->store(dst, "$t2", destName, type);
                if(doLater) framePtr->store(dst, "$t2", operand1->getId(), type);
            }
            
        }else{assert(0);}

    }
    
    Type getType(const Frame* framePtr)const override{
        //Return left operand's type if is assignment
        if(oper->getId() == "="){
            return operand1->getType(framePtr);
            
        }else{
            if(operand1->getType(framePtr) == Type::DOUBLE || operand2->getType(framePtr) == Type::DOUBLE){
                return Type::DOUBLE;
                
            }else if(operand1->getType(framePtr) == Type::FLOAT || operand2->getType(framePtr) == Type::FLOAT){
                return Type::FLOAT;
                
            }else if(isAddr(operand1->getType(framePtr)) || isAddr(operand2->getType(framePtr))){
                if(isAddr(operand1->getType(framePtr))){
                    return operand1->getType(framePtr);
                   
                }else{
                    return operand2->getType(framePtr);
                }
            
            }else{return Type::INT;}  
        }
        
    }
    
    double eval()const override{
        if(oper->getId() == "+")
            return operand1->eval() + operand2->eval();
        
        else if(oper->getId() == "-")
            return operand1->eval() - operand2->eval();
            
        else if(oper->getId() == "*")
            return operand1->eval() * operand2->eval();
            
        else if(oper->getId() == "/")
            return operand1->eval() / operand2->eval();
            
        else
            throw std::runtime_error("Called eval() on class: BinaryOperation. Operator " + oper->getId() + " not supported yet");
    }
    
    bool isIdentifier()const override{
        //If either is identifier, we return true
        return (operand1->isIdentifier() || operand2->isIdentifier());
    }
};

class CastExpr: public Expr{
private:
    NodePtr typeName;
    NodePtr unaryExpr;
    
public:
    CastExpr(NodePtr _typeName, NodePtr _unaryExpr)
        :typeName(_typeName), unaryExpr(_unaryExpr){}
        
    virtual void print(std::ostream& dst)const override{
        dst << "(";
        typeName->print(dst);
        dst << ")";
        unaryExpr->print(dst);
    }
   
   virtual void printPy(std::ostream& dst, int depth = 0)const override{}
};

#endif
