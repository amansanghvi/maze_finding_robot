#pragma once

#include <stdint.h> 
#include "Arduino.h"

class LED
{
private:
	uint8_t pin;
  int state;

public:
	LED(uint8_t _pin);
	void enable();
	void disable();
  int getState();
};

LED::LED(uint8_t _pin)
{
	pin = _pin;
	pinMode(pin, OUTPUT);
	//Serial.print("created LED");
};

void LED::enable()
{
	digitalWrite(pin, HIGH);
  state = HIGH;
	//Serial.print("Enabled LED");
};

void LED::disable()
{
	digitalWrite(pin, LOW);
  state = LOW;
	//Serial.print("Disabled LED");
};

int LED::getState()
{ 
  return state;
};


