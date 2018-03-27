#include <qbcan.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

//sensor init
//pressure and temp
BMP180 bmp;

//dust sensor
int dustMeasurePin = 0;
int irLedPower = 2;


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
char payload[50];
RFM69 radio;

//SD card
const int sdPin = 4;

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

  
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //To use the high power capabilities of the RFM69HW
  radio.encrypt(ENCRYPTKEY);
}

void loop(){
  double pressure, temparature, dustDensity;
  bmp.getData(temparature, pressure);
  
  digitalWrite(irLedPower, LOW);
  delayMicroseconds(200);
  dustDensity = analogRead(dustMeasurePin);
  delayMicroseconds(40);
  digitalWrite(irLedPower,HIGH);
  dustDensity = dustDensity * (5.0 / 1024.0);
  dustDensity = 0.17 * dustDensity - 0.1;

  sprintf(payload, "T:%d,P:%d,D:%d", (int)temparature,(int)pressure,dustDensity);

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
