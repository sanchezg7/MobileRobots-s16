#include <robotMover.h>
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <irDist.h>

//lcd colors
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

const int SFSensor = A0;
const int SLSensor = A1;
const int SRSensor = A2;
const int LFSensor = A3;

int go = 0; //go flag for Navigation

//SENSOR Readings
int shrtF = 0;
int shrtL = 0;

//SERVOR Velocities
int rVel;
int lVel;

uint8_t buttons;

robotMover myMover;

void setup() {
  // put your setup code here, to run once:
  myMover.attach_servos();
  myMover.cmd_vel(90, 90);

  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.setBacklight(WHITE);
  lcd.print("Navigation");
  lcd.setCursor(0,0);

}

void loop() {
  // put your main code here, to run repeatedly:
  buttons = lcd.readButtons();

  if(buttons){
    delay(200); //debounce
    if(buttons & BUTTON_SELECT){
      if(go == 0){
        //start robot moving forward
        lVel = 100;
        rVel = 80;
        go = 1;
        Navigation();
      }
    }
    
  }
  printLCD(trueDist(SFSensor), trueDist(SLSensor), 90, 90);
}

int trueDist(int sensor){
  return computeDistance_SIR(average(sensor, 10)); //10 samples
  
}

int average(int sensor, int loops){
  int value;
  for(int i = 0; i < loops; ++i)
  {
     value += analogRead(sensor);
  }
  return value / loops;
}

void printLCD(int shrtF, int shrtL, int lVel, int rVel)
{

  if( shrtF != -1)
  {
     lcd.setCursor(0,1);
     lcd.print("F: ");
     lcd.print(shrtF);
     lcd.print(" ");
  }
  if(shrtL != -1)
  {
    lcd.setCursor(6,1);
    lcd.print("L: ");
    lcd.print(shrtL);
    lcd.print(" ");
  }

  if(lVel != 90)
  {
    lcd.setCursor(0,0);
    lcd.print("lV: ");
    lcd.print(lVel);
  }

  if(rVel != 90)
  {
    lcd.setCursor(6, 0);
    lcd.print("rV: ");
    lcd.print(rVel);
  }
}

void Navigation(){
  while(1){
    shrtF = trueDist(SFSensor);
    shrtL = trueDist(SLSensor);
    //consider long range sensor
    if(shrtF <10)
    {
      if(shrtF >= 5){
        if(shrtL < 5 ) //too close to wall
        {
          //85 and 95 as speed
          lVel = 100;
          rVel = 87;
          delay(1000);
          lVel = 100;
          rVel = 87; 
        } else if(shrtL > 5){ //too far
          lVel = 93;
          rVel = 80;
          delay(1000);
          lVel = 93;
          rVel = 87;
        } else if(shrtL == 5){ //aligned with wall
          lVel = 100;
          rVel = 80;
          delay(1000);
          lVel = 100;
          rVel = 80;
        }
      } else if(shrtF < 5){
        //RIGHT turn
        if (shrtL < 6){
          lcd.setBacklight(RED);
          lVel = 100;
          rVel = 100;
          delay(1000);
          lVel = 100;
          rVel = 100;  
        }
        
      }
    } else if(shrtF == 10){
      if(shrtL >= 9){
        //left turn 
        lcd.setBacklight(BLUE);
        lVel = 80;
        rVel = 80;
        delay(1000);
        lVel = 80;
        rVel = 80;
      }
    }
    printLCD(shrtF, shrtL, lVel, rVel);
    myMover.follow_vel(lVel, rVel);
  }
}

