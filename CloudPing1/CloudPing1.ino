#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GPS.h>
#include "BMP.h"
//#include "dust.h"
#include "rfm.h"

//sensor init
//pressure and temp
#define GROUND 500
double height;
BMP180 bmp(GROUND);

//piezo
#define PIEZO 8

//dust sensor
#define COV_RATIO 0.2         //ug/mmm per mv
#define NO_DUST_VOLTAGE 400   //mv
#define SYS_VOLTAGE 5000      //5V für Arduino nano
const int iled = 2; //drive the led of sensor
const int vout = 0; //analog input
float density, voltage;
int   adcvalue;

//debug printing
#define DEBUG 1

#if DEBUG
#define VPRINT(data) Serial.print(data);
#define VPRINTLN(data) Serial.println(data);
#else
#define VPRINT(data)
#define VPRINTLN(data)
#endif

//gps
SoftwareSerial gpsSerial(4, 3);
Adafruit_GPS GPS(&gpsSerial);
boolean usingInterrupt = true;
/*
//radio
uint8_t key[] = { 0x41, 0x70, 0x30, 0x61, 0x70, 0x35, 0x31, 0x35,
                  0x48, 0x47, 0x56, 0x2d, 0x32, 0x30, 0x31, 0x38};
Radio rfm69(key, 10, 5, 9);
*/

void setup() {
  Serial.begin(57600);
  Serial.println("Start-Up");

  //BMP init
  VPRINTLN("REBOOT");
  VPRINTLN("Starting BMP180 init");
  if (!bmp.begin()) {
    VPRINTLN("failed");
    while (1);
  }
  bmp.calibrate();
  VPRINTLN("success");

  //Dust init
  VPRINTLN("Startin dust sensor init");
  pinMode(iled, OUTPUT);    //LED als Output
  digitalWrite(iled, LOW);  //LED standardmäßig auf 0
  VPRINTLN("success");

  //piezo init
  VPRINTLN("Starting piezo init");
  pinMode(PIEZO, OUTPUT);
  VPRINTLN("success");

  //gps init
  VPRINTLN("Starting gps init");
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  useInterrupt(true);
  VPRINTLN("success");
  /*
  // radio init
  VPRINTLN("Starting radio init");
  if(!rfm69.init()){
    VPRINTLN("failed");
    while(1);
  }
  VPRINTLN("success");
  */
}

// taken from https://github.com/adafruit/Adafruit_GPS


//code from adafruit for gps
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

uint32_t timer = millis();
// adafruit code end

void loop() {
  //parature and pressure
  double temperature, pressure;
  float lon, lat, velocity, newHeight ;
  int hour, minute, second, milisecond, maxHeight;
  
  //bmp
  bmp.getTemperature(temperature);
  bmp.getPressure(pressure, temperature);
  height = bmp.getHeight(pressure);
  
  
  if (height > maxHeight) {
    maxHeight = height;
  }

  if (height < maxHeight - 2){
    digitalWrite(PIEZO, HIGH);
  }
  
  
  //dust
  digitalWrite(iled, HIGH);
  delayMicroseconds(280);
  adcvalue = analogRead(vout);
  digitalWrite(iled, LOW);
  adcvalue = Filter(adcvalue);
  voltage = (SYS_VOLTAGE / 1023.0) * adcvalue * 11;  //Spannung wird umgerechnet

  //Filtern
  if(voltage >= NO_DUST_VOLTAGE){    
    voltage -= NO_DUST_VOLTAGE;
    density = voltage * COV_RATIO;   
  }else{
    VPRINT("Voltage too low: ");
    VPRINTLN(voltage);
    density = 0;
  }


 
  //gps
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))
      return;
  }
  
  if (timer > millis())  timer = millis();
  lat = GPS.latitudeDegrees;
  lon = GPS.longitudeDegrees;
  velocity = GPS.speed * 0.514444; // knots to m/s
  hour = GPS.hour;
  minute = GPS.minute;
  second = GPS.seconds;
  milisecond = GPS.milliseconds;
  String timestamp = (String)hour+";"+(String)minute+";"+(String)second+";"+(String)milisecond;

  String pload = "T:"+(String)temperature+",P:"+(String)pressure+",D:"+(String)density+",LA:"+(String)lat+",LO:"+(String)lon+",V:"+(String)velocity+",TI:"+ timestamp +",H:"+(String)height;
  char payload[pload.length()];
  pload.toCharArray(payload, pload.length());
  
  //send to Partner
  Serial.println(payload);
  /*
  //send with radio
  rfm69.sendData(payload);
  */
  delay(1000);
}

int Filter(int m) {
  static int flag_first = 0, _buff[10], sum;
  const int _buff_max = 10;
  int i;
  
  if(flag_first == 0) {
    flag_first = 1;

    for(i = 0, sum = 0; i < _buff_max; i++) {
      _buff[i] = m;
      sum += _buff[i];
    }
    return m;
  }else{
    sum -= _buff[0];
    for(i = 0; i < (_buff_max - 1); i++) {
      _buff[i] = _buff[i + 1];
    }
    _buff[9] = m;
    sum += _buff[9];
    
    i = sum / 10.0;
    return i;
  }
}
