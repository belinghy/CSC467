
#ifndef AST_H_
#define AST_H_ 1
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <unordered_map>
#include <stdarg.h>

#include "symbol.h"
#include "common.h"

// Dummy node just so everything compiles, create your own node/nodes
//
// The code provided below is an example ONLY. You can use/modify it,
// but do not assume that it is correct or complete.
//
// There are many ways of making AST nodes. The approach below is an example
// of a descriminated union. If you choose to use C++, then I suggest looking
// into inheritance.

// forward declare
struct node_;
typedef struct node_ node;
extern node *ast;

typedef enum {
    SCOPE_NODE,

    DECLARATIONS_NODE,
    STATEMENTS_NODE,

    DECLARATION_NODE,
    DECLARATION_WITH_INIT_NODE,
    DECLARATION_CONST_NODE,

    ASSIGNMENT_NODE,
    IF_WITH_ELSE_STATEMENT_NODE,
    IF_STATEMENT_NODE,
    CONSTRUCTOR_NODE,
    FUNCTION_NODE,
    UNARY_EXPRESSION_NODE,
    BINARY_EXPRESSION_NODE,

    BOOL_NODE,
    INT_NODE,
    FLOAT_NODE,
    VAR_NODE,
    ARGUMENTS_NODE,
} node_kind;

struct node_ {

  // an example of tagging each node with a type
  node_kind kind;
  int line;
  Type *type_info;

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
      // Type *type_info;
      char *id;
    } declaration;

    struct {
      char *id;
      node *expression;
      // Type *type_info;
    } declaration_init;

    struct {
      char *id;
      node *expression;
      // Type *type_info;
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
      // Type *type_info;
      node *arguments;    
    } constructor;

    struct {
      int func_id;
      node *arguments;
      // Type *type_info;
    } function;

    struct {
      int op;
      node *right;
      // Type *type_info;
    } unary_expr;

    struct {
      int op;
      node *left;
      node *right;
      // Type *type_info;
    } binary_expr;

    struct {
      int value;
      // Type *type_info;
    } int_literal;

    struct {
      float value;
      // Type *type_info;
    } float_literal;

    struct {
      bool value;
      // Type *type_info;
    } bool_literal;

    struct {
      char *identifier;
      // Type *type_info;
      int index;
    } variable;

    struct {
      node *arguments;
      node *argument;
      // Type *type_info;
      int num_args;
    } arguments;
  };
};

node *ast_allocate(node_kind type, ...);
void ast_free(node *ast);
void ast_print(node * ast);
void get_type(Type *type, char *buf);
char *get_operator(int op);
char *get_function(int id);

#endif /* AST_H_ */
