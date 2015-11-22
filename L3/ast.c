#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0
#define PRINT_INDENT(indent) { for(int i = 0; i < indent; i++) { printf("    "); }}

node *ast_get_child(node *n, int child_index);
void ast_print_node(node *n);
void ast_print_recurse(node *n, int indent, int level);

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
    ast->declarations.declarations = va_arg(args, node *);
    ast->declarations.declaration = va_arg(args, node *);
    break;
  }

  case STATEMENTS_NODE:
  {
    ast->statements.statements = va_arg(args, node *);
    ast->statements.statement = va_arg(args, node *);
    break;
  }

  case DECLARATION_NODE:
  {
    ast->declaration.type_info = va_arg(args, Type *);
    ast->declaration.id = va_arg(args, char *);
    break;
  }

  case DECLARATION_WITH_INIT_NODE:
  {
    ast->declaration_init.type_info = va_arg(args, Type *);
    ast->declaration_init.id = va_arg(args, char *);
    ast->declaration_init.expression = va_arg(args, node *);
    break;
  }

  case DECLARATION_CONST_NODE:
  {
    Type *type_info = va_arg(args, Type *);
    type_info->is_const = true;
    ast->declaration_const.type_info = type_info;
    ast->declaration_const.id = va_arg(args, char *);
    ast->declaration_const.expression = va_arg(args, node *);
    break;
  }

  case ASSIGNMENT_NODE:
    ast->assignment_stmt.op = va_arg(args, int);
    ast->assignment_stmt.left = va_arg(args, node *);
    ast->assignment_stmt.right = va_arg(args, node *);
    break;

  case IF_WITH_ELSE_STATEMENT_NODE:
  {
    ast->if_else_stmt.expression = va_arg(args, node *);
    ast->if_else_stmt.then_stmt = va_arg(args, node *);
    ast->if_else_stmt.else_stmt = va_arg(args, node *);
    break;
  }

  case IF_STATEMENT_NODE:
  {
    ast->if_stmt.expression = va_arg(args, node *);
    ast->if_stmt.then_stmt = va_arg(args, node *);
    break;
  }

  case CONSTRUCTOR_NODE:
  {
    ast->constructor.type_info = va_arg(args, Type *);
    ast->constructor.arguments = va_arg(args, node *);
    break;
  }

  case FUNCTION_NODE:
  {
    ast->function.func_id = va_arg(args, int);
    ast->function.arguments = va_arg(args, node *);
    break;
  }

  case UNARY_EXPRESSION_NODE:
  {
    ast->unary_expr.op = va_arg(args, int);
    ast->unary_expr.right = va_arg(args, node *);
    break;
  }

  case BINARY_EXPRESSION_NODE:
    ast->binary_expr.op = va_arg(args, int);
    ast->binary_expr.left = va_arg(args, node *);
    ast->binary_expr.right = va_arg(args, node *);
    break;

  case BOOL_NODE:
    ast->bool_literal.value = (va_arg(args, int) == 1) ? true : false;
    break;

  case INT_NODE:
    ast->int_literal.value = va_arg(args, int);
    break;

  case FLOAT_NODE:
    ast->float_literal.value = (float) va_arg(args, double);
    break;

  case VAR_NODE:
    ast->variable.identifier = va_arg(args, char *);
    ast->variable.type_info = va_arg(args, Type *);
    ast->variable.index = va_arg(args, int);
    break;

  case ARGUMENTS_NODE:
  {
    ast->arguments.arguments = va_arg(args, node *);
    ast->arguments.argument = va_arg(args, node *);
    break;
  }

  default: break;
  }

  ast->line = va_arg(args, int);

  va_end(args);

  return ast;
}

