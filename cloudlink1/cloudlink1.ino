#include <Wire.h>
#include <SPI.h>
#include <SoftwareSerial.h>  
#include <Adafruit_GPS.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <RH_RF69.h>


//sensor init
//pressure and temp
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
double pressure;  

//piezo
#define PIEZO 8

//dust sensor
const int dustMeasurePin = 0;
const int irLedPower = 2;


//debug printing
#define DEBUG 1

#ifdef DEBUG
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

//radio
#define RF69_FREQ 433.0
#define RFM69_RST 9
#define RFM69_CS 10
#define RFM69_INT 5
RH_RF69 rf69(RFM69_CS, RFM69_INT);

void setup(){
  if(!DEBUG){
    SoftwareSerial Serial(7,6);
  }
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
  VPRINTLN("Startin dust sensor init");
  pinMode(irLedPower,OUTPUT);
  VPRINTLN("success");

  //piezo init
  VPRINTLN("Starting piezo init");
  pinMode(PIEZO,OUTPUT);
  VPRINTLN("success");

  //gps init
  VPRINTLN("Starting gps init");
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  useInterrupt(true);
  VPRINTLN("success");

  //radio init
  VPRINTLN("Starting radio init")
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  if (!rf69.init()) {
    VPRINTLN("failed");
    while (1);
  }
  if (!rf69.setFrequency(RF69_FREQ)) {
    VPRINTLN("failed");
    while (1);
  }
  rf69.setTxPower(20, true);
  uint8_t key[] = { 0x41, 0x70, 0x30, 0x61, 0x70, 0x35, 0x31, 0x35,
                    0x48, 0x47, 0x56, 0x2d, 0x32, 0x30, 0x31, 0x38};
  rf69.setEncryptionKey(key);
}

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

void loop(){
  //temparature and pressure
  sensors_event_t event;
  bmp.getEvent(&event);
  double newPressure, dustDensity;
  float lon, lat, velocity, temparature, height;
  int hour,minute,second,milisecond;
  char payload[100];

  //bmp
  newPressure = event.pressure;
  bmp.getTemperature(&temparature);
  if(newPressure < pressure){
    digitalWrite(PIEZO, HIGH);
  }
  pressure = newPressure;
  height = bmp.pressureToAltitude(SENSORS_PRESSURE_SEALEVELHPA,pressure,temparature);

  //dust
  digitalWrite(irLedPower, LOW);
  delayMicroseconds(200);
  dustDensity = analogRead(dustMeasurePin);
  delayMicroseconds(40);
  digitalWrite(irLedPower,HIGH);
  dustDensity = dustDensity * (5.0 / 1024.0);
  dustDensity = 0.17 * dustDensity - 0.1;

  //gps
  if(GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))
      return;
  }
  if (timer > millis())  timer = millis();
  lat = GPS.latitudeDegrees;
  lon = GPS.longitudeDegrees;
  velocity = GPS.speed*0.514444; // knots to m/s
  hour = GPS.hour;
  minute = GPS.minute;
  second = GPS.seconds;
  milisecond = GPS.milliseconds,
    

  sprintf(payload, "T:%d,P:%d,D:%d,LA;%d,LO:%d,H:%d,M:%d,S:%d,MI:%d,HE:%d", temparature, pressure, dustDensity, lat, lon, velocity, hour, minute, second, milisecond, height);

  //send to Partner
  Serial.println(payload);

  //send with radio
  int sum = 0;
  for(int c = 0; c < sizeof(payload); c++){
     sum += payload[c];
  }
  sprintf(payload, "%d,CHKS:%d", payload, sum);
  rf69.send((uint8_t *)payload, strlen(payload));
  rf69.waitPacketSent();//udp style, dont wait for response
  

  delay(100);
}
