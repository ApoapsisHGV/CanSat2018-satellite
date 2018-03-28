#include <SD.h>
#include <SPI.h>


const int sdPin = 4;

void setup(){
  Serial.begin(9600);
  if(!SD.begin(sdPin)){
    while(1);  
  }
}

void loop(){
  File logFile;
  if(Serial.available()){
    logFile = SD.open("log.txt", FILE_WRITE);
    logFile.println(Serial.readString());
    logFile.close();
  }
  delay(500);
}
