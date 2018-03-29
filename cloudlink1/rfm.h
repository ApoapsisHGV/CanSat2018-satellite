#ifndef rfm_h
#define rfm_h
#include "Arduino.h"
#include <RH_RF69.h>

class Radio{
  public:
    Radio(int CS, int INT);
    int begin(double FREQ, int RST, uint8_t key);
    void sendData(char *payload);
    
  private:
    RH_RF69 _radio;
    
};

#endif
