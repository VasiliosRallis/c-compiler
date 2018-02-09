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

//Keywords
%token T_INT T_CHAR T_VOID T_SHORT T_LONG T_FLOAT T_DOUBLE T_SIGNED T_UNSIGNED
%token T_RETURN T_AUTO T_STRUCT T_BREAK T_ELSE T_SWITCH T_CASE T_ENUM T_REGISTER
%token T_TYPEDEF T_EXTERN T_UNION T_CONST T_CONTINUE T_FOR T_DEFAULT T_GOTO T_SIZEOF
%token T_VOLATILE T_DO T_IF T_STATIC T_WHILE

%token T_LBRACKET T_RBRACKET T_LCURLBRACKET T_RCURLBRACKET 
%token T_SEMICOLON T_COMMA
%token T_IDENTIFIER
%token T_INT_CONSTANT

//Non-terminals declaration
%type <node> PROGRAM BASIC_PROGRAM VARIABLE_DECLR VAR_TYPE DECLR_LIST BLOCK FUNCTION_DEF
%type <node> IDENTIFIER_LIST
%type <number> T_INT_CONSTANT
%type <string> T_IDENTIFIER
%type <string> T_INT T_CHAR T_VOID T_SHORT T_LONG T_FLOAT T_DOUBLE T_SIGNED T_UNSIGNED

%start ROOT

%%

ROOT : PROGRAM { g_root = $1; }

//BASIC_PROGRAM is the simplest program that will actually compile
//A PROGRAM consists of zero or more basic programs I haven't added the zero case I'm not sure how to do it
PROGRAM : PROGRAM BASIC_PROGRAM {$$ = new Program($1, $2);}
        | BASIC_PROGRAM {$$ = $1;}
       
BASIC_PROGRAM : DECLR_LIST {$$ = $1;}
	          | FUNCTION_DEF {$$ = $1;}

FUNCTION_DEF : VAR_TYPE T_IDENTIFIER T_LBRACKET T_RBRACKET BLOCK {$$ = new FunctionDef($1,new Identifier($2),$5) ;}

BLOCK : T_LCURLBRACKET DECLR_LIST T_RCURLBRACKET { $$ = new Block($2); }
      | T_LCURLBRACKET T_RCURLBRACKET {$$ = new Block();}
	
DECLR_LIST : DECLR_LIST VARIABLE_DECLR		{ $$ = new  DeclrList($1,$2); }	
	       | VARIABLE_DECLR				    { $$ = $1; }


VARIABLE_DECLR : VAR_TYPE IDENTIFIER_LIST T_SEMICOLON  { $$ = new VarDeclr($1,$2) ; }

IDENTIFIER_LIST: IDENTIFIER_LIST T_COMMA T_IDENTIFIER { $$ = new IdentifierList($1, new Identifier($3));}
                | T_IDENTIFIER {$$ = new Identifier($1);}
	

VAR_TYPE : T_INT {$$ = new VarType($1);}
         | T_CHAR {$$ = new VarType($1);}
         | T_VOID {$$ = new VarType($1);}
         | T_SHORT {$$ = new VarType($1);}
         | T_LONG {$$ = new VarType($1);}
         | T_FLOAT {$$ = new VarType($1);}
         | T_DOUBLE {$$ = new VarType($1);}
         | T_SIGNED {$$ = new VarType($1);}
         | T_UNSIGNED {$$ = new VarType($1);}


%%

NodePtr g_root; // Definition of variable (to match declaration earlier)

NodePtr parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}
