#include "LED.h"
#include "motor.h"
#include "encoder.h"
#include "map.h"
#include "wheel.h"
#include "sonar.h"
#include "lidar.h"
#include "turnPID.h"
#include "laserPID.h"
#include "MPU6050_tockn.h"
#include <Wire.h>
#include "Arduino.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#define LASER_KP 0.3 // with new bat 0.5 works well, Could be tuned more 
#define LASER_KI 0 // with new bat 0.0001
#define LASER_KD 170 //with new bat 300

#define TURN_KP 0.5 // with new bat 0.5 works well, Could be tuned more 
#define TURN_KI 0// with new bat 0.0001
#define TURN_KD 15 //with new bat 300

#define SETTLE_TIME 200

#define ENCODER_DISTANCE_PER_TICK 5.69
#define ENCODER_COUNT_BIAS 5

#define TURN_ERROR 1

#define WALL_DIST_THRESHOLD 110
#define SONAR_DIST_STOP_THRESHOLD 80
#define SONAR_DEBOUNCE_TIME 50

#define RED_LED 35
#define GREEN_LED 37



// Motors         E,M1,M2
motor right_motor(5,4,10);
motor left_motor(6,7,11);
// Encoders and ptr  (A,B)
encoder left_encoder(18,8);
encoder right_encoder(3,9);
encoder *left_encoder_ptr = &left_encoder;
encoder *right_encoder_ptr = &right_encoder;
// IMU
MPU6050 mpu6050(Wire);
// Utrasonic 
Sonar front_sonar(41,43);
//Lasers
Lidar left_laser(24,60);
Lidar right_laser(28,61);

laserPID laser_PID(LASER_KP, LASER_KI, LASER_KD);
turnPID turn_PID(TURN_KP, TURN_KI, TURN_KD);

unsigned long last_sonar_pulse = millis();
unsigned long last_sonar_dist = 0;

void setup() {
    delay(1000);
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000);
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    attachInterrupt(digitalPinToInterrupt(18), left_encoderISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(3), right_encoderISR, CHANGE);

    // IMU setup
    Wire.begin();
    mpu6050.begin();
    mpu6050.calcGyroOffsets(true);

    // Lidar setup
    left_laser.enable();
    delay(10);
    right_laser.enable();
    delay(10);
    front_sonar.enable();
    
    Serial.begin(9600);
    Serial2.begin(9600);

    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);

    Serial.println("STARTING");
    Map *map = new Map();

    map->set_func(&init_pos, &rotate_to_side, &drive_forward, &wall_exists, &get_priority_side);
    map->explore();
    
    map->go_to_centre();
    map->go_to_pos_and_explore({0, 0});
    map->go_to_pos_and_explore({2, 4});


    digitalWrite(GREEN_LED, LOW); // Turn RED ON and GREEN OFF
    digitalWrite(RED_LED, HIGH);
    map->print();
}

void loop() {
  // put your main code here, to run repeatedly:

}

Side get_priority_side(int priority) {
    switch (priority) {
        case 0:
            return Side::FRONT;
        case 1:
            return Side::RIGHT;
        case 2:
            return Side::LEFT;
        case 3:
            return Side::BACK;
        default:
            return Side::BACK;
    }
}

void init_pos() {
    digitalWrite(GREEN_LED, HIGH); // Turn BOTH ON
    digitalWrite(RED_LED, HIGH);
    delay(1000);
    digitalWrite(GREEN_LED, LOW); // Turn BOTH OFF
    digitalWrite(RED_LED, LOW);

    while(true) {
        if (wall_exists((int)Side::FRONT, 1)) {
            break;
        }
        delay(20);
    }
    digitalWrite(GREEN_LED, LOW); // Turn GREEN ON and RED OFF
    digitalWrite(RED_LED, HIGH);

    while(true) {
        if (!wall_exists((int)Side::FRONT, 1)) {
            break;
        }
        delay(20);
    }

    digitalWrite(GREEN_LED, HIGH); // Turn GREEN ON and RED OFF
    digitalWrite(RED_LED, LOW);
    delay(1000);
}

