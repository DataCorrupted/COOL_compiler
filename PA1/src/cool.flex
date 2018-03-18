/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
  if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
	YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */

/* Used to count parenthesis */
int par_cnt_ = 0;
unsigned int string_err = 0;

#define NULCHAR 0x0001
#define TOOLONG 0x0002

void recordErr(unsigned int& record, const int err){
	record = record | err;
}

const char* reportErr(const int record){
	if (record & NULCHAR) {
		return "String contains null character.";
	} else if (record & TOOLONG){
		return "String constant too long";
	} else {
		/* 
		 * Have to return something, make Rust happy. :) 
		 * No error, I'm happy too :)
		 */
		return "";
	}
}

void addChar(const char c){
	*string_buf_ptr = c;
	/* Use a circular pointer to make sure that the pointer
	 * stays in the buffer. Don't care if it's overriding
	 * past data, since it is already a "Too long string"
	 * error when it happens. All we have to do by then is
	 * to make sure that there is no buffer overflow and we
	 * record this error successfully.
	 * All in all, the abstraction is that, the programmer,
	 * aka, me, can add a pointer without worrying about
	 * overflow or too long string.
	 */
	if (string_buf_ptr - string_buf <= MAX_STR_CONST - 1){
		string_buf_ptr ++;
	} else {
		string_buf_ptr = string_buf;
		recordErr(string_err, TOOLONG);
	}
}

%}

%option noyywrap

/*
 * Define names for regular expressions here.
 */

digit 	[0-9]
letter 	[a-zA-Z]
space 	[ \f\r\t\v]+

/* 
 * Name convension is the same as in cool-support/include/cool-parse.h
 */
TYPEID 		[A-Z]({letter}|{digit}|_)*
OBJECTID 	[a-z]({letter}|{digit}|_)*
INT_CONST 	{digit}+

/* Such usage can only be found in "info flex" but not any tutorial online
 * Whoever maintaining this manual just made a big mistake. 
 * All the keywords has been listed in CoolAid Page 15.
 * Don't use lower case labels, as it collides with C++'s keyword, which
 * stops the code from compiling.
 */
CLASS 		(?i:class)
ELSE 		(?i:else)
FI 			(?i:fi)
IF 			(?i:if)
IN 			(?i:in)
INHERITS	(?i:inherits)
ISVOID 		(?i:isvoid)
LET 		(?i:let)
LOOP 		(?i:loop)
POOL 		(?i:pool)
THEN 		(?i:then)
WHILE 		(?i:while)
CASE 		(?i:case)
ESAC 		(?i:esac)
NEW 		(?i:new)
OF 			(?i:of)
NOT 		(?i:not)

/* Do notice that all keywords are case insensitive but true and false, 
 * who must have the leading character being lower case, which, if I am
 * the one to judge, is stupid.
 */
/*
 * Two days later, I realized that should it start with a capital letter,
 * it would become a object name.
 */
FALSE 		[f](?i:alse)
TRUE 		[t](?i:rue)

%x COMMENT
%x STRING

%%

 /*
  * Define regular expressions for the tokens of COOL here. Make sure, you
  * handle correctly special cases, like:
  *   - Nested comments
  *   - String constants: They use C like systax and can contain escape
  *     sequences. Escape sequence \c is accepted for all characters c. Except
  *     for \n \t \b \f, the result is c.
  *   - Keywords: They are case-insensitive except for the values true and
  *     false, which must begin with a lower-case letter.
  *   - Multiple-character operators (like <-): The scanner should produce a
  *     single token for every such operator.
  *   - Line counting: You should keep the global variable curr_lineno updated
  *     with the correct line number
  */

 /* 
  *Let's deal with comments first.
  */
 /* Line comments */
"--".*"\n" 		{ curr_lineno++; }
"(*"			{
	par_cnt_ ++;
	BEGIN(COMMENT);
}

<COMMENT>"(*" 	{
	par_cnt_ ++;
}
<COMMENT>"*)" 	{
	par_cnt_--;
	if (!par_cnt_){
		BEGIN(INITIAL);
	}
}
<COMMENT>"\n" 	{ curr_lineno++; }
<COMMENT><<EOF>> 	{
	BEGIN(INITIAL);
	cool_yylval.error_msg = "EOF in comment";
	return ERROR;
}

