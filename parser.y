%{
/***********************************************************************
 * --YOUR GROUP INFO SHOULD GO HERE--
 * 
 *   Interface to the parser module for CSC467 course project.
 * 
 *   Phase 2: Implement context free grammar for source language, and
 *            parse tracing functionality.
 *   Phase 3: Construct the AST for the source language program.
 ***********************************************************************/

/***********************************************************************
 *  C Definitions and external declarations for this module.
 *
 *  Phase 3: Include ast.h if needed, and declarations for other global or
 *           external vars, functions etc. as needed.
 ***********************************************************************/

#include <string.h>
#include "common.h"
//#include "ast.h"
//#include "symbol.h"
//#include "semantic.h"
#define YYERROR_VERBOSE
#define yTRACE(x)    { if (traceParser) fprintf(traceFile, "%s\n", x); }

void yyerror(const char* s);    /* what to do in case of error            */
int yylex();              /* procedure for calling lexical analyzer */
extern int yyline;        /* variable holding current line number   */

%}

/***********************************************************************
 *  Yacc/Bison declarations.
 *  Phase 2:
 *    1. Add precedence declarations for operators (after %start declaration)
 *    2. If necessary, add %type declarations for some nonterminals
 *  Phase 3:
 *    1. Add fields to the union below to facilitate the construction of the
 *       AST (the two existing fields allow the lexical analyzer to pass back
 *       semantic info, so they shouldn't be touched).
 *    2. Add <type> modifiers to appropriate %token declarations (using the
 *       fields of the union) so that semantic information can by passed back
 *       by the scanner.
 *    3. Make the %type declarations for the language non-terminals, utilizing
 *       the fields of the union as well.
 ***********************************************************************/

%{
#define YYDEBUG 1
%}


// TODO:Modify me to add more data types
// Can access me from flex useing yyval (yylval?)

%union {
  int     num;
  int     ival;
  float   fval;
  bool    bval;
  char    *str;
  char    cval;
}
// TODO:Replace myToken with your tokens, you can use these tokens in flex
%token           <ival> INTLIT
%token           <fval> FLOATLIT
%token           <bval> BOOLEANLIT
%token           <str>  ID
%token           <str>  ASSIGN
%token           <str>  GT
%token           <str>  LT
%token           <str>  NOT
%token           <str>  EXP
%token           <str>  MUL
%token           <str>  DIV
%token           <str>  ADD
%token           <str>  SUB
%token           <str>  EQ
%token           <str>  NE
%token           <str>  LE
%token           <str>  GE
%token           <str>  AND
%token           <str>  OR
%token           <cval> SEMICOLON
%token           <cval> COMMA
%token           <cval> DOT
%token           <cval> OPEN_BRACKET
%token           <cval> CLOSE_BRACKET
%token           <cval> OPEN_BRACES
%token           <cval> CLOSE_BRACES
%token           <cval> OPEN_SQ_BRACKET
%token           <cval> CLOSE_SQ_BRACKET
%token           <str>  TYPE_INT
%token           <str>  TYPE_BOOL
%token           <str>  TYPE_FLOAT
%token           <str>  TYPE_VEC2
%token           <str>  TYPE_VEC3
%token           <str>  TYPE_VEC4
%token           <str>  TYPE_BVEC2
%token           <str>  TYPE_BVEC3
%token           <str>  TYPE_BVEC4
%token           <str>  TYPE_IVEC2
%token           <str>  TYPE_IVEC3
%token           <str>  TYPE_IVEC4
%token           <str>  FUNC_DP3
%token           <str>  FUNC_LIT
%token           <str>  FUNC_RSQ
%token           <str>  CONST
%token           <str>  WHILE
%token           <str>  IF
%token           <str>  ELSE

/* %token           myToken1 myToken2 */

%nonassoc OR
%nonassoc AND
%nonassoc EQ NE LT LE GT GE
%left ADD SUB
%left MUL DIV
%right EXP
%left NEG /*precedence*/ NOT 
%left OPEN_SQ_BRACKET CLOSE_SQ_BRACKET OPEN_BRACKET CLOSE_BRACKET

%start    program

%%

/***********************************************************************
 *  Yacc/Bison rules
 *  Phase 2:
 *    1. Replace grammar found here with something reflecting the source
 *       language grammar
 *    2. Implement the trace parser option of the compiler
 *  Phase 3:
 *    1. Add code to rules for construction of AST.
 ***********************************************************************/
