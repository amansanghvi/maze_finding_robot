#pragma once
#include "Arduino.h"

const int TRIG_PIN_DEFAULT = 41;
const int ECHO_PIN_DEFAULT = 43;

class Sonar
{
  private:
    int trigger_pin;
    int echo_pin;
  public:
    Sonar();
    Sonar(int _trigger_pin, int _echo_pin);
    void enable();
    unsigned long distance();
};

// Constructors
Sonar::Sonar(){
  trigger_pin = TRIG_PIN_DEFAULT;
  echo_pin = ECHO_PIN_DEFAULT;
};
Sonar::Sonar(int _trigger_pin, int _echo_pin){
  trigger_pin = _trigger_pin;
  echo_pin = _echo_pin;
};

// Enable
void Sonar::enable() {
  pinMode(trigger_pin, OUTPUT); // Sets the trigger_pin as an Output
  pinMode(echo_pin, INPUT); // Sets the echo_pin as an Input
};


// Distance
unsigned long Sonar::distance() {
  // Define Variables
  unsigned long duration;
  unsigned long distance;

  // Clears the trigger_pin
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigger_pin on HIGH state for 10 micro seconds
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigger_pin, LOW);
  
  // Reads the echo_pin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo_pin, HIGH);
  
  // Calculating the Distance in centimetres
  distance = duration*10/29/2;
  
  return distance;
};