void ast_free(node *ast) {

  if (ast == NULL) return;

  switch(ast->kind) {
  printf("%d", ast->kind);
  printf("\n");

  case SCOPE_NODE:
  {
    ast_free(ast->scope.declarations);
    ast_free(ast->scope.statements);
    // free symbol table
    if (ast->scope.symbols != NULL) {
      delete ast->scope.symbols;
    }
    break;
  }

  case DECLARATIONS_NODE:
  {
    ast_free(ast->declarations.declarations);
    ast_free(ast->declarations.declaration);
    break;
  }

  case STATEMENTS_NODE:
  {
    ast_free(ast->statements.statements);
    ast_free(ast->statements.statement);
    break;
  }

  case DECLARATION_NODE:
  {
    free(ast->declaration.type_info); ast->declaration.type_info = NULL;
    free(ast->declaration.id);
    break;
  }

  case DECLARATION_WITH_INIT_NODE:
  {
    ast_free(ast->declaration_init.expression);
    free(ast->declaration_init.type_info); ast->declaration_init.type_info = NULL;
    free(ast->declaration_init.id);
    break;
  }

  case DECLARATION_CONST_NODE:
  {
    ast_free(ast->declaration_const.expression);
    free(ast->declaration_const.type_info); ast->declaration_const.type_info = NULL;
    free(ast->declaration_const.id); // = va_arg(args, char *);
    break;
  }

  case ASSIGNMENT_NODE:
  {
    // ast->assignment_stmt.op = va_arg(args, int);
    ast_free(ast->assignment_stmt.left);
    ast_free(ast->assignment_stmt.right);
    break;
  }

  case IF_WITH_ELSE_STATEMENT_NODE:
  {
    ast_free(ast->if_else_stmt.expression);
    ast_free(ast->if_else_stmt.then_stmt);
    ast_free(ast->if_else_stmt.else_stmt);
    break;
  }

  case IF_STATEMENT_NODE:
  {
    ast_free(ast->if_stmt.expression);
    ast_free(ast->if_stmt.then_stmt);
    break;
  }

  case CONSTRUCTOR_NODE:
  {
    ast_free(ast->constructor.arguments);
    free(ast->constructor.type_info); ast->constructor.type_info = NULL;
    break;
  }

  case FUNCTION_NODE:
  {
    // ast->function.func_id = va_arg(args, int);
    ast_free(ast->function.arguments);
    break;
  }

  case UNARY_EXPRESSION_NODE:
  {
    // ast->unary_expr.op = va_arg(args, int);
    ast_free(ast->unary_expr.right);
    break;
  }

  case BINARY_EXPRESSION_NODE:
  { 
    // ast->binary_expr.op = va_arg(args, int);
    ast_free(ast->binary_expr.left);
    ast_free(ast->binary_expr.right);
    break;
  }

  case BOOL_NODE:
    // ast->bool_literal.value = (va_arg(args, int) == 1) ? true : false;
    break;

  case INT_NODE:
    // ast->int_literal.value = va_arg(args, int);
    break;

  case FLOAT_NODE:
    // ast->float_literal.value = (float) va_arg(args, double);
    break;

  case VAR_NODE:
  {  
    free(ast->variable.identifier);// = va_arg(args, char *);
    free(ast->variable.type_info); ast->variable.type_info = NULL;
    // ast->variable.index = va_arg(args, int);
    break;
  }

  case ARGUMENTS_NODE:
  {
    ast_free(ast->arguments.arguments);
    ast_free(ast->arguments.argument);
    break;
  }

  default: break;
  }

  free(ast); ast = NULL;
}

void ast_print(node *ast) {
  ast_print_recurse(ast, 0, 0);
}

