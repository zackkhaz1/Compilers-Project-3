%skeleton "lalr1.cc"
%require "3.0"
%debug
%defines
%define api.namespace{crona}
%define api.parser.class {Parser}
/* %define parser_class_name {Parser} // For compilation on older version */
%define parse.error verbose
%output "parser.cc"
%token-table

%code requires{
	#include <list>
	#include "tokens.hpp"
	#include "ast.hpp"
	namespace crona {
		class Scanner;
	}

//The following definition is required when
// we don't have the %locations directive
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

//End "requires" code
}

%parse-param { crona::Scanner &scanner }
%parse-param { crona::ProgramNode** root }
%code{
   // C std code for utility functions
   #include <iostream>
   #include <cstdlib>
   #include <fstream>

   // Our code for interoperation between scanner/parser
   #include "scanner.hpp"
   #include "ast.hpp"
   #include "tokens.hpp"

  //Request tokens from our scanner member, not
  // from a global function
  #undef yylex
  #define yylex scanner.yylex
}

%union {
	crona::Token*                         transToken;
	crona::IDToken*                       transIDToken;
	crona::ProgramNode*                   transProgram;
	std::list<crona::DeclNode *> *        transDeclList;
	crona::DeclNode *                     transDecl;
	crona::VarDeclNode *                  transVarDecl;
	crona::TypeNode *                     transType;
	crona::IDNode *                       transID;
	crona::FnDeclNode*										transFn;
	crona::FormalDeclNode*								transFormal;
	std::list<crona::FormalDeclNode*>*		transFormals;
	crona::FormalDeclNode*								transFormalDecl;
	std::list<crona::StmtNode*>*					transFnBody;
	std::list<crona::StmtNode*>*					transStmtList;
	crona::StmtNode*											transStmt;
	crona::AssignExpNode*								  transAssignExp;
	crona::ExpNode*											  transExp;
	crona::CallExpNode*									  transCallExp;
	crona::LValNode*											transLval;
	std::list<ExpNode*>*									transActuals;
}

%define parse.assert

/* Terminals
 *  No need to touch these, but do note the translation type
 *  of each node. Most are just "transToken", which is defined in
 *  the %union above to mean that the token translation is an instance
 *  of crona::Token *, and thus has no fields (other than line and column).
 *  Some terminals, like ID, are "transIDToken", meaning the translation
 *  also has a name field.
*/
%token                   END	   0 "end file"
%token	<transToken>     AND
%token	<transToken>     ARRAY
%token	<transToken>     ASSIGN
%token	<transToken>     BOOL
%token	<transToken>     BYTE
%token	<transToken>     COLON
%token	<transToken>     COMMA
%token	<transToken>     CROSS
%token	<transToken>     CROSSCROSS
%token	<transToken>     DASH
%token	<transToken>     DASHDASH
%token	<transToken>     ELSE
%token	<transToken>     EQUALS
%token	<transToken>     FALSE
%token	<transToken>     HAVOC
%token	<transIDToken>   ID
%token	<transToken>     IF
%token	<transToken>     INT
%token	<transIntToken>  INTLITERAL
%token	<transToken>     GREATER
%token	<transToken>     GREATEREQ
%token	<transToken>     LBRACE
%token	<transToken>     LCURLY
%token	<transToken>     LESS
%token	<transToken>     LESSEQ
%token	<transToken>     LPAREN
%token	<transToken>     NOT
%token	<transToken>     NOTEQUALS
%token	<transToken>     OR
%token	<transToken>     RBRACE
%token	<transToken>     RCURLY
%token	<transToken>     READ
%token	<transToken>     RETURN
%token	<transToken>     RPAREN
%token	<transToken>     SEMICOLON
%token	<transToken>     SLASH
%token	<transToken>     STRING
%token	<transToken>     STAR
%token	<transStrToken>  STRLITERAL
%token	<transToken>     TRUE
%token	<transToken>     VOID
%token	<transToken>     WHILE
%token	<transToken>     WRITE


