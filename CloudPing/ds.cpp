#include "Arduino.h"
#include "ds.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

void DsSensor::begin(){
  OneWire _wire(DS_WIRE_BUS);
  _sensor = DallasTemperature(&_wire);
  _sensor.begin();
}

float DsSensor::getTemperature(){
  _sensor.requestTemperatures();
  return _sensor.getTempCByIndex(0);
}

