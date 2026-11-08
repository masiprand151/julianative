# Build dan distribusi

## Build TypeScript

```powershell
npm.cmd run build:ts
```

Perintah ini menjalankan TypeScript compiler dan `tsc-alias`.

Pemeriksaan tipe tanpa menulis output:

```powershell
npm.cmd run check:ts
```

## Build aplikasi native

Skrip build lengkap:

```powershell
npm.cmd run build:app
```

Skrip tersebut membangun TypeScript, mengonfigurasi CMake dengan Ninja, lalu
membangun executable Qt. Skrip configure bawaan menunjuk compiler MSYS2 pada
`C:/msys64/ucrt64`; jika lingkungan berbeda, sesuaikan konfigurasi CMake dengan
compiler dan Qt yang terpasang.

Untuk build direktori yang sudah dikonfigurasi:

```powershell
cmake --build app/build
```

## Tes

```powershell
npm.cmd test
```

Tes otomatis mengompilasi TypeScript terlebih dahulu, kemudian menjalankan
suite Node.js. Setelah mengubah file C++, jalankan build native juga.

## Development dan hot reload

```powershell
npm.cmd run dev
```

Perubahan `src` dan `app/core` dapat memicu build ulang TypeScript dan restart
renderer Node.js. Perubahan C++ membutuhkan rebuild dan restart proses aplikasi.

## Paket portable

```powershell
npm.cmd run portable
```

Perintah ini membangun aplikasi dan menjalankan skrip persiapan portable.
Distribusi desktop Qt biasanya berisi executable, runtime Node, modul Qt, dan
DLL pendukung. DLL tersebut adalah dependency native yang dibutuhkan aplikasi,
bukan file sementara yang dapat dihapus begitu saja.

File aset dengan path relatif, seperti `assets/logo.svg`, perlu disertakan pada
paket portable. Skrip persiapan portable menyalin folder `assets` ke runtime
aplikasi.

## Catatan rilis

- Uji paket pada mesin yang tidak memiliki lingkungan development.
- Sertakan semua plugin dan modul Qt yang dibutuhkan.
- Pastikan media dan aset lokal mempunyai path yang benar di dalam paket.
- Pertimbangkan penandatanganan executable dan installer untuk distribusi
  publik.
