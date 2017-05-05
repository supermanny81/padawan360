/**
                  ~~~
                 \_0_/
                  | | ___---___
                ____  ____  _______  __
               |  _ \|___ \|  ___\ \/ /
             / | |_) | __) | |_   \  / \
           /   |  _ < / __/|  _|  /  \   \
          |    |_| \_\_____|_|   /_/\_\   |
     ___  |-------------------------------|  ___
     |  |_|    drive and control system   |_|  | 
     |          =====================          |

  Copyright (c) 2015 Manny Garcia, written for the R2Builders Group

  Based on the execellent Padawan360 library by Dan Kraus, which was influenced by DanF's 
  original Padawan control system. 
  
  Hardware:
  Arduino Mega
  USB Host Shield from circuits@home or SainSmart
  Microsoft Xbox 360 Controller
  Xbox 360 USB Wireless Reciver
  Sabertooth Motor Controller
  Syren10 Motor Controller
  Sparkfun WAV Trigger

  Set Sabertooth 2x25/2x12 Dip Switches 1 and 2 Down, All Others Up (9600 baud)

    Mega         ST
    ====         ===========
    GND <------>  0v
    Tx1 <------>  S2
    Rx1 <------>  S1

  For Syren10en Simple Serial Set Switchs 2 & 4 Down, All Others Up (9600 baud)

    Mega         Syren
    ====         ===========
    GND <------>  0v
    Tx2 <------>  S2
    Rx2 <------>  S1

  Connect 2 wires from the UNO to the WAV Trigger's serial connector:

    Mega         WAV Trigger
    ====         ===========
    GND <------> GND
    Tx3 <------> RX
    Rx3 <------> TX (Required)

    Power the WAV trigger separately.
    
*/
#include <SPI.h>
#include <Sabertooth.h>
#include <SyRenSimplified.h>
#include <Wire.h>
#include <XBOXRECV.h>
#include <ArduinoLog.h>
#include "Sounds.h"
#include "PadawanFXConfig.h"
#include "Utility.h"
#include "WavTrigger2.h"

Sabertooth Sabertooth2xXX(128, Serial1);
Sabertooth Syren10(128, Serial2);
WavTrigger2 wTrig;

char vol = DEFAULT_VOLUME;

// 0 = drive motors off ( right stick disabled ) at start
boolean isDriveEnabled = false;

// Automated function variables
// Used as a boolean to turn on/off automated functions like periodic random sounds and periodic dome turns
boolean isInAutomationMode = false;
boolean isBgMusicPlaying = false;
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
boolean periscopeUp = false;
boolean periscopeRandomFast = false; //5, then 4
boolean periscopeSearchLightCCW = false; // send 7, then 3

USB Usb;
XBOXRECV Xbox(&Usb);

