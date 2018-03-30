#include "Arduino.h"
#include "rfm.h"
#include <RH_RF69.h>
#include <SPI.h>

Radio::Radio(int CS, int INT){
  RH_RF69 _radio(CS, INT);  
}

int Radio::begin(double FREQ, int RST, uint8_t key){
  pinMode(RST, OUTPUT);
  digitalWrite(RST, LOW);
  digitalWrite(RST, HIGH);
  delay(10);
  digitalWrite(RST, LOW);
  delay(10);
  int ret = _radio.init();
  if(!ret){
    return ret;
  }
  ret = _radio.setFrequency(FREQ);
  if(!ret){
    return ret;
  }
  _radio.setTxPower(20);
  _radio.setEncryptionKey(key);
  return 0;
}

void Radio::sendData(char *payload){
  int sum = 0;
  for(int c = 0; c < sizeof(payload); c++){
    sum += payload[c];
  }
  sprintf(payload, "%d,CHKS:%d", payload, sum);
  _radio.send((uint8_t *)payload, strlen(payload));
  _radio.waitPacketSent();//udp style, dont wait for response
}

