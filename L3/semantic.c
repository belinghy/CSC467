#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semantic.h"
#include "parser.tab.h"

bool type_check(Type *info1, Type *info2);
int semantic_check_recurse(node *n, SymbolTable *prev_sym);
void binary_check_type(Type *type_out, int line, int op, Type *type1, Type *type2);
void set_type(Type *src, Type *dst);

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
   
    Type *temp_type;

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
      if((s->put(n->declaration.id, n->type_info)) == 1){
        fprintf(errorFile, "ERROR: line %d: redeclaration of variable %s\n", n->line, n->declaration.id);
        errorOccurred = true;
      }
      break;

    case DECLARATION_WITH_INIT_NODE:
    {
      if((s->put(n->declaration_init.id, n->type_info)) == 1){
        fprintf(errorFile, "ERROR: line %d: redeclaration of variable %s\n", n->line, n->declaration_init.id);
        errorOccurred = true;
      } else if (type_check(n->type_info, (n->declaration_init.expression)->type_info)) {
        fprintf(errorFile, "ERROR: line %d: type mismatch in declaration of variable %s\n", n->line, n->declaration_init.id);
        errorOccurred = true;
      }
      break;
    }

    case DECLARATION_CONST_NODE:
    {
      // TODO: FIXME
      if((s->put(n->declaration_const.id, n->type_info)) == 1){
        fprintf(errorFile, "ERROR: line %d: redeclaration of variable %s\n", n->line, n->declaration_const.id);
        errorOccurred = true;
      } else if (n->type_info->is_const) {
        if (((n->declaration_const.expression)->kind == VAR_NODE) && !((n->declaration_const.expression)->type_info->is_const)) {
          fprintf(errorFile, "ERROR: line %d: const variable, %s, cannot be initialized with non-constant type\n", n->line, n->declaration_init.id);
          errorOccurred = true;
        }
      } else if (type_check(n->type_info, (n->declaration_const.expression)->type_info)) {
        fprintf(errorFile, "ERROR: line %d: type mismatch in declaration of variable %s\n", n->line, n->declaration_init.id);
        errorOccurred = true;
      }
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
      semantic_check_recurse(n->binary_expr.left, s);
      semantic_check_recurse(n->binary_expr.right, s);
      binary_check_type(n->type_info, n->line, n->binary_expr.op, (n->binary_expr.left)->type_info, (n->binary_expr.right)->type_info);
      break;
    //BOOL_NODE,
    //INT_NODE,
    //FLOAT_NODE,
    case VAR_NODE:
    {
      /* Checking variable was declared */
      SymAttr *attr = s->lookup(n->variable.identifier);
      if(attr == NULL){
        fprintf(errorFile, "ERROR: line %d: undeclared variable %s\n", n->line, n->variable.identifier);
        errorOccurred = 1;
        break;
      } 

      /* Getting type from declared variable */
      set_type(attr->type, n->type_info);
      //char buf[10];
      //get_type(n->type_info, buf);
      //printf("HELLOO set var %s to type %s \n", n->variable.identifier, buf);

      /* Checking index is not out of bounds */
      if((n->type_info)->length <= n->variable.index){
        fprintf(errorFile, "ERROR: line %d: index %d is out of bounds for variable %s of length %d\n", n->line, n->variable.index, n->variable.identifier, (n->type_info)->length);
        errorOccurred = 1;
      }
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

bool type_check(Type *info1, Type *info2) {
  // return true if error, false if no error
  if (info1->basic_type == info2->basic_type && info1->length == info2->length) {
    return false;
  } else {
    return true;
  }
}

void set_type(Type *src, Type *dst){
    dst->length = src->length;
    dst->basic_type = src->basic_type;
    dst->is_const = src->is_const;
}

bool is_arithmetic(Type *type){
  return(type->basic_type == Type::INT || type->basic_type == Type::FLOAT);
}

bool is_scalar(Type *type){
  return(type->length == 1);
}

void binary_check_type(Type *type_out, int line, int op, Type *type1, Type *type2) {

  /* if one or more arguments have type ANY, we also result in type ANY */
  if((type1->basic_type == Type::ANY) || (type2->basic_type == Type::ANY)){
    return;
  }

  /* Argument type check */
  switch (op) {
    case OR:
    case AND:
      if(type1->basic_type != Type::BOOLEAN){
        fprintf(errorFile, "ERROR: line %d, argument 1 of %s operator does not have boolean type\n", line, get_operator(op));
        errorOccurred = 1;
        return;
      } 
      if (type2->basic_type != Type::BOOLEAN){
        fprintf(errorFile, "ERROR: line %d, argument 2 of %s operator does not have boolean type\n", line, get_operator(op));
        errorOccurred = 1;
        return;
      } 
      break;
    case EQ:
    case NEQ:
    case '+':
    case '-':
    case '*':
    case '/':
    case '^':
    case '>':
    case '<':
    case LEQ:
    case GEQ:
      if(!is_arithmetic(type1)){
        fprintf(errorFile, "ERROR: line %d, argument 1 of %s operator does not have arithmetic type\n", line, get_operator(op));
        errorOccurred = 1;
        return;
      } 
      if (!is_arithmetic(type2)){
        fprintf(errorFile, "ERROR: line %d, argument 2 of %s operator does not have arithmetic type\n", line, get_operator(op));
        errorOccurred = 1;
        return;
      } 

      if (type1->basic_type != type2->basic_type){
        fprintf(errorFile, "ERROR: line %d, arguments of %s operator do not have same basic type\n", line, get_operator(op));
        errorOccurred = 1;
        return;
      } 
      break;
    default:
      break;
  }
  
  /* Length check */
  switch (op) {
    case OR:
    case AND:
      if (type1->length != type2->length){
        fprintf(errorFile, "ERROR: line %d, lengths of %s operator arguments do not match\n", line, get_operator(op));
        errorOccurred = 1;
        return;
      }
      break;
    case EQ:
    case NEQ:
    case '+':
    case '-':
    case '/':
    case '^':
      if (type1->length != type2->length){
        fprintf(errorFile, "ERROR: line %d, lengths of %s operator arguments do not match\n", line, get_operator(op));
        errorOccurred = 1;
        return;
      }
      break;
    case '>':
    case '<':
    case LEQ:
    case GEQ:
      if (!is_scalar(type1) || !is_scalar(type2)){
        fprintf(errorFile, "ERROR: line %d, arguments of %s operator must be scalar\n", line, get_operator(op));
        errorOccurred = 1;
        return;
      }
      break;
    case '*': 
      if (!is_scalar(type1) && !is_scalar(type2) && (type1->length != type2->length)){
        fprintf(errorFile, "ERROR: line %d, lengths of %s operator vector arguments do not match\n", line, get_operator(op));
        errorOccurred = 1;
        return;
      }
      break;
    default:
      break;
  }
  
  /* Set results */
  switch (op) {
    case OR:
    case AND:
      set_type(type1, type_out);
      break;
    case EQ:
    case NEQ:
      type_out->length = type1->length;
      type_out->basic_type = Type::BOOLEAN;
      break;
    case '+':
    case '-':
    case '/':
    case '^':
      type_out->length = type1->length;
      type_out->basic_type = type1->basic_type;
      break;
    case '>':
    case '<':
    case LEQ:
    case GEQ:
      type_out->length = 1;
      type_out->basic_type = Type::BOOLEAN;
      break;
    case '*':
      if(!is_scalar(type1)){
        type_out->length = type1->length;
      } else if(!is_scalar(type2)){
        type_out->length = type2->length;
      } else {
        type_out->length = 1;
      }
      type_out->basic_type = type1->basic_type;
      break;
    default:
      break;
  }
}
