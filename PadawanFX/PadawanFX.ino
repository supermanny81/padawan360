

// =======================================================================================
// /////////////////////////Padawan360 Body Code v1.0 ////////////////////////////////////
// =======================================================================================
/*
by Dan Kraus
dskraus@gmail.com
Astromech: danomite4047

Heavily influenced by DanF's Padwan code which was built for Arduino+Wireless PS2
controller leveraging Bill Porter's PS2X Library. I was running into frequent disconnect
issues with 4 different controllers working in various capacities or not at all. I decided
that PS2 Controllers were going to be more difficult to come by every day, so I explored
some existing libraries out there to leverage and came across the USB Host Shield and it's
support for PS3 and Xbox 360 controllers. Bluetooth dongles were inconsistent as well
so I wanted to be able to have something with parts that other builder's could easily track
down and buy parts even at your local big box store.

Hardware:
Arduino UNO
USB Host Shield from circuits@home
Microsoft Xbox 360 Controller
Xbox 360 USB Wireless Reciver
Sabertooth Motor Controller
Syren10en Motor Controller
Sparkfun MP3 Trigger

This sketch supports I2C and calls events on many sound effect actions to control lights and sounds.
It is NOT set up for Dan's method of using the serial packet to transfer data up to the dome
to trigger some light effects. If you want that, you'll need to reference DanF's original
Padawan code.

Set Sabertooth 2x25/2x12 Dip Switches 1 and 2 Down, All Others Up
For Syren10en Simple Serial Set Switches 1 and 2 Down, All Others Up
For Syren10en Simple Serial Set Switchs 2 & 4 Down, All Others Up
Placed a 10K ohm resistor between S1 & GND on the Syren10en 10 itself

*/


#include <SoftwareSerial.h>
#include <SPI.h>
#include <Sabertooth.h>
#include <SyRenSimplified.h>
#include <Servo.h>
#include <Wire.h>
#include <XBOXRECV.h>
#include "Utility.h"
#include "PadawanFXConfig.h"
#include <AltSoftSerial.h>    // Arduino build environment requires this
#include <wavTrigger.h>


// These are the pins for the Sabertooth and Syren10en
SoftwareSerial STSerial(NOT_A_PIN, 4);
SoftwareSerial Syren10Serial(NOT_A_PIN, 5);
SoftwareSerial trigSerial = SoftwareSerial(NOT_A_PIN, 7);

Sabertooth Sabertooth2xXX(128, STSerial);
Sabertooth Syren10(128, Syren10Serial);

byte vol = DEFAULT_VOLUME;
// 0 = drive motors off ( right stick disabled ) at start
boolean isDriveEnabled = false;

// Automated function variables
// Used as a boolean to turn on/off automated functions like periodic random sounds and periodic dome turns
boolean isInAutomationMode = false;
unsigned long automateMillis = 0;
byte automateDelay = random(5, 20); // set this to min and max seconds between sounds

//How much the dome may turn during automation.
int turnDirection = 20;

// Action number used to randomly choose a sound effect or a dome turn
byte automateAction = 0;
char driveThrottle = 0;
char sticknum = 0;
char domeThrottle = 0;
char turnThrottle = 0;
long xboxBtnPressedSince = 0;
boolean firstLoadOnConnect = false;

MP3TriggerSS mp3Trigger;
USB Usb;
XBOXRECV Xbox(&Usb);

void setup() {
  Serial.begin(38400);
  // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
  while (!Serial);
  Serial.println(F("PadawanFX"));

  if (Usb.Init() == -1) {
    Serial.println(F("\r\nOSC did not start"));
    while (1); //halt
  }

  Syren10Serial.begin(DOMEBAUDRATE);
  Syren10.autobaud();
  Syren10.setTimeout(950);

  STSerial.begin(STBAUDRATE);
  // Send the autobaud command to the Sabertooth controller(s).
  // Sabertooth2xXX.autobaud();
  Sabertooth2xXX.setTimeout(950);

  // The Sabertooth won't act on mixed mode packet serial commands until
  // it has received power levels for BOTH throttle and turning, since it
  // mixes the two together to get diff-drive power levels for both motors.
  Sabertooth2xXX.drive(0);
  Sabertooth2xXX.turn(0);

  mp3Trigger.setup(&trigSerial);
  trigSerial.begin(38400);
  mp3Trigger.setVolume(vol);
}

