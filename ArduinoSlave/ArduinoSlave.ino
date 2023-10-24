#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Arial14.h>
bool Autobrightness = false;    // set to false if ldr not hooked up
int AutobrightnessOfset = 6;  // not smaller than 4
int multiply = 127; // no bigger than 255
int sync = 400; // Smaller than Esp8266 timer (esp timer - 100)
const int WIDTH = 1;// Set Width to the number of displays wide you have
const uint8_t *FONT = Arial14;
int brightness;
char status;
SoftDMD dmd(WIDTH,1);  // DMD controls the entire display
DMD_TextBox box(dmd);  // "box" provides a text box to automatically write to/scroll the display


void setup() {
  pinMode(5, OUTPUT);
  Serial.begin(9600); // defaut is 9600 but for testing purpose set to 19200 (faster for esp timer saving)
  dmd.selectFont(FONT);
  dmd.begin();
  pinMode(A0, INPUT);
  digitalWrite(5, HIGH);
       dmd.setBrightness(255);
}
void loop() {
  if(Serial.available() > 0){
     char status = Serial.read();
     Serial.print(status);          //Uncomnet for debug and disconnect esp
     box.print(status);
     delay(sync);
     if(Autobrightness = true){
     int lvlinv = analogRead(A0) / AutobrightnessOfset;
     int lvl = lvlinv;
     int end = multiply*cos(lvl);
     dmd.setBrightness(end);   
     Serial.print(end);              //Uncomnet for debug and disconnect esp
     Serial.print("  ");             //Uncomnet for debug and disconnect esp
     Serial.print(lvlinv);           //Uncomnet for debug and disconnect esp
     Serial.println("  ");           //Uncomnet for debug and disconnect esp
  }

//  }
//  }
}}
