#ifndef PADAWAN_FX_CONFIG_H_
#define PADAWAN_FX_CONFIG_H_

// Set the baude rate for the Syren motor controller
// for packetized options are: 2400, 9600, 19200 and 38400. I think you need to pick one that works
// and I think it varies across different firmware versions.
// for simple serial use 9600
const int DOMEBAUDRATE = 9600;

// 9600 is the default baud rate for Sabertooth packet serial.
const int STBAUDRATE = 9600;

// Define the neutral zones for each of the analog sticks
const int LEFT_HAT_X_NEUTRAL = 7500;
const int LEFT_HAT_Y_NEUTRAL = 7500;

const int RIGHT_HAT_X_NEUTRAL = 7500;
const int RIGHT_HAT_Y_NEUTRAL = 7500;

//************************** Set speed and turn speeds here************************************//
//set these 3 to whatever speeds work for you. 0-stop, 127-full speed.
const byte DRIVESPEED1 = 50;
//Recommend beginner: 50 to 75, experienced: 100 to 127, I like 100.
const byte DRIVESPEED2 = 100;
//Set to 0 if you only want 2 speeds.
const byte DRIVESPEED3 = 127;

byte drivespeed = DRIVESPEED1;

// the higher this number the faster the droid will spin in place, lower - easier to control.
// Recommend beginner: 40 to 50, experienced: 50 $ up, I like 70
const byte TURNSPEED = 70;

// If using a speed controller for the dome, sets the top speed. You'll want to vary it potenitally
// depending on your motor. My Pittman is really fast so I dial this down a ways from top speed.
// Use a number up to 127 for serial
const byte DOMESPEED = 110;

// Ramping- the lower this number the longer R2 will take to speedup or slow down,
// change this by incriments of 1
const byte RAMPING = 4;

// Compensation is for deadband/deadzone checking. There's a little play in the neutral zone
// which gets a reading of a value of something other than 0 when you're not moving the stick.
// It may vary a bit across controllers and how broken in they are, sometimex 360 controllers
// develop a little bit of play in the stick at the center position. You can do this with the
// direct method calls against the Syren/Sabertooth library itself but it's not supported in all
// serial modes so just manage and check it in software here
// use the lowest number with no drift
// DOMEDEADZONERANGE for the left stick, DRIVEDEADZONERANGE for the left stick
const byte DOMEDEADZONERANGE = 10;
const byte DRIVEDEADZONERANGE = 10;

//************************* Automation Settings *****************************//
#define AUTO_TIME_MIN 5
#define AUTO_TIME_MAX 20

#endif //PADAWAN_FX_CONFIG_H_