void setup() {
  Serial.begin(115200);
  // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
  while (!Serial);
  Wire.begin();
  // Initialize with log level and log output. 
  Log.begin(LOG_LEVEL_VERBOSE, &Serial, true);
  Log.verbose(F("PadawanFX"));

  if (Usb.Init() == -1) {
    Log.fatal(F("\r\nOSC did not start"));
    while (1); //halt
  }

  Serial2.begin(DOMEBAUDRATE);
  Syren10.setTimeout(900);

  Serial1.begin(STBAUDRATE);
  Sabertooth2xXX.setTimeout(900);

  // The Sabertooth won't act on mixed mode packet serial commands until
  // it has received power levels for BOTH throttle and turning, since it
  // mixes the two together to get diff-drive power levels for both motors.
  Sabertooth2xXX.drive(0);
  Sabertooth2xXX.turn(0);

  // WAV Trigger startup
  Serial3.begin(WAVBAUDRATE);
  wTrig.setup(&Serial3);
  wTrig.stopAllTracks();
  print_wav_info();
  set_volume(vol);
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
    isDriveEnabled = false;
    play_sound_track(CONTROLLER_CONNECTED);
    Xbox.setLedMode(ROTATING, 0);
  }

  // enable / disable right stick (droid movement) & play a sound to signal motor state
  if (Xbox.getButtonClick(START, 0)) {
    if (isDriveEnabled) {
      isDriveEnabled = false;
      Xbox.setLedMode(ROTATING, 0);
      play_sound_track(random(HUM_SND_START, HUM_SND_END));
    } else {
      isDriveEnabled = true;
      play_sound_track(PROC_SND_START);
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
      play_sound_track(PROC_SND_START);
    } else {
      isInAutomationMode = true;
      play_sound_track(random(PROC_SND_START + 1, PROC_SND_END));
    }
  }

  // UP on control pad
  if (Xbox.getButtonClick(UP, 0)) {
    // volume up
    if (Xbox.getButtonPress(R1, 0)) {
      if (vol < DEFAULT_VOLUME_MAX) {
        if (vol > DEFAULT_VOLUME_MAX - 3) {
          vol++;
        } else {
          vol += 2;
        }
        set_volume(vol);
      }
    } else if (Xbox.getButtonPress(L1, 0)) {
      send_periscope_command(6);
    }
  }

  // DOWN on control pad
  if (Xbox.getButtonClick(DOWN, 0)) {
    //volume down
    if (Xbox.getButtonPress(R1, 0)) {
      if (vol > DEFAULT_VOLUME_MIN) {
        if (vol < DEFAULT_VOLUME_MIN - 3) {
          vol--;
        } else {
          vol -= 2;
        }
        set_volume(vol);
      }
    } else if (Xbox.getButtonPress(L1, 0)) {
      if (periscopeUp) {
        // periscope down
        send_periscope_command(1);
        periscopeSearchLightCCW = false;
        periscopeRandomFast =  false;
      } else {
        // periscope up/down
        send_periscope_command(2);
      }
      periscopeUp = !periscopeUp;
    }
  }

  // LEFT on control pad
  if (Xbox.getButtonClick(LEFT, 0)) {
    if (Xbox.getButtonPress(R1, 0)) {

    } else if (Xbox.getButtonPress(L1, 0)) {
      if (periscopeRandomFast) {
        // periscope up/down
        send_periscope_command(4);
      } else {
        // periscope up/down
        send_periscope_command(5);
      }
      periscopeRandomFast = !periscopeRandomFast;
    }
  }

  // RIGHT on control pad
  if (Xbox.getButtonClick(RIGHT, 0)) {
    //volume down
    if (Xbox.getButtonPress(R1, 0)) {

    } else if (Xbox.getButtonPress(L1, 0)) {
      if (periscopeSearchLightCCW) {
        // periscope up/down
        send_periscope_command(3);
      } else {
        // periscope up/down
        send_periscope_command(7);
      }
      periscopeSearchLightCCW = !periscopeSearchLightCCW;
    }
  }
  
  // Y Button and Y combo buttons
  if (Xbox.getButtonClick(Y, 0)) {
    if (Xbox.getButtonPress(L1, 0)) {
      play_sound_track(random(LEIA_SND_START, LEIA_SND_END));
    } else if (Xbox.getButtonPress(L2, 0)) {
      play_sound_track(random(SCREAM_SND_START, SCREAM_SND_END));
    } else if (Xbox.getButtonPress(R1, 0)) {
      play_sound_track(SW_SND_THEME);
    } else if (Xbox.getButtonPress(R2, 0)) {
      play_sound_track(PATROL_SND);
    } else {
      play_sound_track(random(HUM_SND_START, HUM_SND_END));
    }
  }

  // X Button and X combo Buttons
  if (Xbox.getButtonClick(X, 0)) {
    if (Xbox.getButtonPress(L1, 0)) {
      play_sound_track(random(CHAT_SND_START, CHAT_SND_END));
    } else if (Xbox.getButtonPress(L2, 0)) {
      play_sound_track(random(WHISTLE_SND_START, WHISTLE_SND_END));
    } else if (Xbox.getButtonPress(R1, 0)) {
      play_sound_track(EMPIRE_SND_THEME);
    } else if (Xbox.getButtonPress(R2, 0)) {
      play_sound_track(random(HOLIDAY_MUS_START, HOLIDAY_MUS_END));
    } else {
      play_sound_track(random(GEN_SND_START, GEN_SND_END));
    }
  }

  // A Button and A combo Buttons
  if (Xbox.getButtonClick(A, 0)) {
    if (Xbox.getButtonPress(L1, 0)) {
      play_sound_track(DOODOO_SND);
    } else if (Xbox.getButtonPress(L2, 0)) {
      play_sound_track(OVERHERE_SND);
    } else if (Xbox.getButtonPress(R1, 0)) {
      play_sound_track(CANTINA_SND_THEME);
    } else if (Xbox.getButtonPress(R2, 0)) {
      play_sound_track(random(R2THEME_MUS_START, R2THEME_MUS_END));
    } else {
      play_sound_track(random(HAPPY_SND_START, HAPPY_SND_END));
    }
  }

  // B Button and B combo Buttons
  if (Xbox.getButtonClick(B, 0)) {
    if (Xbox.getButtonPress(L1, 0)) {
      play_sound_track(random(SAD_SND_START, SAD_SND_END));
    } else if (Xbox.getButtonPress(L2, 0)) {
      play_sound_track(random(RANDOM_MUS_START, RANDOM_MUS_END));
    } else if (Xbox.getButtonPress(R1, 0)) {
      play_sound_track(SW_CHORUS_THEME);
    } else if (Xbox.getButtonPress(R2, 0)) {
      play_sound_track(ANNOYED_SND);
    } else {
      play_sound_track(random(PROC_SND_START, PROC_SND_END));
    }
  }

  // get battery levels
  if (Xbox.getButtonClick(SYNC, 0)) {
    Log.notice(F("Xbox Battery Level: %d"CR), Xbox.getBatteryLevel(0));
  }

  // MOVE OUT THE WAY
  if (Xbox.getButtonClick(L3, 0))  {
    play_sound_track(IMPERIAL_SIREN);
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
    play_sound_track(PROC_SND_START);
  }

  drive();
  is_disconnect();
  automation_mode();
}

