/*#pragma once

#include <stdint.h> 
#include "Arduino.h"
#include "encoder.h"

class wheel
{
	public:
		int getDistance();
    void wheel_turn(encoder *encoder, motor motor, int deg);
    void driveCells(encoder *right_encoder, motor right_motor, encoder *left_encoder, motor left_motor, int cells);
    void turn(encoder *right_encoder, motor right_motor, encoder *left_encoder, motor left_motor, int deg);
    void centeredTurn(encoder *right_encoder, motor right_motor, encoder *left_encoder, motor left_motor, int deg);
};

int wheel::getDistance(encoder *encoder)
{
	return (encoder->getCount()*5.537);
}

// turns the vehicle with one motor on (pivots about the opposite wheel)
void wheel::wheel_turn(encoder *encoder, motor motor, int deg)
{
    // 106.6 wheel space
    int totalRotation = 2*wheelSpace*deg/wheelRadius;
    int totalEncoderCounts = abs(totalRotation/15);
    int complete = 0;
    motor.enable();
    encoder->reset();
    if (deg < 0) {motor.backward(60);}
    else {motor.forward(60);}
    while (!complete)
    {
      if (encoder->getCount() - totalEncoderCounts >= 0)
      {
        motor.disable();
        encoder->reset(); 
        complete = 1;     
      }
    }    
};

void wheel::driveCells(encoder *right_encoder, motor right_motor, encoder *left_encoder, motor left_motor, int cells)
{
  right_encoder->reset();
  left_encoder->reset();
  right_motor.enable();
  left_motor.enable();
  double distance = 0;
  while (distance < 250*cells)
  {
    distance = right_encoder_ptr->getCount()*5.537;
    right_motor.forward(60);
    left_motor.forward(60);
  }
  left_motor.disable();
  right_motor.disable();  
};

// turns vehicel on the spot (uses both wheels going in opposite directions)
void wheel::turn(encoder *right_encoder, motor right_motor, encoder *left_encoder, motor left_motor, int deg)
{
    int totalRotation = wheelSpace*deg/wheelRadius;
    int totalEncoderCounts = abs(totalRotation/15);
    int complete = 0;
    left_motor.enable();
    right_motor.enable();
    left_encoder->reset();
    right_encoder->reset();
    if (deg < 0) 
    {
      left_motor.backward(60);
      right_motor.forward(60);
    }
    else 
    {
      left_motor.forward(60);
      right_motor.backward(60);
    }
    while (!complete)
    {
      if (left_encoder->getCount() - totalEncoderCounts >= 0)
      {
        left_motor.disable();
        right_motor.disable();
        left_encoder->reset();   
        right_encoder->reset(); 
        complete = 1;     
      }
    } 
};

void wheel::centeredTurn(encoder *right_encoder, motor right_motor, encoder *left_encoder, motor left_motor, int deg)
{
    int totalRotation = wheelSpace*deg/wheelRadius;
    int totalEncoderCounts = abs(totalRotation/15) + 1;
    double LDist = 0;
    double RDist = 0;
    int turnComplete = 0;
    int backComplete = 0;
    int forwardComplete = 0;
    left_motor.enable();
    right_motor.enable();
    left_encoder->reset();
    right_encoder->reset();
    
    // go backwards
    left_motor.backward(40);
    right_motor.backward(40);
    while (!backComplete)
    {
      RDist = right_encoder_ptr->getCount()*5.537;
      LDist = left_encoder_ptr->getCount()*5.537;
      if (RDist >= 32 || LDist >= 32)
      {
        left_motor.disable();
        right_motor.disable();
        left_encoder->reset();   
        right_encoder->reset(); 
        backComplete = 1;
      }
    }
    // Peform pure Turn
    if (deg < 0) 
    {
      left_motor.backward(50);
      right_motor.forward(50);
    }
    else 
    {
      left_motor.forward(50);
      right_motor.backward(50);
    }
    while (!turnComplete)
    {
      if ((left_encoder->getCount() - totalEncoderCounts >= 0) || (right_encoder->getCount() - totalEncoderCounts >= 0))
      {
        left_motor.disable();
        right_motor.disable();
        left_encoder->reset();   
        right_encoder->reset(); 
        turnComplete = 1;     
      }
    }

    // go forwards
    left_motor.forward(40);
    right_motor.forward(40);
    while (!forwardComplete)
    {
      RDist = right_encoder_ptr->getCount()*5.537;
      LDist = left_encoder_ptr->getCount()*5.537;
      if (RDist >= 32 || LDist >= 32)
      {
        left_motor.disable();
        right_motor.disable();
        left_encoder->reset();   
        right_encoder->reset(); 
        forwardComplete = 1;
      }
    }
};
*/
