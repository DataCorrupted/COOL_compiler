/*
 *  cool.y
 *              Parser definition for the COOL language.
 *
 */
%{
#include <iostream>
#include "cool-tree.h"
#include "stringtab.h"
#include "utilities.h"

/* Add your own C declarations here */


/************************************************************************/
/*                DONT CHANGE ANYTHING IN THIS SECTION                  */

extern int yylex();           /* the entry point to the lexer  */
extern int curr_lineno;
extern char *curr_filename;
Program ast_root;            /* the result of the parse  */
Classes parse_results;       /* for use in semantic analysis */
int omerrs = 0;              /* number of errors in lexing and parsing */

/*
   The parser will always call the yyerror function when it encounters a parse
   error. The given yyerror implementation (see below) justs prints out the
   location in the file where the error was found. You should not change the
   error message of yyerror, since it will be used for grading puproses.
*/
void yyerror(const char *s);

/*
   The VERBOSE_ERRORS flag can be used in order to provide more detailed error
   messages. You can use the flag like this:

	 if (VERBOSE_ERRORS)
	   fprintf(stderr, "semicolon missing from end of declaration of class\n");

   By default the flag is set to 0. If you want to set it to 1 and see your
   verbose error messages, invoke your parser with the -v flag.

   You should try to provide accurate and detailed error messages. A small part
   of your grade will be for good quality error messages.
*/
extern int VERBOSE_ERRORS;

%}

/* A union of all the types that can be the result of parsing actions. */
%union {
  Boolean boolean;
  Symbol symbol;
  Program program;
  Class_ class_;
  Classes classes;
  Feature feature;
  Features features;
  Formal formal;
  Formals formals;
  Case case_;
  Cases cases;
  Expression expression;
  Expressions expressions;
  char *error_msg;
}

/* 
   Declare the terminals; a few have types for associated lexemes.
   The token ERROR is never used in the parser; thus, it is a parse
   error when the lexer returns it.

   The integer following token declaration is the numeric constant used
   to represent that token internally.  Typically, Bison generates these
   on its own, but we give explicit numbers to prevent version parity
   problems (bison 1.25 and earlier start at 258, later versions -- at
   257)
*/
%token CLASS 258 ELSE 259 FI 260 IF 261 IN 262 
%token INHERITS 263 LET 264 LOOP 265 POOL 266 THEN 267 WHILE 268
%token CASE 269 ESAC 270 OF 271 DARROW 272 NEW 273 ISVOID 274
%token <symbol>  STR_CONST 275 INT_CONST 276 
%token <boolean> BOOL_CONST 277
%token <symbol>  TYPEID 278 OBJECTID 279 
%token ASSIGN 280 NOT 281 LE 282 ERROR 283

/*  DON'T CHANGE ANYTHING ABOVE THIS LINE, OR YOUR PARSER WONT WORK       */
/**************************************************************************/
 
   /* Complete the nonterminal list below, giving a type for the semantic
	  value of each non terminal. (See section 3.6 in the bison 
	  documentation for details). */

/* Declare types for the grammar's non-terminals. */
%type <program> program
%type <classes> class_list
%type <class_> class

/* You will want to change the following line. */
%type <features> feature_list
%type <feature> feature
%type <formal> formal
%type <formals> formal_list
%type <expression> expr
%type <expressions> expr_block
%type <expressions> expr_list
%type <cases> cases
%type <case_> case_
/* Precedence declarations go here. */


%%
/* 
   Save the root of the abstract syntax tree in a global variable.
*/
program : class_list { ast_root = program($1); }
		;

class_list
		: class            /* single class */
				{ $$ = single_Classes($1); }
		| class_list class /* several classes */
				{ $$ = append_Classes($1,single_Classes($2)); }
		;

/* If no parent is specified, the class inherits from the Object class. */
class  : CLASS TYPEID '{' feature_list '}' ';' 
	/*' /*Just to make sure that the syntax high light is not so ugly,
		Make Sublime's lexer happy. */ 
				{ $$ = class_($2,idtable.add_string("Object"),$4,
							  stringtable.add_string(curr_filename)); }
		| CLASS TYPEID INHERITS TYPEID '{' feature_list '}' ';'
				{ $$ = class_($2,$4,$6,stringtable.add_string(curr_filename)); }
		;

