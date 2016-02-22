#include <irDist.h>

int computeDistance_SIR(int value){

  int distance = -1; //-1 signals that it did not qualify as a new distance
  if( value > 473 && value < 530)
  {
    //2 inches
    distance = 2;
  } else if( value > 335 && value < 366) distance = 3; //3 inches
      else  if(value > 246 && value < 268) distance = 4; //4 inches
          else if(value > 195 && value < 229) distance = 5; //5 inches
            else if (value > 175 && value < 191) distance = 6; //6 inches
              else if( value > 143 && value < 167) distance = 7; //7 inches
                else if(value > 121 && value < 136) distance = 8;//149 causes an overlap with 7 & 8 inches, changed to 136
                  else if (value > 107 && value < 135) distance = 9;
                    else if(value < 100) distance = 10; //10 inches

/*
  Serial.print(" sensor: ");
  Serial.print(distance);
  Serial.print(" dValue: ");
  Serial.print(value);
  Serial.print("\n"); */
                                                  
  return distance;
}