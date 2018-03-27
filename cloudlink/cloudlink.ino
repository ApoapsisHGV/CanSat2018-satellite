#include <qbcan.h>
#include <Wire.h>
#include <SPI.h>


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

void setup(){
  Serial.begin(9600);
  VPRINTLN("REBOOT");
  if(bmp.begin()){
    VPRINTLN("BMP180 init success");  
  }
  else{
    VPRINTLN("BMP180 init fail (disconnected?)\n\n");
    while(1);
  }
  pinMode(irLedPower,OUTPUT);
  /*radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //To use the high power capabilities of the RFM69HW
  radio.encrypt(ENCRYPTKEY);
  Serial.println("Transmitting at 433 Mhz");*/  
}

void loop(){
  double pressure, temparature, dustDensity;
  bmp.getData(temparature, pressure);
  VPRINT("Absolute pressure: ");
  VPRINT(pressure);
  VPRINTLN(" mb.");
  VPRINT("Temperature: ");
  VPRINT(temparature);
  VPRINTLN(" deg C.");
  
  digitalWrite(irLedPower, LOW);
  delayMicroseconds(200);
  dustDensity = analogRead(dustMeasurePin);
  delayMicroseconds(40);
  digitalWrite(irLedPower,HIGH);
  dustDensity = dustDensity * (5.0 / 1024.0);
  dustDensity = 0.17 * dustDensity - 0.1;
  VPRINT("Dust density: ");
  VPRINTLN(dustDensity);
  
  
  /*
  sprintf(payload,"T: %d C, P: %d mb.",(int)T,(int)P);
  Serial.println(payload);
  radio.send(GATEWAYID, payload, 50);
  Serial.println("Send complete");
  */
  delay(100);
}
