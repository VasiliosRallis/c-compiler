%code requires{
  #include "ast.hpp"
  
  #include <cassert>
  
  extern NodePtr g_root; // A way of getting the AST out global defined in maths_parser.tab.cpp from bison

  //! This is to fix problems when generating C++
  // We are declaring the functions provided by Flex, so
  // that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
  
  extern FILE* yyin;
  
}

// Represents the value associated with any kind of
// AST node.
%union{
  NodePtr node;
  std::string *string;
}

//Keywords
%token T_INT T_CHAR T_VOID T_SHORT T_LONG T_FLOAT T_DOUBLE T_SIGNED T_UNSIGNED
%token T_RETURN T_AUTO T_STRUCT T_BREAK T_ELSE T_SWITCH T_CASE T_ENUM T_REGISTER
%token T_TYPEDEF T_EXTERN T_UNION T_CONST T_CONTINUE T_FOR T_DEFAULT T_GOTO T_SIZEOF
%token T_VOLATILE T_DO T_IF T_STATIC T_WHILE
%token T_EQUAL T_RIGHT_S_ASSIGN T_LEFT_S_ASSIGN T_ADD_ASSIGN T_SUB_ASSIGN T_MUL_ASSIGN T_DIV_ASSIGN T_MOD_ASSIGN T_AND_ASSIGN T_XOR_ASSIGN T_OR_ASSIGN

%token T_LBRACKET T_RBRACKET T_LCURLBRACKET T_RCURLBRACKET 
%token T_SEMICOLON T_COMMA T_QUESTION T_COLON
%token T_IDENTIFIER
%token T_INT_CONSTANT T_STR_LIT
%token T_AMPERSAND T_EXCLAMATION T_TILDE T_INCREMENT T_DECREMENT T_LOGICAL_OR T_LOGICAL_AND T_OR T_AND T_XOR T_EQUALITY T_INEQUALITY
%token T_SMALLER T_GREATER T_GREATER_EQUAL T_SMALLER_EQUAL T_SHIFT_L T_SHIFT_R T_PLUS T_MINUS T_DIV T_MULT T_MOD
%token T_ARROW T_DOT T_SQUARE_LBRACKET T_SQUARE_RBRACKET

//Non-terminals declaration
%type <node> PROGRAM BASIC_PROGRAM VARIABLE_DECLR VAR_TYPE DECLR_LIST BLOCK FUNCTION_DEF
%type <node> IDENTIFIER_LIST STATEMENT_LIST 
%type <node> INITIALIZER PRIMARY_EXPR STATEMENT EXPR_STATEMENT EXPR SELECTION_STATEMENT ITERATION_STATEMENT
%type <node> TYPE_QUALIFIER DECL_SPECIFIER DECL_SPECIFIER_LIST STOR_CLASS_SPEC
%type <node> ASSIGNMENT_OPER UNARY_EXPR CONDITIONAL_EXPR ASSIGNMENT_EXPR POSTFIX_EXPR CAST_EXPR
%type <node> LOGICAL_OR_EXPR LOGICAL_AND_EXPR INCLUSIVE_OR_EXPR EXCLUSIVE_OR_EXPR AND_EXPR EQUAL_EXPR RELATIONAL_EXPR SHIFT_EXPR ADDITIVE_EXPR MULT_EXPR ARGUMENT_EXPR_LIST JUMP_STATEMENT


