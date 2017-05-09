#ifndef VOLTAGE_H_
#define VOLTAGE_H_

#include <ArduinoLog.h>

#define MIN_VOLTAGE 12.8
#define MAX_VOLTAGE 16.8
#define VOLTAGE_SENSOR_PIN A2

class Voltage {
  private:
    float volts = 0;
    byte percent = 0;
    Voltage() {};
    Voltage(Voltage const&); // copy disabled
    void operator=(Voltage const&); // assigment disabled

  public:
    static Voltage* getInstance() {
      static Voltage voltage;
      return &voltage;
    }

    void sample() {
      volts = (analogRead(VOLTAGE_SENSOR_PIN) / 1024.) * 25;
      percent = map(constrain(volts, MIN_VOLTAGE, MAX_VOLTAGE),
                    MIN_VOLTAGE, MAX_VOLTAGE, 0, 100);
    }

    int getVCC() {
      return volts;
    }

    byte getVCCPct() {
      return percent;
    }

};
#endif //VOLTAGE_H_
