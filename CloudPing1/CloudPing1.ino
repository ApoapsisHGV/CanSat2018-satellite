#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GPS.h>
#include "BMP.h"
#include "rfm.h"
#include "dust.h"
#include "gps.h"
#include "config.h"

#if DEBUG
#define VPRINT(data) Serial.print(data);
#define VPRINTLN(data) Serial.println(data);
#else
#define VPRINT(data)
#define VPRINTLN(data)
#endif


//Sensor initialisieren
//Druck & Temperatur
double height;
BMP180 bmp(BMP_GROUND);

//Feinstaubsensor - Code von waveshare.com
float density, voltage;
int   adcvalue;

//GPS Modul
SoftwareSerial gpsSerial(4, 3);
Adafruit_GPS GPS(&gpsSerial);
boolean usingInterrupt = true;
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
}


//radio
uint8_t key[] = { AES_KEY };
Radio rfm69(key, RADIO_CS, RADIO_INT, RADIO_RST);

void setup() {
  Serial.begin(115200);
  VPRINT("APOAPSIS CANSAT version: ");
  VPRINTLN(VERSION);
  Serial.println("Init of sensors starting...");
  
  //BMP180 init
  VPRINT("Init: BMP180 ");
  if (!bmp.begin()) {
    VPRINTLN("[FAILED]");
    while (1);
  }
  bmp.calibrate();
  VPRINTLN("[OK]");

  //Feinstaubsensor init
  VPRINT("Init: Feinstaubsensor ");
  pinMode(VOUT, INPUT);
  pinMode(ILED, OUTPUT);    //LED als Output
  digitalWrite(ILED, LOW);  //LED standardmäßig auf 0
  VPRINTLN("[OK]");

  //Piezo init
  VPRINTLN("Init: Piezo ");
  pinMode(PIEZO, OUTPUT);
  VPRINTLN("[OK]");

  //GPS init
  VPRINTLN("Init: GPS ");
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  usingInterrupt = useInterrupt(true);
  VPRINTLN("[OK]");
  
  
  // radio init
  VPRINT("Init: Radio ");
  if(!rfm69.init()){
    VPRINTLN("[FAILED]");
    while(1);
  }
  VPRINTLN("[OK]");
  
  Serial.println("Sensor init done");
  Serial.print("Starting loop...");
}

void loop() {
  //Variablen deklarieren
  Serial.println("[OK]");
  double temperature, pressure;
  float lon, lat, velocity, newHeight ;
  int maxHeight;
  String timestamp;
  
  //BMP180
  bmp.getTemperature(temperature);
  bmp.getPressure(pressure, temperature);
  height = bmp.getHeight(pressure);
  
  //Piezo Check 1: Finde die höchste Höhe des Fluges
  if (height > maxHeight) {
    maxHeight = height;
  }
  //Piezo Check 2: Wenn 300 m unter der höchsten Höhe, aktiviere Piezo Buzzer
  if (height < maxHeight - 300){
    digitalWrite(PIEZO, HIGH);
  }
  
  //Feinstaubsensor
  digitalWrite(ILED, HIGH);
  delayMicroseconds(280);
  adcvalue = analogRead(VOUT);
  digitalWrite(ILED, LOW);
  adcvalue = filter(adcvalue);
  voltage = (SYS_VOLTAGE / 1023.0) * adcvalue * 11;  //Spannung wird errechnet

  //Vorfiltern - Wenn die gemessene Spannung zu niedrig ist, wird davon ausgegangen, dass kein Feinstaub vorhanden ist
  if(voltage >= NO_DUST_VOLTAGE){    
    voltage -= NO_DUST_VOLTAGE;
    density = voltage * COV_RATIO;   
  }else{
    //VPRINT("Voltage too low: ");
    //VPRINTLN(voltage);
    density = 0;
  }

  //GPS Modul
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))
      return;
  }
  
  //Zeitstempel erstellen
  lat = GPS.latitudeDegrees;
  lon = GPS.longitudeDegrees;
  velocity = GPS.speed * 0.514444; // knots to m/s
  timestamp = (String)GPS.hour+";"+(String)GPS.minute+";"+(String)GPS.seconds+";"+(String)GPS.milliseconds;

  String pload = "T:"+(String)temperature+",P:"+(String)pressure+",D:"+(String)density+",Vo:"+(String)voltage+",LA:"+(String)lat+",LO:"+(String)lon+",V:"+(String)velocity+",TI:"+ timestamp +",H:"+(String)height;
  //Datenpaket wird erstellt
  char payload[pload.length()];
  pload.toCharArray(payload, pload.length());
  
  //send to Partner
  Serial.println(payload);

  //send with radio
  rfm69.sendData(payload);

  delay(1000);
}
