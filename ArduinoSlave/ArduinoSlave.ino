// Insert file Library
#include <SPI.h>
#include <DMD2.h>
#include <fonts/Arial_Black_16.h>
#include <Wire.h>
#include <DS3231.h>

DS3231  rtc(SCL,SDA);


const int WIDTH = 1;
SPIDMD dmd(WIDTH,1);  // DMD controls the entire display (try spidmd)
DMD_TextBox box(dmd);  // "box" provides a text box to automatically write to/scroll the display

int wtd = 0;
String receivedData; // Variable to store the received data
String timetxt;
String timetxt2;
String temp;
String prev;
String someseg;
String raw;
String space = " ";
String c = "*C";
String MESSAGE = "IEEE ";
int pau = 0;
bool set = false;

const int lightSensorPin = A0;
int older = 10;

boolean isValidData(String data) {          // by chat gpt ? test required
  // Check if the data has the correct delimiter positions
  if (data.charAt(2) != '/' || data.charAt(5) != '/' || data.charAt(8) != '/') {
    return false;
  }

  // Check if the first six characters are numeric
 /*for (int i = 0; i < 6; i++) {
    if (!isDigit(data.charAt(i))) {
      return false;
    }
  } */
  // If all checks pass, the data structure is valid
  return true;
}

void setup() {

  dmd.setBrightness(10); // Set brightness 0 - 255 
  dmd.selectFont(Arial_Black_16); // Font used
  dmd.begin();     // Start DMD 
  pinMode(5, OUTPUT);
  Serial.begin(9600);
  digitalWrite(5, HIGH);
  delay(100);
  digitalWrite(5, LOW);
  rtc.begin();
    while(pau < 9){
  const char *next2 = MESSAGE.c_str();
  while(*next2) {
  //  Serial.print(*next);
    box.print(*next2);
    delay(300);
    next2++;
  }
  pau = pau + 1;
}
}

void loop() {
if (Serial.available() > 0) {
    receivedData = Serial.readStringUntil('\n');

   if (isValidData(receivedData)) {
      // If the data structure is valid, extract and print values
      timetxt = receivedData.substring(0, 2);
      timetxt2 = receivedData.substring(3, 5);
      temp = receivedData.substring(6, 8);
      prev = receivedData.substring(9);
      wtd = 0;
      MESSAGE = timetxt + ":" + timetxt2 + space + temp + c + space + prev + space; 
      if (set == false){
        int min = timetxt2.toInt();
        int hour = timetxt.toInt();
        rtc.setTime(hour, min, 30); 
        set = true;
      }
   }else{

      wtd = wtd + 1;
      raw = rtc.getTimeStr();
      someseg = raw.substring(0, 5);
      MESSAGE = someseg + space + temp + c + space + prev + space; 
   }  
}else{

  wtd = wtd + 1;
  raw = rtc.getTimeStr();
  someseg = raw.substring(0, 5);
  MESSAGE = someseg + space + temp + c + space + prev + space; 
}


     // Read the analog value from the light sensor
  int lightSensorValue = analogRead(lightSensorPin);
  // Map the light sensor value to LCD brightness (adjust the range as needed)
  int dmdBrightness = map(lightSensorValue, 0, 4096, 0, 130);
  if (dmdBrightness != older){
    dmd.setBrightness(older);
    older = dmdBrightness;
  }


  const char *next = MESSAGE.c_str();
  while(*next) {
  //  Serial.print(*next);
    box.print(*next);
    delay(300);
    next++;
  }

if (wtd >= 50){
  digitalWrite(5, HIGH);
  delay(100);
  digitalWrite(5, LOW);
  wtd = 0;
}

}
