
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>

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

struct TypeStruct;
typedef struct TypeStruct Type;

struct TypeStruct {
   enum BasicType{
   INT,
   FLOAT,
   BOOLEAN,
   Any
   };
   
   BasicType basic_type = Any;
 
   int length = 1;
   bool is_const = false;
};

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

  union {
    struct {
      node *declarations;
      node *statements;
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
    } variable;

    struct {
      node *arguments;
      node *argument;
    } arguments;
  };
};

node *ast_allocate(node_kind type, ...);
void ast_free(node *ast);
void ast_print(node * ast);
char *get_type(Type *type);
char *get_operator(int op);
char *get_function(int id);

#endif /* AST_H_ */
