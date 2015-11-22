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
  
  case SCOPE_NODE:
    ast->scope.declarations = va_arg(args, node *);
    ast->scope.statements = va_arg(args, node *);
    break;

  case DECLARATIONS_NODE:
  {
    node *declarations = va_arg(args, node *);
    if (declarations != NULL) {
      ast->declarations.declarations = declarations;
      ast->declarations.declaration = va_arg(args, node *);
    } else {
      ast->declarations.declarations = NULL;
      ast->declarations.declaration = NULL;
    }
    break;
  }

  case STATEMENTS_NODE:
  {
    node *statements = va_arg(args, node *);
    if (statements != NULL) {
      ast->statements.statements = statements;
      ast->statements.statement = va_arg(args, node *);
    } else {
      ast->statements.statements = NULL;
      ast->statements.statements = NULL;
    }
    break;
  }

  case DECLARATION_NODE:
  {
    // TODO
    break;
  }

  case DECLARATION_WITH_INIT_NODE:
  {
    // TODO
    break;
  }

  case DECLARATION_CONST_NODE:
  {
    // TODO
    break;
  }

  case ASSIGNMENT_NODE:
    ast->assignment_stmt.op = va_arg(args, int);
    ast->assignment_stmt.left = va_arg(args, node *);
    ast->assignment_stmt.right = va_arg(args, node *);
    break;

  case IF_WITH_ELSE_STATEMENT_NODE:
  {
    // TODO
    break;
  }

  case IF_STATEMENT_NODE:
  {
    // TODO
    break;
  }

  case CONSTRUCTOR_NODE:
  {
    // TODO
    break;
  }

  case FUNCTION_NODE:
  {
    // TODO
    break;
  }

  case UNARY_EXPRESION_NODE:
  {
    // TODO
    break;
  }

  case BINARY_EXPRESSION_NODE:
    ast->binary_expr.op = va_arg(args, int);
    ast->binary_expr.left = va_arg(args, node *);
    ast->binary_expr.right = va_arg(args, node *);
    break;

  case BOOL_NODE:
  {
    // TODO
    break;
  }

  case INT_NODE:
    ast->int_literal.value = va_arg(args, int);
    break;

  case FLOAT_NODE:
    ast->float_literal.value = va_arg(args, float);
    break;

  case VAR_NODE:
    ast->var_expr.identifier = va_arg(args, char*);
    break;

  case ARGUMENTS_NODE:
  {
    // TODO
    break;
  }

  default: break;
  }

  ast->line = va_arg(args, int);

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

  case SCOPE_NODE:
    break;

  case DECLARATIONS_NODE:
    break;

  case STATEMENTS_NODE:
    break;
  
  case BINARY_EXPRESSION_NODE:
    printf("%c", n->binary_expr.op);
    break;

  case INT_NODE:
    printf("%d", n->int_literal.value);
    break;
  
  case FLOAT_NODE:
    printf("%f", n->float_literal.value);
    break;

  case VAR_NODE:
    printf("%s", n->var_expr.identifier);
    break;

  case ASSIGNMENT_NODE:
    printf("%c", n->assignment_stmt.op);
    break;

  default:
    break;
  }
}

/* if no child at that index, return null */
node *ast_get_child(node *n, int child_index) {

  switch (n->kind) {

  case SCOPE_NODE:
  {
    if (child_index == 0) {
      return n->scope.declarations;
    } else if (child_index == 1) {
      return n->scope.statements;
    }
    break;
  }

  case DECLARATIONS_NODE:
  {
    if (child_index == 0) {
      return n->declarations.declarations;
    } else if (child_index == 1) {
      return n->declarations.declaration;
    }
    break;
  }

  case STATEMENTS_NODE:
  {
    if (child_index == 0) {
      return n->statements.statements;
    } else if (child_index == 1) {
      return n->statements.statement;
    }
    break;
  }
  
  case BINARY_EXPRESSION_NODE:
  {
    if (child_index == 0) {
      return n->binary_expr.left;
    } else if (child_index == 1) {
      return n->binary_expr.right;
    }
    break;
  }

  case INT_NODE:
  case VAR_NODE:
    break;

  case ASSIGNMENT_NODE:
  {
    if (child_index == 0) {
      return n->assignment_stmt.left;
    } else if (child_index == 1) {
      return n->assignment_stmt.right;
    }
    break;
  }

  default:
    break;
  }

  return NULL;
}
