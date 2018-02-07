%code requires{
  #include "ast.hpp"

  #include <cassert>
  extern NodePtr g_root; // A way of getting the AST out global defined in maths_parser.tab.cpp from bison

  //! This is to fix problems when generating C++
  // We are declaring the functions provided by Flex, so
  // that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
}

// Represents the value associated with any kind of
// AST node.
%union{
  NodePtr node;
  double number;
  std::string *string;
}

%token T_INT T_RETURN
%token T_LBRACKET T_RBRACKET T_LCURLBRACKET T_RCURLBRACKET 
%token T_SEMICOLON
%token T_IDENTIFIER
%token T_INT_CONSTANT

//Non-terminals declaration
%type <node> EXPR VARIABLE_DECLR VAR_TYPE DECLR_LIST BLOCK FUNCTION_DEF
%type <number> T_INT_CONSTANT
%type <string> T_IDENTIFIER  T_INT

%start ROOT

%%

/* The TODO notes a are just a guide, and are non-exhaustive.
   The expectation is that you do each one, then compile and test.
   Testing should be done using patterns that target the specific
   feature; the testbench is there to make sure that you haven't
   broken anything while you added it.
*/

ROOT : EXPR { g_root = $1; }

EXPR : DECLR_LIST       {$$ = $1; }
	 | FUNCTION_DEF     {$$ = $1; }

FUNCTION_DEF : VAR_TYPE T_IDENTIFIER T_LBRACKET T_RBRACKET BLOCK {$$ = new FunctionDef($1,*$2,$5) ;}

BLOCK : T_LCURLBRACKET DECLR_LIST T_RCURLBRACKET { $$ = new Block($2); }
	
DECLR_LIST : DECLR_LIST VARIABLE_DECLR		{ $$ = new  DeclrList($1,$2); }	
	       |VARIABLE_DECLR				    { $$ = $1; }


VARIABLE_DECLR : VAR_TYPE T_IDENTIFIER T_SEMICOLON  { $$ = new VarDeclr($1,*$2) ; }	

VAR_TYPE : T_INT			                        {$$ = new VarType(*$1) ; }


%%

NodePtr g_root; // Definition of variable (to match declaration earlier)

NodePtr parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}
