#ifndef rfm_h
#define rfm_h
#include "Arduino.h"
#include <RH_RF69.h>

class Radio{
  public:
    Radio(uint8_t key, int CS, int INT, int RST);
    void sendData(char *payload);
    
  private:
    RH_RF69 _radio;    
};

#endif