program
  :     scope
  ;
scope
  :     OPEN_BRACES declarations statements CLOSE_BRACES
  ;
declarations
  :     declarations declaration
  |     /* epsilon */
  ;
statements
  :     statements statement
  |     /* epsilon */
  ;
statement
  :     variable ASSIGN expression SEMICOLON
  |     IF OPEN_BRACKET expression CLOSE_BRACKET statement else_statement
  |     WHILE OPEN_BRACKET expression CLOSE_BRACKET statement
  |     scope
  |     SEMICOLON
  ;
expression
  :     constructor
  |     function
  |     INTLIT
  |     FLOATLIT
  |     BOOLEANLIT
  |     variable
  |     unary_op expression %prec NEG
  |     expression binary_op expression
  |     OPEN_BRACKET expression CLOSE_BRACKET
  ;
variable
  :     ID
  |     ID OPEN_SQ_BRACKET INTLIT CLOSE_SQ_BRACKET
  ;
else_statement
  :     ELSE statement
  |     /* epsilon */
  ;
declaration
  :     type ID SEMICOLON
  |     type ID ASSIGN expression SEMICOLON
  |     CONST type ID ASSIGN expression SEMICOLON
  ;
constructor
  :     type OPEN_BRACKET arguments CLOSE_BRACKET
  ;
function
  :     function_name OPEN_BRACKET arguments_opt CLOSE_BRACKET
  ;
arguments_opt
  :     arguments
  |     /* epsilon */
  ;
arguments
  :     arguments COMMA expression
  |     expression
  ;
type
  :     TYPE_INT   | TYPE_IVEC2 | TYPE_IVEC3 | TYPE_IVEC4
  |     TYPE_BOOL  | TYPE_BVEC2 | TYPE_BVEC3 | TYPE_BVEC4 
  |     TYPE_FLOAT | TYPE_VEC2  | TYPE_VEC3  | TYPE_VEC4
  ;
binary_op
  :     AND | OR | EQ | NE | LT | LE | GT | GE | ADD | SUB | MUL | DIV | EXP
  ;
unary_op
  :     NOT | SUB
  ;
function_name
  :     FUNC_DP3 | FUNC_LIT | FUNC_RSQ
  ;



/*
program
  :     tokens       
  ;
tokens
  :     tokens token  
  |      
  ;
// TODO: replace myToken with the token the you defined.
// TODO: add built-in variables during lab3 and lab4 as recommended
token
  :     INTLIT
  |     FLOATLIT
  |     BOOLEANLIT
  |     ID
  |     ASSIGN
  |     GT
  |     LT
  |     NOT
  |     EXP
  |     MUL
  |     DIV
  |     ADD
  |     SUB
  |     EQ
  |     NE
  |     LE
  |     GE
  |     AND
  |     OR
  |     TYPE_INT
  |     TYPE_BOOL
  |     TYPE_FLOAT
  |     TYPE_VEC2
  |     TYPE_VEC3
  |     TYPE_VEC4
  |     TYPE_BVEC2
  |     TYPE_BVEC3
  |     TYPE_BVEC4
  |     TYPE_IVEC2
  |     TYPE_IVEC3
  |     TYPE_IVEC4
  |     FUNC_DP3
  |     FUNC_LIT
  |     FUNC_RSQ
  |     CONST
  |     WHILE
  |     IF
  |     ELSE
  ;
*/
%%

/***********************************************************************ol
 * Extra C code.
 *
 * The given yyerror function should not be touched. You may add helper
 * functions as necessary in subsequent phases.
 ***********************************************************************/
void yyerror(const char* s) {
  if (errorOccurred)
    return;    /* Error has already been reported by scanner */
  else
    errorOccurred = 1;
        
  fprintf(errorFile, "\nPARSER ERROR, LINE %d",yyline);
  if (strcmp(s, "parse error")) {
    if (strncmp(s, "parse error, ", 13))
      fprintf(errorFile, ": %s\n", s);
    else
      fprintf(errorFile, ": %s\n", s+13);
  } else
    fprintf(errorFile, ": Reading token %s\n", yytname[YYTRANSLATE(yychar)]);
}
