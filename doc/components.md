# Komponen UI

Komponen dibuat dengan fungsi dan menerima satu object properti, kecuali
`Text`, yang menerima teks sebagai argumen pertama.

Import komponen:

```ts
import {
  Button,
  Card,
  Column,
  FlatList,
  FloatingActionButton,
  Image,
  Modal,
  Row,
  Table,
  Text,
  TextInput,
  Video,
  View,
  Widget,
} from "@julia/core/components";
```

## Window dan layout

### `Widget`

Membuat window aplikasi dan menerima `body` berupa daftar VNode.

Properti utama:

- `title`, `width`, `height`, `position`.
- `resizable`, `fullscreen`, `hideTitleBar`, `alwaysOnTop`.
- `scrollable` untuk mengaktifkan scroll pada isi window.
- `onClose`, `onResize`, `onMaximize`, `onMinimize`.
- `style` dan `body`.

```ts
Widget({
  title: "Catatan",
  width: 720,
  height: 480,
  position: { x: 80, y: 80 },
  body: [Text("Isi aplikasi")],
});
```

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
  style: { padding: 20 },
});
```

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

Tombol mengambang di atas konten window. Letakkan langsung di `Widget.body`.
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

## Tabel

`Table` membuat tabel native. Kolom ditentukan melalui `columns`; setiap kolom
memerlukan `key` dan `title`. `width` memberi lebar awal, `minWidth` memberi
lebar minimum, dan `sortable` mengaktifkan pengurutan.

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
