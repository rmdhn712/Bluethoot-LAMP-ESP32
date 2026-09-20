#include "BluetoothSerial.h"
#include "config.h"
#include "schedule_manager.h"
#include "storage_manager.h"

BluetoothSerial bluetoothSerial;
ScheduleManager scheduleManager;
StorageManager storageManager;

unsigned long lastScheduleCheck = 0;
const unsigned long SCHEDULE_CHECK_INTERVAL = 1000UL;

String btBuffer = "";

// ── Relay ─────────────────────────────────────────────────────────────────────

void initRelay() {
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, RELAY_OFF);
}

void setRelay(bool state) {
  digitalWrite(PIN_RELAY, state ? RELAY_ON : RELAY_OFF);
}

// ── Waktu manual (berjalan dari millis) ───────────────────────────────────────

static unsigned long timeBaseSeconds = 0; // detik sejak tengah malam saat diset
static unsigned long timeBaseMillis  = 0; // millis() saat waktu terakhir diset
static bool          timeIsSet       = false;

void setManualTime(int hour, int minute, int second) {
  timeBaseSeconds = (unsigned long)hour * 3600 + (unsigned long)minute * 60 + second;
  timeBaseMillis  = millis();
  timeIsSet       = true;
}

bool getCurrentTime(int &hour, int &minute, int &second) {
  if (!timeIsSet) return false;
  unsigned long elapsed = (millis() - timeBaseMillis) / 1000UL;
  unsigned long total   = (timeBaseSeconds + elapsed) % 86400UL;
  hour   = total / 3600;
  minute = (total % 3600) / 60;
  second = total % 60;
  return true;
}

// ── Jadwal ────────────────────────────────────────────────────────────────────

void checkSchedule() {
  int hour, minute, second;
  if (!getCurrentTime(hour, minute, second)) return;
  setRelay(scheduleManager.shouldBeOn(hour, minute, second));
}

// ── Parser waktu HH:MM[:SS] ───────────────────────────────────────────────────

bool parseTimeArgs(String args, int &hour, int &minute, int &second) {
  int sep1 = args.indexOf(':');
  if (sep1 < 0) {
    bluetoothSerial.println("Format salah. Gunakan HH:MM atau HH:MM:SS");
    return false;
  }
  int sep2 = args.indexOf(':', sep1 + 1);
  hour     = args.substring(0, sep1).toInt();
  minute   = args.substring(sep1 + 1, sep2 > 0 ? sep2 : args.length()).toInt();
  second   = (sep2 > 0) ? args.substring(sep2 + 1).toInt() : 0;
  if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) {
    bluetoothSerial.println("Jam/menit/detik tidak valid.");
    return false;
  }
  return true;
}

void parseSetScheduleTime(String args, bool isOnTime) {
  int hour, minute, second;
  if (!parseTimeArgs(args, hour, minute, second)) return;
  char buf[32];
  if (isOnTime) {
    scheduleManager.setOnTime(hour, minute, second);
    snprintf(buf, sizeof(buf), "Jadwal ON: %02d:%02d:%02d", hour, minute, second);
  } else {
    scheduleManager.setOffTime(hour, minute, second);
    snprintf(buf, sizeof(buf), "Jadwal OFF: %02d:%02d:%02d", hour, minute, second);
  }
  bluetoothSerial.println(String(buf));
}

// ── Deteksi perintah pendek (tanpa argumen) ───────────────────────────────────

bool isKnownCommand(String cmd) {
  if (cmd.equalsIgnoreCase(CMD_RELAY_ON))     return true;
  if (cmd.equalsIgnoreCase(CMD_RELAY_OFF))    return true;
  if (cmd.equalsIgnoreCase(CMD_SCHEDULE_ON))  return true;
  if (cmd.equalsIgnoreCase(CMD_SCHEDULE_OFF)) return true;
  if (cmd.equalsIgnoreCase(CMD_GET_STATUS))   return true;
  return false;
}

// ── Handler perintah ──────────────────────────────────────────────────────────

