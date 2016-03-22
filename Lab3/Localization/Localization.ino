
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
int Kp =  3; //Kp value
int gainF;
int gainL;
int errorF; //front error
int errorL; //left error


typedef struct orientation{
  char direction;
  int color;
} orientation;

//Localization Variables
orientation mOrientation[4];
int indexDirection = 1;

char robotDirection;

//SERVOR VELOCITIES
int rVel; //right wheel
int lVel; //left 

uint8_t buttons;

robotMover myMover;

void setup() {
  // put your setup code here, to run once:
  myMover.attach_servos();
  myMover.cmd_vel(90,90);

  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.setBacklight(WHITE);
  //lcd.print("Localization");
  lcd.setCursor(0,0);

  void configureArray();
  
  for(int i =0; i <= 16; ++i){
    updateCellStatus(i, 'X');
  }

  indexDirection = 1;
  robotDirection = mOrientation[indexDirection].direction; //set default direction to East

}




//assume our starting position is at cell 13 pointing East
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
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Kp ");
        lcd.print(Kp);
      }
  }
  if (go == 1){
    Localization(Kp);  
  }

  
  
}

void Localization(int Kp){
  //always starting East in block 13
  
  //take a sample of the IR and determine where to go
  //left wall following (left IR sensor)
  //if aligned then said value from gainF
  //if at a corner, then turn right (& update direction)
  //assume the leftIR will follow the wall
  ctlSysWallFllw(Kp);

  //after a certain time has elapsed, assign a new cell 
  //the old cell will be marked visited on the LCD
  //fromCell - leaving this cell (mark as visited if not done so already)
  //toCell - going to this cell

  //if time == numTime 
  //  depending on the direction: update the fromCell as visited
  //  assign the new cell respectively
  /*  Going:
   *  East  --> +1
   *  South --> -4
   *  West  --> -1
   *  North --> +4
   */
  
  
  
  
  return;
}

void ctlSysWallFllw(int Kp)
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
    myMover.rightTurn(); //make right turn
  }

  myMover.follow_vel(lVel, rVel);
}

void turnRight(){
  
  //update robot status
  updateDirection('r');

 //command the movement
  myMover.rightTurn();
}

void turnLeft(){
  //update the robot status
  updateDirection('l');

  //command the movement
  myMover.leftTurn();
  
}


void updateDirection(char turn)
{ //change our cardinal orientation within the cube
  
  //  east --> right turn --> south (++1) 
  //  east --> left turn --> north (--1)
  
    switch(turn){
      case 'r':
        indexDirection += 1;
        break;
      case 'l':
        indexDirection -= 1;
        break;
    }
  
    if(indexDirection < 0){
      indexDirection = 3; 
    }else{
      indexDirection = indexDirection % 4;  
    }   

    //update the robotDirection variable
    robotDirection = (mOrientation[indexDirection].direction);
    //update LCD color
    lcd.setBacklight(mOrientation[indexDirection].color);
}



// 13 14 15 16
//  9 10 11 12
//  5  6  7  8
//  1  2  3  4

void updateCellStatus(int cellNumber, char status){

  //char status is only one character to update the status

  switch(cellNumber){
    case 1:
      lcd.setCursor(5,1);
      lcd.print(status);
      break;
    case 2:
      lcd.setCursor(6,1);
      lcd.print(status);
      break;
    case 3:
      lcd.setCursor(7,1);
      lcd.print(status);
      break;
    case 4:
      lcd.setCursor(8,1);
      lcd.print(status);
      break;
    case 5:
      lcd.setCursor(5,0);
      lcd.print(status);
      break;
    case 6:
      lcd.setCursor(6,0);
      lcd.print(status);
      break;
    case 7:
      lcd.setCursor(7,0);
      lcd.print(status);
      break;
    case 8:
      lcd.setCursor(8,0);
      lcd.print(status);
      break;
    case 9:
      lcd.setCursor(0,1);
      lcd.print(status);
      break;
    case 10:
      lcd.setCursor(1,1);
      lcd.print(status);
      break;
    case 11:
      lcd.setCursor(2,1);
      lcd.print(status);
      break;
    case 12:
      lcd.setCursor(3,1);
      lcd.print(status);
      break;
    case 13:
      lcd.setCursor(0,0);
      lcd.print(status);
      break;
    case 14:
      lcd.setCursor(1,0);
      lcd.print(status);
      break;
    case 15:
      lcd.setCursor(2,0);
      lcd.print(status);
      break;
    case 16:
      lcd.setCursor(3,0);
      lcd.print(status);
      break;
  }
}

//make multiple measurements and return the average
int average(int sensor, int loops)
{
  int value;
  for(int i = 0; i < loops; ++i)
  {
     value += analogRead(sensor);
  }
  return value / loops;
}

void configureArray(){

  //Red - Right
  //Green - Left
  //Blue - Up
  //Yellow - Down
  
  //north
  mOrientation[0].direction = 'N';
  mOrientation[0].color = BLUE;

  //east
  mOrientation[1].direction = 'E';
  mOrientation[1].color = RED;

  //west 
  mOrientation[2].direction = 'W';
  mOrientation[2].color = GREEN;

  //south
  mOrientation[3].direction = 'S';
  mOrientation[3].color = YELLOW;
}