%type <string> T_INT_CONSTANT
%type <string> T_IDENTIFIER T_STR_LIT
%type <string> T_INT T_CHAR T_VOID T_SHORT T_LONG T_FLOAT T_DOUBLE T_SIGNED T_UNSIGNED
%type <string> T_CONST T_VOLATILE T_TYPEDEF T_EXTERN T_STATIC T_AUTO T_REGISTER
%type <string> T_WHILE T_DO
%type <string> T_EQUAL T_RIGHT_S_ASSIGN T_LEFT_S_ASSIGN T_ADD_ASSIGN T_SUB_ASSIGN T_MUL_ASSIGN T_DIV_ASSIGN T_MOD_ASSIGN T_AND_ASSIGN T_XOR_ASSIGN T_OR_ASSIGN
%type <string> T_AMPERSAND T_EXCLAMATION T_TILDE T_INCREMENT T_DECREMENT T_LOGICAL_OR T_LOGICAL_AND T_OR T_AND T_XOR T_EQUALITY T_INEQUALITY
%type <string> T_SMALLER T_GREATER T_GREATER_EQUAL T_SMALLER_EQUAL T_SHIFT_L T_SHIFT_R T_PLUS T_MINUS T_DIV T_MULT T_MOD UNARY_OPER
%type <string> T_ARROW T_DOT T_SQUARE_LBRACKET T_SQUARE_RBRACKET T_LBRACKET T_RBRACKET
%type <string> T_GOTO T_CONTINUE T_BREAK T_RETURN


%start ROOT

%%

ROOT : PROGRAM { g_root = $1; }

//BASIC_PROGRAM is the simplest program that will actually compile
//A PROGRAM consists of zero or more basic programs I haven't added the zero case I'm not sure how to do it
PROGRAM : BASIC_PROGRAM {$$ = $1;}
        | PROGRAM BASIC_PROGRAM {$$ = new Program($1, $2);}
       
BASIC_PROGRAM : FUNCTION_DEF {$$ = $1;}
	          | DECLR_LIST {$$ = $1;}

FUNCTION_DEF : DECL_SPECIFIER_LIST T_IDENTIFIER T_LBRACKET T_RBRACKET BLOCK {$$ = new FunctionDef($1,new Identifier(new std::string("2")),$5) ;}

BLOCK : T_LCURLBRACKET STATEMENT_LIST T_RCURLBRACKET { $$ = new Block(NULL,$2); }
	  | T_LCURLBRACKET DECLR_LIST T_RCURLBRACKET { $$ = new Block($2); }
	  | T_LCURLBRACKET DECLR_LIST STATEMENT_LIST T_RCURLBRACKET { $$ = new Block($2,$3) ; }
      | T_LCURLBRACKET T_RCURLBRACKET {$$ = new Block();}
      
SELECTION_STATEMENT: T_IF T_LBRACKET EXPR T_RBRACKET STATEMENT {$$ = new IfStatement($3, $5);}
                   | T_IF T_LBRACKET EXPR T_RBRACKET STATEMENT T_ELSE STATEMENT {$$ = new IfStatement($3,$5,$7);}
                   | T_SWITCH T_LBRACKET EXPR T_RBRACKET STATEMENT {$$ = new CaseStatement($3, $5);}
                   
ITERATION_STATEMENT: T_WHILE T_LBRACKET EXPR T_RBRACKET STATEMENT {$$ = new WhileStatement($3, $5);}
                   | T_DO STATEMENT T_WHILE T_LBRACKET EXPR T_RBRACKET T_SEMICOLON {$$ = new DoStatement($2, $5);}
                   | T_FOR T_LBRACKET T_SEMICOLON T_SEMICOLON T_RBRACKET STATEMENT {$$ = new ForStatement(NULL,NULL,NULL,$6);}
                   | T_FOR T_LBRACKET EXPR T_SEMICOLON T_SEMICOLON T_RBRACKET STATEMENT {$$ = new ForStatement($3,NULL,NULL,$7);}
                   | T_FOR T_LBRACKET EXPR T_SEMICOLON EXPR T_SEMICOLON T_RBRACKET STATEMENT {$$ = new ForStatement($3,$5,NULL,$8);}
                   | T_FOR T_LBRACKET EXPR T_SEMICOLON EXPR T_SEMICOLON EXPR T_RBRACKET STATEMENT {$$ = new ForStatement($3,$5,$7,$9);}

EXPR: ASSIGNMENT_EXPR {$$ = $1;} //Add more

ASSIGNMENT_EXPR: CONDITIONAL_EXPR {$$ = $1;}
               | UNARY_EXPR ASSIGNMENT_OPER ASSIGNMENT_EXPR {$$ = new AssignmentExpr($1, $2, $3);}
               
