# Pemecahan masalah

## `npm` diblokir di PowerShell

Jika PowerShell menolak menjalankan `npm.ps1`, gunakan `npm.cmd`:

```powershell
npm.cmd test
```

## Header Qt tidak ditemukan oleh IntelliSense

Pastikan Qt 6 dengan modul yang diperlukan sudah terpasang dan konfigurasi
include path editor menunjuk direktori Qt yang sesuai. Proyek menggunakan
Qt Core, Gui, Widgets, Network, Svg, Multimedia, dan MultimediaWidgets.

Jika CMake build juga gagal, konfigurasi compiler dan Qt CMake package perlu
diperiksa; memperbaiki IntelliSense saja tidak memasang dependency yang belum
ada.

## CMake tidak menemukan Qt

Periksa instalasi Qt dan pastikan CMake dapat menemukan `Qt6Config.cmake`.
Atur `CMAKE_PREFIX_PATH` ketika menjalankan konfigurasi CMake jika Qt tidak
berada pada lokasi pencarian standar. Pastikan kit Qt cocok dengan compiler
yang digunakan.

## Perubahan TypeScript tidak terlihat

1. Jalankan `npm.cmd run check:ts` untuk menemukan error tipe.
2. Jalankan `npm.cmd run build:ts` atau restart `npm.cmd run dev`.
3. Pastikan file berada di `src` atau `app/core` jika mengandalkan hot reload.
4. Perubahan C++ memerlukan build native dan restart aplikasi.

## `FlatList` tidak menggulir

- Beri tinggi terbatas pada daftar vertikal atau lebar terbatas pada daftar
  horizontal.
- Pastikan `scrollEnabled` tidak bernilai `false`.
- Berikan `keyExtractor` yang mengembalikan key unik dan stabil.
- Gunakan `estimatedItemSize` yang masuk akal untuk ukuran baris pada umumnya.

## `LocalStorage` menolak promise

Gunakan `try/catch` di sekitar operasi asynchronous. Pastikan nilai yang
diberikan ke `setItem` dapat diserialisasi sebagai JSON dan direktori data
aplikasi dapat ditulis. Jika pesan error menyatakan `Access is denied`,
pastikan `local-storage.json` tidak read-only atau dikunci proses lain, dan
akun pengguna memiliki izin menulis ke direktori data aplikasi. Path lengkap
file sekarang disertakan pada error saat membuka atau mengganti file gagal.

File yang rusak atau bukan object JSON juga menyebabkan operasi storage gagal.
Jangan hapus file sebelum membuat salinan jika data di dalamnya masih
diperlukan.

## Portable gagal membuka aplikasi atau aset

Pastikan runtime Node, executable, library Qt, plugin Qt, modul media, dan
folder aset ikut disertakan. Jalankan paket pada komputer bersih untuk
menemukan dependency yang hanya tersedia di mesin development.
