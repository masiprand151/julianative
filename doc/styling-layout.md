# Style dan layout

Komponen menerima object `style`. Style dapat ditulis langsung atau didefinisikan
sekali menggunakan `StyleSheet.create`.

```ts
import { StyleSheet } from "@julia/core/components";

const styles = StyleSheet.create({
  primaryButton: {
    padding: { top: 10, right: 16, bottom: 10, left: 16 },
    backgroundColor: "#147d72",
    color: "#ffffff",
    borderRadius: 6,
    cursor: "pointer",
    ":hover": { backgroundColor: "#0f655c" },
  },
});
```

## Ukuran dan jarak

Properti ukuran yang umum:

- `width`, `height`, `minWidth`, `minHeight`, `maxWidth`, `maxHeight`.
- `padding` untuk ruang di dalam elemen.
- `margin` untuk jarak di luar elemen.
- `gap` untuk jarak antar item dalam layout.

Nilai panjang dapat berupa angka (piksel), string `px`, atau `pt`. Lebar dan
tinggi juga menerima persentase pada widget yang mendukung ukuran persentase.

Padding dan margin dapat berupa satu nilai yang diterapkan ke semua sisi:

```ts
View({ style: { padding: 12, margin: 8 } });
```

Atau object per sisi:

```ts
View({
  style: {
    padding: { top: 8, right: 12, bottom: 16, left: 12 },
    margin: { top: 0, right: 4, bottom: 8, left: 4 },
  },
});
```

Sisi yang tidak disebut dalam object bernilai `0`. Gunakan nama `top`, `right`,
`bottom`, dan `left`.

## Mengatur layout

`Column` dan `Row` adalah pilihan sederhana untuk susunan vertikal atau
horizontal. `Grid` menyusun item dalam beberapa kolom. `View` menerima
`direction`, `justifyContent`, dan `alignItems`.

```ts
View({
  direction: "row",
  justifyContent: "space-between",
  alignItems: "center",
  style: { gap: 12, padding: { left: 16, right: 16 } },
  children: [
    Text("Judul"),
    Button({ text: "Aksi" }),
  ],
});
```

Gunakan `Grid` saat item perlu mengalir ke baris berikutnya. Jumlah kolom
ditentukan oleh `columns`; jarak antar sel dapat memakai `gap` atau diatur
terpisah dengan `rowGap` dan `columnGap`.

```ts
Grid({
  columns: 3,
  rowGap: 12,
  columnGap: 16,
  children: [Text("Satu"), Text("Dua"), Text("Tiga"), Text("Empat")],
});
```

Nilai `justifyContent`: `flex-start`, `center`, `flex-end`, `space-between`,
atau `space-around`. Nilai `alignItems`: `flex-start`, `center`, `flex-end`,
atau `stretch`.

## Gaya kondisional dan nama properti

Properti dapat menggunakan camelCase (`backgroundColor`) atau nama CSS umum
(`background-color`). Style state yang tersedia antara lain `:hover`, `:focus`,
`:active`, `:pressed`, dan `:disabled`.

```ts
Button({
  text: "Kirim",
  style: {
    backgroundColor: "#147d72",
    ":hover": { backgroundColor: "#0f655c" },
    ":disabled": { color: "#888888" },
  },
});
```

Renderer native hanya menerapkan properti yang didukung Qt. Jika properti tidak
terlihat berpengaruh, periksa ejaan dan dukungan properti tersebut.

## Scroll

`View` yang scrollable sebaiknya memiliki ukuran yang dibatasi pada arah scroll.
Contohnya, scroll vertikal membutuhkan tinggi terbatas.

```ts
View({
  scrollable: true,
  scrollDirection: "vertical",
  style: { height: 240, padding: { top: 8, bottom: 8 } },
  children: [Text("Konten panjang")],
});
```
