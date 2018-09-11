#include "Arduino.h"
#include "rfm.h"
#include <RH_RF69.h>
#include <SPI.h>

Radio::Radio(uint8_t* key, int CS, int INT, int RST) {
  RH_RF69 _radio(CS, INT);
  _RST = RST;
  _key = key;
}

int Radio::init() {
  pinMode(_RST, OUTPUT);
  digitalWrite(_RST, LOW);
  digitalWrite(_RST, HIGH);
  delay(10);
  digitalWrite(_RST, LOW);
  delay(10);

  if (!_radio.init()) {
    return false;
  }
  _radio.setFrequency(433.0);
  _radio.setTxPower(20);
  _radio.setEncryptionKey(_key);
  _radio.setModemConfig(RH_RF69::GFSK_Rb250Fd250);
  return true;
}


void Radio::sendData(char *payload) {
  if (sizeof(payload) > 60) {
    int counter = 0;
    while ((counter + 1) * 60 < sizeof(payload)) {
      String strbuf;
      if (counter + 60 < sizeof(payload)) {
        for (int i = 0; i < 60; i++) {
          strbuf += payload[i + counter * 60];
        }
        counter++;
        char charBuff[strbuf.length()];
        strbuf.toCharArray(charBuff, strbuf.length());
        _radio.send((uint8_t*)charBuff, sizeof(charBuff));
        _radio.waitPacketSent();
      }
    }
    int left = sizeof(payload) - counter * 60;
    if (left != 0) {
      String strbuf;
      int c = 0;
      while (c != left) {
        strbuf += payload[counter * 60 + c];
        c++;
      }
      char charBuff[strbuf.length()];
      strbuf.toCharArray(charBuff, strbuf.length());
      _radio.send((uint8_t*)charBuff, sizeof(charBuff));
      _radio.waitPacketSent();
    }
  }
  else {
    _radio.send((uint8_t *)payload, sizeof(payload));
    _radio.waitPacketSent();
  }
}

