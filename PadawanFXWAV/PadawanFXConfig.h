#ifndef PADAWAN_FX_CONFIG
#define PADAWAN_FX_CONFIG

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
const byte DOMESPEED = 100;

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

//************************* Sound Settings *****************************//

// 0 = full volume, 255 off (after 64 inaudible) <== mp3Triger
// 10 = max, -70 min
#define DEFAULT_VOLUME_MIN -70
#define DEFAULT_VOLUME_MAX 10
#define DEFAULT_VOLUME -20

#define CONTROLLER_CONNECTED 1

#define GEN_SND_START 1
#define GEN_SND_END 24

#define CHAT_SND_START 26
#define CHAT_SND_END 43

#define HAPPY_SND_START 51
#define HAPPY_SND_END 58

#define SAD_SND_START 75
#define SAD_SND_END 79

#define HUM_SND_START 130
#define HUM_SND_END 155

#define LEIA_SND_START 161
#define LEIA_SND_END 164

#define SCREAM_SND_START 126
#define SCREAM_SND_END 128

#define PROC_SND_START 240
#define PROC_SND_END 244

#define WHISTLE_SND_START 101
#define WHISTLE_SND_END 103

#define AUTO_SND_START 1
#define AUTO_SND_END 50

#define SW_SND_THEME 190
#define EMPIRE_SND_THEME 192
#define CANTINA_SND_THEME 191
#define SW_CHORUS_THEME 193

#define MUS_THRILLER 203
#define RANDOM_MUS_START 179
#define RANDOM_MUS_END 186

#define R2THEME_MUS_START 176
#define R2THEME_MUS_END 178

#define DOODOO_SND 49
#define OVERHERE_SND 50
#define PATROL_SND 48
#define ANNOYED_SND 75

#endif


/**
Y = Hum
Y + R1 = SW THEME
Y + R2 = Patrol
Y + L1 = Leia Theme 
Y + L2 = Scream

X = General
X + R1 = Imperial March
X + L1 = Chat
X + L2 = Whistle

A = Happy
A + R1 = Cantina
A + R2 = R2 SW Themes
A + L1 = DooDoo
A + L2 = Over here

B = Processing
B + R1 = Chorus Theme
B + R2 = Annoyed
B + L1 = Sad
B + L2 = Random Music
**/

