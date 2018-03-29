#ifndef bmp_h
#define bmp_h

#include "Arduino.h"
#include <SFE_BMP180.h>

class BMP180{
    public:
        BMP180(int height);
        void getTemperature(double &T);
        void getPressure(double &P, double &T);
        double getHeight(double &P);
        int begin();
        void calibrate();
    private:
        int _height;
        double _baselinePressure;
        SFE_BMP180 _sensor;
};

#endif
