#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semantic.h"

int semantic_check( node *n) {

    printf("Sematic check ...");

    switch(n->kind) {
    case SCOPE_NODE:
      //TODO: push and pop new symbol table
      break;

    //DECLARATIONS_NODE
    //STATEMENTS_NODE
 
    case DECLARATION_NODE:
    {
      //TODO: add to symbol table
      break;
    }
    case DECLARATION_WITH_INIT_NODE:
    {
      //TODO: check type, add to symbol table
      break;
    }
    case DECLARATION_CONST_NODE:
    {
      //TODO: check type, check const, add to symbol table
      break;
    }
    case ASSIGNMENT_NODE:
    {
      //TODO: check type
      break;
    }
    case IF_WITH_ELSE_STATEMENT_NODE:
    {
      //TODO: check type of expr is boolean
      break;
    }
    case IF_STATEMENT_NODE:
    {
      //TODO: check type of expr is boolean
      break;
    }
    case CONSTRUCTOR_NODE:
    {
      //TODO: check number and type of arguments
      break;
    }
    case FUNCTION_NODE:
    {
      //TODO: check number and type of arguments
      break;
    }
    case UNARY_EXPRESSION_NODE:
    {
      //TODO: check type of arguments
      break;
    }
    case BINARY_EXPRESSION_NODE:
    {
      //TODO: check type of arguments
      break;
    }
    //BOOL_NODE,
    //INT_NODE,
    //FLOAT_NODE,
    case VAR_NODE:
    {
      //TODO: check is declared, get type from symbol table, check index not out of bounds
      break;
    }
    case ARGUMENTS_NODE:
    {
      //TODO: get number of arguments
      break;
    }
    default:
      break;
    }
  return 0; // failed checks
}
