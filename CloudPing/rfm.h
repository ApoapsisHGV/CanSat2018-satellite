#ifndef rfm_h
#define rfm_h
#include "Arduino.h"
#include <RH_RF69.h>

class Radio {
  public:
    Radio(uint8_t *key, int CS, int INT, int RST);
    int init();
    void sendData(char *payload);
    RH_RF69 _radio;

  private:
    int _RST;
    uint8_t *_key;
};

#endif
