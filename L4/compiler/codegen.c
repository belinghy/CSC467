#include "codegen.h"
#define dumpInstr(x)    { if (dumpInstructions) fprintf(dumpFile, "%s\n", x); }

// Declarations
void getVariableName(node *n, char *id_name);
void genCodeRecurse(node *n);

// Main function
void genCode(node *ast){
  genCodeRecurse(ast);
}

void genCodeRecurse(node *n) {
  if (n == NULL) return;

  // TODO copied for semantic.c, change to print stuff
  // No need to look at symbol table?
  switch(n->kind) {
    case SCOPE_NODE:
    {
      if (n->scope.symbols->prev_scope == NULL) {
        // is in program scope
        dumpInstr("!!ARBfp1.0\n")
        char buf[256];
        sprintf(buf, "TEMP %s;", "TEMP_VARI_UNIQUE"); // used as a general variable to pass values
        dumpInstr("TEMP BIN_EXPR_TEMP_VAR_LEFT;") // used to store LHS value of binary expressions
        dumpInstr("TEMP BIN_EXPR_TEMP_VAR_RIGHT;") // used to store RHS value

        dumpInstr(buf)
      }
      
      genCodeRecurse(n->scope.declarations);
      genCodeRecurse(n->scope.statements);

      if (n->scope.symbols->prev_scope == NULL) {
        dumpInstr("\nEND")
      }
      break;
    }

    case DECLARATIONS_NODE:
    {
      genCodeRecurse(n->declarations.declarations);
      genCodeRecurse(n->declarations.declaration);
      break;
    }

    case STATEMENTS_NODE:
    {
      genCodeRecurse(n->statements.statements);
      genCodeRecurse(n->statements.statement);
      break;
    }

    case DECLARATION_NODE: 
    {
      char buf[256];
      char id_name[70];
      getVariableName(n, id_name);
      sprintf(buf, "TEMP %s;", id_name);
      dumpInstr(buf)
      break;
    }

    case DECLARATION_WITH_INIT_NODE:
    {
      char buf[256];
      char id_name[70];
      getVariableName(n, id_name);
      sprintf(buf, "TEMP %s;", id_name);
      dumpInstr(buf)

      genCodeRecurse(n->declaration_init.expression);
      break;
    }

    case DECLARATION_CONST_NODE:
    {
      genCodeRecurse(n->declaration_const.expression);
      break;
    }
    case ASSIGNMENT_NODE:
    {
      // LHS is always variable
      // genCodeRecurse(n->assignment_stmt.left); // handle this below
      // generate RHS expression first
      genCodeRecurse(n->assignment_stmt.right); 

      char buf[256];
      char id_name[70];
      getVariableName(n->assignment_stmt.left, id_name);
      sprintf(buf, "MOV %s, TEMP_VARI_UNIQUE;", id_name);
      dumpInstr(buf)
      break;
    }
    case IF_WITH_ELSE_STATEMENT_NODE:
    {
      genCodeRecurse(n->if_else_stmt.expression);
      genCodeRecurse(n->if_else_stmt.then_stmt);
      genCodeRecurse(n->if_else_stmt.else_stmt);
      break;
    }
    case IF_STATEMENT_NODE:
    {
      genCodeRecurse(n->if_stmt.expression);
      genCodeRecurse(n->if_stmt.then_stmt);
      break;
    }
    case CONSTRUCTOR_NODE:
    {
      genCodeRecurse(n->constructor.arguments);
      break;
    }
    case FUNCTION_NODE:
    {
      genCodeRecurse(n->function.arguments);
      break;
    }
    case UNARY_EXPRESSION_NODE:
    {
      genCodeRecurse(n->unary_expr.right);

      // Result is already stored in TEMP_VARI_UNIQUE, only need to do negation or unary minus
      if (n->unary_expr.op == '!') {
        dumpInstr("NOT TEMP_VARI_UNIQUE, TEMP_VARI_UNIQUE, -1.0;")

      } else if (n->unary_expr.op == '-') {
        dumpInstr("MUL TEMP_VARI_UNIQUE, TEMP_VARI_UNIQUE, -1.0;")

      } else {
        // FIXME: We can't get in here
        dumpInstr("WHY ARE YOU HERE?!;")

      }
      break;
    }
    case BINARY_EXPRESSION_NODE:
    {
      dumpInstr("\n# Binary Expression")
      genCodeRecurse(n->binary_expr.left);
      dumpInstr("MOV BIN_EXPR_TEMP_VAR_LEFT, TEMP_VARI_UNIQUE;")

      genCodeRecurse(n->binary_expr.right);
      dumpInstr("MOV BIN_EXPR_TEMP_VAR_RIGHT, TEMP_VARI_UNIQUE;")

      // FIXME: add other operators
      if (n->binary_expr.op == '+') {
        dumpInstr("ADD TEMP_VARI_UNIQUE, BIN_EXPR_TEMP_VAR_RIGHT, BIN_EXPR_TEMP_VAR_LEFT;")

      } else if (n->binary_expr.op == '-') {
        dumpInstr("SUB TEMP_VARI_UNIQUE, BIN_EXPR_TEMP_VAR_RIGHT, BIN_EXPR_TEMP_VAR_LEFT;")

      } else if (n->binary_expr.op == '*') {
        dumpInstr("MUL TEMP_VARI_UNIQUE, BIN_EXPR_TEMP_VAR_RIGHT, BIN_EXPR_TEMP_VAR_LEFT;")

      } else {
        dumpInstr("FIXME: add other operators")

      }

      dumpInstr("# End Binary Expression\n")
      break;
    }
    case BOOL_NODE:
    {
      // can still print bool
      // although if (true) is handled by IF-STMT
      break;
    }
    case INT_NODE:
    {
      char buf[256];
      sprintf(buf, "MOV TEMP_VARI_UNIQUE, %f;", (float)n->int_literal.value);
      dumpInstr(buf)
      break;
    }
    case FLOAT_NODE:
    {
      char buf[256];
      sprintf(buf, "MOV TEMP_VARI_UNIQUE, %f;", n->float_literal.value);
      dumpInstr(buf)
      break;
    }

    case VAR_NODE:
    {
      char buf[256];
      char id_name[70];
      getVariableName(n, id_name);
      sprintf(buf, "MOV TEMP_VARI_UNIQUE, %s;", id_name);
      dumpInstr(buf)
      break;
    }
    case ARGUMENTS_NODE:
    {
      genCodeRecurse(n->arguments.arguments);
      genCodeRecurse(n->arguments.argument);
      break;
    }
    default:
      break;
    }
}