CONDITIONAL_EXPR: LOGICAL_OR_EXPR {$$ = $1;}
                | LOGICAL_OR_EXPR T_QUESTION EXPR T_COLON CONDITIONAL_EXPR {$$ = new ConditionalExpr($1, $3, $5);}
                
LOGICAL_OR_EXPR: LOGICAL_AND_EXPR {$$ = $1;}
               | LOGICAL_OR_EXPR T_LOGICAL_OR LOGICAL_AND_EXPR {$$ = new BinaryOperation($1, $2, $3);}
               
LOGICAL_AND_EXPR: INCLUSIVE_OR_EXPR {$$ = $1;}
                | LOGICAL_AND_EXPR T_LOGICAL_AND INCLUSIVE_OR_EXPR {$$ = new BinaryOperation($1, $2, $3);}
                    
INCLUSIVE_OR_EXPR: EXCLUSIVE_OR_EXPR {$$ = $1;}
                 | INCLUSIVE_OR_EXPR T_OR EXCLUSIVE_OR_EXPR {$$ = new BinaryOperation($1, $2, $3);}
                 
EXCLUSIVE_OR_EXPR: AND_EXPR {$$ = $1;}
                 | EXCLUSIVE_OR_EXPR T_XOR AND_EXPR {$$ = new BinaryOperation($1, $2, $3);}
                 
AND_EXPR: EQUAL_EXPR {$$ = $1;}
        | AND_EXPR T_AND EQUAL_EXPR {$$ = new BinaryOperation($1, $2, $3);}
        
EQUAL_EXPR: RELATIONAL_EXPR {$$ = $1;}
          | EQUAL_EXPR T_EQUALITY RELATIONAL_EXPR {$$ = new BinaryOperation($1, $2, $3);}
          | EQUAL_EXPR T_INEQUALITY RELATIONAL_EXPR {$$ = new BinaryOperation($1, $2, $3);}
     
RELATIONAL_EXPR: SHIFT_EXPR {$$ = $1;}
               | RELATIONAL_EXPR T_SMALLER SHIFT_EXPR {$$ = new BinaryOperation($1, $2, $3);}
               | RELATIONAL_EXPR T_GREATER SHIFT_EXPR {$$ = new BinaryOperation($1, $2, $3);}
               | RELATIONAL_EXPR T_SMALLER_EQUAL SHIFT_EXPR {$$ = new BinaryOperation($1, $2, $3);}
               | RELATIONAL_EXPR T_GREATER_EQUAL SHIFT_EXPR {$$ = new BinaryOperation($1, $2, $3);}
               
SHIFT_EXPR: ADDITIVE_EXPR {$$ = $1;}
          | SHIFT_EXPR T_SHIFT_L ADDITIVE_EXPR {$$ = new BinaryOperation($1, $2, $3);}
          | SHIFT_EXPR T_SHIFT_R ADDITIVE_EXPR {$$ = new BinaryOperation($1, $2, $3);}
          
ADDITIVE_EXPR: MULT_EXPR {$$ = $1;}
             | ADDITIVE_EXPR T_PLUS MULT_EXPR {$$ = new BinaryOperation($1, $2, $3);}
             | ADDITIVE_EXPR T_MINUS MULT_EXPR {$$ = new BinaryOperation($1, $2, $3);}
             
MULT_EXPR: CAST_EXPR {$$ = $1;}
         | MULT_EXPR T_MULT CAST_EXPR {$$ = new BinaryOperation($1, $2, $3);}
         | MULT_EXPR T_DIV CAST_EXPR {$$ = new BinaryOperation($1, $2, $3);}
         | MULT_EXPR T_MOD CAST_EXPR {$$ = new BinaryOperation($1, $2, $3);}
                       
UNARY_EXPR: POSTFIX_EXPR {$$ = $1;}
          | T_INCREMENT UNARY_EXPR {$$ = new UnaryExpr($1,$2);}
          | T_DECREMENT UNARY_EXPR {$$ = new UnaryExpr($1,$2);}
          | UNARY_OPER CAST_EXPR {$$ = new UnaryExpr($1,$2);} //Add more
          
