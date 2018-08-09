# Satellite
Der Code für den Satelliten

## Requirements:
* https://github.com/adafruit/Adafruit_GPS/archive/master.zip
* https://github.com/LowPowerLab/SFE_BMP180/archive/master.zip
* https://github.com/PaulStoffregen/RadioHead/archive/master.zip
* https://github.com/PaulStoffregen/OneWire/archive/master.zip
* https://github.com/milesburton/Arduino-Temperature-Control-Library/archive/master.zip







#ifndef bmp_h
#define bmp_h

#include "Arduino.h"
#include <SFE_BMP180.h>

class BMP180{
    public:
        void getTemperature(double &T);
        void getPressure(double &P, double &T);
        int begin();
        double getHeight(double &P);
        void calibrate();
    private:
        double _baselinePressure;
        SFE_BMP180 _sensor;
};

#endif