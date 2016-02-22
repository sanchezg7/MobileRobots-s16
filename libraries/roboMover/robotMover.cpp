#include <robotMover.h>

	Servo LServo;
	Servo RServo;

robotMover::robotMover()
{
	//constructor
	
}

void robotMover::cmd_vel(int left, int right)
{

  if(left != -1) LServo.write(left);
  if(right != -1) RServo.write(right);
 
}
void robotMover::attach_servos()
{
	LServo.attach(2);
	RServo.attach(3);
	return;

}

void robotMover::move_fwd(int time)
{
    cmd_vel(100, 80);//l & r
    delay(time);
    cmd_vel(90, 90);
}

void robotMover::move_bkwd(int time)
{
  cmd_vel(80, 100); //move the robot backwards
  delay(time);
  cmd_vel(90, 90);
}

void robotMover::spin_cntclkwse()
{
  cmd_vel(90, 80);
  delay(1350);
  cmd_vel(90, 90);
}

void robotMover::spin_clkwse()
{
  cmd_vel(100, 90);
  delay(1250);
  cmd_vel(90,90);
}

void robotMover::cnt_circle_20(int time)
{
	cmd_vel(94, 83);
	delay(time);
	cmd_vel(90,90); //stop the robot
}

void robotMover::circle_20(int time)
{
	cmd_vel(96, 87);	
	delay(time);
	cmd_vel(90,90); //stop the robot
}

void robotMover::fig8_circle_20(int time)
{
	cmd_vel(96, 87);
	delay(time); 
	cmd_vel(90,90);
}

void robotMover::fig8_cnt_circle_20(int time)
{
	cmd_vel(94, 83); //might work
	delay(time); 
	cmd_vel(90,90);
}
