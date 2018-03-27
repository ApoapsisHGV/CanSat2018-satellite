#include <qbcan.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h> 
#include <TinyGPS.h> 

//sensor init
//pressure and temp
BMP180 bmp;

//dust sensor
const int dustMeasurePin = 0;
const int irLedPower = 2;


//debug printing
#define DEBUG

#ifdef DEBUG
 #define VPRINT(data) Serial.print(data);
 #define VPRINTLN(data) Serial.println(data);
#endif 

#ifndef DEBUG
 #define VPRINT(data)
 #define VPRINTLN(data)
#endif


//radio stuff
#define NODEID 2
#define NETWORKID 192
#define GATEWAYID 1
#define ENCRYPTKEY "Ap0ap515_HGV2018"
RFM69 radio;

//SD card
const int sdPin = 4;

//gps
TinyGPS gps;
SoftwareSerial gpsSerial(3,4);

void setup(){
  Serial.begin(9600);

  //BMP init
  VPRINTLN("REBOOT");
  VPRINTLN("Starting BMP180 init");
  if(!bmp.begin()){
    VPRINTLN("failed");
    while(1);
  }
  VPRINTLN("success");  

  //Dust init
  pinMode(irLedPower,OUTPUT);

  //SD init
  VPRINTLN("Starting SD init");
  if(!SD.begin(sdPin)){
    VPRINTLN("failed");
    while(1);  
  }
  VPRINTLN("success");

  //gps init
  gpsSerial.begin(9600);

  //radio init
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower();
  radio.encrypt(ENCRYPTKEY);
}

void loop(){
  //temparature and pressure
  double pressure, temparature, dustDensity;
  float lon, lat;
  char payload[50];
  bmp.getData(temparature, pressure);

  //dust
  digitalWrite(irLedPower, LOW);
  delayMicroseconds(200);
  dustDensity = analogRead(dustMeasurePin);
  delayMicroseconds(40);
  digitalWrite(irLedPower,HIGH);
  dustDensity = dustDensity * (5.0 / 1024.0);
  dustDensity = 0.17 * dustDensity - 0.1;

  //gps
  if(gpsSerial.available()){
    gps.f_get_position(&lat, &lon);
  }

  sprintf(payload, "T:%d,P:%d,D:%d,LA;%d,LO:%d", (int)temparature, (int)pressure, dustDensity, lat, lon);

  VPRINTLN("Writing to logfile");
  File logFile = SD.open("cloudlink_log.txt", FILE_WRITE);
  if (logFile){
    logFile.write(payload);  
  }
  else{
    VPRINTLN("failed");
  }
  VPRINTLN("success");

  VPRINTLN("sending data");
  radio.send(GATEWAYID, payload, 50);
  VPRINTLN("success");
  delay(100);
}
