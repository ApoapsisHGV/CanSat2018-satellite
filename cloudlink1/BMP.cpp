#include "Arduino.h"
#include "BMP.h"
#include <SFE_BMP180.h>
#include <Wire.h> 


BMP180::BMP180(int height){
    _height = height;
}

int BMP180::begin(){
  return _sensor.begin();  
}

void BMP180::calibrate(){
  double T;
  getTemperature(T);
  getPressure(_baselinePressure, T);
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
  return(44330.0*(1-pow(P/_baselinePressure,1/5.255)));
}
