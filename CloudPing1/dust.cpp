#include "Arduino.h"
#include "dust.h"

#define COV_RATIO 0.2           //ug/m^3 per mV
#define NO_DUST_VOLTAGE 400     //mV
#define SYS_VOLTAGE 5000        //5V for Arduino nano

//Declaring Pins
Dust::Dust(int iled, int vout){
  _iled = iled;
  _vout = vout;
}

void Dust::init(){
  pinMode(_iled, OUTPUT);
  digitalWrite(_iled, LOW);
}

//Filter Funcion. This was taken from the Waveshare Wiki. More Info here: https://www.waveshare.com/wiki/Dust_Sensor
int Dust::filter(int m) {
  static int flag_first = 0, _buff[10], sum;
  const int _buff_max = 10;
  int i;
  
  if(flag_first == 0) {
    flag_first = 1;

    for(i = 0, sum = 0; i < _buff_max; i++) {
      _buff[i] = m;
      sum += _buff[i];
    }
    return m;
  }else{
    sum -= _buff[0];
    for(i = 0; i < (_buff_max - 1); i++) {
      _buff[i] = _buff[i + 1];
    }
    _buff[9] = m;
    sum += _buff[9];
    
    i = sum / 10.0;
    return i;
  }
}


double Dust::getDensity(){
  
  //Getting a measurement
  digitalWrite(_iled, HIGH);
  delayMicroseconds(280);
  int adcvalue = analogRead(_vout);
  digitalWrite(_iled, LOW);
  
  adcvalue = filter(adcvalue);
  
  double voltage = (SYS_VOLTAGE / 1023.0) * adcvalue * 11; //Calculating the voltage at the analog pin
  
  //Filtering for low noise
  int density;
  if(voltage >= NO_DUST_VOLTAGE){    
    voltage -= NO_DUST_VOLTAGE;
    density = voltage * COV_RATIO;   
  }else{
    density = 0;
  }

  return density;
}

