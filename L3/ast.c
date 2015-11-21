#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0

node *ast_get_child(node *n, int child_index);
void ast_print_node(node *n);
void ast_print_recurse(node *n, int level);

node *ast = NULL;

node *ast_allocate(node_kind kind, ...) {
  va_list args;

  // make the node
  node *ast = (node *) malloc(sizeof(node));
  memset(ast, 0, sizeof *ast);
  ast->kind = kind;

  va_start(args, kind); 

  switch(kind) {
  
  // ...

  case BINARY_EXPRESSION_NODE:
    ast->binary_expr.op = va_arg(args, int);
    ast->binary_expr.left = va_arg(args, node *);
    ast->binary_expr.right = va_arg(args, node *);
    break;

  case INT_NODE:
    ast->integer_expr = va_arg(args, int);
    break;

  case IDENT_NODE:
    ast->id_expr = va_arg(args, char*);
    break;

  case ASSIGNMENT_NODE:
    ast->assignment_stmt.op = va_arg(args, int);
    ast->assignment_stmt.left = va_arg(args, node *);
    ast->assignment_stmt.right = va_arg(args, node *);
    break;

  case STATEMENTS_NODE:
    {
      ast->statements.name = va_arg(args, char *);
      node *statements_list = va_arg(args, node *);
      if(statements_list == NULL){
        // new list
        (*ast).statements.children = new std::vector<node *>();
      } else {
        if ( ((*statements_list).statements.children) == NULL ) {
          (*statements_list).statements.children = new std::vector<node *>();
        }
        node *new_statement = va_arg(args, node *);
        //concatonate
        (*((*statements_list).statements.children)).push_back(new_statement);
        free(ast);
        //ast = statements_list;
      }
      break;
    }
  default: break;
  }

  va_end(args);

  return ast;
}

void ast_free(node *ast) {
}

void ast_print(node *ast) {
  ast_print_recurse(ast, 0);
}

void ast_print_recurse(node *n, int level) {
  if (n == NULL) return;

  for (int i = 0; i < level; i++) {
    printf("    ");
  }

  ast_print_node(n);
  printf("\n");

  int child_index = 0;
  node *child;
  while ((child = ast_get_child(n, child_index))) {
    ast_print_recurse(child, (level + 1));
    child_index++;
  }
}

void ast_print_node(node *n) {
  switch (n->kind) {
  
  case BINARY_EXPRESSION_NODE:
    printf("%d", n->binary_expr.op);
    break;

  case INT_NODE:
    printf("%d", n->integer_expr);
    break;
  
  case IDENT_NODE:
    printf("%s", n->id_expr);
    break;

  case ASSIGNMENT_NODE:
    printf("%c", n->assignment_stmt.op);
    break;

  case STATEMENTS_NODE:
    printf("%s", n->statements.name);
    break;

  default:
    break;
  }
}

/* if no child at that index, return null */
node *ast_get_child(node *n, int child_index) {

  switch (n->kind) {
  
  case BINARY_EXPRESSION_NODE:
    if (child_index == 0) {
      return n->binary_expr.left;
    } else if (child_index == 1) {
      return n->binary_expr.right;
    }
    break;
  
  case INT_NODE:
  case IDENT_NODE:
    break;

  case ASSIGNMENT_NODE:
    if (child_index == 0) {
      return n->assignment_stmt.left;
    } else if (child_index == 1) {
      return n->assignment_stmt.right;
    }
    break;

  case STATEMENTS_NODE:
    if (((*n).statements.children) != NULL &&
        child_index < (*((*n).statements.children)).size()) {
      return (*((*n).statements.children)).at(child_index);
    }
    break;

  default:
    break;
  }

  return NULL;
}
