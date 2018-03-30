#ifndef dust_h
#define dust_h
#include "Arduino.h"

class Dust{
  public:
    Dust(int iled, int vout);
    double getDensity();
    
  private:
    int _iled;
    int _vout;
};

#endif
