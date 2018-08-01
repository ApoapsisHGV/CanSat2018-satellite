#include "Arduino.h"
#include "rfm.h"
#include <RH_RF69.h>
#include <SPI.h>

Radio::Radio(uint8_t key, int CS, int INT, int RST){
  RH_RF69 _radio(CS, INT);
  _RST = RST;
  _key = key;
}

int Radio::init(){
  pinMode(_RST, OUTPUT);
  digitalWrite(_RST, LOW);
  digitalWrite(_RST, HIGH);
  delay(10);
  digitalWrite(_RST, LOW);
  delay(10);
  
  if(!_radio.init()){
    return false; 
   }
  _radio.setFrequency(433.0);
  _radio.setTxPower(20);
  _radio.setEncryptionKey(_key);
  return true;
}


void Radio::sendData(char *payload){
  int sum = 0;
  for(int c = 0; c < sizeof(payload); c++){
    sum += payload[c];
  }
  sprintf(payload, "%d,CHKS:%d", payload, sum);
  
  _radio.send((uint8_t *)payload, sizeof(payload));
}