void loop() {
  Usb.Task();

  //if we're not connected, return so we don't bother doing anything else.
  // set all movement to 0 so if we lose connection we don't have a runaway droid!
  // a restraining bolt and jawa droid caller won't save us here!
  if (!Xbox.XboxReceiverConnected || !Xbox.Xbox360Connected[0]) {
    Sabertooth2xXX.drive(0);
    Sabertooth2xXX.turn(0);
    Syren10.motor(1, 0);
    firstLoadOnConnect = false;
    xboxBtnPressedSince = 0;
    return;
  }

  // After the controller connects, Blink all the LEDs so we know drives are disengaged at start
  if (!firstLoadOnConnect) {
    firstLoadOnConnect = true;
    mp3Trigger.play(CONTROLLER_CONNECTED);
    Xbox.setLedMode(ROTATING, 0);
  }

  // enable / disable right stick (droid movement) & play a sound to signal motor state
  if (Xbox.getButtonClick(START, 0)) {
    if (isDriveEnabled) {
      isDriveEnabled = false;
      Xbox.setLedMode(ROTATING, 0);
      mp3Trigger.play(random(HUM_SND_START, HUM_SND_END));
    } else {
      isDriveEnabled = true;
      mp3Trigger.play(PROC_SND_START);
      // //When the drive is enabled, set our LED accordingly to indicate speed
      if (drivespeed == DRIVESPEED1) {
        Xbox.setLedOn(LED1, 0);
      } else if (drivespeed == DRIVESPEED2 && (DRIVESPEED3 != 0)) {
        Xbox.setLedOn(LED2, 0);
      } else {
        Xbox.setLedOn(LED3, 0);
      }
    }
  }

  //Toggle automation mode with the BACK button
  if (Xbox.getButtonClick(BACK, 0)) {
    if (isInAutomationMode) {
      isInAutomationMode = false;
      automateAction = 0;
      mp3Trigger.play(PROC_SND_START);
    } else {
      isInAutomationMode = true;
      mp3Trigger.play(random(PROC_SND_START + 1, PROC_SND_END));
    }
  }

  // Plays random sounds or dome movements for automations when in automation mode
  if (isInAutomationMode) {
    unsigned long currentMillis = millis();

    if (currentMillis - automateMillis > (automateDelay * 1000)) {
      automateMillis = millis();
      automateAction = random(1, 5);

      if (automateAction > 1) {
        mp3Trigger.play(random(AUTO_SND_START, AUTO_SND_END));
      }
      if (automateAction < 4) {
        Syren10.motor(1, turnDirection);
        delay(750);
        Syren10.motor(1, 0);
        if (turnDirection > 0) {
          turnDirection = -45;
        } else {
          turnDirection = 45;
        }
      }
      // sets the mix, max seconds between automation actions - sounds and dome movement
      automateDelay = random(AUTO_TIME_MIN, AUTO_TIME_MAX);
    }
  }

  // Volume Control of MP3 Trigger
  // Hold R1 and Press Up/down on D-pad to increase/decrease volume
  if (Xbox.getButtonClick(UP, 0)) {
    // volume up
    if (Xbox.getButtonPress(R1, 0)) {
      if (vol > 0) {
        if (vol < 3) {
          vol--;
        } else {
          vol-=3;
        }
        mp3Trigger.setVolume(vol);
      }
    }
  }
  if (Xbox.getButtonClick(DOWN, 0)) {
    //volume down
    if (Xbox.getButtonPress(R1, 0)) {
      if (vol < 75) {
        vol+=3;
        mp3Trigger.setVolume(vol);
      }
    }
  }

  // Logic display brightness.
  // Hold L1 and press up/down on dpad to increase/decrease brightness
//  if (Xbox.getButtonClick(UP, 0)) {
//    if (Xbox.getButtonPress(L1, 0)) {
//    }
//  }
//  if (Xbox.getButtonClick(DOWN, 0)) {
//    if (Xbox.getButtonPress(L1, 0)) {
//    }
//  }

  // Y Button and Y combo buttons
  if (Xbox.getButtonClick(Y, 0)) {
    if (Xbox.getButtonPress(L1, 0)) {
      mp3Trigger.play(random(LEIA_SND_START, LEIA_SND_END));
    } else if (Xbox.getButtonPress(L2, 0)) {
      mp3Trigger.play(random(SCREAM_SND_START, SCREAM_SND_END));
    } else if (Xbox.getButtonPress(R1, 0)) {
      mp3Trigger.play(SW_SND_THEME);
    } else if (Xbox.getButtonPress(R2, 0)) {
      mp3Trigger.play(PATROL_SND);
    } else {
      mp3Trigger.play(random(HUM_SND_START, HUM_SND_END));
    }
  }

  // X Button and X combo Buttons
  if (Xbox.getButtonClick(X, 0)) {
    if (Xbox.getButtonPress(L1, 0)) {
      mp3Trigger.play(random(CHAT_SND_START, CHAT_SND_END));
    } else if (Xbox.getButtonPress(L2, 0)) {
      mp3Trigger.play(random(WHISTLE_SND_START, WHISTLE_SND_END));
    } else if (Xbox.getButtonPress(R1, 0)) {
      mp3Trigger.play(EMPIRE_SND_THEME);
    } else {
      mp3Trigger.play(random(GEN_SND_START, GEN_SND_END));
    }
  }

  // A Button and A combo Buttons
  if (Xbox.getButtonClick(A, 0)) {
    if (Xbox.getButtonPress(L1, 0)) {
      mp3Trigger.play(DOODOO_SND);
    } else if (Xbox.getButtonPress(L2, 0)) {
      mp3Trigger.play(OVERHERE_SND);
    } else if (Xbox.getButtonPress(R1, 0)) {
      mp3Trigger.play(CANTINA_SND_THEME);
    } else if (Xbox.getButtonPress(R2, 0)) {
      mp3Trigger.play(random(R2THEME_MUS_START, R2THEME_MUS_END));
    } else {
      mp3Trigger.play(random(HAPPY_SND_START, HAPPY_SND_END));
    }
  }

  // B Button and B combo Buttons
  if (Xbox.getButtonClick(B, 0)) {
    if (Xbox.getButtonPress(L1, 0)) {
      mp3Trigger.play(random(SAD_SND_START, SAD_SND_END));
    } else if (Xbox.getButtonPress(L2, 0)) {
      mp3Trigger.play(random(RANDOM_MUS_START, RANDOM_MUS_END));
    } else if (Xbox.getButtonPress(R1, 0)) {
      mp3Trigger.play(SW_CHORUS_THEME);
    } else if (Xbox.getButtonPress(R2, 0)) {
      mp3Trigger.play(ANNOYED_SND);
    } else {
      mp3Trigger.play(random(PROC_SND_START, PROC_SND_END));
    }
  }

  // turn hp light on & off with Left Analog Stick Press (L3)
  if (Xbox.getButtonClick(L3, 0))  {
  }


  // Change drivespeed if drive is eanbled
  // Press Right Analog Stick (R3)
  // Set LEDs for speed - 1 LED, Low. 2 LED - Med. 3 LED High
  if (Xbox.getButtonClick(R3, 0) && isDriveEnabled) {
    //if in lowest speed
    if (drivespeed == DRIVESPEED1) {
      //change to medium speed and play sound 3-tone
      drivespeed = DRIVESPEED2;
      Xbox.setLedOn(LED2, 0);
    } else if (drivespeed == DRIVESPEED2 && (DRIVESPEED3 != 0)) {
      //change to high speed and play sound scream
      drivespeed = DRIVESPEED3;
      Xbox.setLedOn(LED3, 0);
    } else {
      //we must be in high speed
      //change to low speed and play sound 2-tone
      drivespeed = DRIVESPEED1;
      Xbox.setLedOn(LED1, 0);
    }
    mp3Trigger.play(PROC_SND_START);
  }


  // FOOT DRIVES
  // Xbox 360 analog stick values are signed 16 bit integer value
  // Sabertooth runs at 8 bit signed. -127 to 127 for speed (full speed reverse and  full speed forward)
  // Map the 360 stick values to our min/max current drive speed
  if (Xbox.getAnalogHat(RightHatY, 0) > RIGHT_HAT_Y_NEUTRAL || Xbox.getAnalogHat(RightHatY, 0) < -RIGHT_HAT_Y_NEUTRAL) {
    sticknum = (map(Xbox.getAnalogHat(RightHatY, 0), -32768, 32767, -drivespeed, drivespeed));
    if (sticknum > -DRIVEDEADZONERANGE && sticknum < DRIVEDEADZONERANGE) {
      // stick is in dead zone - don't drive
      driveThrottle = 0;
    } else {
      if (driveThrottle < sticknum) {
        if (sticknum - driveThrottle < (RAMPING + 1) ) {
          driveThrottle += RAMPING;
        } else {
          driveThrottle = sticknum;
        }
      } else if (driveThrottle > sticknum) {
        if (driveThrottle - sticknum < (RAMPING + 1) ) {
          driveThrottle -= RAMPING;
        } else {
          driveThrottle = sticknum;
        }
      }
    }
  } else {
    driveThrottle = 0;
  }

  if (Xbox.getAnalogHat(RightHatX, 0) > RIGHT_HAT_X_NEUTRAL || Xbox.getAnalogHat(RightHatX, 0) < -RIGHT_HAT_X_NEUTRAL) {
    turnThrottle = map(Xbox.getAnalogHat(RightHatX, 0), -32768, 32767, -TURNSPEED, TURNSPEED);
  } else {
    turnThrottle = 0;
  }

  // DRIVE!
  // right stick (drive)
  if (isDriveEnabled) {
    // Only do deadzone check for turning here. Our Drive throttle speed has some math applied
    // for RAMPING and stuff, so just keep it separate here
    if (turnThrottle > -DRIVEDEADZONERANGE && turnThrottle < DRIVEDEADZONERANGE) {
      // stick is in dead zone - don't turn
      turnThrottle = 0;
    }
    Sabertooth2xXX.turn(turnThrottle);
    Sabertooth2xXX.drive(driveThrottle);
  }

  // DOME DRIVE!
  if (Xbox.getAnalogHat(LeftHatX, 0) > LEFT_HAT_X_NEUTRAL || Xbox.getAnalogHat(LeftHatX, 0) < -LEFT_HAT_X_NEUTRAL) {
    domeThrottle = (map(Xbox.getAnalogHat(LeftHatX, 0), -32768, 32767, -DOMESPEED, DOMESPEED));
    if (domeThrottle > -DOMEDEADZONERANGE && domeThrottle < DOMEDEADZONERANGE) {
      //stick in dead zone - don't spin dome
      domeThrottle = 0;
    }
  } else {
    domeThrottle = 0;
  }
  Syren10.motor(1, domeThrottle);


  proccessControllerShutdown();
  //printControllerStatus();
  //printThrottle();
  //countCycles();
  mp3Trigger.update();
} // END loop()


