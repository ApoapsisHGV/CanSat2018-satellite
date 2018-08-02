#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GPS.h>
#include "BMP.h"
#include "rfm.h"
#include "gps.h"
#include "config.h"


//Sensor initialisieren
//Druck & Temperatur
double height;
BMP180 bmp;

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

void beep_long(){
    digitalWrite(PIEZO, HIGH);
    delay(2000);
    digitalWrite(PIEZO, HIGH);
}

void setup() {
  Serial.begin(115200);
  //Piezo init
  pinMode(PIEZO, OUTPUT);
  
  //BMP180 init
  if (!bmp.begin()) {
    beep_long();
    while (1);
  }
  bmp.calibrate();

  //Feinstaubsensor init
  pinMode(VOUT, INPUT);
  pinMode(ILED, OUTPUT);    //LED als Output
  digitalWrite(ILED, LOW);  //LED standardmäßig auf 0

  //GPS init
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  usingInterrupt = useInterrupt(true);
  
  
  // radio init
  if(!rfm69.init()){
    beep_long();
    while(1);
  }
   digitalWrite(PIEZO, HIGH);
   delay(500);
   digitalWrite(PIEZO, HIGH);
}

void loop() {
  //Variablen deklarieren
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
  voltage = (SYS_VOLTAGE / 1023.0) * adcvalue * 11;  //Spannung wird errechnet

  //Vorfiltern - Wenn die gemessene Spannung zu niedrig ist, wird davon ausgegangen, dass kein Feinstaub vorhanden ist
  if(voltage >= NO_DUST_VOLTAGE){    
    voltage -= NO_DUST_VOLTAGE;
    density = voltage * COV_RATIO;   
  }else{
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

  delay(500);
}
