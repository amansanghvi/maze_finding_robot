#pragma once
#include "Arduino.h"

#define CENTERED_DIST 80


#define ONE_WALL_KP 0.4
#define ONE_WALL_KI 0 //0.0001
#define ONE_WALL_KD 120

class laserPID
{
private:
	unsigned long prev_time;
	double Kp;
	double Ki;
	double Kd;
	long cum_error;
	double rate_error;
	double prev_error;
  int flag = 0;
  int check = 0;
public:
	laserPID(double Kp, double Ki, double Kd);
	double computePID(uint8_t leftDist, uint8_t rightDist);
  double computePID2(uint8_t leftDist, uint8_t rightDist);
  double two_wall_PID(uint8_t leftDist, uint8_t rightDist);
  double left_wall_PID(uint8_t leftDist);
  double right_wall_PID(uint8_t rightDist);
	void resetPID();
};

// Constructor
laserPID::laserPID(double _Kp, double _Ki, double _Kd)
{
	Kp = _Kp;
	Ki = _Ki;
	Kd = _Kd;
};

// calculate output from input and setpoint
double laserPID::computePID(uint8_t left_dist, uint8_t right_dist)
{
 
	int error = 0 - (right_dist - left_dist);                                // determine error
	unsigned long current_time = millis();							//get current time
	int elapsed_time = (current_time - prev_time);   //compute time elapsed from previous computation

	cum_error += error * elapsed_time;								// compute integral
	rate_error = (error - prev_error) / elapsed_time;				// compute derivative

	double output = Kp * error + Ki * cum_error + Kd * rate_error;  //PID output               

	prev_error = error;											    //remember current error
	prev_time = current_time;										//remember current time

	return output;
};

// square wave PID
double laserPID::computePID2(uint8_t left_dist, uint8_t right_dist)
{
  double output;
  // check which walls are detected
  if ((left_dist < 180) && (right_dist < 180)) {
    check = 1;
    if (flag - check != 0) resetPID();
    output = two_wall_PID(left_dist, right_dist);
  }
  else if (left_dist < 150) {
    check = 2;
    if (flag - check != 0) resetPID();
    output = left_wall_PID(left_dist);
  }
  else if (right_dist < 150) {
    check = 3;
    if (flag - check != 0) resetPID();
    output = right_wall_PID(right_dist);
  }
  else {
    output = 0;
  }
  
  return output;
};

double laserPID::two_wall_PID(uint8_t left_dist, uint8_t right_dist)
{
  flag = 1;
  double error = 0 - (right_dist - left_dist);                                // determine error
  unsigned long current_time = millis();              //get current time
  int elapsed_time = (current_time - prev_time);   //compute time elapsed from previous computation

  cum_error += error * elapsed_time;                // compute integral
  rate_error = (error - prev_error) / elapsed_time;       // compute derivative

  double output = Kp * error + Ki * cum_error + Kd * rate_error;  //PID output               

  prev_error = error;                         //remember current error
  prev_time = current_time;                   //remember current time

  return output;
};

double laserPID::left_wall_PID(uint8_t left_dist)
{
  flag = 2;
  double error = left_dist - CENTERED_DIST;                                // determine error
  unsigned long current_time = millis();              //get current time
  int elapsed_time = (current_time - prev_time);   //compute time elapsed from previous computation

  cum_error += error * elapsed_time;                // compute integral
  rate_error = (error - prev_error) / elapsed_time;       // compute derivative

  double output = ONE_WALL_KP * error + ONE_WALL_KI * cum_error + ONE_WALL_KD * rate_error;  //PID output               

  prev_error = error;                         //remember current error
  prev_time = current_time;                   //remember current time

  return output;
};

double laserPID::right_wall_PID(uint8_t right_dist)
{
  flag = 3;
  double error = CENTERED_DIST - right_dist;                                // determine error
  unsigned long current_time = millis();              //get current time
  int elapsed_time = (current_time - prev_time);   //compute time elapsed from previous computation

  cum_error += error * elapsed_time;                // compute integral
  rate_error = (error - prev_error) / elapsed_time;       // compute derivative

  double output = ONE_WALL_KP * error + ONE_WALL_KI * cum_error + ONE_WALL_KD * rate_error;  //PID output               

  prev_error = error;                         //remember current error
  prev_time = current_time;                   //remember current time

  return output;
};

// reset errors 
void laserPID::resetPID()
{
	cum_error = 0;
	rate_error = 0;
	prev_error = 0;
	prev_time = millis();
};
