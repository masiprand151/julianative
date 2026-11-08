# Komponen UI

Komponen dibuat dengan fungsi dan menerima satu object properti, kecuali
`Text`, yang menerima teks sebagai argumen pertama.

Import komponen:

```ts
import {
  Button,
  Builder,
  Card,
  Column,
  FlatList,
  FloatingActionButton,
  Grid,
  Image,
  Modal,
  Row,
  Table,
  Text,
  TextInput,
  Video,
  View,
} from "@julia/core/components";
```

## Window dan layout

### `Builder`

Membuat window aplikasi dan menerima `body` berupa daftar VNode.

Properti utama:

- `title`, `width`, `height`, `position`.
- `resizable`, `fullscreen`, `hideTitleBar`, `alwaysOnTop`.
- `scrollable` untuk mengaktifkan scroll pada isi window.
- `onClose`, `onResize`, `onMaximize`, `onMinimize`.
- `style` dan `body`.

```ts
Builder({
  title: "Catatan",
  width: 720,
  height: 480,
  position: { x: 80, y: 80 },
  body: [Text("Isi aplikasi")],
});
```

`position` dapat berupa koordinat layar `{ x, y }` atau `"center"`,
`"top-left"`, `"top-right"`, `"bottom-left"`, dan `"bottom-right"`.
Koordinat `{ x, y }` menentukan posisi window secara langsung dan tidak
diubah berdasarkan title bar. Posisi bernama menggunakan area layar yang
tersedia (tidak menutupi taskbar/dock).

Jika `position` tidak diisi, window memakai posisi default dekat kiri atas.
Pada mode fullscreen, posisi diabaikan. Untuk memusatkan window:

```ts
Builder({
  width: 720,
  height: 480,
  position: "center",
  body: [],
});
```

Callback window: `onResize(width, height)` menerima dimensi baru dalam piksel;
`onClose`, `onMaximize`, dan `onMinimize` tidak menerima argumen.

### `View`, `Row`, dan `Column`

- `View` adalah container umum; arahnya dapat diatur dengan `direction` atau
  `style.flexDirection`.
- `Row` menata anak secara horizontal.
- `Column` menata anak secara vertikal.
- `children` berisi komponen di dalam container.
- `spacing` atau `style.gap` mengatur jarak antar anak.
- `padding` atau `style.padding` mengatur ruang dalam container.
- `View` juga memiliki `scrollable` dan `scrollDirection`.

```ts
Column({
  spacing: 10,
  padding: { top: 12, right: 16, bottom: 12, left: 16 },
  children: [
    Text("Judul"),
    Row({ children: [Button({ text: "Simpan" })] }),
  ],
});
```

### `Grid`

`Grid` menata anak dari kiri ke kanan, lalu melanjutkan ke baris berikutnya.
`columns` menentukan jumlah kolom (default `2`), sedangkan `spacing` atau
`gap` mengatur jarak baris dan kolom. Jaraknya juga dapat diatur terpisah
dengan `rowGap` dan `columnGap`. `padding` dan `style` mengikuti pola layout
lainnya.

```ts
Grid({
  columns: 2,
  rowGap: 12,
  columnGap: 16,
  children: [
    Card({ title: "Satu", children: [Text("Konten satu")] }),
    Card({ title: "Dua", children: [Text("Konten dua")] }),
    Card({ title: "Tiga", children: [Text("Konten tiga")] }),
  ],
});
```

`columns` harus berupa bilangan bulat positif.

### `Text`

Menerima string atau angka:

```ts
Text("Halo", { style: { fontSize: 20, fontWeight: "bold" } });
Text(42);
```

### `Card`

Container dengan judul dan gaya bawaan. `style` dapat digunakan untuk
menyesuaikan container.

```ts
Card({
  title: "Profil",
  children: [Text("Nama pengguna")],
  style: {
    padding: 20,
    borderWidth: 1,
    borderColor: "#d5e0dc",
    borderLeftWidth: 4,
    borderLeftColor: "#147d72",
  },
});
```