formal  : OBJECTID ":" TYPEID   { $$ = formal($1, $3); }
formal_list:
		  %empty 				{ $$ = nil_Formals(); }
		| formal_list formal 	{ $$ = append_Formals($1, single_Formals($2)); }

// Feature list may be empty, but no empty features in list. 
feature_list:       
		  %empty                { $$ = nil_Features(); }
		| feature_list feature  { $$ = append_Features($1, single_Features($2)); }
		;

feature:
		// Methods
		  OBJECTID "(" formal_list ")" ":" TYPEID "{" expr "}" ";" {
			$$ = method($1, $3, $6, $8);
		}
		// Attributes, no expression given.
		| OBJECTID ":" TYPEID {
			$$ = attr($1, $3, no_expr());
		}
		// Attributes, expression given.
		| OBJECTID ":" TYPEID ASSIGN expr ";"{
			$$ = attr($1, $3, $5);
		}

// Doing this reversely seems easier, 
// start from easy expressions like constants.
expr:
		// Constant
		  BOOL_CONST			{ $$ = bool_const($1); }
		| STR_CONST				{ $$ = string_const($1); }
		| INT_CONST				{ $$ = int_const($1); }

		// ID
		| OBJECTID 				{ $$ = object($1); }

		// (expr)
		| "(" expr ")" 			{ $$ = $2; }

		// boolean operation
		| NOT expr 				{ $$ = comp($2); }
		| expr "=" expr 		{ $$ = eq($1, $3); }
		| expr LE expr 			{ $$ = leq($1, $3); }
		| expr "<" expr 		{ $$ = lt($1, $3); }
		| "~" expr 				{ $$ = neg($2); }

		// arithmetic operation
		| expr "/" expr 		{ $$ = divide($1, $3); }  
		| expr "*" expr			{ $$ = mul($1, $3); }  
		| expr "-" expr			{ $$ = sub($1, $3); } 
		| expr "+" expr			{ $$ = plus($1, $3); }

		// isvoid expr 
		| ISVOID expr 			{ $$ = isvoid($2); }

		// new TYPE
		| NEW TYPEID 			{ $$ = new_($2); }

		// case expr of [ID: TYPE = expr;]+ esac
		| CASE expr OF cases ESAC	{ 
			$$ = typcase($2, $4);
		}

		// let [ID: TYPE [<-expr]],+ in expr
		// | LET let_list IN expr 	{ $$ = let(); }
		// Deal with let later.

		// {expr;+}
		| "{" expr_block "}" 	{ $$ = block($2); }

		// while expr loop expr pool
		| WHILE expr LOOP expr POOL {
			$$ = loop($2, $4);
		}

		// if expr then expr else expr fi
		| IF expr THEN expr ELSE expr FI{
			$$ = cond($2, $4, $6);
		}

		// Classes and Methods.
/*		These 3 involves Class and it's a little bit complicated.
		| OBJECTID "(" expr_list ")" 		
		| expr "." OBJECTID "(" expr_list ")"
		| expr "." "@" TYPEID "." OBJECTID "(" expr_list ")"
*/

		// ID <- expr
		| OBJECTID ASSIGN expr 	{ $$ = assign($1, $3); }
		;


cases:
		  case_ 		{ $$ = single_Cases($1); }
		| cases case_ 	{ $$ = append_Cases($1, single_Cases($2)); }
case_:
		  OBJECTID ":" TYPEID DARROW expr ";" {
		  	$$ = branch($1, $3, $5);
		  }

expr_block:
		  expr 					{ $$ = single_Expressions($1); }
		| expr_block ";" expr 	{ 
			$$ = append_Expressions($1, single_Expressions($3)); 
		}

expr_list:
		  %empty 				{ $$ = nil_Expressions(); }
		| expr_list "," expr 	{ 
			$$ = append_Expressions($1, single_Expressions($3)); 
		}

/* end of grammar */
%%

/* This function is called automatically when Bison detects a parse error. */
void yyerror(const char *s)
{
  cerr << "\"" << curr_filename << "\", line " << curr_lineno << ": " \
	<< s << " at or near ";
  print_cool_token(yychar);
  cerr << endl;
  omerrs++;

  if(omerrs>20) {
	  if (VERBOSE_ERRORS)
		 fprintf(stderr, "More than 20 errors\n");
	  exit(1);
  }
}

