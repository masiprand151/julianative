# Daftar dan penyimpanan lokal

## FlatList

`FlatList` membuat daftar dari array. Gunakan `renderItem` untuk mengubah data
menjadi komponen.

```ts
FlatList({
  data: people,
  keyExtractor: (person) => person.id,
  renderItem: ({ item, index }) => Text(`${index + 1}. ${item.name}`),
  style: { height: 240 },
});
```

Properti utama:

| Properti | Fungsi |
| --- | --- |
| `data` | Data item; bertipe array readonly. |
| `renderItem` | Membuat VNode untuk setiap item yang perlu ditampilkan. |
| `keyExtractor` | Mengambil key stabil berupa string atau angka. |
| `horizontal` | Menggunakan orientasi horizontal jika `true`. |
| `scrollEnabled` | Mengaktifkan atau menonaktifkan scroll. |
| `estimatedItemSize` | Perkiraan ukuran item yang belum diukur, default 56 px. |
| `initialNumToRender` | Jumlah minimum item pada render awal, default 12. |
| `overscan` | Item tambahan sebelum dan sesudah viewport, default 3. |
| `ItemSeparatorComponent` | Membuat separator di antara item. |
| `ListEmptyComponent` | Konten yang ditampilkan jika data kosong. |
| `style` | Gaya area daftar; beri tinggi untuk daftar vertikal. |
| `contentContainerStyle` | Gaya container daftar, termasuk padding dan gap. |

### Key dan reconciliation

Berikan key unik dan stabil jika data bisa disisipkan, dihapus, atau diurutkan
ulang. Key tidak boleh duplikat. Jika `keyExtractor` tidak diberikan, indeks
array digunakan; key indeks kurang sesuai ketika urutan atau isi data berubah.

Item dengan key dan konten yang sama dapat dipertahankan saat urutan atau
jendela virtual berubah.

### Virtualisasi

Daftar merender item di sekitar viewport, bukan seluruh widget item sekaligus.
Atur tinggi `FlatList` vertikal atau lebar daftar horizontal agar native runtime
dapat menghitung viewport. `estimatedItemSize` hanya perkiraan bagi baris yang
belum diukur; item terlihat diukur oleh runtime. Baris dengan ukuran berbeda
dapat digunakan, namun estimasi yang mendekati ukuran mayoritas baris membantu
posisi scroll awal.

Virtualisasi mengurangi jumlah widget native aktif, tetapi array `data` tetap
berada di memori aplikasi.

### Separator dan daftar kosong

```ts
FlatList({
  data: [],
  renderItem: ({ item }) => Text(item.name),
  ListEmptyComponent: () => Text("Belum ada data"),
});
```

`ItemSeparatorComponent` menerima `leadingItem` dan `trailingItem`.

## LocalStorage

`LocalStorage` menyimpan data JSON secara asynchronous ke file dalam direktori
data aplikasi milik pengguna. Data bertahan ketika aplikasi dimulai ulang.

```ts
import { LocalStorage } from "@julia/core";

type Settings = { theme: "light" | "dark" };

async function saveSettings(settings: Settings) {
  await LocalStorage.setItem("settings", settings);
}

async function loadSettings() {
  return LocalStorage.getItem<Settings>("settings");
}
```

API:

- `getItem<T>(key)` mengembalikan `Promise<T | undefined>`; hasil `undefined`
  berarti key belum tersimpan.
- `setItem(key, value)` menyimpan nilai JSON dan mengembalikan
  `Promise<void>`.
- `removeItem(key)` menghapus satu key.
- `clear()` menghapus semua nilai.

Nilai harus dapat diserialisasi sebagai JSON: object, array, string, angka,
boolean, atau `null`. Fungsi, `undefined`, dan nilai non-JSON lainnya tidak
dapat disimpan. Gunakan `try/catch` saat operasi perlu menampilkan atau
menangani error:

```ts
try {
  await LocalStorage.setItem("settings", { theme: "dark" });
} catch (error) {
  console.error("Gagal menyimpan pengaturan:", error);
}
```

Generic `getItem<Settings>()` membantu pemeriksaan tipe saat compile saja; data
yang dibaca dari file tidak divalidasi terhadap bentuk `Settings` pada runtime.
Periksa nilai dari storage sebelum menggunakannya bila file dapat diubah di
luar aplikasi atau skemanya berubah.

Di Windows, file berada di direktori data aplikasi pengguna, umumnya di bawah
`%APPDATA%\\JuliaNative\\JuliaNative\\local-storage.json`. Lokasi persisnya
ditentukan oleh Qt dan dapat berbeda menurut platform. Data tidak otomatis
disimpan di samping executable portable.

Jika operasi gagal, promise ditolak. Tangani error dan periksa pesan beserta
path file; jangan menyembunyikan error. Hindari menyimpan password atau
rahasia tanpa mekanisme enkripsi yang sesuai.