Border dapat diatur untuk semua sisi dengan `borderWidth`, `borderColor`,
`borderStyle`, atau `border`. Untuk mengatur sisi tertentu, gunakan properti
`borderTop`, `borderRight`, `borderBottom`, `borderLeft`, atau properti rinci
seperti `borderBottomWidth`, `borderBottomColor`, dan `borderBottomStyle`.
Pengaturan sisi tertentu akan mengesampingkan pengaturan border umum pada sisi
tersebut.
Jika hanya lebar border sisi tertentu yang diisi dan tidak ada gaya border
umum, gaya sisi tersebut otomatis menjadi `solid`.

## Input dan aksi

### `Button`

```ts
Button({
  text: "Simpan",
  onClick: () => console.log("Disimpan"),
  style: { cursor: "pointer" },
});
```

### `TextInput`

`type` yang didukung: `text`, `password`, `number`, dan `email`. `onChange`
menerima nilai sebagai string.

```ts
TextInput({
  type: "email",
  value: email,
  placeholder: "Alamat email",
  onChange: setEmail,
});
```

### `FloatingActionButton`

Tombol mengambang di atas konten window. Letakkan langsung di `Builder.body`.
`placement` dapat berupa `top-left`, `top-right`, `bottom-left`, atau
`bottom-right`. `margin` mengatur jaraknya dari tepi.

```ts
FloatingActionButton({
  icon: "+",
  accessibilityLabel: "Tambah catatan",
  placement: "bottom-right",
  margin: 24,
  onPress: () => console.log("Tambah"),
});
```

## Media dan dialog

### `Image`

Menerima path lokal atau URL HTTP(S). Path relatif diambil dari working
directory aplikasi. Format yang didukung: PNG, JPG/JPEG, dan SVG.

```ts
Image({
  source: "assets/logo.svg",
  alt: "Logo",
  resizeMode: "contain",
  style: { width: 160, height: 100 },
});
```

`resizeMode`: `contain` (default), `cover`, atau `fill`.

### `Video`

```ts
Video({
  source: "assets/demo.mp4",
  autoplay: false,
  loop: true,
  muted: false,
  style: { width: 640, height: 360 },
});
```

### `Modal`

Tampilkan atau sembunyikan modal menggunakan state. `onClose` dijalankan saat
dialog ditutup melalui kontrol native.

```ts
Modal({
  visible,
  title: "Konfirmasi",
  onClose: () => setVisible(false),
  children: [Text("Lanjutkan?")],
});
```

`visible` default-nya `false`; `title` default-nya string kosong. Perubahan
visibility dilakukan dengan merender ulang `Modal` memakai nilai state baru.

## Tabel

`Table` membuat tabel native. Kolom ditentukan melalui `columns`; setiap kolom
memerlukan `key` dan `title`. `width` memberi lebar awal, `minWidth` memberi
lebar minimum, dan `sortable` mengaktifkan pengurutan.

Properti tabel:

- `headerStyle`, `headerTextStyle`, `rowStyle`, dan `cellStyle` menyesuaikan
  tampilan bagian tabel. Baris memiliki latar dan teks default.
- `emptyComponent` menampilkan VNode atau hasil fungsi saat tidak ada baris.
- `columns[].render(item, index)` membuat isi cell khusus.
- `columns[].compare(left, right)` menyediakan pembanding khusus untuk
  pengurutan kolom. Tanpa pembanding, angka diurutkan numerik dan nilai lain
  sebagai teks, tanpa membedakan kapitalisasi.
- Header kolom dapat diurutkan kecuali `sortable: false`; perubahan lebar kolom
  dapat dilakukan dengan menyeret pembatas header. Lebar hasil resize bertahan
  selama instance tabel tersebut hidup.

```ts
Table({
  data: [
    { name: "Ada", year: 1843 },
    { name: "Grace", year: 1952 },
  ],
  columns: [
    { key: "name", title: "Nama", width: 180 },
    { key: "year", title: "Tahun" },
  ],
  style: { height: 240 },
});
```

Saat ini tabel merender semua baris; untuk daftar besar gunakan `FlatList`.
