#include <SD.h>
#include <SPI.h>

const boolean debug = false;
const boolean deleteSD = false;
const int sdPin = 10;

void setup(){
  File logfile;
  Serial.begin(115200);
  delay(1000);
  if(!SD.begin(sdPin)){
    Serial.println("Initialisierung fehlgeschlagen - SD Karte vorhanden?");
    while(1);  
  }
  if(SD.exists("log.txt")){
    if(deleteSD){
      Serial.println("log.txt wird gelöscht");
      SD.remove("log.txt");
    }else{
      logfile = SD.open("log.txt", FILE_WRITE);
      for(int i = i; i < 5; i++){
        logfile.println(" ");
      }
      logfile.println("REBOOT");
      logfile.println("Neuer Datensatz");
      logfile.println(" ");
    }
  }
  Serial.println("Initialisierung erfolgreich");
}

void loop(){
  File logFile;
  if(Serial.available()){
    String data;
    logFile = SD.open("log.txt", FILE_WRITE);
    data = Serial.readString();
    if(debug){
      Serial.println(data);
    }
    logFile.println(data);
    logFile.close();
  }
  delay(500);
}
