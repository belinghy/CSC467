#include "codegen.h"
#define dumpInstr(x)    { if (dumpInstructions) fprintf(dumpFile, "%s\n", x); }

// Declarations
char* getVariableName(char *s);
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
      genCodeRecurse(n->scope.declarations);
      genCodeRecurse(n->scope.statements);
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
      sprintf(buf, "TEMP %s;\n", getVariableName(n->declaration.id));
      dumpInstr(buf)
      break;
    }

    case DECLARATION_WITH_INIT_NODE:
    {
      char buf[256];
      sprintf(buf, "TEMP %s;\n", getVariableName(n->declaration.id));
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
      genCodeRecurse(n->assignment_stmt.left);
      genCodeRecurse(n->assignment_stmt.right);
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
      break;
    }
    case BINARY_EXPRESSION_NODE:
      genCodeRecurse(n->binary_expr.left);
      genCodeRecurse(n->binary_expr.right);
      break;

    //BOOL_NODE,
    //INT_NODE,
    //FLOAT_NODE,

    case VAR_NODE:
    {
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

char* getVariableName(char *s) {
  if (strcmp(s, "gl_FragColor") == 0) {
    return "result.color";

  } else if (strcmp(s, "gl_FragDepth") == 0) {
    return "result.depth";
  
  } else if (strcmp(s, "gl_FragCoord") == 0) {
    return "fragment.position";
  
  } else if (strcmp(s, "gl_TexCoord") == 0) {
    return "fragment.texcoord";
  
  } else if (strcmp(s, "gl_Color") == 0) {
    return "fragment.color";
  
  } else if (strcmp(s, "gl_Secondary") == 0) {
    return "fragment.color.secondary";
  
  } else if (strcmp(s, "gl_FogFragCoord") == 0) {
    return "fragment.fogcoord";
  
  } else if (strcmp(s, "gl_Light_Half") == 0) {
    return "state.light[0].half";
  
  } else if (strcmp(s, "gl_Light_Ambient") == 0) {
    return "state.lightmodel.ambient";
  
  } else if (strcmp(s, "gl_Material_Shininess") == 0) {
    return "state.material.shininess";
  
  } else if (strcmp(s, "env1") == 0) {
    return "program.env[1]";
  
  } else if (strcmp(s, "env2") == 0) {
    return "program.env[2]";
  
  } else if (strcmp(s, "env3") == 0) {
    return "program.env[3]";
  
  } else {
    return s;
  }
}