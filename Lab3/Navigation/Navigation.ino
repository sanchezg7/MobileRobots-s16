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
#define SET_PT 5 //5 inches

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
int go = 0;

const int SFSensor = A0;
const int SLSensor = A1;
const int SRSensor = A2;
const int LFSensor = A3;

//SENSOR READINGS
int shrtF = 0; //short front reading
int shrtL = 0; //short left reading

//CONTROL SYSTEM VARIABLES
double Kp = 3; //Kp value
int gainF;
int gainL;
int errorF; //front error
int errorL; //left error

//SERVOR VELOCITIES
int rVel; //right wheel
int lVel; //left wheel

uint8_t buttons;

robotMover myMover;

void setup() {
  // put your setup code here, to run once:
  myMover.attach_servos();
  myMover.cmd_vel(90,90);

  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.setBacklight(WHITE);
  lcd.print("Navigation");
  lcd.setCursor(0,0);
}

void loop() {
  // put your main code here, to run repeatedly:
  buttons = lcd.readButtons();
  
  if(buttons)
  {
    delay(200);
    if(buttons & BUTTON_SELECT)
    {
      if(go == 0)
      {
        lVel = 100;
        rVel = 80;        
        go = 1;
      }
      
           
    } else if(buttons & BUTTON_UP)
      {
        lcd.setCursor(0,0);
        lcd.print("Kp ");
        lcd.print(Kp);
      }
  }
  if (go == 1){
    Navigation(Kp);  
  }

  
  printLCD(computeDistance_SIR(average(SFSensor,5))
      , computeDistance_SIR(average(SLSensor,5)), 90, 90);
  
  delay(250);

}

void Navigation(int Kp)
{
  shrtF = average(SFSensor, 5);
  shrtL = average(SLSensor, 5);

  shrtF = computeDistance_SIR(shrtF);
  shrtL = computeDistance_SIR(shrtL);

  errorL = SET_PT - shrtL; //error from LEFT
  errorF = SET_PT - shrtF; //error from FRONT
  gainL = Kp * errorL;
  gainF = Kp * errorF;

  if(shrtF > 6) //check front wall to know to keep going forward
  {
    if(shrtL > 5) //too far from left wall
    {
      lVel = 93;
      rVel = 85;
    }else if(shrtL < 5) //too close to left wall
    {
      lVel = 95;
      rVel = 87;
    }else if(shrtL == 5) //align
    {
      lVel = 90 - gainF;
      rVel = 90 + gainF;
    }else
    {
      lVel = 90 - gainF;
      rVel = 90 + gainF;
    }
  } else if(shrtF <= 5)//decision to make right turn
  {
    //lVel = 100;
    //rVel = 100;
    //to implement: stop then turn right on the spot
    //start moving
  }

  follow_vel(lVel, rVel);
}

void follow_vel(int lVel, int rVel)
{
  if(lVel > 100) lVel = 100;
    else if(lVel < 90) lVel = 90; //prevent go backward

   //check for saturation in right wheel
   if(rVel < 81) rVel = 81;
        else if(rVel > 90) rVel = 90;

    myMover.cmd_vel(lVel, rVel);

    Serial.print("lVel:");
    Serial.print(lVel);
    Serial.print("\trVel");
    Serial.print(rVel);
    Serial.print("\n");
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
