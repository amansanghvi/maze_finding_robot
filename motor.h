#pragma once

#include <stdint.h> 
#include "Arduino.h"

class motor
{
private:
    uint8_t E;
    uint8_t M;
    uint8_t M2;
    uint8_t bias = 0;
    

public:
    motor(uint8_t E_, uint8_t _M, uint8_t _M2);
    motor(uint8_t E_, uint8_t _M, uint8_t _M2, uint8_t bias);
    void enable();
    void disable();
    void forward(int speed);
    void backward(int speed);
};

motor::motor(uint8_t _E, uint8_t _M, uint8_t _M2)
{
    E = _E;
    M = _M;
  M2 = _M2;
    pinMode(M, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(A0,INPUT);
};

motor::motor(uint8_t _E, uint8_t _M, uint8_t _M2, uint8_t _bias)
{
    bias = _bias;
    motor(_E, _M, _M2);
};

void motor::enable()
{
    digitalWrite(E, HIGH);
};

void motor::disable()
{
    digitalWrite(E, HIGH);
    digitalWrite(M,LOW);
    digitalWrite(M2,LOW);
};

void motor::forward(int speed)
{
    int volt = analogRead(A0);
    double bat_volt = volt/1024.0*5*2;
    double adj_speed = (speed*9)/bat_volt + bias;
    adj_speed = min(adj_speed,100);
    uint8_t pwm = static_cast<uint8_t>(adj_speed * 255 / 100.0);
    digitalWrite(M, HIGH);
    digitalWrite(M2, LOW);// ------------------------------
    analogWrite(E, pwm);
};

void motor::backward(int speed)
{
    int volt = analogRead(A0);
    double bat_volt = volt/1024.0*5*2;
    double adj_speed = (speed*9)/bat_volt + bias;
    adj_speed = min(adj_speed,100);
    uint8_t pwm = static_cast<uint8_t>(adj_speed * 255 / 100.0);
    digitalWrite(M, LOW);
    digitalWrite(M2, HIGH); //-------------------------------------
    analogWrite(E, pwm);
};
