# Satellite
Der Code für den Satelliten

## Requirements:
* https://github.com/adafruit/Adafruit_GPS/archive/master.zip
* https://github.com/LowPowerLab/SFE_BMP180/archive/master.zip
* https://github.com/PaulStoffregen/RadioHead/archive/master.zip


## Concept:
* Cloudlink1: Sensor part, collects all the data and sends them to Cloudlink2 and Groundstation.
* Cloudlink2: Logs data to SD, due to SD logging consuming about 35% RAM of an Arduino Nano
