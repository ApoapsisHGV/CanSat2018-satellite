#include "Arduino.h"
#include "dust.h"

#define COV_RATIO 0.2
#define NO_DUST_VOLTAGE 400
#define SYS_VOLTAGE 5000

Dust::Dust(int iled, int vout){
  _iled = iled;
  _vout = vout;
  pinMode(_iled, OUTPUT);
  digitalWrite(_iled, LOW);
}

double Dust::getDensity(){
  digitalWrite(_iled, HIGH);
  delayMicroseconds(280);
  int adcvalue = analogRead(_vout);
  digitalWrite(_iled, LOW);
  
  double voltage = (SYS_VOLTAGE / 1023.0) * adcvalue * 11;
  
  int density;
  if(voltage >= NO_DUST_VOLTAGE){    
    voltage -= NO_DUST_VOLTAGE;
    density = voltage * COV_RATIO;   
  }else{
    density = 0;
  }

  return density;
}