void getVariableName(node *n, char *id_name) {
  char *id;
  switch (n->kind) {
    case VAR_NODE:
      id = n->variable.identifier;
      break;
    case DECLARATION_NODE:
      id = n->declaration.id;
      break;
    case DECLARATION_WITH_INIT_NODE:
      id = n->declaration_init.id;
      break;
    case DECLARATION_CONST_NODE:
      id = n->declaration_const.id;
      break;
    default:
      dumpInstr("FIXME: unknown node kind in getVariableName()")
      break;
  }

  if (strcmp(id, "gl_FragColor") == 0) {
    id = "result.color";

  } else if (strcmp(id, "gl_FragDepth") == 0) {
    id = "result.depth";
  
  } else if (strcmp(id, "gl_FragCoord") == 0) {
    id = "fragment.position";
  
  } else if (strcmp(id, "gl_TexCoord") == 0) {
    id = "fragment.texcoord";
  
  } else if (strcmp(id, "gl_Color") == 0) {
    id = "fragment.color";
  
  } else if (strcmp(id, "gl_Secondary") == 0) {
    id = "fragment.color.secondary";
  
  } else if (strcmp(id, "gl_FogFragCoord") == 0) {
    id = "fragment.fogcoord";
  
  } else if (strcmp(id, "gl_Light_Half") == 0) {
    id = "state.light[0].half";
  
  } else if (strcmp(id, "gl_Light_Ambient") == 0) {
    id = "state.lightmodel.ambient";
  
  } else if (strcmp(id, "gl_Material_Shininess") == 0) {
    id = "state.material.shininess";
  
  } else if (strcmp(id, "env1") == 0) {
    id = "program.env[1]";
  
  } else if (strcmp(id, "env2") == 0) {
    id = "program.env[2]";
  
  } else if (strcmp(id, "env3") == 0) {
    id = "program.env[3]";
  
  }

  if (n->kind == VAR_NODE) {
    if (n->variable.index == -1) {
      sprintf(id_name, "%s", id);

    } else if (n->variable.index == 0) {
      sprintf(id_name, "%s.x", id);

    } else if (n->variable.index == 1) {
      sprintf(id_name, "%s.y", id);

    } else if (n->variable.index == 2) {
      sprintf(id_name, "%s.z", id);

    } else if (n->variable.index == 3) {
      sprintf(id_name, "%s.w", id);

    } else {
      dumpInstr("FIXME: array out of bound")
    }
    return;
  } else if (n->kind == DECLARATION_NODE || n->kind == DECLARATION_WITH_INIT_NODE || n->kind == DECLARATION_CONST_NODE ) {
    sprintf(id_name, "%s", id);
    return;
  } else {
    return;
  }
}