CAST_EXPR: UNARY_EXPR {$$ = $1;}
         | T_LBRACKET VAR_TYPE T_RBRACKET CAST_EXPR {$$ = new CastExpr($2, $4);} //Temporary use VarType instead of TYPE_NAME
         
UNARY_OPER: T_AMPERSAND {$$ = $1;}
          | T_MULT      {$$ = $1;}
          | T_PLUS      {$$ = $1;}
          | T_MINUS     {$$ = $1;}
          | T_TILDE     {$$ = $1;}
          | T_EXCLAMATION {$$ = $1;}
          
POSTFIX_EXPR: PRIMARY_EXPR {$$ = $1;}
            | POSTFIX_EXPR T_SQUARE_LBRACKET EXPR T_SQUARE_RBRACKET {$$ = new PostfixExpr($1, $2, $3, $4);}
            | POSTFIX_EXPR T_LBRACKET ARGUMENT_EXPR_LIST T_RBRACKET {$$ = new PostfixExpr($1, $2, $3, $4);}
            | POSTFIX_EXPR T_LBRACKET T_RBRACKET {$$ = new PostfixExpr($1, $2, NULL, $3);}
            | POSTFIX_EXPR T_DOT T_IDENTIFIER {$$ = new PostfixExpr($1, $2, new Identifier($3), NULL);}
            | POSTFIX_EXPR T_ARROW T_IDENTIFIER {$$ = new PostfixExpr($1, $2, new Identifier($3), NULL);}
            | POSTFIX_EXPR T_INCREMENT {$$ = new PostfixExpr($1, NULL, NULL, NULL);}
            | POSTFIX_EXPR T_DECREMENT {$$ = new PostfixExpr($1, NULL, NULL, NULL);}
            
ARGUMENT_EXPR_LIST: ASSIGNMENT_EXPR {$$ = $1;}
                  | ARGUMENT_EXPR_LIST T_COMMA ASSIGNMENT_EXPR {$$ = new ArgumentExprList($1, $3);}

ASSIGNMENT_OPER: T_EQUAL {$$ = new Operator($1);}
               | T_RIGHT_S_ASSIGN {$$ = new Operator($1);}
               | T_LEFT_S_ASSIGN {$$ = new Operator($1);}
               | T_ADD_ASSIGN {$$ = new Operator($1);}
               | T_SUB_ASSIGN {$$ = new Operator($1);}
               | T_MUL_ASSIGN {$$ = new Operator($1);}
               | T_DIV_ASSIGN {$$ = new Operator($1);}
               | T_MOD_ASSIGN {$$ = new Operator($1);}
               | T_AND_ASSIGN {$$ = new Operator($1);}
               | T_XOR_ASSIGN {$$ = new Operator($1);}
               | T_OR_ASSIGN {$$ = new Operator($1);}

STATEMENT_LIST : STATEMENT_LIST STATEMENT	{ $$ = new StatementList($1,$2); }
		       | STATEMENT { $$ = $1; }

STATEMENT : BLOCK { $$ = $1; }
	      | EXPR_STATEMENT { $$ = $1; }
	      | SELECTION_STATEMENT {$$ = $1;}
	      | ITERATION_STATEMENT {$$ = $1;}
	      | JUMP_STATEMENT {$$ = $1;}

EXPR_STATEMENT : DECLR_LIST	{ $$ = $1; }
               | EXPR T_SEMICOLON {$$ = new ExprStatement($1);}
               | T_SEMICOLON {$$ = new ExprStatement(NULL);} // AM NOT SURE ABOUT THIS According to documentation, int main() { int a=7; { } int c =5 ;} is allowed, but not int main() { int a =7; { } int c; } but tested on compiler and both compiles.
	
JUMP_STATEMENT: T_GOTO T_IDENTIFIER T_SEMICOLON {$$ = new JumpStatement($1, new Identifier($2));}
              | T_CONTINUE T_SEMICOLON {$$ = new JumpStatement($1, NULL);}
              | T_BREAK T_SEMICOLON {$$ = new JumpStatement($1, NULL);}
              | T_RETURN EXPR T_SEMICOLON {$$ = new JumpStatement($1, $2);}
              | T_RETURN T_SEMICOLON {$$ = new JumpStatement($1, NULL);}
              
