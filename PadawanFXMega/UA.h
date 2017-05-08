#ifndef UA_H_
#define UA_H_

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <ArduinoLog.h>

#include "libs/TimedServos/TimedServos.h"

#define SV_UA_BOARD 1
#define SV_UA_TOP 0
#define SV_UA_IS_INVERSED true
#define SV_UA_TOP_MIN 280
#define SV_UA_TOP_MAX 510

#define SV_UA_BOTTOM 1
#define SV_UA_BOTTOM_IS_INVERSED true
#define SV_UA_BOTTOM_MIN 300
#define SV_UA_BOTTOM_MAX 550

class UA {

    boolean is_top_open = false;
    boolean is_bottom_open = false;

  private:
    TimedServos* ts = TimedServos::getInstance();
    UA();
    UA(UA const&); // copy disabled
    void operator=(UA const&); // assigment disabled

  public:
    static UA* getInstance();
    void set_upper_arm_position(byte pos);
    void set_lower_arm_position(byte pos);
    void toggle_upper();
    void toggle_lower();

    void open_all();
    void close_all();
};
#endif //UA_H_

