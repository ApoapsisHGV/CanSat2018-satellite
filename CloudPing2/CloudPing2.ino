#include <SD.h>
#include <SPI.h>
#include "configure.h"

#if DEBUG
#define VPRINT(data) Serial.print(data);
#define VPRINTLN(data) Serial.println(data);
#else
#define VPRINT(data)
#define VPRINTLN(data)
#endif

File logfile;
char buffer[300] = "";
int buffer_counter = 0;

void setup() {
  Serial.begin(115200);
  Serial.print("Init: SD ");
  if (!SD.begin(SD_PIN)) {
    Serial.println("[FAILED]");
    while (1);
  }
  Serial.println("[OK]");

  if (SD.exists("log.txt") && DELETE_OLD) {
    Serial.print("Deleting old log...");
    SD.remove("log.txt");
    Serial.println("Done");
  }
  logfile = SD.open("log.txt", FILE_WRITE);
  logfile.write("\n\n\nREBOOT\n\n");
  logfile.close();
}

void loop() {
  if (Serial.available()) {
    int data;
    data = Serial.read();
    if (data == "\n") {
      logfile = SD.open("log.txt", FILE_WRITE);
      buffer[buffer_counter] = data;
      logfile.write(buffer);
      logfile.close();
      for (int c = 0; c < sizeof(buffer); c++) {
        buffer[c] = 0;
      }
      buffer_counter = 0;
    }
    else {
      buffer[buffer_counter] = data;
      buffer_counter++;
    }
  }
}

