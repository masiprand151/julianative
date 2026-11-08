# Dokumentasi JuliaNative

Dokumentasi ini menjelaskan cara membangun aplikasi, menyusun antarmuka,
menggunakan komponen, mengelola state, dan menyimpan data di JuliaNative.
Contoh ditulis dengan TypeScript.

## Mulai membaca

1. [Mulai menggunakan JuliaNative](getting-started.md) — kebutuhan, instalasi,
   dan menjalankan aplikasi pertama.
2. [Siklus hidup aplikasi](application-lifecycle.md) — entry point, callback
   window, logging, dan keluar dari aplikasi.
3. [Struktur proyek](project-structure.md) — fungsi folder dan alur kerja UI.
4. [Komponen UI](components.md) — properti dan contoh komponen yang tersedia.
5. [Style, layout, padding, dan margin](styling-layout.md) — cara menyusun dan
   memberi gaya pada antarmuka.
6. [State dan navigasi](state-navigation.md) — state komponen dan perpindahan
   halaman.
7. [Daftar dan penyimpanan lokal](lists-storage.md) — `FlatList`, key, virtualisasi,
   dan `LocalStorage`.
8. [Build dan distribusi](build-distribution.md) — build development, hot reload,
   dan paket portable.
9. [Pemecahan masalah](troubleshooting.md) — langkah awal untuk masalah umum.

## Gambaran singkat

UI ditulis di TypeScript sebagai susunan komponen. Renderer mengubah susunan itu
menjadi pesan JSON, lalu runtime Qt/C++ membangun widget native. Aplikasi
dimulai dari `src/main.ts`; implementasi API komponen berada di `app/core`, dan
render native berada di `app`.

Untuk ringkasan fitur, lihat [README utama](../README.md).
