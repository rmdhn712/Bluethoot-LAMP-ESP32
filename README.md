# Smart Bluetooth Relay Controller (ESP32)

Proyek ini adalah sistem kontrol relay otomatis berbasis ESP32 menggunakan komunikasi **Bluetooth Serial**. Sistem ini dirancang untuk mengendalikan lampu atau perangkat elektronik secara manual maupun otomatis berdasarkan jadwal waktu yang ditentukan. Data jadwal disimpan secara permanen di memori flash ESP32 menggunakan library `Preferences` sehingga konfigurasi tidak hilang saat perangkat dimatikan atau kehilangan daya.

---

## 🚀 Fitur Utama

*   **Kontrol Manual:** Menyalakan dan mematikan relay secara instan lewat perintah Bluetooth.
*   **Penjadwalan Otomatis:** Mengatur waktu hidup (`ON`) dan mati (`OFF`) perangkat secara spesifik.
*   **Sistem Waktu Mandiri:** Berjalan secara *real-time* berbasis internal `millis()` sejak waktu diatur (tanpa wajib modul RTC eksternal).
*   **Penyimpanan Permanen (Non-Volatile):** Jadwal dan status otomatisasi tetap tersimpan aman di flash memory.
*   **Eksekusi Cepat:** Mendukung deteksi perintah pendek instan untuk respon kontrol yang cepat.

---

## 📦 Komponen yang Diperlukan

Untuk merakit sistem ini, Anda membutuhkan komponen-komponen berikut:

1.  **ESP32 Development Board** (misalnya tipe NodeMCU ESP32 atau ESP32 DOIT DevKit V1) sebagai otak utama yang memproses program dan menyediakan konektivitas Bluetooth.
2.  **Modul Relay 5V atau 3.3V (1-Channel)** sebagai sakelar elektronik untuk memutus dan menyambung arus listrik bertegangan tinggi (AC).
3.  **Lampu atau Beban Elektronik** beserta fiting dan kabel colokan listrik sebagai perangkat yang akan dikontrol.
4.  **Sumber Daya (Power Supply):** Adaptor Micro USB / USB-C 5V untuk memberi daya ke board ESP32.
5.  **Kabel Jumper (Female-to-Female):** Untuk menghubungkan pin ESP32 ke modul relay.

---

## 🔌 Skema Wiring (Pengabelan)

Perhatikan pinout pada ESP32 Anda. Berdasarkan konfigurasi default pada berkas `config.h`, pin data yang digunakan untuk mengendalikan relay adalah **GPIO 26** dengan logika **Active LOW** (`LOW` = Relay ON, `HIGH` = Relay OFF).

### A. Koneksi ESP32 ke Modul Relay (Arus Lemah / DC)
Hubungkan pin-pin menggunakan kabel jumper dengan konfigurasi berikut:

| Pin ESP32 | Pin Modul Relay | Keterangan |
| :--- | :--- | :--- |
| **GND** | **GND** | Jalur negatif / ground bersama |
| **GPIO 26** | **IN / Signal** | Jalur data pemicu relay |
| **VIN / 5V** | **VCC** | Jalur positif daya (pastikan modul relay mendukung tegangan input 5V) |

*Catatan: Jika Anda menggunakan modul relay yang murni berjalan di logika 3.3V, Anda bisa menghubungkan VCC relay ke pin **3V3** pada ESP32.*

### B. Koneksi Modul Relay ke Lampu & Sumber Listrik AC (Arus Kuat)
*Sakelar relay dipasang secara **seri** dengan salah satu kabel dari jala-jala listrik (disarankan pada jalur Live/Fasa).*

1.  Potong salah satu jalur kabel dari colokan listrik AC.
2.  Hubungkan ujung kabel AC dari colokan dinding ke terminal **COM (Common)** pada relay.
3.  Hubungkan ujung kabel sisanya yang menuju ke lampu ke terminal **NO (Normally Open)** pada relay.

### C. Diagram Visual ASCII

