#include <ArduinoLog.h>
#include "Sounds.h"
#include "SoundConfig.h"

Sounds::Sounds() {}

Sounds* Sounds::getInstance() {
  static Sounds sound;
  return &sound;
}

void Sounds::setup(Stream* stream) {
  // WAV Trigger startup
  wTrig.setup(&Serial3);
  wTrig.stopAllTracks();
  printWTrigInfo();
  setVol(vol);
}

void Sounds::setVol(int vol) {
  Log.notice(F("Setting volume: %d"CR), vol);
  wTrig.masterGain(vol);
}

void Sounds::play(int track) {
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

void Sounds::printWTrigInfo() {
  wTrig.getVersion();
  uint8_t* sysVersion;
  sysVersion = wTrig.returnSysVersion();
  Log.notice(F("Sys Version: %x"CR), sysVersion);
  wTrig.getSysInfo();
  Log.notice(F(" -- Number of tracks: %d"CR), wTrig.returnSysinfoTracks());
  Log.notice(F(" -- Number of voices: %d"CR), wTrig.returnSysinfoVoices());
}

void Sounds::volDown() {
  if (vol > DEFAULT_VOLUME_MIN) {
    if (vol < DEFAULT_VOLUME_MIN - 3) {
      vol--;
    } else {
      vol -= 2;
    }
    setVol(vol);
  }
}

void Sounds::volUp() {
  if (vol < DEFAULT_VOLUME_MAX) {
    if (vol > DEFAULT_VOLUME_MAX - 3) {
      vol++;
    } else {
      vol += 2;
    }
    setVol(vol);
  }
}

