#include "Arduino.h"
#include "rfm.h"
#include <RFM69.h>
#include <SPI.h>

Radio::Radio(uint8_t key, int NETWORKID, int NODEID, int TONODEID){
  _targetid = TONODEID;
  _radio.initialize(RF69_433MHZ, NODEID, NETWORKID);
  _radio.setHighPower();
  _radio.encrypt(key);
}




void Radio::sendData(char *payload){
  int sum = 0;
  for(int c = 0; c < sizeof(payload); c++){
    sum += payload[c];
  }
  sprintf(payload, "%d,CHKS:%d", payload, sum);
  _radio.send(_targetid,  payload, sizeof(payload));
}

