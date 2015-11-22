#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semantic.h"

int semantic_check_recurse(node *n, SymbolTable *prev_sym);

int semantic_check( node *ast) {

    printf("Semantic check ...\n");

  return semantic_check_recurse(ast, NULL); // failed checks
}

/*
  union {
    struct {
      node *declarations;
      node *statements;
      SymbolTable *symbols;
    } scope;

    struct {
      node *declarations;
      node *declaration;
    } declarations;

    struct {
      node *statements;
      node *statement;
    } statements;

    struct {
      Type *type_info;
      char *id;
    } declaration;

    struct {
      char *id;
      node *expression;
      Type *type_info;
    } declaration_init;

    struct {
      char *id;
      node *expression;
      Type *type_info;
    } declaration_const;

    struct {
      int op;
      node *left;
      node *right;
    } assignment_stmt;

    struct {
      node *expression;
      node *then_stmt;
      node *else_stmt; 
    } if_else_stmt;

    struct {
      node *expression;
      node *then_stmt;
    } if_stmt;

    struct {
      Type *type_info;
      node *arguments;    
    } constructor;

    struct {
      int func_id;
      node *arguments;
    } function;

    struct {
      int op;
      // Type *type_info;
      node *right;
    } unary_expr;

    struct {
      int op;
      // Type *type_info;
      node *left;
      node *right;
    } binary_expr;

    struct {
      int value;
    } int_literal;

    struct {
      float value;
    } float_literal;

    struct {
      bool value;
    } bool_literal;

    struct {
      char *identifier;
      Type *type_info;
      int index;
    } variable;

    struct {
      node *arguments;
      node *argument;
    } arguments;
  };
*/

int semantic_check_recurse(node *n, SymbolTable *s){
    if(n == NULL){
      return 1;
    }

    switch(n->kind) {
    case SCOPE_NODE:
      n->scope.symbols = new SymbolTable(s);
      semantic_check_recurse(n->scope.declarations, n->scope.symbols);
      semantic_check_recurse(n->scope.statements, n->scope.symbols);
      break;

    case DECLARATIONS_NODE:
      semantic_check_recurse(n->declarations.declarations, s);
      semantic_check_recurse(n->declarations.declaration, s);
      break;

    case STATEMENTS_NODE:
      semantic_check_recurse(n->statements.statements, s);
      semantic_check_recurse(n->statements.statement, s);
      break;

    case DECLARATION_NODE:
      /* Enter variable into symbol table */
      if((s->put(n->declaration.id, n->declaration.type_info)) == 1){
          printf("ERROR: line %d: redeclaration of variable %s\n", n->line, n->declaration.id);
      }
      break;

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
      semantic_check_recurse(n->assignment_stmt.left, s);
      semantic_check_recurse(n->assignment_stmt.right, s);
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
      SymAttr *attr = s->lookup(n->variable.identifier);
      if(attr == NULL){
        printf("ERROR: line %d: undeclared variable %s\n", n->line, n->variable.identifier);
        return 0;
      } 

      printf("Got symbol %s \n", attr->name);
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

    return 0;
}
