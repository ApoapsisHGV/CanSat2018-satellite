#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GPS.h>
#include "BMP.h"
#include "rfm.h"
#include "gps.h"
#include "config.h"


File logfile;

#if DEBUG
#define VPRINT(data) Serial.print(data); logfile.print(data);
#define VPRINTLN(data) Serial.println(data); logfile.println(data);
#else
#define VPRINT(data)
#define VPRINTLN(data)
#endif

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

long datacounter = 0;

void beep_long(){
  digitalWrite(PIEZO, HIGH);
  delay(2000);
  digitalWrite(PIEZO, LOW);
}

void beep_short(){
  digitalWrite(PIEZO, HIGH);
  delay(500);
  digitalWrite(PIEZO, LOW);
}

void setup() {
  Serial.begin(115200);

  Serial.print("CLOUDPING VERSION: ");
  Serial.println(VERSION);
  Serial.println("Init starting");
  
  //Piezo init
  Serial.print("Initializing piezo ");
  pinMode(PIEZO, OUTPUT);
  Serial.print("[OK]\nRunning piezo self test ");
  beep_short();
  delay(1000);
  Serial.println("[OK]");

  // SD init
  Serial.print("Init: SD ");
  if (!SD.begin(SD_PIN)) {
    Serial.println("[FAILED]");
    beep_long();
    while (1);
  }
  Serial.println("[OK]");

  if (SD.exists("log.txt") && DELETE_OLD) {
    Serial.print("Deleting old log...");
    SD.remove("log.txt");
    Serial.println("Done");
  }

  logfile = SD.open("log.txt", FILE_WRITE);
  
  //BMP180 init
  VPRINT("Initializing BMP180 ");
  if (!bmp.begin()) {
    VPRINTLN("[FAILED]");
    beep_long();
    delay(1000);
    beep_long();
    while (1);
  }
  bmp.calibrate();
  VPRINTLN("[OK]");

  VPRINT("Initializing fine dust sensor ");
  //Feinstaubsensor init
  pinMode(VOUT, INPUT);
  pinMode(ILED, OUTPUT);    //LED als Output
  digitalWrite(ILED, LOW);  //LED standardmäßig auf 0
  VPRINTLN("[OK]");

  VPRINTLN("Initializing GPS ");
  //GPS init
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  usingInterrupt = useInterrupt(true);
  VPRINTLN("[OK]");
  
  // radio init
  VPRINT("Initializing RFM69 ");
  if(!rfm69.init()){
    VPRINTLN("[FAILED]");
    beep_long();
    delay(1000);
    beep_long();
    delay(1000);
    beep_long();
    while(1);
  }
  VPRINTLN("[OK]");
  beep_short();
  logfile.close();
}

void loop() {
  //Variablen deklarieren
  double temperature, pressure;
  float lon, lat, velocity;
  String timestamp;
  int maxHeight;
  
  //BMP180
  bmp.getTemperature(temperature);
  bmp.getPressure(pressure, temperature);
  height = bmp.getHeight(pressure);
  if (height > maxHeight) {
    maxHeight = height;
  }
  else if (height < maxHeight - 300){
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

  datacounter++;

  String pload = "T:"+(String)temperature+",P:"+(String)pressure+",D:"+(String)density+",Vo:"+(String)voltage+",DC:"+(String)datacounter;
  //Datenpaket wird erstellt
  if (GPS.newNMEAreceived()) {
    pload += ",NE:"+String(GPS.lastNMEA());  
  }
  
  char payload[pload.length()];
  pload.toCharArray(payload, pload.length());

  logfile = SD.open("log.txt", FILE_WRITE);
  //send to Partner
  logfile.println(payload);
  logfile.close();
  
  //send with radio
  rfm69.sendData(payload);

  delay(500);
}
