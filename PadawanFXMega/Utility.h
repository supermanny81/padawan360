int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

long time = 0;
int cycles = 0;

void countCycles() {
  if (millis() - time > 1000) {
    Serial.print(cycles);
    Serial.println(F("cycles processed in 1s"));
    time = millis();
    cycles = 1;
  } else {
    cycles++;
  }
}


