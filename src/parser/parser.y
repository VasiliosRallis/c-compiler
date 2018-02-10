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
  std::string *string;
}

//Keywords
%token T_INT T_CHAR T_VOID T_SHORT T_LONG T_FLOAT T_DOUBLE T_SIGNED T_UNSIGNED
%token T_RETURN T_AUTO T_STRUCT T_BREAK T_ELSE T_SWITCH T_CASE T_ENUM T_REGISTER
%token T_TYPEDEF T_EXTERN T_UNION T_CONST T_CONTINUE T_FOR T_DEFAULT T_GOTO T_SIZEOF
%token T_VOLATILE T_DO T_IF T_STATIC T_WHILE

%token T_EQUAL

%token T_LBRACKET T_RBRACKET T_LCURLBRACKET T_RCURLBRACKET 
%token T_SEMICOLON T_COMMA
%token T_IDENTIFIER
%token T_INT_CONSTANT T_STR_LIT

//Non-terminals declaration
%type <node> PROGRAM BASIC_PROGRAM VARIABLE_DECLR VAR_TYPE DECLR_LIST BLOCK FUNCTION_DEF
%type <node> IDENTIFIER_LIST STATEMENT_LIST 
%type <node> INITIALIZER PRIMARY_EXPR STATEMENT EXPR_STATEMENT EXPR SELECTION_STATEMENT ITERATION_STATEMENT
%type <node> TYPE_QUALIFIER DECL_SPECIFIER DECL_SPECIFIER_LIST STOR_CLASS_SPEC


%type <string> T_INT_CONSTANT
%type <string> T_IDENTIFIER T_STR_LIT
%type <string> T_INT T_CHAR T_VOID T_SHORT T_LONG T_FLOAT T_DOUBLE T_SIGNED T_UNSIGNED
%type <string> T_CONST T_VOLATILE T_TYPEDEF T_EXTERN T_STATIC T_AUTO T_REGISTER
%type <string> T_WHILE T_DO

%start ROOT

%%

ROOT : PROGRAM { g_root = $1; }

//BASIC_PROGRAM is the simplest program that will actually compile
//A PROGRAM consists of zero or more basic programs I haven't added the zero case I'm not sure how to do it
PROGRAM : PROGRAM BASIC_PROGRAM {$$ = new Program($1, $2);}
        | BASIC_PROGRAM {$$ = $1;}
       
BASIC_PROGRAM : DECLR_LIST {$$ = $1;}
	          | FUNCTION_DEF {$$ = $1;}

FUNCTION_DEF : DECL_SPECIFIER_LIST T_IDENTIFIER T_LBRACKET T_RBRACKET BLOCK {$$ = new FunctionDef($1,new Identifier($2),$5) ;}

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

EXPR: PRIMARY_EXPR T_EQUAL PRIMARY_EXPR {$$ = new Expr($1, $3);}

STATEMENT_LIST : STATEMENT_LIST STATEMENT	{ $$ = new StatementList($1,$2); }
		       | STATEMENT { $$ = $1; }

STATEMENT : BLOCK { $$ = $1; }
	      | EXPR_STATEMENT { $$ = $1; }
	      | SELECTION_STATEMENT {$$ = $1;}
	      | ITERATION_STATEMENT {$$ = $1;}

EXPR_STATEMENT : DECLR_LIST	{ $$ = $1; } // AM NOT SURE ABOUT THIS According to documentation, int main() { int a=7; { } int c =5 ;} is allowed, but not int main() { int a =7; { } int c; } but tested on compiler and both compiles.
	
DECLR_LIST : DECLR_LIST VARIABLE_DECLR		{ $$ = new  DeclrList($1,$2); }	
	       | VARIABLE_DECLR				    { $$ = $1; }

VARIABLE_DECLR : DECL_SPECIFIER_LIST IDENTIFIER_LIST T_SEMICOLON  { $$ = new VarDeclr($1,$2) ; }
		       | DECL_SPECIFIER_LIST IDENTIFIER_LIST T_EQUAL INITIALIZER T_SEMICOLON { $$ = new VarInit($1,$2,$4) ; }

INITIALIZER : PRIMARY_EXPR {$$ = $1 ;}

PRIMARY_EXPR : T_IDENTIFIER	 {$$ =  new Identifier($1);}
		     | T_INT_CONSTANT  {$$ = new IntConst($1);}
		     | T_STR_LIT {$$ = new StrLit($1);}
		     | T_LBRACKET EXPR T_RBRACKET {$$ = new PrimaryExpr($2);}


IDENTIFIER_LIST: IDENTIFIER_LIST T_COMMA T_IDENTIFIER { $$ = new IdentifierList($1, new Identifier($3));}
               | T_IDENTIFIER {$$ = new Identifier($1);}
               
DECL_SPECIFIER_LIST: DECL_SPECIFIER_LIST DECL_SPECIFIER {$$ = new DeclSpecifierList($1, $2);}
                   | DECL_SPECIFIER {$$ = $1;}
                   
DECL_SPECIFIER: VAR_TYPE {$$ = $1;}
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
              

%%

NodePtr g_root; // Definition of variable (to match declaration earlier)

NodePtr parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}
