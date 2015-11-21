#ifndef _TYPE_H
#define _TYPE_H

struct Type{
   enum BasicType{
   INT,
   FLOAT,
   BOOLEAN,
   Any
   };
   
   BasicType basic_type;
 
   int length;
};
#endif