void ast_print_recurse(node *n, int indent, int level) {
  if (n == NULL) return;

  switch(n->kind) {
  
  case SCOPE_NODE:
    PRINT_INDENT(indent); printf("(SCOPE \n");
    ast_print_recurse(n->scope.declarations, indent + 1, 0);
    ast_print_recurse(n->scope.statements, indent + 1, 0);
    PRINT_INDENT(indent); printf("END_SCOPE)\n");
    break;

  case DECLARATIONS_NODE:
  {
    if (level == 0) {
      PRINT_INDENT(indent); printf("(DECLARATIONS \n");
    }
    ast_print_recurse(n->declarations.declarations, indent, level + 1);
    ast_print_recurse(n->declarations.declaration, indent + 1, level);
    if (level == 0) {
      PRINT_INDENT(indent); printf("END_DECLARATIONS)\n");
    }
    break;
  }

  case STATEMENTS_NODE:
  {
    if (level == 0) {
      PRINT_INDENT(indent); printf("(STATEMENTS \n");
    }
    ast_print_recurse(n->statements.statements, indent, level + 1);
    ast_print_recurse(n->statements.statement, indent + 1, level);
    if (level == 0) {
      PRINT_INDENT(indent); printf("END_STATEMENTS)\n");
    }
    break;
  }

  case DECLARATION_NODE:
  {
    PRINT_INDENT(indent);
    char buf[20];
    get_type(n->declaration.type_info, buf);
    printf("(DECLARATION %s %s)\n",
              n->declaration.id,
              buf);
    // free(buf);
    break;
  }
  
  case DECLARATION_WITH_INIT_NODE:
  {
    PRINT_INDENT(indent); printf("(DECLARATION ");
    char buf[20];
    get_type(n->declaration_init.type_info, buf);
    printf("%s %s ", n->declaration_const.id,
                    buf);
    // free(buf);
    ast_print_recurse(n->declaration_const.expression, indent, level);
    printf(")\n");
    break;
  }

  case DECLARATION_CONST_NODE: /* No need to print const */
  {
    PRINT_INDENT(indent); printf("(DECLARATION ");
    char buf[20];
    get_type(n->declaration_init.type_info, buf);
    printf("%s %s ", n->declaration_init.id,
                    buf);
    // free(buf);
    ast_print_recurse(n->declaration_init.expression, indent, level);
    printf(")\n");
    break;
  }

  case ASSIGNMENT_NODE:
  {
    PRINT_INDENT(indent); printf("(ASSIGN ");
    ast_print_recurse(n->assignment_stmt.left, indent, level);
    printf(" ");
    ast_print_recurse(n->assignment_stmt.right, indent, level);
    printf(")\n");
    break;
  }

  case IF_WITH_ELSE_STATEMENT_NODE:
  {
    PRINT_INDENT(indent); printf("(IF ");
    ast_print_recurse(n->if_else_stmt.expression, indent, level);
    printf("\n");
    ast_print_recurse(n->if_else_stmt.then_stmt, indent + 1, level);
    if(n->kind == IF_WITH_ELSE_STATEMENT_NODE){
        printf("\n"); PRINT_INDENT(indent); printf(" ELSE\n");
        ast_print_recurse(n->if_else_stmt.else_stmt, indent + 1, level);
    }
    PRINT_INDENT(indent); printf(" END_IF)\n");
    break;
  }

  case IF_STATEMENT_NODE:
  {
    PRINT_INDENT(indent); printf("(IF ");
    ast_print_recurse(n->if_stmt.expression, indent, level);
    printf("\n");
    ast_print_recurse(n->if_stmt.then_stmt, indent + 1, level);
    PRINT_INDENT(indent); printf(" END_IF)\n");
    break;
  }

  case CONSTRUCTOR_NODE:
  {
    printf("(CALL ");
    char buf[20];
    get_type(n->declaration.type_info, buf);
    printf("%s", buf);
    // free(buf);
    ast_print_recurse(n->constructor.arguments, indent, level);
    printf(")");
    break;
  }

  case FUNCTION_NODE:
  {
    printf("(CALL ");
    printf("%s",get_function(n->function.func_id));
    ast_print_recurse(n->function.arguments, indent, level);
    printf(")");
    break;
  }

  case UNARY_EXPRESSION_NODE:
  {
    printf("(UNARY ");
    printf("%s ", get_operator(n->unary_expr.op)); //TODO: type
    ast_print_recurse(n->unary_expr.right, indent, level);
    printf(")");
    break;
  }

  case BINARY_EXPRESSION_NODE:
  {
    printf("(BINARY ");//TODO: type
    printf("%s ", get_operator(n->binary_expr.op));
    ast_print_recurse(n->binary_expr.left, indent, level);
    printf(" ");
    ast_print_recurse(n->binary_expr.right, indent, level);
    printf(")");
    break;
  }

  case BOOL_NODE:
  { 
    if (n->bool_literal.value == 1) {
      printf("true");
    } else {
      printf("false");
    }
    break;
  }

  case INT_NODE:
  {
    printf("%d", n->int_literal.value);
    break;
  }

  case FLOAT_NODE:
  {
    printf("%f", n->float_literal.value);
    break;
  }

  case VAR_NODE:
  {
    if ((n->variable.type_info)->length == 1) {
      printf("%s", n->variable.identifier);
    } else {
      char buf[20];
      get_type(n->variable.type_info, buf);
      printf("(INDEX %s %s %d)",
                buf,
                n->variable.identifier,
                n->variable.index);
      // free(buf);
    }
    // n->variable.type_info = va_arg(args, Type *);
    break;
  }

  case ARGUMENTS_NODE:
  {
    ast_print_recurse(n->arguments.arguments, indent, level);
    printf(" ");
    ast_print_recurse(n->arguments.argument, indent, level);
    break;
  }

  default: break;
  }
}