/* Nonterminals
*  TODO: You will need to add more nonterminals
*  to this list as you add productions to the grammar
*  below (along with indicating the appropriate translation
*  attribute type). Note that the specifier in brackets
*  indicates the type of the translation attribute using
*  the names defined in the %union directive above
*/
/*    (attribute type)    (nonterminal)    */
%type <transProgram>    program
%type <transDeclList>   globals
%type <transDecl>       decl
%type <transVarDecl>    varDecl
%type <transType>       type
%type <transID>         id
%type <transFn>         fnDecl
%type <transFormals>    formals
%type <transFormals>    formalsList
%type <transFormal>     formalDecl
%type <transStmtList>   fnBody
%type <transStmtList>   stmtList
%type <transStmt>      	stmt
%type <transAssignExp>  assignExp
%type <transExp>        exp
%type <transExp>        term
%type <transCallExp>    callExp
%type <transLval>				lval
%type <transActuals>		actualsList

%right ASSIGN
%left OR
%left AND
%nonassoc LESS GREATER LESSEQ GREATEREQ EQUALS NOTEQUALS
%left DASH CROSS
%left STAR SLASH
%left NOT

%%

program 	: globals
		  {
		  $$ = new ProgramNode($1);
		  *root = $$;
		  }

globals 	: globals decl
	  	  {
	  	  $$ = $1;
	  	  DeclNode * declNode = $2;
		  $$->push_back(declNode);
	  	  }
		| /* epsilon */
		  { $$ = new std::list<DeclNode * >(); }

decl 		: varDecl SEMICOLON { $$ = $1; }
		| fnDecl { $$ = $1; }

varDecl 	: id COLON type
		  {
		  size_t line = $1->line();
		  size_t col = $1->col();
		  $$ = new VarDeclNode(line, col, $3, $1);
		  }

type 		: INT { $$ = new IntTypeNode($1->line(), $1->col()); }
		| INT ARRAY LBRACE INTLITERAL RBRACE {

			$$ = new ArrayTypeNode($1->line(), $1->col(), new IntTypeNode($1->line(), $1->col())); }

		| BOOL {$$ = new BoolTypeNode($1->line(), $1->col()); }

		| BOOL ARRAY LBRACE INTLITERAL RBRACE {
			$$ = new ArrayTypeNode($1->line(), $1->col(), new BoolTypeNode($1->line(), $1->col())); }

		| BYTE { $$ = new ByteTypeNode($1->line(), $1->col());}

		| BYTE ARRAY LBRACE INTLITERAL RBRACE {
		$$ = new ArrayTypeNode($1->line(), $1->col(), new ByteTypeNode($1->line(), $1->col()));}
		| STRING
		  { $$ = new ArrayTypeNode($1->line(), $1->col(), new ByteTypeNode($1->line(),$1->col())); }

		| VOID {$$ = new VoidTypeNode($1->line(), $1->col());}

fnDecl 		: id COLON type formals fnBody {$$ = new FnDeclNode($1->line(), $1->col(), $3, $1, $4, $5);}

formals 	: LPAREN RPAREN { $$ = new std::list<FormalDeclNode*>(); }
		| LPAREN formalsList RPAREN { $$ = $2; }


formalsList	: formalDecl {
	$$ = new std::list<FormalDeclNode*>();
 	$$->push_back($1); }
		| formalDecl COMMA formalsList {$$ = $3; $$->push_back($1); }

formalDecl 	: id COLON type { $$ = new FormalDeclNode($1->line(), $1->col(), $3, $1); }

fnBody		: LCURLY stmtList RCURLY { $$ = $2;}

stmtList 	: /* epsilon */
		  { $$ = new std::list<StmtNode*>();}

		| stmtList stmt {$$ = $1; $$->push_back($2);}

