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
#define  yyinput      input
#define yTRACE(x)    { if (traceScanner) fprintf(traceFile, "TOKEN %3d : %s\n", x, yytext); }
#define yERROR(x)    { fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: %s\n", yyline, x); errorOccurred = TRUE; yyterminate(); }

int CheckInt(void);
int yyline = 1;

bool strToInt(const char* str, int* num);

%}
%option noyywrap

WS          [ \t]
NL          [\n]
digit       [0-9]
integer     [-+]?([1-9]{digit}*|0)
operator    (\=|\>|\<|\!|\^|\*|\/|\+|\-|\=\=|\!\=|\<\=|\>\=|\&\&|\|\|)
decimal     (([-+]?{digit}*\.{digit}+)|([-+]?{digit}+\.{digit}*)|([-+]?{digit}+\.{digit}+))
boolean     (true|false)
id          [a-zA-Z|_][a-zA-Z0-9|_]*
keyword     (int|bool|float|vec2|vec3|vec4|bvec2|bvec3|bvec4|ivec2|ivec3|ivec4|dp3|lit|rsq|const|while|if|else)

separator   (\;|\,|\.|\(|\)|\[|\]|\{|\})

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

{NL}        {yyline++;}

{operator}  {
              yylval.str = yytext;
              if (strcmp(yytext, "=") == 0) {
                yTRACE(ASSIGN);
                return ASSIGN;
              } else if (strcmp(yytext, ">") == 0) {
                yTRACE(GT);
                return GT;
              } else if (strcmp(yytext, "<") == 0) {
                yTRACE(LT);
                return LT;
              } else if (strcmp(yytext, "!") == 0) {
                yTRACE(NOT);
                return NOT;
              } else if (strcmp(yytext, "^") == 0) {
                yTRACE(EXP);
                return EXP;
              } else if (strcmp(yytext, "*") == 0) {
                yTRACE(MUL);
                return MUL;
              } else if (strcmp(yytext, "/") == 0) {
                yTRACE(DIV);
                return DIV;
              } else if (strcmp(yytext, "+") == 0) {
                yTRACE(ADD);
                return ADD;
              } else if (strcmp(yytext, "-") == 0) {
                yTRACE(SUB);
                return SUB;
              } else if (strcmp(yytext, "==") == 0) {
                yTRACE(EQ);
                return EQ;
              } else if (strcmp(yytext, "!=") == 0) {
                yTRACE(NE);
                return NE;
              } else if (strcmp(yytext, "<=") == 0) {
                yTRACE(LE);
                return LE;
              } else if (strcmp(yytext, ">=") == 0) {
                yTRACE(GE);
                return GE;
              } else if (strcmp(yytext, "&&") == 0) {
                yTRACE(AND);
                return AND;
              } else if (strcmp(yytext, "||") == 0) {
                yTRACE(OR);
                return OR;
              }
              yERROR("Unknown operator");
            }

{separator} {
              yylval.cval = yytext[0];
              if (strcmp(yytext, ";") == 0) {
                yTRACE(SEMICOLON);
                return SEMICOLON;
              } else if (strcmp(yytext, ",") == 0) {
                yTRACE(COMMA);
                return COMMA;
              } else if (strcmp(yytext, ".") == 0) {
                yTRACE(DOT);
                return DOT;
              } else if (strcmp(yytext, "(") == 0) {
                yTRACE(OPEN_BRACKET);
                return OPEN_BRACKET;
              } else if (strcmp(yytext, ")") == 0) {
                yTRACE(CLOSE_BRACKET);
                return CLOSE_BRACKET;
              } else if (strcmp(yytext, "{") == 0) {
                yTRACE(OPEN_BRACES);
                return OPEN_BRACES;
              } else if (strcmp(yytext, "}") == 0) {
                yTRACE(CLOSE_BRACES);
                return CLOSE_BRACES;
              } else if (strcmp(yytext, "[") == 0) {
                yTRACE(OPEN_SQ_BRACKET);
                return OPEN_SQ_BRACKET;
              } else if (strcmp(yytext, "]") == 0) {
                yTRACE(CLOSE_SQ_BRACKET);
                return CLOSE_SQ_BRACKET;
              }
              yERROR("Unknown separator");
            }

