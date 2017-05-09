#ifndef SOUNDS_H_
#define SOUNDS_H_
//************************* Sound Settings *****************************//
#define WAVBAUDRATE 57600

// 10 = max, -70 min
#define DEFAULT_VOLUME_MIN -70
#define DEFAULT_VOLUME_MAX 10
#define DEFAULT_VOLUME -20

#include "libs/WavTrigger2/WavTrigger2.h"

class Sounds {
    private:
      WavTrigger2 wTrig;
      char vol = DEFAULT_VOLUME;
      boolean isBgMusicPlaying = false;
      Sounds();
      Sounds(Sounds const&); // copy disabled
      void operator=(Sounds const&); // assigment disabled
      void setVol(int vol);

  public:
    static Sounds* getInstance();
    void setup(Stream* stream);
    void volUp();
    void volDown();
    void play(int track);
    void printWTrigInfo();
    
};

#endif //SOUNDS_H
