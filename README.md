# Satellite
Der Code für den Satelliten

## Requirements:
* https://github.com/adafruit/Adafruit_GPS/archive/master.zip
* https://github.com/LowPowerLab/SFE_BMP180/archive/master.zip
* https://github.com/PaulStoffregen/RadioHead/archive/master.zip
* https://github.com/PaulStoffregen/OneWire/archive/master.zip
* https://github.com/milesburton/Arduino-Temperature-Control-Library/archive/master.zip

## Beep codes
* First short beep shows that the piezo is working
* one long beep means SD init failed
* two long beeps mean BMP init failed
* three long beeps mean RFM69 init failed
* After all beeps another short beep shows that the setup function is done -> best case would be two short beeps