DECLR_LIST : DECLR_LIST VARIABLE_DECLR		{ $$ = new  DeclrList($1,$2); }	
	       | VARIABLE_DECLR				    { $$ = $1; }

VARIABLE_DECLR : DECL_SPECIFIER_LIST T_SEMICOLON {$$ = new VarDeclr($1, NULL);}
              | DECL_SPECIFIER_LIST IDENTIFIER_LIST T_LBRACKET T_SEMICOLON  { $$ = new VarDeclr($1,$2) ; std::cerr << "Variable Declaration!"; }
		      | DECL_SPECIFIER_LIST IDENTIFIER_LIST T_EQUAL INITIALIZER T_SEMICOLON { $$ = new VarInit($1,$2,$4) ; std::cerr << "Variable Declaration!";}

INITIALIZER : PRIMARY_EXPR {$$ = $1 ;}

PRIMARY_EXPR : T_IDENTIFIER	 {$$ =  new Identifier($1);}
		     | T_INT_CONSTANT  {$$ = new IntConst($1);}
		     | T_STR_LIT {$$ = new StrLit($1);}
		     | T_LBRACKET EXPR T_RBRACKET {$$ = new PrimaryExpr($2);}


IDENTIFIER_LIST: IDENTIFIER_LIST T_COMMA T_IDENTIFIER { $$ = new IdentifierList($1, new Identifier($3));}
               | T_IDENTIFIER {$$ = new Identifier($1); std::cerr << "Identifier List" << std::endl;}
               
DECL_SPECIFIER_LIST: DECL_SPECIFIER_LIST DECL_SPECIFIER {$$ = new DeclSpecifierList($1, $2); std::cerr << "Declaration Specifier List" << std::endl;}
                   | DECL_SPECIFIER {$$ = $1; std::cerr << "Declaration Specifier List" << std::endl;}
                   
DECL_SPECIFIER: VAR_TYPE {$$ = $1; std::cerr << "Declaration Specifier" << std::endl;}
              | TYPE_QUALIFIER {$$ = $1;}
              | STOR_CLASS_SPEC {$$ = $1;}
	
VAR_TYPE : T_INT {$$ = new VarType($1);}
         | T_CHAR {$$ = new VarType($1);}
         | T_VOID {$$ = new VarType($1);}
         | T_SHORT {$$ = new VarType($1);}
         | T_LONG {$$ = new VarType($1);}
         | T_FLOAT {$$ = new VarType($1);}
         | T_DOUBLE {$$ = new VarType($1);}
         | T_SIGNED {$$ = new VarType($1);}
         | T_UNSIGNED {$$ = new VarType($1);}

TYPE_QUALIFIER: T_CONST {$$ = new TypeQualifier($1);}
              | T_VOLATILE {$$ = new TypeQualifier($1);}

//According to the C-Spec "At most one storage-class specifier may be given in the declaration specifiers in a declaration"
//http://port70.net/~nsz/c/c89/c89-draft.html#storage-class-specifier
STOR_CLASS_SPEC: T_TYPEDEF {$$ = new StorClassSpec($1);}
               | T_EXTERN {$$ = new StorClassSpec($1);}
               | T_STATIC {$$ = new StorClassSpec($1);}
               | T_AUTO {$$ = new StorClassSpec($1);}
               | T_REGISTER {$$ = new StorClassSpec($1);}
              
FUNCTION_DEF : DECL_SPECIFIER_LIST T_IDENTIFIER T_LBRACKET T_RBRACKET BLOCK {$$ = new FunctionDef($1,new Identifier(new std::string("2")),$5) ;}
%%

NodePtr g_root; // Definition of variable (to match declaration earlier)

NodePtr parseAST(const char file[]){

  g_root = 0;
  
  yyin = fopen(file, "r");
  yyparse();
  
  return g_root;
}
