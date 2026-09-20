#include "schedule_manager.h"

ScheduleManager::ScheduleManager()
  : _scheduleEnabled(false) {
  _onTime  = {18, 0, 0};
  _offTime = {6,  0, 0};
}

void ScheduleManager::setOnTime(int hour, int minute, int second) {
  _onTime = {hour, minute, second};
}

void ScheduleManager::setOffTime(int hour, int minute, int second) {
  _offTime = {hour, minute, second};
}

void ScheduleManager::enableSchedule(bool enabled) {
  _scheduleEnabled = enabled;
}

bool ScheduleManager::isScheduleEnabled() const {
  return _scheduleEnabled;
}

ScheduleTime ScheduleManager::getOnTime() const {
  return _onTime;
}

ScheduleTime ScheduleManager::getOffTime() const {
  return _offTime;
}

int ScheduleManager::toSeconds(int hour, int minute, int second) const {
  return hour * 3600 + minute * 60 + second;
}

bool ScheduleManager::shouldBeOn(int currentHour, int currentMinute, int currentSecond) const {
  if (!_scheduleEnabled) return false;

  int now   = toSeconds(currentHour, currentMinute, currentSecond);
  int onAt  = toSeconds(_onTime.hour,  _onTime.minute,  _onTime.second);
  int offAt = toSeconds(_offTime.hour, _offTime.minute, _offTime.second);

  if (onAt < offAt) {
    return (now >= onAt && now < offAt);
  } else {
    return (now >= onAt || now < offAt);
  }
}

String ScheduleManager::getStatusString() const {
  char buf[80];
  snprintf(buf, sizeof(buf),
    "SCH:%s ON=%02d:%02d:%02d OFF=%02d:%02d:%02d",
    _scheduleEnabled ? "ON" : "OFF",
    _onTime.hour,  _onTime.minute,  _onTime.second,
    _offTime.hour, _offTime.minute, _offTime.second
  );
  return String(buf);
}
