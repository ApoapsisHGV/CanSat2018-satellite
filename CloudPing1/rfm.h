#ifndef rfm_h
#define rfm_h
#include "Arduino.h"
#include <RFM69.h>

class Radio{
  public:
    Radio(uint8_t key, int NETWORKID, int NODEID, int TONODEID);
    void sendData(char *payload);
    
  private:
    RFM69 _radio;
    int _targetid;
    
};

#endif
