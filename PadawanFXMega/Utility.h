#include <ArduinoLog.h>

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

long time = 0L;
int cycles = 0;
long interval = 1000L;

void setTime(long t) {
  time = t + interval;
}

void countCycles() {
  if (millis() > time) {
    cycles++;
    Log.notice(F("%d cycles processed in ~%d millis."CR), cycles, interval);
    setTime(millis());
    cycles = 1;
  } else {
    cycles++;
  }
}
