#ifndef ds_h
#define ds_h

#include "Arduino.h"
#include <OneWire.h>
#include <DallasTemperature.h>

class DsSensor{
  public:
    void begin();
    float getTemperature();
  private:
    DallasTemperature _sensor;
};
#endif
