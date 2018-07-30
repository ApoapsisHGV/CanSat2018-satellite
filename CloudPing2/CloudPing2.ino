#include <SD.h>
#include <SPI.h>


const int sdPin = 10;

void setup(){
  Serial.begin(115200);
  if(!SD.begin(sdPin)){
    Serial.println("init failed");
    while(1);  
  }
  Serial.println("init succesfull");
}

void loop(){
  File logFile;
  if(Serial.available()){
    String data;
    logFile = SD.open("log.txt", FILE_WRITE);
    data = Serial.readString();
    Serial.println(data);
    logFile.println(data);
    logFile.close();
  }
  delay(500);
}
