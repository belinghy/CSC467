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

void yyerror(char* s);    /* what to do in case of error            */
int yylex();              /* procedure for calling lexical analyzer */
extern int yyline;        /* variable holding current line number   */

enum {
  DP3 = 0, 
  LIT = 1, 
  RSQ = 2
};

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

// defines the yyval union
%union {
  int as_int;
  int as_vec;
  float as_float;
  char *as_str;
  int as_func;
}

%token          FLOAT_T
%token          INT_T
%token          BOOL_T
%token          CONST
%token          FALSE_C TRUE_C
%token          FUNC
%token          IF WHILE ELSE
%token          AND OR NEQ EQ LEQ GEQ

// links specific values of tokens to yyval
%token <as_vec>   VEC_T
%token <as_vec>   BVEC_T
%token <as_vec>   IVEC_T
%token <as_float> FLOAT_C
%token <as_int>   INT_C
%token <as_str>   ID

%left     OR
%left     AND
%left     EQ NEQ '<' LEQ '>' GEQ
%left     '+' '-'
%left     '*' '/'
%right    '^'
%right    '!' UMINUS
%left     '[' ']' '(' ')'

%start    program

%%

/***********************************************************************
 *  Yacc/Bison rules
 *  Phase 2:
 *    1. Replace grammar found here with something reflecting the source
 *       language grammar
 *    2. Implement the trace parser option of the compiler
 ***********************************************************************/
program
  :     scope    {yTRACE("program: scope");}
  ;
scope
  :     '{' declarations statements '}'    {yTRACE("scope: '{' declarations statements '}'");}
  ;
declarations
  :     declarations declaration     {yTRACE("declarations: declarations declaration");}
  |     /* epsilon */                {yTRACE("declarations: epsilon");}
  ;
statements
  :     statements statement    {yTRACE("statements: statements statement");}
  |     /* epsilon */           {yTRACE("statements: epsilon");}
  ;
statement
  :     statement_other_than_conditional
  |     complete_conditional
  |     incomplete_conditional
  ;
complete_conditional
  :     IF '(' expression ')' statement_other_than_conditional ELSE statement
  |     IF '(' expression ')' complete_conditional ELSE statement
  ;
incomplete_conditional
  :     IF '(' expression ')' statement
  ;
statement_other_than_conditional
  :     variable '=' expression ';'                       {yTRACE("statement: variable '=' expression ';'");}
  |     WHILE '(' expression ')' statement                
  |     scope
  |     ';'
  ;
expression
  :     constructor {yTRACE("statements: epsilon");}
  |     function
  |     INT_C  {yTRACE("expression: INT_C");}
  |     FLOAT_C {yTRACE("expression: FLOAT_C");}
  |     TRUE_C
  |     FALSE_C
  |     variable {yTRACE("expression: variable");}
  |     unary_op expression %prec UMINUS
  |     expression binary_op expression
  |     '(' expression ')'
  ;
variable
  :     ID {yTRACE("variable: ID");}
  |     ID '[' INT_C ']' {yTRACE("variable: ID '[' INT_C ']'");}
  ;
declaration
  :     type ID ';'  {yTRACE("declaration: type ID ");}
  |     type ID '=' expression ';'  {yTRACE("declaration: type ID '=' expression ';'\n");}
  |     CONST type ID '=' expression ';' {yTRACE("declaration: CONST type ID '=' expression ';'\n");}
  ;
constructor
  :     type '(' arguments ')'
  ;
function
  :     function_name '(' arguments_opt ')'
  ;
arguments_opt
  :     arguments
  |     /* epsilon */
  ;
arguments
  :     arguments ',' expression
  |     expression
  ;
type
  :     INT_T  
  |     IVEC_T
  |     BOOL_T  
  |     BVEC_T
  |     FLOAT_T 
  |     VEC_T
  ;
binary_op    
  :     AND 
  |     OR 
  |     '=' 
  |     NEQ 
  |     '<' 
  |     LEQ 
  |     '>' 
  |     GEQ 
  |     '+' 
  |     '-' 
  |     '*'
  |     '/' 
  |     '^' 
  ;
unary_op
  :     '!' 
  |     '-'
  ;
function_name
  :     FUNC
  ;

/*program
  :   tokens       
  ;
tokens
  :  tokens token  
  |
  ;
token
  : ID 
  | AND
  | OR
  | NEQ
  | LEQ
  | GEQ
  | EQ
  | TRUE_C
  | FALSE_C
  | INT_C
  | FLOAT_C
  | CONST
  | ELSE
  | IF
  | WHILE
  | FLOAT_T
  | INT_T
  | BOOL_T
  | VEC_T
  | IVEC_T
  | BVEC_T
  | FUNC               
  | '+'
  | '-'
  | '*'
  | '/'
  | '^'  
  | '!'
  | '='
  | '<'
  | '>'   
  | ','
  | ';'
  | '('
  | ')'
  | '['
  | ']'
  | '{'
  | '}'                                    
  ;

*/
%%

/***********************************************************************ol
 * Extra C code.
 *
 * The given yyerror function should not be touched. You may add helper
 * functions as necessary in subsequent phases.
 ***********************************************************************/
void yyerror(char* s) {
  if(errorOccurred) {
    return;    /* Error has already been reported by scanner */
  } else {
    errorOccurred = 1;
  }

  fprintf(errorFile, "\nPARSER ERROR, LINE %d", yyline);
  
  if(strcmp(s, "parse error")) {
    if(strncmp(s, "parse error, ", 13)) {
      fprintf(errorFile, ": %s\n", s);
    } else {
      fprintf(errorFile, ": %s\n", s+13);
    }
  } else {
    fprintf(errorFile, ": Reading token %s\n", yytname[YYTRANSLATE(yychar)]);
  }
}