void drive() {
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
}

/**
   Determines if the controller needs to be shutdown.  The disconnect signal is sent once the XBOX
   button has been pressed for more than 3s, a rumble will indicate the controller is being shutdown.
*/
void is_disconnect() {
  if (Xbox.getButtonPress(XBOX, 0)) {
    if (xboxBtnPressedSince == 0) {
      xboxBtnPressedSince = millis();
    } else if (millis() - xboxBtnPressedSince > 3000 && millis() - xboxBtnPressedSince < 4000) {
      Xbox.setRumbleOn(50, 127, 0);
      xboxBtnPressedSince = 0;
      Log.warning(F("Shutting down controller.  Elapsed time: %d"CR), xboxBtnPressedSince);
    } else if (millis() - xboxBtnPressedSince > 4000) {
      Xbox.disconnect(0);
    }
  } else {
    xboxBtnPressedSince = 0;
  }
}

void automation_mode() {
    // Plays random sounds or dome movements for automations when in automation mode
  if (isInAutomationMode) {
    unsigned long currentMillis = millis();
    if (currentMillis - automateMillis > (automateDelay * 1000)) {
      automateMillis = millis();
      automateAction = random(1, 5);
      if (automateAction > 1) {
        play_sound_track(random(AUTO_SND_START, AUTO_SND_END));
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
}

void print_wav_info() {
  wTrig.getVersion();
  uint8_t* sysVersion;
  sysVersion = wTrig.returnSysVersion();
  Log.notice(F("Sys Version: %x"CR), sysVersion);
  wTrig.getSysInfo();
  Log.notice(F(" -- Number of tracks: %d"CR), wTrig.returnSysinfoTracks());
  Log.notice(F(" -- Number of voices: %d"CR), wTrig.returnSysinfoVoices());
}

void play_sound_track(int track) {
  wTrig.getStatus();
  uint16_t* tracks = wTrig.returnTracksPlaying();

  if (track > BG_MUS_START) {
    isBgMusicPlaying = !isBgMusicPlaying;
  }

  for (byte i = 0; i < 14; i++) {
    Log.trace(F("Background tracks [%d] : %d"CR), i, tracks[i]);
    if (tracks[i] <= BG_MUS_START) {
      wTrig.trackStop(tracks[i]);
    } else if (tracks[i] > BG_MUS_START && track > BG_MUS_START && isBgMusicPlaying == false) {
      wTrig.trackStop(tracks[i]);
    }
  }

  Log.notice(F("Playing track: %d"CR), track);
  if (track > BG_MUS_START && isBgMusicPlaying == false) {
    return;
  }
  wTrig.trackPlayPoly(track);
}

void set_volume(int vol) {
  Log.notice(F("Setting volume: %d"CR), vol);
  wTrig.masterGain(vol);
}

void send_periscope_command(byte cmd) {
  // 0: DO NOTHING - ALLOW I2C TO TAKE CONTROL
  // 1: DOWN POSITION - ALL OFF
  // 2: FAST UP - RANDOM LIGHTS
  // 3: SEARCHLIGHT - CCW
  // 4: RANDOM - FAST
  // 5: RANDOM - SLOW
  // 6: DAGOBAH - WHITE LIGHTS - FACE FORWARD
  // 7: SEARCHLIGHT CW
  Wire.beginTransmission(0x20); // transmit to device #20
  Wire.write(cmd); // sends one byte 0011
  Wire.endTransmission(); // stop transmitting
  Log.notice(F("Sent command: %d"CR), cmd);
}
