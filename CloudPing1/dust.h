#ifndef dust_h
#define dust_h
#include "Arduino.h"

class Dust{
  public:
    Dust(int iled, int vout);
    double getDensity();
    void init();
    int filter(int m);
    
  private:
    int _iled;
    int _vout;
};

#endif