char *get_function(int id){ //TODO: get enums declared here?
  switch(id){
    case 0:
      return "dp3"; 
    case 1:
      return "lit"; 
    case 2:
      return "rsq";
    default:
      return "UNKNOWN-FUNC";
  }
}

void get_type(Type *type, char *buf) {
  if (type->length == 1) {
    switch (type->basic_type) {
      case Type::INT:
        sprintf(buf, "%s", "int");
        // return "int";
        break;
      case Type::FLOAT:
        sprintf(buf, "%s", "float");
        // return "float";
        break;
      case Type::BOOLEAN:
        sprintf(buf, "%s", "bool");
        // return "bool";
        break;
      default:
        sprintf(buf, "%s", "ANY-TYPE");
        // return "ANY-TYPE";
        break;
    }
  } else {
    switch (type->basic_type) {
      //char tmpstr[6];
      case Type::INT:
        sprintf(buf, "ivec%d", type->length);
        // return tmpstr;
        break;
      case Type::FLOAT:
        sprintf(buf, "vec%d", type->length);
        // return tmpstr;
        break;
      case Type::BOOLEAN:
        sprintf(buf, "bvec%d", type->length);
        // return tmpstr;
        break;
      default:
        sprintf(buf, "%s", "ANY-TYPE-VECTOR");
        // return "ANY-TYPE-VECTOR";
        break;
    }
  }
}

char *get_operator(int op) {
  switch (op) {
    case OR:
      return "||";
    case AND:
      return "&&";
    case EQ:
      return "==";
    case NEQ:
      return "!=";
    case '>':
      return ">";
    case '<':
      return "<";
    case LEQ:
      return "<=";
    case GEQ:
      return ">=";
    case '+':
      return "+";
    case '-':
      return "-";
    case '*':
      return "*";
    case '/':
      return "/";
    case '^':
      return "^";
    case '!':
      return "!";
    case UMINUS:
      return "-";
    case '=':
      return "=";
    default:
      return "UNKNOWN-OP";
  }
}

/*
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

  case DECLARATION_NODE:
  
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
    printf("%s", n->variable.identifier);
    break;

  case ASSIGNMENT_NODE:
    printf("%c", n->assignment_stmt.op);
    break;

  default:
    break;
  }
}

// if no child at that index, return null
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

  case DECLARATION_NODE:
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

  case ARGUMENTS_NODE:
  {
    // TODO
    break;
  }

  default:
    break;
  }

  return NULL;
}
*/
