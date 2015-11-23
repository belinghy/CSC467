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
  bool exists(std::string name){
    if(htmap.find(name) == htmap.end()) {
        return false;
    }
    else {
        return true;
    }
  }

public:
  std::unordered_map<std::string, SymAttr> htmap;
  SymbolTable *prev_scope; /* enclosing scope */

  /* 0 - successful put, 1 - redeclaration error */
  int put(char *name, Type *type) {
    
    std::string name_s = std::string(name);

    /* check for redeclaration in current scope */
    if(exists(name_s)){
      return 1;
    } else {
      htmap[name_s] = SymAttr();
      htmap[name_s].name = name;
      htmap[name_s].type = type;
      return 0;
    }
  }

  SymAttr *lookup(char *name) {

    std::string name_s = std::string(name);

    if(exists(name_s)){
      return &htmap[name_s];    
    } 

    /* Lookup in enclosing scopes */
    SymbolTable *curr_scope = prev_scope;
    while((curr_scope != NULL)){
      if(curr_scope->exists(name_s)) return &(curr_scope->htmap[name_s]);
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
