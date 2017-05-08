/**
  TimedServos.h - A library for managing timed servo movements on PWM boards.

  Copyright (c) 2015 Manny Garcia, written for the R2Builders Group

  BSD license, all text above must be included in any redistribution
**/
#ifndef TimedServos_h
#define TimedServos_h

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "Wire.h"
#include "Adafruit_PWMServoDriver.h"

#define PWM_MAX_TRAVEL_PER_MILLI .5

class TimedServos {

    typedef struct
    {
      uint8_t startPos = 0;
      uint8_t endPos = 0;
      uint8_t currPos = 0;
      uint16_t timeAllotted = 0;
      unsigned long millisAtCommand = 0;
      uint16_t srvMin;
      uint16_t srvMax;
      boolean isInversed = false;
      boolean isDisabled = false;
    }  TimedServo;

    typedef struct
    {
      TimedServo channels[16];
      Adafruit_PWMServoDriver pwm;
    } PWMBoard;

  private:
    TimedServos();
    TimedServos(TimedServos const&); // copy disabled
    void operator=(TimedServos const&); // assigment disabled
    void setServoPulse(Adafruit_PWMServoDriver pwm, uint8_t srvNum, uint8_t srvPos, uint16_t srvMin, uint16_t srvMax);
    void disableChannel(Adafruit_PWMServoDriver pwm, uint8_t srvNum);

  public:
    PWMBoard servoBoards[2];
    static TimedServos* getInstance();

    /**
     * Sets the targeted servo position and the amount of time alloted to reach the position.
     */
    void setServoPosition(uint8_t board, uint8_t channel, uint8_t srvPos, uint16_t timeAlloted);

    /**
     * Configures the TimedServo classes, boards and thier assignments.  This method must be called
     * once before any servo movement is attempted.
     */
    void setup();

     /**
     * This method needs to be called in a loop and will iterate through any sets of movements that are currently
     * in action or disable a servo once it has reached it position for the defined period of time.
     */
    void loop();
};

#endif // TimedServos.h