```text
       +-----------------------------------+

       |            ESP32 BOARD            |
       |                                   |
       |    [VIN]    [GND]    [GPIO 26]    |
       +------|--------|----------|--------+

              |        |          |
              | VCC    | GND      | Signal (IN)
              v        v          v
       +-----------------------------------+

       |        MODUL RELAY 1-CHANNEL      |
       |                                   |
       |    [VCC]    [GND]     [IN]        |
       |                                   |
       |    [ NO ]  [ COM ]   [ NC ]       |
       +-------|-------|-------------------+

               |       |
               |       +-----------> [ Colokan Listrik AC (Dinding) ]
               |
               v
         [ LAMPU / BEBAN ] ----------> [ Jalur Netral Listrik AC ]
```

⚠️ **PERINGATAN KESELAMATAN:** Arus listrik AC 220V sangat berbahaya. Pastikan colokan listrik sama sekali **tidak terhubung** ke stopkontak dinding saat Anda melakukan proses pengabelan pada terminal relay (COM/NO).

---

## 📚 Library yang Digunakan

Semua library yang digunakan dalam proyek ini adalah **library bawaan (built-in)** dari ESP32 Arduino Core. Anda **tidak perlu mengunduh library tambahan** melalui Library Manager.

1.  **`BluetoothSerial.h`** : Mengaktifkan fitur Bluetooth Classic pada ESP32 agar dapat bertindak sebagai port serial nirkabel untuk menerima dan mengirim data ke smartphone.
2.  **`Preferences.h`** : Mengelola pembacaan dan penulisan data ke dalam memori flash internal ESP32 (Non-Volatile Storage) dalam bentuk pasangan *Key-Value* untuk menyimpan jadwal secara permanen.
3.  **`Arduino.h`** : Menyediakan fungsi-fungsi dasar ekosistem Arduino (seperti `pinMode`, `digitalWrite`, `millis`, dan objek `String`).

---

## 💻 Struktur Kode & Komponen

Proyek ini dibagi menjadi beberapa modul terpisah agar kode tetap rapi dan mudah dirawat:

*   **`main.ino`** : Mengatur inisialisasi awal (*setup*), alur utama program (*loop*), pembacaan buffer Bluetooth, serta *parsing* argumen perintah.
*   **`config.h`** : Menyimpan definisi pin hardware, nama Bluetooth (`BT_Lampu`), logika relay, serta daftar kata kunci perintah.
*   **`ScheduleManager` (`schedule_manager.h/.cpp`)** : Mengelola logika waktu penjadwalan serta menentukan apakah relay harus aktif atau mati pada jam tertentu.
*   **`StorageManager` (`storage_manager.h/.cpp`)** : Mengatur proses baca-tulis konfigurasi jadwal dari dan ke memori internal ESP32 (`Preferences`).

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
*   **`STATUS`** : Mengambil status terkini perangkat (status jadwal, waktu ON/OFF, serta jam internal perangkat saat ini).

---

## ⚙️ Cara Penggunaan

1.  **Upload Kode:** Buka Arduino IDE, pastikan seluruh berkas kode proyek berada dalam satu folder sketch, lalu upload program ke ESP32 Anda.
2.  **Koneksi Bluetooth:** Nyalakan Bluetooth di smartphone, cari perangkat bernama **`BT_Lampu`**, lalu lakukan *pairing*.
3.  **Sinkronisasi Waktu:** Setelah terhubung melalui aplikasi serial terminal, kirim perintah waktu lokal saat ini untuk mencocokkan jam internal sistem, misalnya:
    ```text
    SETTIME 08:00:00
    ```
4.  **Atur Jadwal Otomatis:** Masukkan jadwal operasional yang Anda inginkan:
    ```text
    SETON 18:00:00
    SETOFF 06:00:00
    ```
5.  **Selesai:** Perangkat secara otomatis mengaktifkan jadwal tersebut dan menjaga status relay tetap sinkron (aktif dari pukul 18:00 hingga 06:00). Status ini akan terus dipertahankan meskipun ESP32 mengalami *restart*.
