#include "storage_manager.h"

const char* StorageManager::NAMESPACE       = "lampu";
const char* StorageManager::KEY_SCH_ENABLED = "sch_en";
const char* StorageManager::KEY_ON_H        = "on_h";
const char* StorageManager::KEY_ON_M        = "on_m";
const char* StorageManager::KEY_ON_S        = "on_s";
const char* StorageManager::KEY_OFF_H       = "off_h";
const char* StorageManager::KEY_OFF_M       = "off_m";
const char* StorageManager::KEY_OFF_S       = "off_s";

StorageManager::StorageManager() {}

void StorageManager::saveSchedule(const ScheduleManager &scheduleManager) {
  _prefs.begin(NAMESPACE, false);
  _prefs.putBool(KEY_SCH_ENABLED, scheduleManager.isScheduleEnabled());
  _prefs.putInt(KEY_ON_H,  scheduleManager.getOnTime().hour);
  _prefs.putInt(KEY_ON_M,  scheduleManager.getOnTime().minute);
  _prefs.putInt(KEY_ON_S,  scheduleManager.getOnTime().second);
  _prefs.putInt(KEY_OFF_H, scheduleManager.getOffTime().hour);
  _prefs.putInt(KEY_OFF_M, scheduleManager.getOffTime().minute);
  _prefs.putInt(KEY_OFF_S, scheduleManager.getOffTime().second);
  _prefs.end();
}

void StorageManager::loadSchedule(ScheduleManager &scheduleManager) {
  _prefs.begin(NAMESPACE, true);

  bool enabled = _prefs.getBool(KEY_SCH_ENABLED, false);
  int onH  = _prefs.getInt(KEY_ON_H,  18);
  int onM  = _prefs.getInt(KEY_ON_M,  0);
  int onS  = _prefs.getInt(KEY_ON_S,  0);
  int offH = _prefs.getInt(KEY_OFF_H, 6);
  int offM = _prefs.getInt(KEY_OFF_M, 0);
  int offS = _prefs.getInt(KEY_OFF_S, 0);

  _prefs.end();

  scheduleManager.setOnTime(onH, onM, onS);
  scheduleManager.setOffTime(offH, offM, offS);
  scheduleManager.enableSchedule(enabled);
}
