#include "UA.h"

UA::UA() {
  ts->servoBoards[SV_UA_BOARD].channels[SV_UA_TOP].isInversed = SV_UA_IS_INVERSED;
  ts->servoBoards[SV_UA_BOARD].channels[SV_UA_TOP].srvMin = SV_UA_TOP_MIN;
  ts->servoBoards[SV_UA_BOARD].channels[SV_UA_TOP].srvMax = SV_UA_TOP_MAX;

  ts->servoBoards[SV_UA_BOARD].channels[SV_UA_BOTTOM].isInversed = SV_UA_BOTTOM_IS_INVERSED;
  ts->servoBoards[SV_UA_BOARD].channels[SV_UA_BOTTOM].srvMin = SV_UA_BOTTOM_MIN;
  ts->servoBoards[SV_UA_BOARD].channels[SV_UA_BOTTOM].srvMax = SV_UA_BOTTOM_MAX;
  Log.notice(F("UA setup."CR));
}

UA* UA::getInstance() {
  static UA ua;
  return &ua;
}

void UA::set_upper_arm_position(byte pos) {
  ts->setServoPosition(SV_UA_BOARD, SV_UA_TOP, pos, 0);
  Log.notice(F("Setting top UA to: %d"CR), pos);
}

void UA::set_lower_arm_position(byte pos) {
  ts->setServoPosition(SV_UA_BOARD, SV_UA_BOTTOM, pos, 0);
  Log.notice(F("Setting bottom UA to: %d"CR), pos);
}

void UA::toggle_upper() {
  byte pos = 0;
  if (!is_top_open) {
    pos = 127;
  }
  is_top_open = !is_top_open;
  set_upper_arm_position(pos);
}

void UA::toggle_lower() {
  byte pos = 0;
  if (!is_bottom_open) {
    pos = 127;
  }
  is_bottom_open = !is_bottom_open;
  set_lower_arm_position(pos);
}

void UA::open_all() {
  is_top_open = false;
  is_bottom_open = false;
  toggle_upper();
  toggle_lower();
}

void UA::close_all() {
  is_top_open = true;
  is_bottom_open = true;
  toggle_upper();
  toggle_lower();
}

