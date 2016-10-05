/*
MP3Trigger.h
@author David Wicks
@url	sansumbrella.com

Doesn't handle messages from the trigger at this point
*/

#ifndef MP3_TRIGGER_H
#define MP3_TRIGGER_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif

class MP3TriggerSS{
  public:
	MP3TriggerSS();
	~MP3TriggerSS();
	void setup(Stream* serial);
  	void play();
	void stop();
	void trigger(byte track);	//1–255
	void play(byte track);		//0—255
  void play(int track);    //0—255
  void play(long track);    //0—255
	void forward();				//move ahead one track
	void reverse();				//move back one track
	void setVolume(byte level);	//0-255
	void statusRequest();
	
	void setLooping(bool doLoop, byte track);		//turn looping on/off
	void setLoopingTrack(byte track);	//select the track to loop
	void update();						//make sure to call this during your loop()
	
private:
	bool mDoLoop;
	byte mLoopTrack;
	bool mPlaying;
	void loop();
	Stream* s;
};

#endif  
