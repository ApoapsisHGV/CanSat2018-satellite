#include <SD.h>
#include <SPI.h>

const int sdPin = 4;
String data;

void setup(){
  Serial.begin(9600);
  if(!SD.begin(sdPin)){
    while(1);  
  }
  File logFile;
  Serial.println("opening file");
  logFile = SD.open("sd_is_shit.txt", FILE_WRITE);
  Serial.println("done");
  logFile.write("fuck sd");
  Serial.println("write done");
  logFile.close();
  Serial.println("file closed");
}

void loop(){
  /*File logFile;
  Serial.println("opening file");
  logFile = SD.open("cloudlink_log.txt", FILE_WRITE);
  Serial.println("DONE");
  if(Serial.available()){
    Serial.println("Reading shit");
    data = Serial.readString();
    Serial.println(data);
    logFile.println("fuck you coelle");
    Serial.println("DONE WRITING");
  }
  logFile.close();
  delay(1000);*/
}