void rotate_to_side(Side next_side) {
    switch(next_side) {
        case Side::RIGHT:
            turnIMU(right_motor, left_motor, 90, mpu6050, turn_PID);
            break;
        case Side::LEFT:
            turnIMU(right_motor, left_motor, -90, mpu6050, turn_PID);
            break;
        case Side::BACK:
            turnIMU(right_motor, left_motor, 175, mpu6050, turn_PID);
            break;
    }
    delay(SETTLE_TIME);
}

void drive_forward() {

    driveCells(right_encoder_ptr, right_motor, left_encoder_ptr, left_motor, 1);
    delay(SETTLE_TIME);
    return;
}

bool wall_exists(int side, int move_counter) {
    switch(side) {
        case (int)Side::FRONT: //FRONT
            return front_sonar.distance() < WALL_DIST_THRESHOLD;
        case (int)Side::RIGHT:
            return right_laser.distance() < WALL_DIST_THRESHOLD;
        case (int)Side::LEFT:
            return left_laser.distance() < WALL_DIST_THRESHOLD;
        case (int)Side::BACK:
            return false;
    }
switch (move_counter) {
    case 0: // 0,0, 0,1
        switch (side) {
            case (int)Side::FRONT:
                return false;
            case (int)Side::RIGHT:
                return true;
            case (int)Side::LEFT:
                return true;
            case (int)Side::BACK:
                return false;
        }
    case 1:
        switch (side) {
            case (int)Side::FRONT:
                return false;
            case (int)Side::RIGHT:
                return true;
            case (int)Side::LEFT:
                return true;
            case (int)Side::BACK:
                return false;
        }
    case 2:
        switch (side) {
            case (int)Side::FRONT:
                return true;
            case (int)Side::RIGHT:
                return false;
            case (int)Side::LEFT:
                return true;
            case (int)Side::BACK:
                return false;
        }
    case 3:
        switch (side) {
            case (int)Side::FRONT:
                return true;
            case (int)Side::RIGHT:
                return false;
            case (int)Side::LEFT:
                return true;
            case (int)Side::BACK:
                return false;
        }
    case 4:
        switch (side) {
            case (int)Side::FRONT:
                return false;
            case (int)Side::RIGHT:
                return true;
            case (int)Side::LEFT:
                return false;
            case (int)Side::BACK:
                return false;
        }
    case 5:
        switch (side) {
            case (int)Side::FRONT:
                return true;
            case (int)Side::RIGHT:
                return true;
            case (int)Side::LEFT:
                return false;
            case (int)Side::BACK:
                return false;
        }
    case 6: // 0,7
        switch (side) {
            case (int)Side::FRONT:
                return false;
            case (int)Side::RIGHT:
                return true;
            case (int)Side::LEFT:
                return true;
            case (int)Side::BACK:
                return false;
        }
    case 7: // 1,7
        switch (side) {
            case (int)Side::FRONT:
                return false;
            case (int)Side::RIGHT:
                return true;
            case (int)Side::LEFT:
                return false;
            case (int)Side::BACK:
                return false;
        }
    case 8:
        switch (side) {
            case (int)Side::FRONT:
                return false;
            case (int)Side::RIGHT:
                return true;
            case (int)Side::LEFT:
                return false;
            case (int)Side::BACK:
                return false;
        }
    case 9: 
        switch (side) {
            case (int)Side::FRONT:
                return false;
            case (int)Side::RIGHT:
                return true;
            case (int)Side::LEFT:
                return false;
            case (int)Side::BACK:
                return false;
        }
    case 10:
        switch (side) {
            case (int)Side::FRONT:
                return true;
            case (int)Side::RIGHT:
                return true;
            case (int)Side::LEFT:
                return false;
            case (int)Side::BACK:
                return false;
        }
    case 11: // 4, 7
        switch (side) {
            case (int)Side::FRONT:
                return true;
            case (int)Side::RIGHT:
                return false;
            case (int)Side::LEFT:
                return false;
            case (int)Side::BACK:
                return false;
        }
    case 12:
        switch (side) {
            case (int)Side::FRONT:
                return true;
            case (int)Side::RIGHT:
                return true;
            case (int)Side::LEFT:
                return false;
            case (int)Side::BACK:
                return false;
        }
    case 13:
        switch (side) {
            case (int)Side::FRONT:
                return true;
            case (int)Side::RIGHT:
                return false;
            case (int)Side::LEFT:
                return true;
            case (int)Side::BACK:
                return false;
        }
    case 14:
        switch (side) {
            case (int)Side::FRONT:
                return false;
            case (int)Side::RIGHT:
                return false;
            case (int)Side::LEFT:
                return false;
            case (int)Side::BACK:
                return false;
        }
}
    return false;
}

