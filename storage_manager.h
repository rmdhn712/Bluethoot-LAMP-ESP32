#pragma once

#include <Preferences.h>
#include "schedule_manager.h"

class StorageManager {
public:
  StorageManager();

  void saveSchedule(const ScheduleManager &scheduleManager);
  void loadSchedule(ScheduleManager &scheduleManager);

private:
  Preferences _prefs;

  static const char* NAMESPACE;
  static const char* KEY_SCH_ENABLED;
  static const char* KEY_ON_H;
  static const char* KEY_ON_M;
  static const char* KEY_ON_S;
  static const char* KEY_OFF_H;
  static const char* KEY_OFF_M;
  static const char* KEY_OFF_S;
};
