#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semantic.h"
#include "parser.tab.h"

bool type_check(Type *info1, Type *info2);
void semantic_check_recurse(node *n, SymbolTable *prev_sym);
void unary_check_type(Type *type_out, int line, int op, Type *type_in);
void binary_check_type(Type *type_out, int line, int op, Type *type1, Type *type2);
void function_check_args(int line, int func_id, Type *ret_type, Type *arg_type, int num_args);
void set_type(Type *src, Type *dst);
void insert_predefined_variables(SymbolTable *s);

int semantic_check( node *ast) {

    printf("Semantic check ...\n");

    semantic_check_recurse(ast, NULL); // failed checks
    return errorOccurred ? 0 : 1;
}

SymbolTable *root;
void semantic_check_recurse(node *n, SymbolTable *s){
    if(n == NULL){
      return;
    }
   
    Type *temp_type;

    switch(n->kind) {
    case SCOPE_NODE:
      n->scope.symbols = new SymbolTable(s);
      if (s == NULL) {
        root = n->scope.symbols;
        insert_predefined_variables(root);
      }
      semantic_check_recurse(n->scope.declarations, n->scope.symbols);
      semantic_check_recurse(n->scope.statements, n->scope.symbols);
      break;

    case DECLARATIONS_NODE:
    {
      semantic_check_recurse(n->declarations.declarations, s);
      semantic_check_recurse(n->declarations.declaration, s);
      if (n->declarations.declarations == NULL) {
        (n->declarations).num_decls = 0;
      } else {
        (n->declarations).num_decls = (n->declarations.declarations)->declarations.num_decls + 1;
      }
      break;
    }

    case STATEMENTS_NODE:
    {
      semantic_check_recurse(n->statements.statements, s);
      semantic_check_recurse(n->statements.statement, s);
      if (n->statements.statements == NULL) {
        (n->statements).num_stmts = 0;
      } else {
        (n->statements).num_stmts = (n->statements.statements)->statements.num_stmts + 1;
      }
      break;
    }

    case DECLARATION_NODE:
      /* Enter variable into symbol table */
      if((s->put(n->declaration.id, n->type_info)) == 1){
        fprintf(errorFile, "ERROR: line %d: redeclaration of variable %s\n", n->line, n->declaration.id);
        errorOccurred = true;
      }
      break;

    case DECLARATION_WITH_INIT_NODE:
    {
      semantic_check_recurse(n->declaration_init.expression, s);

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
      semantic_check_recurse(n->declaration_const.expression, s);

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
      semantic_check_recurse(n->assignment_stmt.left, s);
      semantic_check_recurse(n->assignment_stmt.right, s);

      // writeonly variables cannot be read
      if (n->assignment_stmt.right->type_info->writeonly) {
        fprintf(errorFile, "ERROR: line %d: writeonly variable cannot be read\n", n->line);
        errorOccurred = true;
      }

      // readonly variables cannot be assigned
      if (root->lookup(n->assignment_stmt.left->variable.identifier) != NULL &&
          root->lookup(n->assignment_stmt.left->variable.identifier)->type->readonly) {
        fprintf(errorFile, "ERROR: line %d: readonly variable, %s, cannot be assigned\n", n->line, n->assignment_stmt.left->variable.identifier);
        errorOccurred = true;
      }

      // readonly and const can only be assigned to const
      if (n->assignment_stmt.left->type_info->is_const &&
          !(n->assignment_stmt.right->type_info->is_const)) {
        fprintf(errorFile, "ERROR: line %d: non-const variable cannot be assigned to a const variable\n", n->line);
        errorOccurred = true;
      }

      // const variable reassignment not allowed
      if (n->assignment_stmt.left->type_info->is_const) {
        fprintf(errorFile, "ERROR: line %d: const variable, %s, cannot be reassigned\n", n->line, n->assignment_stmt.left->variable.identifier);
        errorOccurred = true;
      }

      // type mismatch in assignment for scalar
      if (n->assignment_stmt.left->type_info->basic_type != n->assignment_stmt.right->type_info->basic_type) {
        fprintf(errorFile, "ERROR: line %d: type mismatch in assignment of variable %s\n", n->line, n->assignment_stmt.left->variable.identifier);
        errorOccurred = true;
      } else {
        if (n->assignment_stmt.left->type_info->length != n->assignment_stmt.right->type_info->length && 
            n->assignment_stmt.left->variable.index == -1) {
          fprintf(errorFile, "ERROR: line %d: array dimension mismatch in assignment of variable %s\n", n->line, n->assignment_stmt.left->variable.identifier);
          errorOccurred = true;
        }
      }

      set_type(n->assignment_stmt.left->type_info, n->type_info);
      break;
    }
    case IF_WITH_ELSE_STATEMENT_NODE:
    {
      semantic_check_recurse(n->if_else_stmt.expression, s);
      semantic_check_recurse(n->if_else_stmt.then_stmt, s);
      semantic_check_recurse(n->if_else_stmt.else_stmt, s);
      if (n->if_else_stmt.expression->type_info->basic_type != Type::BOOLEAN) {
        char buf[20];
        get_type(n->if_else_stmt.expression->type_info, buf);
        fprintf(errorFile, "ERROR: line %d: expected boolean in IF expression, but got %s instead\n", n->if_else_stmt.expression->line, buf);
        errorOccurred = true;
      }
      break;
    }
    case IF_STATEMENT_NODE:
    {
      semantic_check_recurse(n->if_stmt.expression, s);
      semantic_check_recurse(n->if_stmt.then_stmt, s);
      if (n->if_stmt.expression->type_info->basic_type != Type::BOOLEAN) {
        char buf[20];
        get_type(n->if_stmt.expression->type_info, buf);
        fprintf(errorFile, "ERROR: line %d: expected boolean in IF expression, but got %s instead\n", n->if_stmt.expression->line, buf);
        errorOccurred = true;
      }
      break;
    }
    case CONSTRUCTOR_NODE:
    {
      semantic_check_recurse(n->constructor.arguments, s);
      if((n->constructor.arguments) == NULL){
        if(n->type_info->length != 0){
          fprintf(errorFile, "ERROR: line %d: 0 input arguments, expecting %d\n", n->line, n->type_info->length);
          errorOccurred = true;
        }
      } else {
        if(n->type_info->length != (n->constructor.arguments)->arguments.num_args){ /* Number of arguments check */
          fprintf(errorFile, "ERROR: line %d: %d input arguments, expecting %d\n", n->line, (n->constructor.arguments)->arguments.num_args, n->type_info->length);
          errorOccurred = true;
        } else if((n->constructor.arguments)->type_info->basic_type != n->type_info->basic_type  /* Arguments type check - ANY type is allowed*/
                  && ((n->constructor.arguments)->type_info->basic_type != Type::ANY)){
          fprintf(errorFile, "ERROR: line %d: incorrect argument type\n", n->line);
          errorOccurred = true;
        }
      }
      break;
    }
    case FUNCTION_NODE:
    {
      semantic_check_recurse(n->function.arguments, s);
      function_check_args(n->line, n->function.func_id, n->type_info, (n->function.arguments)->type_info, (n->function.arguments)->arguments.num_args);
      break;
    }
    case UNARY_EXPRESSION_NODE:
    {
      semantic_check_recurse(n->unary_expr.right, s);
      unary_check_type(n->type_info, n->line, n->unary_expr.op, (n->unary_expr.right)->type_info);
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
        errorOccurred = true;
        break;
      } 

      /* Getting type from declared variable */
      set_type(attr->type, n->type_info);
      /*char buf[10];
      get_type(n->type_info, buf);
      printf("HELLOO set var %s to type %s \n", n->variable.identifier, buf);*/

      /* Checking index is not out of bounds */
      if((n->type_info)->length <= n->variable.index){
        fprintf(errorFile, "ERROR: line %d: index %d is out of bounds for variable %s of length %d\n", n->line, n->variable.index, n->variable.identifier, (n->type_info)->length);
        errorOccurred = true;
      }
      break;
    }
    case ARGUMENTS_NODE:
    {
      semantic_check_recurse(n->arguments.arguments, s);
      semantic_check_recurse(n->arguments.argument, s);
      if((n->arguments.arguments) == NULL){ /* no arguments child */
         n->arguments.num_args = 1; /* Add one argument for the single argument */
         set_type((n->arguments.argument)->type_info, n->type_info); /* use the type of the single argument */

      } else {
         n->arguments.num_args = (n->arguments.arguments)->arguments.num_args + 1; /* Plus one for the single argument */
          if(type_check((n->arguments.arguments)->type_info, (n->arguments.argument)->type_info) /* Arguments are not the same type (excepting the ANY type) */
             && (n->arguments.arguments)->type_info->basic_type != Type::ANY){
            fprintf(errorFile, "ERROR: line %d: incorrect argument type\n", n->line);
            errorOccurred = true;
            n->type_info->basic_type = Type::ANY; /* Use any type from now on */
            n->type_info->length = -1;
          } else {
            set_type((n->arguments.arguments)->type_info, n->type_info); /* Pass up the arguments type*/
          }
      }
      break;
    }
    default:
      break;
    }
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
    dst->readonly = src->readonly;
    dst->writeonly = src->writeonly;
}

