/*
 * This file includes the defination of point class
 */

#define point 1

#ifndef constant
#include "Constant.h"
#endif

class Point{
  public:
  float p_value;
  int p_timestamp;

  bool isNULL(void){
    if(p_value == -1 && p_timestamp == -1)
      return true;
    else
      return false;
  }

  Point(void){
    
  }
  
  Point(float val,int t){
    p_value = val;
    p_timestamp = t;
  }
};

Point NULLPoint(-1,-1);
