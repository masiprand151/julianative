# Siklus hidup aplikasi

## Entry point

Entry point proyek berada di `src/main.ts`. Fungsi utama membuat satu VNode
`Widget`, lalu `Application.run` memulai renderer dan mendengarkan event dari
runtime native:

```ts
import { Application } from "@julia/core";
import { Text, Widget } from "@julia/core/components";

function Main() {
  return Widget({
    title: "Aplikasi saya",
    body: [Text("Siap digunakan")],
  });
}

Application.run(Main);
```

Berikan fungsi ke `Application.run`, bukan hasil pemanggilannya. Renderer akan
memanggil fungsi itu lagi saat state berubah.

## API `Application`

### `Application.run(main)`

Memulai proses UI. `main` adalah fungsi yang mengembalikan VNode root, biasanya
`Widget`. Panggil sekali dari entry point.

### `Application.exit()`

Mengirim perintah keluar ke runtime native:

```ts
Button({
  text: "Keluar",
  onClick: () => Application.exit(),
});
```

### `Application.sendToNative(message)`

API tingkat rendah untuk mengirim pesan JSON ke runtime native. Aplikasi biasa
umumnya tidak perlu memanggilnya; gunakan API komponen yang tersedia kecuali
sedang mengembangkan bridge native.

## Event window

Callback dipasang pada `Widget`:

```ts
Widget({
  onClose: () => console.log("Window ditutup"),
  onResize: (width, height) =>
    console.log(`Ukuran: ${width} x ${height}`),
  onMaximize: () => console.log("Window dimaksimalkan"),
  onMinimize: () => console.log("Window diminimalkan"),
  body: [],
});
```

`onResize` menerima lebar dan tinggi baru. Callback lainnya tidak menerima
argumen. Callback ditautkan ulang saat UI dirender ulang, jadi gunakan state
terbaru dari closure render saat memasangnya.

## Logging

Dalam runtime aplikasi, `console.log(...)` diarahkan ke stderr proses Node.
Pesan tampil di terminal atau log development, bukan sebagai teks UI. Untuk
menampilkan informasi kepada pengguna, gunakan komponen `Text`, dialog, atau
komponen UI lainnya.

## Development reload

`npm run dev` mengawasi perubahan TypeScript di `src` dan `app/core`. Renderer
Node dimulai ulang dan UI diperbarui di window yang sama. Perubahan C++ tidak
dimuat ulang otomatis; rebuild dan restart aplikasi diperlukan.