<COMMENT>. 		{ ; } // Comments, don't know, don't care.

 /* Let's deal with strings! */

<INITIAL>"\"" 	{
	BEGIN(STRING);
	// So far, no error.
	string_err = 0x0000;
	// The buffer has to be reset.
	memset(string_buf, 0, MAX_STR_CONST);
	// The pointer should also be pointing to the first element.
	string_buf_ptr = string_buf;

}
<STRING><<EOF>> {
	cool_yylval.error_msg = "EOF in string constant";
	BEGIN(INITIAL);
	return ERROR;	
}
<STRING>"\n" 	{
	cool_yylval.error_msg = "Unterminated string constant";
	curr_lineno ++;
	BEGIN(INITIAL);
	return ERROR;
}

<STRING>"\0"	{ recordErr(string_err, NULCHAR); }
<STRING>"\\"[btnf\n] 	{
	const char c = yytext[1];
	if        (c == 'n') { addChar('\n'); } 
	  else if (c == 't') { addChar('\t'); }
	  else if (c == 'b') { addChar('\b'); }
	  else if (c == 'f') { addChar('\f'); }
	  else if (c == '\n') { addChar('\n'); curr_lineno ++;}
}

<STRING>"\\"[^btnf\n] 	{ addChar(yytext[1]); }

<STRING>"\"" 	{
	BEGIN(INITIAL);
	const char* e = reportErr(string_err);
	if (strlen(e) == 0){
		cool_yylval.symbol = stringtable.add_string(string_buf);
		return STR_CONST;
	} else {
		cool_yylval.error_msg = e;
		return ERROR;
	}
}

<STRING>. 		{ addChar(*yytext); }

 /* Dealing with space and emptylines here. */
"\n"			{ curr_lineno++;}
{space}			{ ; } 					// Just ignore spaces in all forms.

 /* Key words. */
"=>" 			{ return DARROW; }
"<=" 			{ return LE; }
"<-" 			{ return ASSIGN; }
"<" 			{ return (int) '<'; }
"." 			{ return (int) '.'; }
"@" 			{ return (int) '@'; }
"~" 			{ return (int) '~'; }
"*" 			{ return (int) '*'; }
"/" 			{ return (int) '/'; }
"+" 			{ return (int) '+'; }
"-" 			{ return (int) '-'; }
":" 			{ return (int) ':'; }
";" 			{ return (int) ';'; }
"{" 			{ return (int) '{'; }
"}" 			{ return (int) '}'; }
"(" 			{ return (int) '('; }
")" 			{ return (int) ')'; }
"=" 			{ return (int) '='; }
"," 			{ return (int) ','; }


{CLASS} 		{ return CLASS; }
{ELSE} 			{ return ELSE; }
{FI} 			{ return FI;}
{IF} 			{ return IF;}
{IN} 			{ return IN;}
{INHERITS} 		{ return INHERITS;}
{ISVOID} 		{ return ISVOID;}
{LET} 			{ return LET;}
{LOOP} 			{ return LOOP; }
{POOL} 			{ return POOL; }
{THEN} 			{ return THEN; }
{WHILE} 		{ return WHILE; }
{CASE} 			{ return CASE; }
{ESAC} 			{ return ESAC; }
{NEW} 			{ return NEW; }
{OF} 			{ return OF; }
{NOT} 			{ return NOT; }


 /* Check utilities.cc line 176~191 */
{INT_CONST} 	{ 
  cool_yylval.symbol = inttable.add_string(yytext); 
  return INT_CONST; 
}
{TRUE}|{FALSE} 	{
	cool_yylval.boolean = (yytext[0] == 't');
	return BOOL_CONST;
}
{OBJECTID} 		{ 
	cool_yylval.symbol = idtable.add_string(yytext); 
	return OBJECTID; 
}
{TYPEID} 		{ 
	cool_yylval.symbol = idtable.add_string(yytext); 
	return TYPEID; 
}

 /*
  * Whatever leftover should be an error.
  */
. 				{
	cool_yylval.error_msg = yytext;
	return ERROR;
}

%%
