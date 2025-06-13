# Monitor Level Air King Fikra

## Deskripsi Singkat

Sistem monitoring level air terdistribusi untuk tangki eksperimen, menampilkan deteksi kondisi kritis, penyimpanan data, dan pelaporan.

## Persyaratan Build

*   CMake (versi ~3.16 atau lebih tinggi)
*   Kompiler C++ (mendukung C++20)

## Script Build & Cara Pemakaian

**1. Persiapan Awal:**

*   **Clone Repository:**
    ```bash
    git clone https://github.com/PridFr/Monitor-Level-Air-King-Fikra.git
    cd Monitor-Level-Air-King-Fikra
    ```
*   **Pastikan `nlohmann/json.hpp` ada di `lib/nlohmann/`**. Jika tidak, unduh dan letakkan di sana.

**2. Proses Build (Menggunakan CMake):**

*   **Buat Direktori Build:**
    ```bash
    mkdir build
    cd build
    ```
*   **Konfigurasi Proyek (dari dalam direktori `build`):**
    ```bash
    cmake .. 
    ```
    *(Catatan: Jika ada error terkait path `"Penyimpanan & Ekspor Data"`, mengganti nama folder tersebut adalah solusi terbaik).*
*   **Kompilasi (dari dalam direktori `build`):**
    *   Linux/macOS:
        ```bash
        make
        ```
    *   Windows (MinGW/MSVC via CMake):
        ```bash
        cmake --build .
        ```

**3. Menjalankan Aplikasi Utama (Data Manager & Simulator):**

*   Setelah build berhasil, executable utama akan berada di direktori `build` (atau subdirektori seperti `build/Debug/`).
*   Nama executable akan sesuai dengan yang didefinisikan di `CMakeLists.txt` utama (misalnya, `MonitorLevelAirApp` atau `Monitor_Level_Air_King_Fikra`).
*   **Jalankan dari direktori `build`:**
    *   Linux/macOS: `./NamaExecutableAnda`
    *   Windows: `.\NamaExecutableAnda.exe`

**4. (Jika Ada) Menjalankan Aplikasi Server & Client Terpisah:**

*   Jika modul `server/` dan `client/` dibangun sebagai executable terpisah (tergantung konfigurasi `CMakeLists.txt` mereka masing-masing dan `CMakeLists.txt` utama):
    *   Anda perlu menjalankan executable server terlebih dahulu.
    *   Kemudian jalankan satu atau lebih instance executable client.
    *   Pastikan konfigurasi port dan alamat IP server sesuai antara client dan server.

**Output yang Diharapkan:**

*   Aplikasi utama (`main.cpp` di `"Penyimpanan & Ekspor Data"/src/`) akan menginisialisasi `DataManager`.
*   Simulasi data (jika ada di `main.cpp`) akan berjalan, atau aplikasi akan menunggu data dari server.
*   Log aktivitas akan muncul di konsol.
*   File output seperti `water_tank_data.dat` (data biner) dan `critical_events_report.json` (laporan JSON) akan dibuat/diperbarui di direktori kerja aplikasi.
*   Jika ada file konfigurasi (`app_config.json`), aplikasi akan mencoba memuatnya atau membuat default.

---