#pragma once

#include <Arduino.h>

struct ScheduleTime {
  int hour;
  int minute;
  int second;
};

class ScheduleManager {
public:
  ScheduleManager();

  void setOnTime(int hour, int minute, int second);
  void setOffTime(int hour, int minute, int second);
  void enableSchedule(bool enabled);

  bool isScheduleEnabled() const;
  ScheduleTime getOnTime() const;
  ScheduleTime getOffTime() const;

  bool shouldBeOn(int currentHour, int currentMinute, int currentSecond) const;
  String getStatusString() const;

private:
  ScheduleTime _onTime;
  ScheduleTime _offTime;
  bool _scheduleEnabled;

  int toSeconds(int hour, int minute, int second) const;
};
