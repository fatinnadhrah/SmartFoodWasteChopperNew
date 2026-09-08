# Smart Food Waste Chopper with IoT-Based Monitoring and Notification System

## PENGENALAN

Smart Food Waste Chopper with IoT-Based Monitoring and Notification System merupakan sebuah projek yang dibangunkan untuk membantu menguruskan sisa makanan dengan lebih sistematik.

Projek ini menggunakan sistem kawalan berasaskan ESP32 untuk mengawal proses pencincangan sisa makanan. Sistem dilengkapi dengan mekanisme keselamatan menggunakan limit switch bagi mengesan keadaan penutup sebelum proses pencincangan dimulakan.

Motor DC digunakan untuk menjalankan mekanisme pencincangan. Status mesin seperti keadaan mesin, penutup dan motor dihantar ke sistem web melalui sambungan Wi-Fi untuk tujuan pemantauan.

---

## IoT

### Peranti IoT

Peranti IoT yang digunakan dalam projek ini ialah:

- ESP32
- Wi-Fi sebagai sambungan komunikasi antara ESP32 dan sistem web

### Sensor / Input

Input yang digunakan dalam sistem ialah:

- Limit Switch
  - Digunakan untuk mengesan keadaan penutup sama ada CLOSED atau OPEN.
  - Sistem akan memastikan penutup berada dalam keadaan CLOSED sebelum motor dibenarkan beroperasi.

- Push Button
  - Digunakan sebagai input untuk memulakan proses pencincangan.
  - Motor hanya akan dimulakan apabila push button ditekan dan penutup berada dalam keadaan CLOSED.

### Output

Output yang digunakan ialah:

- DC Motor 775
  - Digunakan untuk memutarkan mekanisme pencincangan.

- BTS7960 Motor Driver
  - Digunakan untuk mengawal arah dan kelajuan motor DC.

- Buzzer
  - Digunakan sebagai pemberitahuan apabila proses pencincangan telah selesai.

- LCD 20x4 I2C
  - Digunakan untuk memaparkan status sistem secara langsung.

### Pin yang digunakan

| Komponen | ESP32 GPIO |
|---|---:|
| Limit Switch | GPIO 12 |
| Push Button | GPIO 25 |
| SG90 Servo Lock | GPIO 26 |
| Buzzer | GPIO 18 |
| BTS7960 RPWM | GPIO 32 |
| BTS7960 LPWM | GPIO 33 |
| BTS7960 R_EN | GPIO 16 |
| BTS7960 L_EN | GPIO 17 |
| LCD SDA | GPIO 21 |
| LCD SCL | GPIO 22 |

---

## DATABASE

### Pangkalan Data

Pangkalan data yang digunakan ialah:

- MySQL
- phpMyAdmin digunakan untuk menguruskan pangkalan data.

### Nama Database
### Fungsi Jadual machine_status

Jadual machine_status digunakan untuk menyimpan maklumat status semasa mesin.

Maklumat yang disimpan termasuk:

| Field | Fungsi |
|---|---|
| `id` | ID unik bagi setiap rekod |
| `machine_status` | Menyimpan status mesin seperti `READY`, `PROCESSING`, `COMPLETE` dan `ERROR` |
| `detection` | Menyimpan maklumat detection |
| `lid_status` | Menyimpan keadaan penutup seperti `OPEN` atau `CLOSED` |
| `motor_status` | Menyimpan keadaan motor seperti `ON` atau `OFF` |
| `remaining_time` | Menyimpan baki masa proses pencincangan |
| `updated_at` | Menyimpan tarikh dan masa rekod dikemas kini |

ESP32 menghantar data status ke `save_data.php`, kemudian data tersebut disimpan ke dalam database MySQL.

## INTERFACE
### Interface yang digunakan ialah:
- Website
- PHP, HTML, CSS dan JavaScript
- XAMPP 

### Fungsi Website
- Website berfungsi sebagai dashboard untuk memantau keadaan mesin Smart Food Waste Chopper.

### Fungsi yang disediakan:
1. Machine Status
Memaparkan status mesin seperti:
- READY
- PROCESSING
- COMPLETE
- ERROR
2. Lid Status
Memaparkan keadaan penutup:
- OPEN
- CLOSED
3. Motor Status
Memaparkan keadaan motor:
- ON
- OFF
4. Remaining Time
- Memaparkan baki masa proses pencincangan.
5. Process Completion Notification
- Memberikan pemberitahuan apabila proses pencincangan telah selesai.
6. IoT Monitoring
- Menerima dan memaparkan data yang dihantar oleh ESP32 melalui Wi-Fi.

## Status Projek
Anggaran kemajuan semasa: 80%
Modul Yang Telah Disiapkan
 [✓] ESP32 setup
 [✓] Motor DC 775
 [✓] BTS7960 motor driver
 [✓] Push button control
 [✓] Limit switch untuk pengesanan penutup
 [✓] Buzzer notification
 [✓] LCD status display
 [x] Wi-Fi connection
 [x] PHP server connection
 [x] MySQL database
 [x] Data transmission daripada ESP32 ke database
 [x] Website monitoring interface
 [x] Motor safety control apabila penutup terbuka