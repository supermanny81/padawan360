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

void MP3TriggerSS::setup(SoftwareSerial* serial)
{
	s = serial;
	s->begin(38400);
}

// 
// Looping functions
// 
void MP3TriggerSS::setLooping(bool doLoop, byte track)
{
	mDoLoop = doLoop;
	mLoopTrack = track;
	
	if(!mPlaying && mDoLoop)
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
	if( s->available() )
	{
		int data = s->read();
		if(char(data) == 'X' || char(data) == 'x')
		{
			if(mDoLoop)
			{	
				loop();
			} else
			{
				mPlaying = false;
			}
		} else if(char(data) == 'E')
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
	
	if(wasPlaying)
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
	s->write('p');
	s->write(track);
	mPlaying = true;
}

void MP3TriggerSS::setVolume(byte level)
{
	// level = level ^ B11111111;	//flip it around, so the higher number > higher volume
	s->write('v');
	s->write(level);
}

// 
// Response functions
// 

void MP3TriggerSS::statusRequest()
{
	s->println("MP3TriggerSS::statusRequest is not yet implemented");
	s->flush();
	s->write('S');
	s->write('1');
	delay(5);
	s->read();
	//will need to work on this one to make it useful
	// if (Serial.available() > 0)
	// {
	// 	// read the incoming byte:
	// 	int incomingByte = Serial.read();
	// 
	// 	// say what you got:
	// 	Serial.print("I received: ");
	// 	Serial.println(incomingByte, DEC);
	// }
}