stmt		: varDecl SEMICOLON {$$ = $1;}
		| assignExp SEMICOLON { $$ = new AssignStmtNode($1->line(), $1->col(), $1);}

		| lval DASHDASH SEMICOLON { $$ = new PostDecStmtNode($1->line(), $1->col(), $1);}

		| lval CROSSCROSS SEMICOLON { $$ = new PostIncStmtNode($1->line(), $1->col(), $1); }

		| READ lval SEMICOLON { $$ = new ReadStmtNode($1->line(), $1->col(), $2);}

		| WRITE exp SEMICOLON { $$ = new WriteStmtNode($1->line(), $1->col(), $2);}

		| IF LPAREN exp RPAREN LCURLY stmtList RCURLY
		  { $$ = new IfStmtNode($1->col(),$1->col(), $3, $6); }

		| IF LPAREN exp RPAREN LCURLY stmtList RCURLY ELSE LCURLY stmtList RCURLY
		  { $$ = new IfElseStmtNode($1->line(), $1->col(), $3, $6, $10); }

		| WHILE LPAREN exp RPAREN LCURLY stmtList RCURLY
		  { $$ = new WhileStmtNode($1->line(), $1->col(), $3, $6); }

		| RETURN exp SEMICOLON { $$ = new ReturnStmtNode($1->line(), $1->col(), $2);}

		| RETURN SEMICOLON {$$ = new ReturnStmtNode($1->line(), $1->col(), NULL);}

		| callExp SEMICOLON {$$ = new CallStmtNode($1->line(), $1->col(), $1); }


exp		: assignExp { $$ = $1; }

		| exp DASH exp { $$ = new MinusNode($1->line(), $1->col(), $1, $3); }

		| exp CROSS exp { $$ = new PlusNode($1->line(), $1->col(), $1, $3); }

		| exp STAR exp { $$ = new TimesNode($1->line(), $1->col(), $1, $3); }

		| exp SLASH exp { $$ = new DivideNode($1->line(), $1->col(), $1, $3); }

		| exp AND exp { $$ = new AndNode($1->line(), $1->col(), $1, $3); }

		| exp OR exp { $$ = new OrNode($1->line(), $1->col(), $1, $3); }

		| exp EQUALS exp { $$ = new EqualsNode($1->line(), $1->col(), $1, $3); }

		| exp NOTEQUALS exp { $$ = new NotEqualsNode($1->line(), $1->col(), $1, $3); }

		| exp GREATER exp { $$ = new GreaterNode($1->line(), $1->col(), $1, $3); }

		| exp GREATEREQ exp { $$ = new GreaterEqNode($1->line(), $1->col(), $1, $3); }

		| exp LESS exp { $$ = new LessNode($1->line(), $1->col(), $1, $3); }

		| exp LESSEQ exp { $$ = new LessEqNode($1->line(), $1->col(), $1, $3); }

		| NOT exp { $$ = new NotNode($1->line(), $1->col(), $2); }

		| DASH term { $$ = new NegNode($1->line(), $1->col(), $2); }

		| term { $$ = $1; }

assignExp	: lval ASSIGN exp { $$ = new AssignExpNode($1->line(), $1->col(), $1, $3); }

callExp		: id LPAREN RPAREN
		  {
		  std::list<ExpNode*>* listOfExp = new std::list<ExpNode*>();
		  $$ = new CallExpNode($1->line(), $1->col(), $1, listOfExp);
		  }

		| id LPAREN actualsList RPAREN
		  { $$ = new CallExpNode($1->line(), $1->col(), $1, $3); }


actualsList	: exp
		  {
		  $$ = new std::list<ExpNode*>();
		  $$->push_back($1);
		  }

		| actualsList COMMA exp
		  {
		  $$ = $1;
		  $$->push_back($3);
		  }

term 		: lval { $$ = $1; }
		| INTLITERAL {}
		| STRLITERAL { }
		| TRUE { $$ = new TrueNode($1->line(), $1->col()); }
		| FALSE { $$ = new FalseNode($1->line(), $1->col());}
		| HAVOC {$$ = new HavocNode($1->line(), $1->col()); }
		| LPAREN exp RPAREN { }
		| callExp {}

lval		: id {}
		| id LBRACE exp RBRACE { }

id		: ID { }

%%

void crona::Parser::error(const std::string& msg){
	std::cout << msg << std::endl;
	std::cerr << "syntax error" << std::endl;
}