/**
 * Determines if the controller needs to be shutdown.  The disconnect signal is sent once the XBOX 
 * button has been pressed for more than 3s, a rumble will indicate the controller is being shutdown.
 */
void proccessControllerShutdown() {
  if (Xbox.getButtonPress(XBOX, 0)) {
    if (xboxBtnPressedSince == 0) {
      xboxBtnPressedSince = millis();
    } else if (millis() - xboxBtnPressedSince > 3000) {
      Serial.print(F("Shutting down controller.  Elapsed time: "));
      Serial.println(xboxBtnPressedSince);
      Xbox.setRumbleOn(50, 127, 0);
      xboxBtnPressedSince = 0;
      delay(500);
      Xbox.disconnect(0);
    }  
  } else {
    xboxBtnPressedSince = 0;
  }
}

void printThrottle() {
  Serial.print(F("TURN: "));
  Serial.print(-turnThrottle, DEC);
  Serial.print(F(", DRIVE THROTTLE: "));
  Serial.print(driveThrottle, DEC);
  Serial.print(F(" DOME THROTTLE: "));
  Serial.println(domeThrottle, DEC);
}

void printControllerStatus() {
  Serial.print(F("LEFT: (X="));
  Serial.print(Xbox.getAnalogHat(LeftHatX, 0));
  Serial.print(F(", Y="));
  Serial.print(Xbox.getAnalogHat(LeftHatY, 0));
  Serial.print(F("), RIGHT: (X="));
  Serial.print(Xbox.getAnalogHat(RightHatX, 0));
  Serial.print(F(", Y="));
  Serial.print(Xbox.getAnalogHat(RightHatY, 0));
  Serial.println(F(")"));
}

