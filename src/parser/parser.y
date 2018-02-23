%code requires{
  #include "ast.hpp"
  #include <vector>
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
  StrPtr string;
  VectorPtr nodeVector;
  const DirectDeclarator* dirDeclPtr;
  const Block* block;
  const Expr* expr;
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
%type <node> PROGRAM EXT_DECLARATION VAR_TYPE FUNCTION_DEF DECLARATION INIT_DECLARATOR
%type <dirDeclPtr> DIRECT_DECLARATOR
%type <block> BLOCK
%type <node> STATEMENT EXPR_STATEMENT EXPR SELECTION_STATEMENT ITERATION_STATEMENT PARAMETER_DECL
%type <node> TYPE_QUALIFIER DECL_SPECIFIER STOR_CLASS_SPEC
%type <node> ASSIGNMENT_OPER
%type <node>  LABELED_STATEMENT JUMP_STATEMENT
%type <nodeVector> INIT_DECLARATOR_LIST STATEMENT_LIST DECLR_LIST DECL_SPECIFIER_LIST ARGUMENT_EXPR_LIST PARAMETER_LIST
//%type <nodeVector> IDENTIFIER_LIST
%type <expr> ASSIGNMENT_EXPR CONDITIONAL_EXPR UNARY_EXPR POSTFIX_EXPR CAST_EXPR LOGICAL_OR_EXPR LOGICAL_AND_EXPR PRIMARY_EXPR 
%type <expr> INCLUSIVE_OR_EXPR EXCLUSIVE_OR_EXPR AND_EXPR EQUAL_EXPR RELATIONAL_EXPR SHIFT_EXPR ADDITIVE_EXPR MULT_EXPR 

%type <string> T_INT_CONSTANT
%type <string> T_IDENTIFIER T_STR_LIT
%type <string> T_INT T_CHAR T_VOID T_SHORT T_LONG T_FLOAT T_DOUBLE T_SIGNED T_UNSIGNED
%type <string> T_CONST T_VOLATILE T_TYPEDEF T_EXTERN T_STATIC T_AUTO T_REGISTER
%type <string> T_WHILE T_DO
%type <string> T_EQUAL T_RIGHT_S_ASSIGN T_LEFT_S_ASSIGN T_ADD_ASSIGN T_SUB_ASSIGN T_MUL_ASSIGN T_DIV_ASSIGN T_MOD_ASSIGN T_AND_ASSIGN T_XOR_ASSIGN T_OR_ASSIGN
%type <string> T_AMPERSAND T_EXCLAMATION T_TILDE T_INCREMENT T_DECREMENT T_LOGICAL_OR T_LOGICAL_AND T_OR T_AND T_XOR T_EQUALITY T_INEQUALITY
%type <string> T_SMALLER T_GREATER T_GREATER_EQUAL T_SMALLER_EQUAL T_SHIFT_L T_SHIFT_R T_PLUS T_MINUS T_DIV T_MULT T_MOD UNARY_OPER
%type <string> T_ARROW T_DOT T_SQUARE_LBRACKET T_SQUARE_RBRACKET T_LBRACKET T_RBRACKET T_CASE T_DEFAULT T_GOTO T_RETURN T_CONTINUE T_BREAK


%start ROOT

%%

ROOT : PROGRAM {g_root = $1;}

PROGRAM : PROGRAM EXT_DECLARATION   {$$ = new Program($1, $2);}
        | EXT_DECLARATION           {$$ = $1;}
       
EXT_DECLARATION: FUNCTION_DEF {$$ = $1;}
		       | DECLARATION  {$$ =$1;}

DECLARATION: DECL_SPECIFIER_LIST T_SEMICOLON                      {$$ = new Declaration($1,NULL);}        // We probably dont want : int ; , int int;
           | DECL_SPECIFIER_LIST INIT_DECLARATOR_LIST T_SEMICOLON {$$ = new Declaration($1,$2);}          // And this as well : int int x;

INIT_DECLARATOR_LIST: INIT_DECLARATOR                               {$$ = new std::vector<NodePtr>{$1};}  // int x;
			        | INIT_DECLARATOR_LIST T_COMMA INIT_DECLARATOR  {$$ = $1 ; $1->push_back($3);}        // int x, y=5;

INIT_DECLARATOR: DIRECT_DECLARATOR                     {$$ = $1;}                                   // int x;
		       | T_IDENTIFIER T_EQUAL ASSIGNMENT_EXPR  {$$ = new InitDeclarator($1,$3);}            // int x =5

DIRECT_DECLARATOR: T_IDENTIFIER                                        {$$ = new DirectDeclarator($1, NULL, NULL, NULL);} 
		         | T_IDENTIFIER T_LBRACKET T_RBRACKET                  {$$ = new DirectDeclarator($1, $2, NULL, $3);}// function declaration : int f() ;
//		         | DIRECT_DECLARATOR T_LBRACKET IDENTIFIER_LIST T_RBRACKET  {$$ = new DirectDeclarator($1, $2, $3, $4);} // Not sure if need this int f(a,b);
		         | T_IDENTIFIER T_LBRACKET PARAMETER_LIST T_RBRACKET   {$$ = new DirectDeclarator($1, $2, $3, $4);} // function declaration : int f(int a, int b) ;
		         
PARAMETER_LIST: PARAMETER_DECL                          {$$ = new std::vector<NodePtr>{$1};}    // 
              | PARAMETER_LIST T_COMMA PARAMETER_DECL   {$$ = $1; $1->push_back($3);}           // Inside brackets of ( int x, int y)
		
PARAMETER_DECL: DECL_SPECIFIER                   {$$ = new ParameterDeclaration($1, NULL);}     // NOT SURE IF WE WANT THIS : Inside brackets of ( int )
              | DECL_SPECIFIER DIRECT_DECLARATOR {$$ = new ParameterDeclaration($1, $2);}       // Inside brackets of (int x)

FUNCTION_DEF : DECL_SPECIFIER_LIST DIRECT_DECLARATOR BLOCK {$$ = new FunctionDef($1,$2,$3);}    // Function Definition : int x() { } or int x(int a,intb) { }
                                                                                                // NOT SURE IF we want DECL_SPEC LIST and also int x { }

BLOCK : T_LCURLBRACKET STATEMENT_LIST T_RCURLBRACKET            {$$ = new Block(NULL, $2);}
	  | T_LCURLBRACKET DECLR_LIST T_RCURLBRACKET                {$$ = new Block($2, NULL);}     // DO we want internal declaration to be done same way as external declaration
	  | T_LCURLBRACKET DECLR_LIST STATEMENT_LIST T_RCURLBRACKET {$$ = new Block($2,$3);}        // We are relying on declaration for both now
      | T_LCURLBRACKET T_RCURLBRACKET                           {$$ = new Block(NULL, NULL);}
      
SELECTION_STATEMENT: T_IF T_LBRACKET EXPR T_RBRACKET STATEMENT {$$ = new IfStatement($3, $5);}
                   | T_IF T_LBRACKET EXPR T_RBRACKET STATEMENT T_ELSE STATEMENT {$$ = new IfStatement($3,$5,$7);}
                   | T_SWITCH T_LBRACKET EXPR T_RBRACKET STATEMENT {$$ = new CaseStatement($3, $5);}
                   
ITERATION_STATEMENT: T_WHILE T_LBRACKET EXPR T_RBRACKET STATEMENT                                   {$$ = new WhileStatement($3, $5);}
                   | T_DO STATEMENT T_WHILE T_LBRACKET EXPR T_RBRACKET T_SEMICOLON                  {$$ = new DoStatement($2, $5);}
                   | T_FOR T_LBRACKET T_SEMICOLON T_SEMICOLON T_RBRACKET STATEMENT                  {$$ = new ForStatement(NULL,NULL,NULL,$6);}
                   | T_FOR T_LBRACKET EXPR T_SEMICOLON T_SEMICOLON T_RBRACKET STATEMENT             {$$ = new ForStatement($3,NULL,NULL,$7);}
                   | T_FOR T_LBRACKET EXPR T_SEMICOLON EXPR T_SEMICOLON T_RBRACKET STATEMENT        {$$ = new ForStatement($3,$5,NULL,$8);}
                   | T_FOR T_LBRACKET EXPR T_SEMICOLON EXPR T_SEMICOLON EXPR T_RBRACKET STATEMENT   {$$ = new ForStatement($3,$5,$7,$9);}

EXPR: ASSIGNMENT_EXPR {$$ = $1;} //Add more

ASSIGNMENT_EXPR: CONDITIONAL_EXPR {$$ = $1;}
               | UNARY_EXPR ASSIGNMENT_OPER ASSIGNMENT_EXPR {$$ = new BinaryOperation($1, $2, $3);}
               
CONDITIONAL_EXPR: LOGICAL_OR_EXPR {$$ = $1;}
                | LOGICAL_OR_EXPR T_QUESTION EXPR T_COLON CONDITIONAL_EXPR {$$ = new ConditionalExpr($1, $3, $5);}
                
LOGICAL_OR_EXPR: LOGICAL_AND_EXPR {$$ = $1;}
               | LOGICAL_OR_EXPR T_LOGICAL_OR LOGICAL_AND_EXPR {$$ = new BinaryOperation($1, new Operator($2, Operator::LOGICAL_OR), $3);}
               
LOGICAL_AND_EXPR: INCLUSIVE_OR_EXPR {$$ = $1;}
                | LOGICAL_AND_EXPR T_LOGICAL_AND INCLUSIVE_OR_EXPR {$$ = new BinaryOperation($1, new Operator($2, Operator::LOGICAL_AND), $3);}
                
INCLUSIVE_OR_EXPR: EXCLUSIVE_OR_EXPR {$$ = $1;}
                 | INCLUSIVE_OR_EXPR T_OR EXCLUSIVE_OR_EXPR {$$ = new BinaryOperation($1, new Operator($2, Operator::INCLUSIVE_OR), $3);}
                 
EXCLUSIVE_OR_EXPR: AND_EXPR {$$ = $1;}
                 | EXCLUSIVE_OR_EXPR T_XOR AND_EXPR {$$ = new BinaryOperation($1, new Operator($2, Operator::EXCLUSIVE_OR), $3);}
                 
AND_EXPR: EQUAL_EXPR {$$ = $1;}
        | AND_EXPR T_AND EQUAL_EXPR {$$ = new BinaryOperation($1, new Operator($2, Operator::AND), $3);}
        
EQUAL_EXPR: RELATIONAL_EXPR                             {$$ = $1;}
          | EQUAL_EXPR T_EQUALITY RELATIONAL_EXPR       {$$ = new BinaryOperation($1, new Operator($2, Operator::EQUALITY), $3);}
          | EQUAL_EXPR T_INEQUALITY RELATIONAL_EXPR     {$$ = new BinaryOperation($1, new Operator($2, Operator::EQUALITY), $3);}
     
RELATIONAL_EXPR: SHIFT_EXPR                                     {$$ = $1;}
               | RELATIONAL_EXPR T_SMALLER SHIFT_EXPR           {$$ = new BinaryOperation($1, new Operator($2, Operator::RELATIONAL), $3);}
               | RELATIONAL_EXPR T_GREATER SHIFT_EXPR           {$$ = new BinaryOperation($1, new Operator($2, Operator::RELATIONAL), $3);}
               | RELATIONAL_EXPR T_SMALLER_EQUAL SHIFT_EXPR     {$$ = new BinaryOperation($1, new Operator($2, Operator::RELATIONAL), $3);}
               | RELATIONAL_EXPR T_GREATER_EQUAL SHIFT_EXPR     {$$ = new BinaryOperation($1, new Operator($2, Operator::RELATIONAL), $3);}
               
SHIFT_EXPR: ADDITIVE_EXPR                       {$$ = $1;}
          | SHIFT_EXPR T_SHIFT_L ADDITIVE_EXPR  {$$ = new BinaryOperation($1, new Operator($2, Operator::SHIFT), $3);}
          | SHIFT_EXPR T_SHIFT_R ADDITIVE_EXPR  {$$ = new BinaryOperation($1, new Operator($2, Operator::SHIFT), $3);}
          
ADDITIVE_EXPR: MULT_EXPR                        {$$ = $1;}
             | ADDITIVE_EXPR T_PLUS MULT_EXPR   {$$ = new BinaryOperation($1, new Operator($2, Operator::ADD), $3);}
             | ADDITIVE_EXPR T_MINUS MULT_EXPR  {$$ = new BinaryOperation($1, new Operator($2, Operator::ADD), $3);}
             
MULT_EXPR: CAST_EXPR                    {$$ = $1;}
         | MULT_EXPR T_MULT CAST_EXPR   {$$ = new BinaryOperation($1, new Operator($2, Operator::MUL), $3);}
         | MULT_EXPR T_DIV CAST_EXPR    {$$ = new BinaryOperation($1, new Operator($2, Operator::MUL), $3);}
         | MULT_EXPR T_MOD CAST_EXPR    {$$ = new BinaryOperation($1, new Operator($2, Operator::MUL), $3);}
         
CAST_EXPR: UNARY_EXPR {$$ = $1;}
         | T_LBRACKET VAR_TYPE T_RBRACKET CAST_EXPR {$$ = new CastExpr($2, $4);} //Temporary use VarType instead of TYPE_NAME
                       
UNARY_EXPR: POSTFIX_EXPR            {$$ = $1;}
          | T_INCREMENT UNARY_EXPR  {$$ = new UnaryExpr($1,$2);}
          | T_DECREMENT UNARY_EXPR  {$$ = new UnaryExpr($1,$2);}
          | UNARY_OPER CAST_EXPR    {$$ = new UnaryExpr($1,$2);} //Add more

UNARY_OPER: T_AMPERSAND     {$$ = $1;}
          | T_MULT          {$$ = $1;}
          | T_PLUS          {$$ = $1;}
          | T_MINUS         {$$ = $1;}
          | T_TILDE         {$$ = $1;}
          | T_EXCLAMATION   {$$ = $1;}
          
          
POSTFIX_EXPR: PRIMARY_EXPR                                          {$$ = $1;}
            | POSTFIX_EXPR T_SQUARE_LBRACKET EXPR T_SQUARE_RBRACKET {$$ = new PostfixExpr($1, $2, new std::vector<NodePtr>{$3}, $4);}
            | POSTFIX_EXPR T_LBRACKET ARGUMENT_EXPR_LIST T_RBRACKET {$$ = new PostfixExpr($1, $2, $3, $4);}
            | POSTFIX_EXPR T_LBRACKET T_RBRACKET                    {$$ = new PostfixExpr($1, $2, NULL, $3);}
            | POSTFIX_EXPR T_DOT T_IDENTIFIER                       {$$ = new PostfixExpr($1, $2, NULL, $3);}
            | POSTFIX_EXPR T_ARROW T_IDENTIFIER                     {$$ = new PostfixExpr($1, $2, NULL, $3);}
            | POSTFIX_EXPR T_INCREMENT                              {$$ = new PostfixExpr($1, $2, NULL, NULL);}
            | POSTFIX_EXPR T_DECREMENT                              {$$ = new PostfixExpr($1, $2, NULL, NULL);}
            
ARGUMENT_EXPR_LIST: ASSIGNMENT_EXPR                            {$$ = new std::vector<NodePtr>{$1};}
                  | ARGUMENT_EXPR_LIST T_COMMA ASSIGNMENT_EXPR {$$ = $1; $1->push_back($3);}


STATEMENT_LIST : STATEMENT	                {$$ = new std::vector<NodePtr>{$1};}
		       | STATEMENT_LIST STATEMENT   {$$ = $1; $1->push_back($2);}

STATEMENT : LABELED_STATEMENT   {$$ = $1;}
          | BLOCK               {$$ = $1;}
	      | EXPR_STATEMENT      {$$ = $1;}
	      | SELECTION_STATEMENT {$$ = $1;}
	      | ITERATION_STATEMENT {$$ = $1;}
	      | JUMP_STATEMENT      {$$ = $1;}
	      
EXPR_STATEMENT : EXPR T_SEMICOLON {$$ = new ExprStatement($1);}
               | T_SEMICOLON {$$ = new ExprStatement(NULL);} 
	
DECLR_LIST : DECLARATION	                {$$ = new std::vector<NodePtr>{$1};}	
	       | DECLR_LIST DECLARATION		    {$$ = $1; $1->push_back($2);}
	       
//IDENTIFIER_LIST: T_IDENTIFIER                           {$$ = new std::vector<NodePtr>{new StringNode($1)};}
//               | IDENTIFIER_LIST T_COMMA T_IDENTIFIER    {$$ = $1; $1->push_back(new StringNode($3));}

PRIMARY_EXPR : T_IDENTIFIER	                {$$ = new PrimaryExpr(new StringNode($1));}
		     | T_INT_CONSTANT               {$$ = new PrimaryExpr(new IntConst($1));}
		     | T_STR_LIT                    {$$ = new PrimaryExpr(new StringNode($1));}
		     | T_LBRACKET EXPR T_RBRACKET   {$$ = new PrimaryExpr($2);}
               
DECL_SPECIFIER_LIST: DECL_SPECIFIER                     {$$ = new std::vector<NodePtr>{$1};}
                   | DECL_SPECIFIER_LIST DECL_SPECIFIER {$$ = $1; $1->push_back($2);}
                   
DECL_SPECIFIER: VAR_TYPE            {$$ = $1;}
              | TYPE_QUALIFIER      {$$ = $1;}
              | STOR_CLASS_SPEC     {$$ = $1;}
              
              
ASSIGNMENT_OPER: T_EQUAL            {$$ = new Operator($1, Operator::ASSIGN);}
               | T_RIGHT_S_ASSIGN   {$$ = new Operator($1, Operator::ASSIGN);}
               | T_LEFT_S_ASSIGN    {$$ = new Operator($1, Operator::ASSIGN);}
               | T_ADD_ASSIGN       {$$ = new Operator($1, Operator::ASSIGN);}
               | T_SUB_ASSIGN       {$$ = new Operator($1, Operator::ASSIGN);}
               | T_MUL_ASSIGN       {$$ = new Operator($1, Operator::ASSIGN);}
               | T_DIV_ASSIGN       {$$ = new Operator($1, Operator::ASSIGN);}
               | T_MOD_ASSIGN       {$$ = new Operator($1, Operator::ASSIGN);}
               | T_AND_ASSIGN       {$$ = new Operator($1, Operator::ASSIGN);}
               | T_XOR_ASSIGN       {$$ = new Operator($1, Operator::ASSIGN);}
               | T_OR_ASSIGN        {$$ = new Operator($1, Operator::ASSIGN);}
	
VAR_TYPE : T_INT        {$$ = new DeclSpecifier($1);}
         | T_CHAR       {$$ = new DeclSpecifier($1);}
         | T_VOID       {$$ = new DeclSpecifier($1);}
         | T_SHORT      {$$ = new DeclSpecifier($1);}
         | T_LONG       {$$ = new DeclSpecifier($1);}
         | T_FLOAT      {$$ = new DeclSpecifier($1);}
         | T_DOUBLE     {$$ = new DeclSpecifier($1);}
         | T_SIGNED     {$$ = new DeclSpecifier($1);}
         | T_UNSIGNED   {$$ = new DeclSpecifier($1);}

TYPE_QUALIFIER: T_CONST     {$$ = new DeclSpecifier($1);}
              | T_VOLATILE  {$$ = new DeclSpecifier($1);}

STOR_CLASS_SPEC: T_TYPEDEF  {$$ = new DeclSpecifier($1);}
               | T_EXTERN   {$$ = new DeclSpecifier($1);}
               | T_STATIC   {$$ = new DeclSpecifier($1);}
               | T_AUTO     {$$ = new DeclSpecifier($1);}
               | T_REGISTER {$$ = new DeclSpecifier($1);}
               
LABELED_STATEMENT: T_IDENTIFIER T_COLON STATEMENT               {$$ = new LabeledStatement($1, NULL, $3);}
                 | T_CASE CONDITIONAL_EXPR T_COLON STATEMENT    {$$ = new LabeledStatement($1, $2, $4);}
                 | T_DEFAULT T_COLON STATEMENT                  {$$ = new LabeledStatement($1, NULL, $3);}
                 
JUMP_STATEMENT: T_GOTO T_IDENTIFIER T_SEMICOLON     {$$ = new JumpStatement($1, $2, NULL);}
              | T_CONTINUE T_SEMICOLON              {$$ = new JumpStatement($1, NULL, NULL);}
              | T_BREAK T_SEMICOLON                 {$$ = new JumpStatement($1, NULL, NULL);}
              | T_RETURN T_SEMICOLON                {$$ = new JumpStatement($1, NULL, NULL);}
              | T_RETURN EXPR T_SEMICOLON           {$$ = new JumpStatement($1, NULL, $2);}
%%

NodePtr g_root; // Definition of variable (to match declaration earlier)

NodePtr parseAST(const char file[]){

  g_root = 0;
  
  yyin = fopen(file, "r");
  yyparse();
  
  return g_root;
}
