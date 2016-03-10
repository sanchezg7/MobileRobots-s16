#include <robotMover.h>
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <irDist.h>


#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

unsigned long tBegin = 0;
unsigned long tCurrent = 0;

uint8_t buttons;

//void setup() {
//  // put your setup code here, to run once:
//  Serial.begin(9600);
//  lcd.begin(16,2);
//  lcd.setBacklight(WHITE);
//  tBegin = millis();
//  lcd.print("-->");
//  lcd.print(tBegin);
//}
//
//void loop() {
//  // put your main code here, to run repeatedly:
////  buttons = lcd.readButtons();
////
////  if(buttons){
////    
////    lcd.setCursor(0,1);
////    lcd.print("-->");
////    tCurrent = millis();
////    lcd.print(tCurrent);
////    delay(500);
////  }
//    tCurrent = millis();
//    if((tCurrent - tBegin) % 1000 == 0){
//      lcd.setBacklight(RED);
//      //delay(500);
//    }
//    lcd.setBacklight(WHITE);
//}

unsigned long start, finished, elapsed;
 
void setup()
{
  Serial.begin(9600);
}
 
void loop()
{
  Serial.println("Start...");
  start=millis();
  delay(1000);
  finished=millis();
  Serial.println("Finished");
  elapsed=finished-start;
  Serial.print(elapsed);
  Serial.print(" milliseconds elapsed");
  Serial.print("\t Start: ");
  Serial.println(start);

  //when incorportating buttons, include a delay to debounce (delay(200))
  
  delay(500);
}
