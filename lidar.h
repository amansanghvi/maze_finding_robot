#pragma once
#include "Arduino.h"
#include "Adafruit_VL6180X.h"

const int SHUTDOWN_PIN_DEFAULT = 47;
const int ADDRESS_DEFAULT = VL6180X_DEFAULT_I2C_ADDR;

class Lidar
{
  private:
    int shutdown_pin;
    int address;
    Adafruit_VL6180X laser;
    //PID constants
    double kp = 2;
    double ki = 5;
    double kd = 1;
    unsigned long currentTime, previousTime;
    double elapsedTime;
    double error;
    double lastError;
    double input, output;
    double Setpoint = 84.75;
    double cumError, rateError;
  public:
    Lidar();
    Lidar(int _shutdown_pin, int _address);
    void enable();
    uint8_t distance();
    double computePID();
    void resetPID();
}; 

// Constructors
Lidar::Lidar() {
  shutdown_pin = SHUTDOWN_PIN_DEFAULT;
  address = ADDRESS_DEFAULT;
};
Lidar::Lidar(int _shutdown_pin, int _address) {
  shutdown_pin = _shutdown_pin;
  address = _address;
  pinMode(shutdown_pin, OUTPUT); // Set the Shutdown Pin as an Output Pin
  digitalWrite(shutdown_pin, LOW); // Deactivate the Lidar
};

// Enable
// NOTE: The enable functions should only ever be called after ALL lidar objects are created
void Lidar::enable() {
  digitalWrite(shutdown_pin, HIGH); // Activate the Lidar
  delay(10);
    if (! laser.begin(NULL, address)) { // Change Lidar address
        Serial.println("Failed to find sensor");
    }
};

// Distance
uint8_t Lidar::distance() {
    uint8_t result = 255;
    uint8_t dist = laser.readRange();
    uint8_t status = laser.readRangeStatus();
    if (status == VL6180X_ERROR_NONE) {
        result = dist;
    } else {
        result = 255;
    }
    return result;
};

double Lidar::computePID()
{     
    input = distance();
    if (input > 100)
    {
      error = lastError;
    }
    currentTime = millis();                //get current time
    elapsedTime = (double)(currentTime - previousTime);        //compute time elapsed from previous computation
    
    error = Setpoint - input;                                // determine error
    cumError += error * elapsedTime;                // compute integral
    rateError = (error - lastError)/elapsedTime;   // compute derivative

    double output = kp*error + ki*cumError + kd*rateError;                //PID output               

    lastError = error;                                //remember current error
    previousTime = currentTime;                        //remember current time

    return output;                                        //have function return the PID output
}

void Lidar::resetPID()
{
  lastError = 0;
  previousTime = millis();
};
