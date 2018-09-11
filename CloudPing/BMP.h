#ifndef bmp_h
#define bmp_h

#include "Arduino.h"
#include <SFE_BMP180.h>

class BMP180 {
  public:
    void getTemperature(double &T);
    void getPressure(double &P, double &T);
    int begin();
    double getHeight(double &P);
    void calibrate();
  private:
    double _baselinePressure;
    SFE_BMP180 _sensor;
};

#endif
