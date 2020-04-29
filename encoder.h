#pragma once

#include <stdint.h> 
#include "Arduino.h"

class encoder
{
private:
	uint8_t A;
	uint8_t B;
	int Direction;
  double duration;
	uint8_t prev_A;
  volatile int count;
  int velocity;
  
public:
	encoder(uint8_t _A, uint8_t _B);
	void reset();
	void encoderISR();
	int getDirection();
  int getCount();
  void encoderPrint();
  void setVelocity(int vel);
  int getVelocity();
  double getDuration();
};

encoder::encoder(uint8_t _A, uint8_t _B)
{
	A = _A;
	B = _B;
	Direction = true;
  duration = 0;
	count = 0;
	prev_A = 0;
  velocity = 0;
	pinMode(B, INPUT);
};

void encoder::encoderISR()
{
	int Lstate = digitalRead(A);
	if ((prev_A == LOW) && Lstate == HIGH) {
		int val = digitalRead(B);
		if (val == LOW && Direction) {
			Direction = false;                       //Reverse
		}
		else if (val == HIGH && !Direction) {
			Direction = true;                        //Forward
		}
	}
	prev_A = Lstate;
  if(!Direction)  duration++;
  else  duration--;
	count++;
};

int encoder::getDirection()
{
	return Direction;
};

void encoder::reset()
{
	count = 0;
  Direction = true;
  duration = 0;
};

int encoder::getCount()
{
  return count;
};

void encoder::setVelocity(int vel)
{
  velocity = vel;
};

int encoder::getVelocity()
{
  return velocity;
};

void encoder::encoderPrint()
{
  if (Direction == true)
  {
    Serial.print("Forward: ");
  }
  else 
  {
    Serial.print("Backward: ");
  }
  Serial.print(count);
  Serial.print('\n');
};

double encoder::getDuration()
{
  return duration;
};