void turnIMU(motor right_motor, motor left_motor, double deg, MPU6050 mpu6050, turnPID turn_PID)
{
    turn_PID.resetPID();
    mpu6050.update();
    double yaw_offset = mpu6050.getAngleZ();
    double set_point = deg;
    set_point += deg > 0 ? -3 : 3;
    //Setpoint = -1*Setpoint;
    int complete = 0;
    while (complete < 50)
    {
        mpu6050.update();
        double curr_yaw = mpu6050.getAngleZ() - yaw_offset;
        double control = turn_PID.computePID(set_point, curr_yaw);
        int right_motor_speed = abs(control);
        int left_motor_speed = abs(control);

        right_motor_speed = max(right_motor_speed, 35);
        left_motor_speed = max(left_motor_speed,35);
        right_motor_speed = min(right_motor_speed,50); //50 is a good speed 
        left_motor_speed = min(left_motor_speed,50);

        if(control > 0){
            right_motor.forward(right_motor_speed);
            left_motor.backward(left_motor_speed);
        } else if(control < 0) {
            right_motor.backward(right_motor_speed);
            left_motor.forward(left_motor_speed);
        } else {
            right_motor.backward(0);
            left_motor.forward(0);
        }
        
        if(abs(curr_yaw-set_point) < TURN_ERROR){
            complete++;
            right_motor.backward(0);
            left_motor.forward(0);
        } else {
            complete = 0;
        }
    } 
    //Serial.println("complete");
}

void driveCells(encoder *right_encoder, motor right_motor, encoder *left_encoder, motor left_motor, int cells)
{
  right_encoder->reset();
  left_encoder->reset();
  double distance = 0;
  last_sonar_dist = front_sonar.distance();
  last_sonar_pulse = millis();
  laser_PID.resetPID();
  
  while (distance < 250*cells && last_sonar_dist > SONAR_DIST_STOP_THRESHOLD)
  {
    
    distance = (min(right_encoder_ptr->getCount(), left_encoder_ptr->getCount())+ENCODER_COUNT_BIAS)*ENCODER_DISTANCE_PER_TICK;
    follow_wall(right_motor, left_motor, left_laser, right_laser);
    if (millis() - last_sonar_pulse > SONAR_DEBOUNCE_TIME) {
      last_sonar_dist = front_sonar.distance();
      last_sonar_pulse = millis();
      // Serial2.print(right_encoder_ptr->getCount()); Serial2.println("mm");
    } 
  }

  left_motor.disable();
  right_motor.disable();  
};

void follow_wall(motor right_motor, motor left_motor, Lidar left_laser, Lidar right_laser)
{

  // dist between lasers 80.5
  // dist to wall is 84.75
  //right_motor.enable();
  //left_motor.enable(); 
  uint8_t leftDist = left_laser.distance();
  uint8_t rightDist = right_laser.distance();


  //double control = laser_PID.computePID(leftDist, rightDist);
  double control = laser_PID.computePID2(leftDist, rightDist);
  int right_motor_speed = 50+control;
  int left_motor_speed = 50-control;

  right_motor_speed = max(right_motor_speed, 0);
  left_motor_speed = max(left_motor_speed,0);
  right_motor_speed = min(right_motor_speed,60); //60 is a good speed 
  left_motor_speed = min(left_motor_speed,60);

  right_motor.forward(right_motor_speed);
  left_motor.forward(left_motor_speed);
  // Serial2.print(right_motor_speed);
  // Serial2.print(" ");
  // Serial2.println(left_motor_speed);
}

void left_encoderISR()
{
  left_encoder_ptr->encoderISR();
}

void right_encoderISR()
{
  right_encoder_ptr->encoderISR();
}