{integer}   {
              int num; 
              if (!strToInt(yytext, &num)) {
                yERROR("Can't convert string to integer. Possible overflow.");
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
              if(strcmp(yytext, "int") == 0){
                yTRACE(TYPE_INT);
                return TYPE_INT;
              } else if(strcmp(yytext, "bool") == 0) {
                yTRACE(TYPE_BOOL);
                return TYPE_BOOL;
              } else if(strcmp(yytext, "float") == 0) {
                yTRACE(TYPE_FLOAT);
                return TYPE_FLOAT;
              } else if(strcmp(yytext, "vec2") == 0) {
                yTRACE(TYPE_VEC2);
                return TYPE_VEC2;
              } else if(strcmp(yytext, "vec3") == 0) {
                yTRACE(TYPE_VEC3);
                return TYPE_VEC3;
              } else if(strcmp(yytext, "vec4") == 0) {
                yTRACE(TYPE_VEC4);
                return TYPE_VEC4;
              } else if(strcmp(yytext, "bvec2") == 0) {
                yTRACE(TYPE_BVEC2);
                return TYPE_BVEC2;
              } else if(strcmp(yytext, "bvec3") == 0) {
                yTRACE(TYPE_BVEC3);
                return TYPE_BVEC3;
              } else if(strcmp(yytext, "bvec4") == 0) {
                yTRACE(TYPE_BVEC4);
                return TYPE_BVEC4;
              } else if(strcmp(yytext, "ivec2") == 0) {
                yTRACE(TYPE_IVEC2);
                return TYPE_IVEC2;
              } else if(strcmp(yytext, "ivec3") == 0) {
                yTRACE(TYPE_IVEC3);
                return TYPE_IVEC3;
              } else if(strcmp(yytext, "ivec4") == 0) {
                yTRACE(TYPE_IVEC4);
                return TYPE_IVEC4;
              } else if(strcmp(yytext, "dp3") == 0) {
                yTRACE(FUNC_DP3);
                return FUNC_DP3;
              } else if(strcmp(yytext, "lit") == 0) {
                yTRACE(FUNC_LIT);
                return FUNC_LIT;
              } else if(strcmp(yytext, "rsq") == 0) {
                yTRACE(FUNC_RSQ);
                return FUNC_RSQ;
              } else if(strcmp(yytext, "const") == 0) {
                yTRACE(CONST);
                return CONST;
              } else if(strcmp(yytext, "while") == 0) {
                yTRACE(WHILE);
                return WHILE;
              } else if(strcmp(yytext, "if") == 0) {
                yTRACE(IF);
                return IF;
              } else if(strcmp(yytext, "else") == 0) {
                yTRACE(ELSE);
                return ELSE;
              }
              yERROR("Unknown keyword");
            }

{id}        {
              /* C doesn't set a limit for the maximum length of a variable */
              /* maximum size is bounded by the size of the stack */
              yylval.str = yytext;
              if (strlen(yytext) >= 32) {
                yERROR("Identifier exceeds maximum length");
              }
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
  const char* LIMIT = "2097151";
  const int LIMIT_LEN = strlen(LIMIT);

  if (str[0] == '-') {
    startIndex = 1;
    numLen--;
    LIMIT = "2097152";
  } else if (str[0] == '+') {
    startIndex = 1;
    numLen--;
    LIMIT = "2097151";
  }

  if (numLen > LIMIT_LEN) {
    return false;
  }

  if (numLen < LIMIT_LEN) {
    *num = atoi(str);
    return true;
  }
 
  /* num Len == 10 */
  for (int i = startIndex; i < len; i++) {
    if (str[i] > LIMIT[i-startIndex]) {
      return false;
    } else if (str[i] < LIMIT[i-startIndex]) {
      break;
    }
  }
  
  *num = atoi(str);
  return true;
}
