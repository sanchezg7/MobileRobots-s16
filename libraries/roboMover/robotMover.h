#ifndef __roboBullMotion_h
#define __roboBullMotion_h
#include <Servo.h>
#include <Wire.h>
#include <Arduino.h>


 //extern Servo LServo;
 //extern Servo RServo;

class robotMover{
private:
	Servo LServo;
	Servo RServo;
	
public:
	robotMover();
	void cmd_vel(int left, int right);
	void move_fwd(int time);
	void move_bkwd(int time);
	void spin_cntclkwse();
	void spin_clkwse();
	void attach_servos();
	void circle_20(int time);
	void cnt_circle_20(int time);
	void fig8_circle_20(int time);
	void fig8_cnt_circle_20(int time);
	void follow_vel(int lVel, int rVel);
	void rightTurn();
	void leftTurn();

};

#endif