bool is_arithmetic(Type *type){
  return(type->basic_type == Type::INT || type->basic_type == Type::FLOAT);
}

bool is_scalar(Type *type){
  return(type->length == 1);
}

/* Checks build-in function arguments match function specifications, and sets return type
 * Assumes same type for all function arguments
 */
void function_check_args(int line, int func_id, Type *ret_type, Type *arg_type, int num_args){
  switch(func_id){
    case 0: //DP3
    if(num_args != 2){
      fprintf(errorFile, "ERROR: line %d, %d input arguments for %s function, expecting %d\n", line, num_args, get_function(func_id), 2);
      errorOccurred = true;
    } else if(arg_type->basic_type != Type::FLOAT && arg_type->basic_type != Type::INT && arg_type->basic_type != Type::ANY){
      fprintf(errorFile, "ERROR: line %d, incorrect argument type\n", line);
      errorOccurred = true;
    } else if(arg_type->length != 3 && arg_type->length != 4 && arg_type->basic_type != Type::ANY){
      fprintf(errorFile, "ERROR: line %d, incorrect argument type\n", line);
      errorOccurred = true;
    }
    ret_type->basic_type = arg_type->basic_type;
    ret_type->length = 1;
    break;
    case 1: //LIT
    if(num_args != 1){
      fprintf(errorFile, "ERROR: line %d, %d input arguments for %s function, expecting %d\n", line, num_args, get_function(func_id), 1);
      errorOccurred = true;
    } else if(arg_type->basic_type != Type::FLOAT && arg_type->basic_type != Type::ANY){
      fprintf(errorFile, "ERROR: line %d, incorrect argument type\n", line);
      errorOccurred = true;
    } else if(arg_type->length != 4 && arg_type->basic_type != Type::ANY){
      fprintf(errorFile, "ERROR: line %d, incorrect argument type\n", line);
      errorOccurred = true;
    } 
    ret_type->basic_type = Type::FLOAT;
    ret_type->length = 4;
    break;
    case 2: //RSQ
    if(num_args != 1){
      fprintf(errorFile, "ERROR: line %d, %d input arguments for %s function, expecting %d\n", line, num_args, get_function(func_id), 1);
      errorOccurred = true;
    } else if(arg_type->basic_type != Type::FLOAT && arg_type->basic_type != Type::INT && arg_type->basic_type != Type::ANY){
      fprintf(errorFile, "ERROR: line %d, incorrect argument type\n", line);
      errorOccurred = true;
    } else if(arg_type->length != 1 && arg_type->basic_type != Type::ANY){
      fprintf(errorFile, "ERROR: line %d, incorrect argument type\n", line);
      errorOccurred = true;
    }
    ret_type->basic_type = Type::FLOAT;
    ret_type->length = 1;
    break;
  }
}

