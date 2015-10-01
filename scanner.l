%{
/**********************************************************************
 *
 * **YOUR GROUP INFO SHOULD GO HERE**
 *
 *  Interface to the scanner module for CSC467 course project
 **********************************************************************/


#include "common.h"
#include "parser.tab.h"
#include "string.h"


#define YY_USER_INIT { yyin = inputFile; }
#define	yyinput      input
#define yTRACE(x)    { if (traceScanner) fprintf(traceFile, "TOKEN %3d : %s\n", x, yytext); }
#define yERROR(x)    { fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: %s\n", yyline, x); errorOccurred = TRUE; yyterminate(); }

int CheckInt(void);
int yyline = 1;

bool strToInt(const char* str, int* num);

%}
%option noyywrap

WS          [ \t\n]
digit       [0-9]
integer     [-+]?([1-9]{digit}*|0)
decimal     (([-+]?{digit}*\.{digit}+)|([-+]?{digit}+\.{digit}*)|([-+]?{digit}+\.{digit}+))
boolean     (true|false)
id          [a-zA-Z|_][a-zA-Z0-9|_]*
keyword     (int|bool|float|vec2|vec3|vec4|bvec2|bvec3|bvec4|ivec2|ivec3|ivec4|dp3|lit|rsq|void|const|while|if|else)
operator    (\=|\>|\<|\!|\^|\*|\/|\+|\=\=|\!\=|\<\=|\>\=|\&\&|\|\|)

%%
"/*"        {
              int c1      = 0;
              int c2      = yyinput();
              int curline = yyline;
              for(;;) {
                if (c2 == EOF) {
                  fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: Unmatched /*\n", curline);
                  errorOccurred = TRUE;
                  yyterminate();
                }
                if (c1 == '*' && c2 == '/')
                  break;
                c1 = c2;
                c2 = yyinput();
                if (c1 == '\n' && c2 != EOF)
                  yyline++;
              }
            }

{WS}        { ;}

{operator}  {
              yylval.str = yytext;
              yTRACE(OPERATOR);
              return OPERATOR;
            }

{integer}   {
              int num; 
              if (!strToInt(yytext, &num)) {
                yERROR("Can't convert string to integer");
              }
              yylval.ival = num;
              yTRACE(INTLIT);
              return INTLIT;
            }

{decimal}   {
              yylval.fval = atof(yytext);
              yTRACE(FLOATLIT);
              return FLOATLIT;
            }

{boolean}   {
              if (strcmp(yytext, "true") == 0) {
                yylval.bval = true;
              } else if (strcmp(yytext, "false") == 0) {
                yylval.bval = false;
              }
              yTRACE(BOOLEANLIT);
              return BOOLEANLIT;
            }

{keyword}   {
              yylval.str = yytext;
              yTRACE(KEYWORD);
              return KEYWORD;
            }

{id}        {
              yylval.str = yytext;
              yTRACE(ID);
              return ID;
            }

.           { yERROR("Unknown token"); }

%%

bool strToInt (const char* str, int* num) {
  /* Assume integers are 32 bits - max 2147483647 min -2147483648 */
  /* [1-9][0-9]* | 0 Never starts with 000000* */

  int len = strlen(str);
  int startIndex = 0;
  int numLen = len;
  const char* LIMIT = "2147483647";

  if (str[0] == '-') {
    startIndex = 1;
    numLen--;
    LIMIT = "2147483648";
  } else if (str[0] == '+') {
    startIndex = 1;
    numLen--;
    LIMIT = "2147483647";
  }

  if (numLen > 10) {
    return false;
  }

  if (numLen < 10) {
    *num = atoi(str);
    return true;
  }
 
  /* num Len == 10 */

  for (int i = startIndex; i < len; i++) {
    if (str[i] > LIMIT[i-startIndex]) {
      return false;
    }
  }

  *num = atoi(str);
  return true;
}
