/*
MP3Trigger.cpp
@author David Wicks
@url	sansumbrella.com
*/

#include "MP3TriggerSS.h"

MP3TriggerSS::MP3TriggerSS()
{
  mDoLoop = false;
  mPlaying = false;
}

MP3TriggerSS::~MP3TriggerSS()
{
  s->flush();
  s = NULL;
}

void MP3TriggerSS::setup(Stream* serial)
{
  s = serial;
}

//
// Looping functions
//
void MP3TriggerSS::setLooping(bool doLoop, byte track)
{
  mDoLoop = doLoop;
  mLoopTrack = track;

  if (!mPlaying && mDoLoop)
  {
    loop();
  }
}

void MP3TriggerSS::setLoopingTrack(byte track)
{
  mLoopTrack = track;
}

void MP3TriggerSS::update()
{
  while ( s->available() > 1)
  {
    byte data = s->read();
    Serial.print(data);
    if (char(data) == 'X' || char(data) == 'x')
    {
      if (mDoLoop)
      {
        loop();
      } else
      {
        mPlaying = false;
      }
    } else if (char(data) == 'E')
    {
      mPlaying = false;
    }
  }
}

void MP3TriggerSS::loop()
{
  trigger(mLoopTrack);
}

void MP3TriggerSS::stop()
{
  bool wasPlaying = mPlaying;
  mDoLoop = false;
  mPlaying = false;

  if (wasPlaying)
  {
    play();
  }
}

//
// Single-byte built-in functions
//

void MP3TriggerSS::play()
{
  s->write('O');
  mPlaying = !mPlaying;
}

void MP3TriggerSS::forward()
{
  s->write('F');
}

void MP3TriggerSS::reverse()
{
  s->write('R');
}

//
// Built-in two-byte functions
//

void MP3TriggerSS::trigger(byte track)
{
  s->write('t');
  s->write(track);
  mPlaying = true;
}

void MP3TriggerSS::play(byte track)
{
  Serial.print(F("\np"));
  Serial.print(track, DEC);
  Serial.println();
  s->write('p');
  s->write(track);
  mPlaying = true;
}


void MP3TriggerSS::play(int track)
{
  play((byte)track);
}

void MP3TriggerSS::play(long track)
{
  play((byte)track);
}

void MP3TriggerSS::setVolume(byte level)
{
  Serial.print(F("\nv"));
  Serial.print(level, DEC);
  Serial.println();
  // level = level ^ B11111111;	//flip it around, so the higher number > higher volume
  s->print('v');
  s->write(level);
}

//
// Response functions
//
void MP3TriggerSS::statusRequest()
{
  Serial.println(F("MP3TriggerSS::statusRequest is not yet implemented"));
  s->write('S');
  s->write('1');
  s->write('S');
  s->write('0');
  
  while (s->available() > 0)
  {
    // read the incoming byte:
    char incomingByte = s->read();

    Serial.print(incomingByte);
  }
  Serial.println();
}