/* Checks the input type of the unary operator and modifies the the output type accordingly 
 * Throws an error if the input type is incorrect
 */
void unary_check_type(Type *type_out, int line, int op, Type *type_in) {

  /* if the input argument has type ANY, output is also type ANY */
  if(type_in->basic_type == Type::ANY){
    return;
  }

  /* Argument type check */
  switch(op){
    case '!':
      if(type_in->basic_type != Type::BOOLEAN){
        fprintf(errorFile, "ERROR: line %d, argument 1 of ! operator does not have boolean type\n", line);
        errorOccurred = true;
        return;
      }
      break;
    case '-':
      if(!is_arithmetic(type_in)){
        fprintf(errorFile, "ERROR: line %d, argument 1 of %s operator does not have arithmetic type\n", line, get_operator(op));
        errorOccurred = true;
        return;
      }
      break;
    default:
      break;
  }

  /* Set results (output type)*/
  type_out->length = type_in->length;
  type_out->basic_type = type_in->basic_type;

}

/* Checks the input types of the binary operator and modifies the the output type accordingly 
 * Throws an error if the input types are incorrect
 */
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
        errorOccurred = true;
        return;
      } 
      if (type2->basic_type != Type::BOOLEAN){
        fprintf(errorFile, "ERROR: line %d, argument 2 of %s operator does not have boolean type\n", line, get_operator(op));
        errorOccurred = true;
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
        errorOccurred = true;
        return;
      } 
      if (!is_arithmetic(type2)){
        fprintf(errorFile, "ERROR: line %d, argument 2 of %s operator does not have arithmetic type\n", line, get_operator(op));
        errorOccurred = true;
        return;
      } 

      if (type1->basic_type != type2->basic_type){
        fprintf(errorFile, "ERROR: line %d, arguments of %s operator do not have same basic type\n", line, get_operator(op));
        errorOccurred = true;
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
        errorOccurred = true;
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
        errorOccurred = true;
        return;
      }
      break;
    case '>':
    case '<':
    case LEQ:
    case GEQ:
      if (!is_scalar(type1) || !is_scalar(type2)){
        fprintf(errorFile, "ERROR: line %d, arguments of %s operator must be scalar\n", line, get_operator(op));
        errorOccurred = true;
        return;
      }
      break;
    case '*': 
      if (!is_scalar(type1) && !is_scalar(type2) && (type1->length != type2->length)){
        fprintf(errorFile, "ERROR: line %d, lengths of %s operator vector arguments do not match\n", line, get_operator(op));
        errorOccurred = true;
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
      type_out->writeonly = type1->writeonly || type2->writeonly;
      type_out->readonly = type1->readonly || type2->readonly;
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
      type_out->writeonly = type1->writeonly || type2->writeonly;
      type_out->readonly = type1->readonly || type2->readonly;
      break;
    default:
      break;
  }
}

