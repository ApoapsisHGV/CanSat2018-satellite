#ifndef config_h
#define config_h

#define VERSION "0.9.1"

#define DEBUG 0

// Piezo config
// Piezo Pin
#define PIEZO 8

// Feinstaubsensor config
#define COV_RATIO 0.2         // Umrechnungsrate von mV zu /ug/mmm für Feinstaubsensor
#define NO_DUST_VOLTAGE 400   // Fehlertoleranz in mV für Feinstaubsensor
#define SYS_VOLTAGE 5000      // Spannung am Feinstaubsensor
#define ILED 5                // ILED Pin für den Feinstaubsensor
#define VOUT A0               // Der analoge Messpin für den Feinstaubsensor

// Radio config
#define AES_KEY 0x41, 0x70, 0x30, 0x61, 0x70, 0x35, 0x31, 0x35, 0x48, 0x47, 0x56, 0x2d, 0x32, 0x30, 0x31, 0x38
#define RADIO_CS 10
#define RADIO_INT 2
#define RADIO_RST 9

// SD ping
#define DELETE_OLD 0
#define SD_PIN 10

#endif