void handleCommand(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  if (cmd.equalsIgnoreCase(CMD_RELAY_ON)) {
    setRelay(true);
    bluetoothSerial.println("Lampu NYALA.");

  } else if (cmd.equalsIgnoreCase(CMD_RELAY_OFF)) {
    setRelay(false);
    bluetoothSerial.println("Lampu MATI.");

  } else if (cmd.equalsIgnoreCase(CMD_SCHEDULE_ON)) {
    scheduleManager.enableSchedule(true);
    storageManager.saveSchedule(scheduleManager);
    bluetoothSerial.println("Schedule AKTIF.");
    checkSchedule();
    lastScheduleCheck = millis();

  } else if (cmd.equalsIgnoreCase(CMD_SCHEDULE_OFF)) {
    scheduleManager.enableSchedule(false);
    storageManager.saveSchedule(scheduleManager);
    bluetoothSerial.println("Schedule NONAKTIF.");

  } else if (cmd.length() > (int)strlen(CMD_SET_ON_TIME) &&
             cmd.substring(0, strlen(CMD_SET_ON_TIME)).equalsIgnoreCase(CMD_SET_ON_TIME)) {
    parseSetScheduleTime(cmd.substring(strlen(CMD_SET_ON_TIME)), true);
    scheduleManager.enableSchedule(true);
    storageManager.saveSchedule(scheduleManager);
    bluetoothSerial.println("Schedule AKTIF.");
    checkSchedule();
    lastScheduleCheck = millis();

  } else if (cmd.length() > (int)strlen(CMD_SET_OFF_TIME) &&
             cmd.substring(0, strlen(CMD_SET_OFF_TIME)).equalsIgnoreCase(CMD_SET_OFF_TIME)) {
    parseSetScheduleTime(cmd.substring(strlen(CMD_SET_OFF_TIME)), false);
    scheduleManager.enableSchedule(true);
    storageManager.saveSchedule(scheduleManager);
    bluetoothSerial.println("Schedule AKTIF.");
    checkSchedule();
    lastScheduleCheck = millis();

  } else if (cmd.length() > (int)strlen(CMD_SET_TIME) &&
             cmd.substring(0, strlen(CMD_SET_TIME)).equalsIgnoreCase(CMD_SET_TIME)) {
    // SETTIME HH:MM[:SS] — set jam perangkat secara manual
    int hour, minute, second;
    if (parseTimeArgs(cmd.substring(strlen(CMD_SET_TIME)), hour, minute, second)) {
      setManualTime(hour, minute, second);
      char buf[40];
      snprintf(buf, sizeof(buf), "Waktu diset: %02d:%02d:%02d", hour, minute, second);
      bluetoothSerial.println(String(buf));
    }

  } else if (cmd.equalsIgnoreCase(CMD_GET_STATUS)) {
    bluetoothSerial.println(scheduleManager.getStatusString());
    int hour, minute, second;
    if (getCurrentTime(hour, minute, second)) {
      char buf[24];
      snprintf(buf, sizeof(buf), "Waktu: %02d:%02d:%02d", hour, minute, second);
      bluetoothSerial.println(String(buf));
    } else {
      bluetoothSerial.println("Waktu belum diset. Kirim: SETTIME HH:MM:SS");
    }

  } else {
    bluetoothSerial.println("Perintah tidak dikenal: " + cmd);
  }
}

// ── Setup & Loop ──────────────────────────────────────────────────────────────

void setup() {
  Serial.begin(115200);
  initRelay();
  storageManager.loadSchedule(scheduleManager);
  bluetoothSerial.begin(DEVICE_NAME);
  bluetoothSerial.println("Bluetooth siap.");
  bluetoothSerial.println("Set waktu dengan perintah: SETTIME HH:MM:SS");
}

void loop() {
  while (bluetoothSerial.available()) {
    char c = (char)bluetoothSerial.read();
    if (c == '\n' || c == '\r') {
      if (btBuffer.length() > 0) {
        handleCommand(btBuffer);
        btBuffer = "";
      }
      continue;
    }
    btBuffer += c;
    if (isKnownCommand(btBuffer)) {
      handleCommand(btBuffer);
      btBuffer = "";
    }
  }

  unsigned long now = millis();
  if (scheduleManager.isScheduleEnabled() && (now - lastScheduleCheck >= SCHEDULE_CHECK_INTERVAL)) {
    checkSchedule();
    lastScheduleCheck = now;
  }
}
