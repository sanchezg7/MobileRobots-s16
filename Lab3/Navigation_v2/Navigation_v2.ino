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
int shrtR = 0;

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
  printLCD(trueDist(SFSensor, 20), trueDist(SLSensor, 20), trueDist_L(LFSensor, 20), 90, 90);
}


int trueDist(int sensor, int loops){
  int value = 0;
  for(int i = 0; i < loops; ++i)
  {
    int distance = computeDistance_SIR(analogRead(sensor));
    if(distance != -1)
    {
      value += distance;  
    }
    
  }
  return value / loops;
}

int trueDist_L(int sensor, int loops){
  int value = 0;
  for(int i = 0; i<loops; ++i)
  {
    int distance = computeLongDistance(analogRead(sensor));
    if(distance != -1)
    {
      value+= distance;
    }
  }
  return value / loops;
  
}

void printLCD(int shrtF, int shrtL, int lngF, int lVel, int rVel)
{
  

  if( shrtF != -1)
  {
     lcd.setCursor(0,1);
     lcd.print("F:");
     lcd.print(shrtF);
     lcd.print(" ");
  }
  if(shrtL != -1)
  {
    lcd.setCursor(5,1);
    lcd.print("L:");
    lcd.print(shrtL);
    lcd.print(" ");
  }

//  if(lngF != -1){
//    lcd.setCursor(10, 1);
//    lcd.print("l:");
//    lcd.print(lngF);
//  }

  if(lVel != 90)
  {
    
    lcd.setCursor(0,0);
    lcd.print("lV: ");
    lcd.print(lVel);
  }

  if(rVel != 90)
  {
    lcd.setCursor(7, 0);
    lcd.print("rV: ");
    lcd.print(rVel);
  }
}

void Navigation(){
  lcd.clear();
  int lngF; //long Front value
  bool cornerFlg = false;
  bool leftTrnFlg = false;

  int counter = 0;
  int wfCounter = 0;
  while(1)
  {
    shrtF = trueDist(SFSensor, 10);
    shrtL = trueDist(SLSensor, 10);
    shrtR = trueDist(SRSensor, 10);
    
    printLCD(shrtF, shrtL, lngF, lVel, rVel);

    //CORNER case - YELLOW
    //LEFT turn - GREEN
    //too FAR - TEAL
    //too CLOSE - RED
    

      //WALL FOLLOWING
      if(shrtF > 6)
      {
        //we dont care about the FRONT wall
          if(shrtL <= 5 ) //TOO CLOSE TO THE WALL
          {
            ++wfCounter;
            moveAwayFromWall();
            if(wfCounter == 3)
            { //offset the extreme moving away
              moveCloserToWall(); 
              wfCounter = 0;
            }
            
          } else if(shrtL >= 6)
            { //TOO FAR FROM THE WALL
              if(shrtL >= 9)
              {
                //LEFT TURN CHECK
                if(!leftTrnFlg)
                {
                  lcd.setBacklight(RED);
                  lcd.setCursor(0,0);
                  lcd.clear();
                  lcd.print("Left Turn Check!");
                  leftTrnFlg = true;
                  //move forward a little
                  moveForwardALittle();
                  stopRobot();
                  continue;  
                }else
                {//THIS IS WHERE THE LEFT TURN OCCURS        
                  leftTrnFlg = false;
                  
                  //perform the left turn!
                  leftTurn();
                  wfCounter = 0; 
  
                }
            }else
            { // too FAR
              lcd.setBacklight(TEAL);
              ++wfCounter;
              moveCloserToWall();
              if(wfCounter == 3)
              { //offset the moving away
                //moveAwayFromWall();
                wfCounter = 0;  
              }
            }
          } else if(shrtL == 5){ //aligned with wall
            lcd.setBacklight(WHITE);
            alignWithWall();
            //wfCounter = 0;
          }
         
      } else//GETTING CLOSE TO WALL FROM FRONT
      { 
         //STOP the robot and resample
        if(shrtL <= 5)//check left sensor to make sure it's a corner
        {
          if(!cornerFlg)
            {
              lcd.clear();
              lcd.setBacklight(YELLOW);
              lcd.print("Corner case check");
              stopRobot();
              cornerFlg = true;  
              continue;
            }else 
            { 
              //CORNER confirmed 
              cornerFlg = false; //reset the flag
              rightTurn();
              wfCounter = 0;
            }
        } 
      }

    if(leftTrnFlg){
      leftTrnFlg = false;
    }
    if(cornerFlg){
      cornerFlg = false;
    }
  }
}

void rightTurn(){
  
  lVel = 100;
  rVel = 90;
  myMover.cmd_vel(lVel, rVel);
  delay(1100);
  
}

void leftTurn(){
  //perform the left turn!
  //myMover.cmd_vel(90, 80);
  lVel = 90;
  rVel = 80;
  myMover.cmd_vel(lVel, rVel);
  delay(1200);
}


// <= 5 95, 88
// >= 7 92, 85
void moveCloserToWall(){ //too far from wall
  
  //myMover.cmd_vel(93, 85);
  lVel = 95; 
  rVel = 83; //SPEED right up
  myMover.cmd_vel(lVel, rVel);
  //delay(200);  
}

void moveAwayFromWall(){ //too close to wall
 // myMover.cmd_vel(95, 87);
 lVel = 97; //SPEED left up
 rVel = 86; 
 myMover.cmd_vel(lVel, rVel);
 //delay(200);       
}

void alignWithWall(){ //align
  //myMover.cmd_vel(95, 85);
  lVel = 95,
  rVel = 86; //right motor is weaker
  myMover.cmd_vel(lVel, rVel);
  //delay(200); //allow robot to update quicker when aligned
}

void moveForwardALittle(){//move forward a little before left turn
  //myMover.cmd_vel(95, 85);//allow robot to move forward a bit before turning left
  lVel = 95;
  rVel = 85;
  myMover.cmd_vel(lVel, rVel);
  delay(1200); //allow the robot to move to the center of the cell to spin to the left
}

void stopRobot(){//stops robot
  //myMover.cmd_vel(90,90);
  lVel = 90;
  rVel = 90;
  myMover.cmd_vel(lVel, rVel);
  delay(500);
}

