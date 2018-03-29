#include "Arduino.h"
#include "BMP.h"
#include <SFE_BMP180.h>
#include <Wire.h> 


BMP180::BMP180(int height){
    _height = height;
}

int BMP180::init(){
  return _sensor.begin();  
}

void BMP180::getTemperature(double &T){
  delay(_sensor.startTemperature());
  _sensor.getTemperature(T);
}

void BMP180::getPressure(double &P, double &T){
  delay(_sensor.startPressure(3));
  _sensor.getPressure(P, T);
}

double BMP180::getHeight(double &P){
  return _sensor.altitude(P,_sensor.sealevel(_height, P));
}
