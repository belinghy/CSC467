#ifndef _SYMBOL_H
#define _SYMBOL_H
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <unordered_map>

#include "common.h"

//TODO: move functions to symbol.c
class SymAttr{
public:
  char *name;
  Type *type;
};

class SymbolTable{
private:
  std::unordered_map<char *, SymAttr> htmap;
  SymbolTable *prev_scope; /* enclosing scope */
  
  bool exists(char *name){
    if(htmap.find(name) == htmap.end()) {
        return false;
    }
    else {
        return true;
    }
  }

public:

  /* 0 - successful put, 1 - redeclaration error */
  int put(char *name, Type *type) {
    /* check for redeclaration in current scope */
    if(exists(name)){
      return 1;
    } else {
      htmap[name] = SymAttr();
      htmap[name].name = name;
      htmap[name].type = type;
      return 0;
    }
  }

  SymAttr *lookup(char *name) {

    if(exists(name)){
      return &htmap[name];    
    } 

    /* Lookup in enclosing scopes */
    SymbolTable *curr_scope = prev_scope;
    while((curr_scope != NULL)){
      if(curr_scope->exists(name)) return &(curr_scope->htmap[name]);
      curr_scope = curr_scope->prev_scope;      
    }
    return NULL;
  }

  SymbolTable(){
    prev_scope = NULL;
  }

  SymbolTable(SymbolTable *prev_scope_){
    prev_scope = prev_scope_;
  }
};
#endif
