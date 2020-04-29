#pragma once
#include "Arduino.h"

class turnPID
{
private:
	unsigned long prev_time;
	double Kp;
	double Ki;
	double Kd;
	long cum_error;
	double rate_error;
	double prev_error;
public:
	turnPID(double Kp, double Ki, double Kd);
	double computePID(double setPoint, double sensor_input);
	void resetPID();
};

// Constructor
turnPID::turnPID(double _Kp, double _Ki, double _Kd)
{
	Kp = _Kp;
	Ki = _Ki;
	Kd = _Kd;
};

// calculate output from input and setpoint
double turnPID::computePID(double setPoint, double sensor_input)
{
	
	unsigned long current_time = millis();							//get current time
	int elapsed_time = (current_time - prev_time);   //compute time elapsed from previous computation

	double error = 0-(setPoint - sensor_input);                                // determine error
	cum_error += error * elapsed_time;								// compute integral
	rate_error = (error - prev_error) / (elapsed_time*1.0);				// compute derivative

	double output = Kp * error + Ki * cum_error + Kd * rate_error;  //PID output               

	prev_error = error;											    //remember current error
	prev_time = current_time;										//remember current time

	return output;
};

// reset errors 
void turnPID::resetPID()
{
	cum_error = 0;
	rate_error = 0;
	prev_error = 0;
	prev_time = millis();
};
