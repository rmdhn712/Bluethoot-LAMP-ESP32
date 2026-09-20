# Smart Bluetooth Relay Controller (ESP32)

Proyek ini adalah sistem kontrol relay otomatis berbasis ESP32 menggunakan komunikasi **Bluetooth Serial**. Sistem ini dirancang untuk mengendalikan lampu atau perangkat elektronik secara manual maupun otomatis berdasarkan jadwal waktu yang ditentukan. Data jadwal disimpan secara permanen di memori flash ESP32 menggunakan library `Preferences` sehingga tidak hilang saat perangkat dimatikan.

---

## 🚀 Fitur Utama

*   **Kontrol Manual:** Menyalakan dan mematikan relay secara instan lewat perintah Bluetooth.
*   **Penjadwalan Otomatis:** Mengatur waktu hidup (`ON`) dan mati (`OFF`) perangkat.
*   **Sistem Waktu Mandiri:** Berjalan secara *real-time* berbasis internal `millis()` sejak waktu diatur (tanpa wajib modul RTC eksternal).
*   **Penyimpanan Permanen (Non-Volatile):** Jadwal dan status otomatisasi tetap tersimpan aman di flash memory.
*   **Eksekusi Cepat:** Mendukung deteksi perintah pendek instan untuk respon kontrol yang cepat.

---

## 🛠️ Konfigurasi Pin & Hardware

Berdasarkan berkas `config.h`, berikut adalah konfigurasi default hardware yang digunakan:

*   **Nama Perangkat Bluetooth:** `BT_Lampu`
*   **Pin Relay:** GPIO `26`
*   **Logika Relay:** Active LOW (`LOW` = Relay ON, `HIGH` = Relay OFF)

---

## 💻 Struktur Kode & Komponen

Proyek ini dibagi menjadi beberapa modul terpisah agar kode tetap rapi dan modular:

1.  **`main.ino`**: Mengatur inisialisasi awal (*setup*), alur utama program (*loop*), pembacaan buffer Bluetooth, serta *parsing* argumen perintah.
2.  **`config.h`**: Menyimpan definisi pin hardware, nama Bluetooth, logika relay, serta daftar kata kunci perintah.
3.  **`ScheduleManager` (`schedule_manager.h/.cpp`)**: Bertanggung jawab mengelola logika waktu penjadwalan serta menentukan apakah relay harus aktif atau mati pada jam tertentu.
4.  **`StorageManager` (`storage_manager.h/.cpp`)**: Mengatur proses baca-tulis konfigurasi jadwal dari dan ke memori internal ESP32 (`Preferences`).

---

## 📱 Protokol & Perintah Bluetooth

Anda dapat mengontrol perangkat menggunakan aplikasi Bluetooth Serial Terminal (seperti *Serial Bluetooth Terminal* di Android). Semua perintah bersifat *case-insensitive* (tidak sensitif huruf besar/kecil).

### 1. Kontrol Manual Relay
*   **`NYALA`** : Menyalakan lampu / relay secara manual.
*   **`MATI`** : Mematikan lampu / relay secara manual.

### 2. Manajemen Jadwal (Schedule)
*   **`ON`** : Mengaktifkan mode otomatisasi berdasarkan jadwal.
*   **`OFF`** : Menonaktifkan mode otomatisasi jadwal.
*   **`SETON <HH:MM>`** atau **`SETON <HH:MM:SS>`** : Mengatur jam lampu otomatis menyala (Contoh: `SETON 18:30`).
*   **`SETOFF <HH:MM>`** atau **`SETOFF <HH:MM:SS>`** : Mengatur jam lampu otomatis mati (Contoh: `SETOFF 06:00`).

### 3. Konfigurasi Waktu & Status
*   **`SETTIME <HH:MM:SS>`** : Mengatur waktu dasar perangkat saat ini secara manual agar sistem penjadwalan berjalan akurat (Contoh: `SETTIME 14:05:00`).
*   **`STATUS`** : Mengambil status terkini perangkat yang mencakup status jadwal, waktu ON/OFF, serta jam internal perangkat saat ini.

---

## ⚙️ Cara Penggunaan

1.  **Upload Kode:** Pastikan semua file (`main.ino`, `config.h`, `schedule_manager.h/.cpp`, `storage_manager.h/.cpp`) berada dalam satu folder sketch Arduino, lalu upload ke ESP32 Anda.
2.  **Koneksi Bluetooth:** Nyalakan Bluetooth di smartphone, cari perangkat bernama **`BT_Lampu`**, lalu lakukan *pairing*.
3.  **Sinkronisasi Waktu Pertama:** Setelah terhubung ke aplikasi serial terminal, kirim perintah waktu saat ini untuk mencocokkan jam sistem, misalnya:
    ```text
    SETTIME 08:00:00
    ```
4.  **Atur Jadwal:** Masukkan jadwal operasional yang diinginkan:
    ```text
    SETON 18:00:00
    SETOFF 06:00:00
    ```
5.  **Selesai:** Perangkat sekarang akan otomatis menjaga relay tetap `ON` antara pukul 18:00 hingga 06:00 pagi.
