#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <unordered_map>

#include "symbol.h"
#include "ast.h"

class SymAttr{
public:
  char *name;
  Type type;
};

class SymbolTable{
private:
    std::unordered_map<char *, SymAttr> htmap;

public:
    void put(char *name, SymAttr attr) {
            htmap[name] = attr;
    }

    SymAttr get(char *name) {
            return htmap[name];
    }
};