void insert_predefined_variables(SymbolTable *s) {
  // gl_FragColor - writeonly
  Type *gl_FragColor = (Type *) malloc(sizeof(Type));
  gl_FragColor->length = 4;
  gl_FragColor->basic_type = Type::FLOAT;
  gl_FragColor->is_const = false;
  gl_FragColor->readonly = false;
  gl_FragColor->writeonly = true;
  s->put("gl_FragColor", gl_FragColor);

  // gl_FragDepth - writeonly
  Type *gl_FragDepth = (Type *) malloc(sizeof(Type));
  gl_FragDepth->length = 1;
  gl_FragDepth->basic_type = Type::BOOLEAN;
  gl_FragDepth->is_const = false;
  gl_FragDepth->readonly = false;
  gl_FragDepth->writeonly = true;
  s->put("gl_FragDepth", gl_FragDepth);

  // gl_FragCoord - writeonly
  Type *gl_FragCoord = (Type *) malloc(sizeof(Type));
  gl_FragCoord->length = 4;
  gl_FragCoord->basic_type = Type::FLOAT;
  gl_FragCoord->is_const = false;
  gl_FragCoord->readonly = false;
  gl_FragCoord->writeonly = false;
  s->put("gl_FragCoord", gl_FragCoord);

  // gl_TexCoord - readonly
  Type *gl_TexCoord = (Type *) malloc(sizeof(Type));
  gl_TexCoord->length = 4;
  gl_TexCoord->basic_type = Type::FLOAT;
  gl_TexCoord->is_const = false;
  gl_TexCoord->readonly = true;
  gl_TexCoord->writeonly = false;
  s->put("gl_TexCoord", gl_TexCoord);

  // gl_Color - readonly
  Type *gl_Color = (Type *) malloc(sizeof(Type));
  gl_Color->length = 4;
  gl_Color->basic_type = Type::FLOAT;
  gl_Color->is_const = false;
  gl_Color->readonly = true;
  gl_Color->writeonly = false;
  s->put("gl_Color", gl_Color);

  // gl_Secondary - readonly
  Type *gl_Secondary = (Type *) malloc(sizeof(Type));
  gl_Secondary->length = 4;
  gl_Secondary->basic_type = Type::FLOAT;
  gl_Secondary->is_const = false;
  gl_Secondary->readonly = true;
  gl_Secondary->writeonly = false;
  s->put("gl_Secondary", gl_Secondary);

  // gl_FogFragCoord - readonly
  Type *gl_FogFragCoord = (Type *) malloc(sizeof(Type));
  gl_FogFragCoord->length = 4;
  gl_FogFragCoord->basic_type = Type::FLOAT;
  gl_FogFragCoord->is_const = false;
  gl_FogFragCoord->readonly = true;
  gl_FogFragCoord->writeonly = false;
  s->put("gl_FogFragCoord", gl_FogFragCoord);

  // gl_Light_Half - readonly const
  Type *gl_Light_Half = (Type *) malloc(sizeof(Type));
  gl_Light_Half->length = 4;
  gl_Light_Half->basic_type = Type::FLOAT;
  gl_Light_Half->is_const = true;
  gl_Light_Half->readonly = true;
  gl_Light_Half->writeonly = false;
  s->put("gl_Light_Half", gl_Light_Half);

  // gl_Light_Ambient - readonly const
  Type *gl_Light_Ambient = (Type *) malloc(sizeof(Type));
  gl_Light_Ambient->length = 4;
  gl_Light_Ambient->basic_type = Type::FLOAT;
  gl_Light_Ambient->is_const = true;
  gl_Light_Ambient->readonly = true;
  gl_Light_Ambient->writeonly = false;
  s->put("gl_Light_Ambient", gl_Light_Ambient);

  // gl_Material_Shininess - readonly const
  Type *gl_Material_Shininess = (Type *) malloc(sizeof(Type));
  gl_Material_Shininess->length = 4;
  gl_Material_Shininess->basic_type = Type::FLOAT;
  gl_Material_Shininess->is_const = true;
  gl_Material_Shininess->readonly = true;
  gl_Material_Shininess->writeonly = false;
  s->put("gl_Material_Shininess", gl_Material_Shininess);

  // env1 - readonly const
  Type *env1 = (Type *) malloc(sizeof(Type));
  env1->length = 4;
  env1->basic_type = Type::FLOAT;
  env1->is_const = true;
  env1->readonly = true;
  env1->writeonly = false;
  s->put("env1", env1);

  // env2 - readonly const
  Type *env2 = (Type *) malloc(sizeof(Type));
  env2->length = 4;
  env2->basic_type = Type::FLOAT;
  env2->is_const = true;
  env2->readonly = true;
  env2->writeonly = false;
  s->put("env2", env2);

  // env3 - readonly const
  Type *env3 = (Type *) malloc(sizeof(Type));
  env3->length = 4;
  env3->basic_type = Type::FLOAT;
  env3->is_const = true;
  env3->readonly = true;
  env3->writeonly = false;
  s->put("env3", env3);

}
