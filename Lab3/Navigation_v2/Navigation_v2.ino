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
  printLCD(trueDist(SFSensor, 10), trueDist(SLSensor, 10), 90, 90);
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
    lcd.setCursor(7, 0);
    lcd.print("rV: ");
    lcd.print(rVel);
  }
}

void Navigation(){
  int lngF; //long Front value
  bool cornerFlg = false;
  bool leftTrnFlg = false;
  
  while(1){
    shrtF = trueDist(SFSensor, 10);
    shrtL = trueDist(SLSensor, 10);
    lngF = trueDist(LFSensor, 10);

    //CORNER case - YELLOW
    //LEFT turn - GREEN
    //too FAR - TEAL
    //too CLOSE - RED

    

      //GETTING CLOSE TO WALL
      if(shrtF < 5)
      {
          //STOP the robot and resample
          if(!cornerFlg)
          {
            myMover.follow_vel(90,90);
            lcd.clear();
            lcd.setBacklight(YELLOW);
            lcd.print("Corner case check");
            delay(1000);
            cornerFlg = true;  
            continue;
          }else if(shrtL <= 5) 
          { //check for for six if test doesnt work
            //CORNER confirmed (right turn, to proceed)
            cornerFlg = false; //reset the flag
            
            lVel = 100;
            rVel = 100;
            myMover.cmd_vel(lVel, rVel);
            
            delay(550); //FINE TUNE the right turn
            
          }
        
      } else { //we dont care about the FRONT wall
          //WALL FOLLOW SUCKA
          
          if(shrtL < 5 ) //too close to wall
          {
            lVel = 100;
            rVel = 85; 
            myMover.follow_vel(lVel, rVel);
            
            delay(200);
            
            
          } else if(shrtL > 5){ //too far
            if(shrtL >= 9)
            {
              //LEFT TURN CHECK
              if(!leftTrnFlg){
                leftTrnFlg = true;
                delay(750); //allow the robot to move to the center of the cell to spin to the left
                continue;  
              }else{
                leftTrnFlg = false;
                
                
                myMover.follow_vel(90,90);
                lcd.setCursor(0,0);
                lcd.clear();
                lcd.print("Left Turn Check!");
                
                //perform the left turn!
                lVel = 80;
                rVel = 80;
                
                myMover.follow_vel(lVel, rVel);
                delay(900);

                //make the robot go straight and then re-evaluate
                lVel = 95;
                rVel = 85;
                myMover.follow_vel(100, 80);
                delay(750);
              }
            }else{ // too FAR
              lcd.setBacklight(TEAL);
              lVel = 93;
              rVel = 85;
              myMover.follow_vel(lVel, rVel);
              
              delay(200);  
            }
            
            
          } else if(shrtL == 5){ //aligned with wall
            lcd.setBacklight(WHITE);
            lVel = 95;
            rVel = 85;
            myMover.follow_vel(lVel, rVel);
            
            delay(500); //allow robot to update quicker when aligned
          }
      }

    if(leftTrnFlg){
      leftTrnFlg = false;
    }
    if(cornerFlg){
      cornerFlg = false;
    }
    
    printLCD(shrtF, shrtL, lVel, rVel);
    //myMover.follow_vel(lVel, rVel);
  }
}

void rightTurn(){
  myMover.cmd_vel(100, 100);
  delay(1000);
  
}

