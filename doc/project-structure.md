# Struktur proyek

Berikut bagian proyek yang paling sering disentuh saat mengembangkan aplikasi.

| Path | Kegunaan |
| --- | --- |
| `src/main.ts` | Entry point UI: membuat jendela utama dan menjalankan aplikasi. |
| `src/screens/` | Modul layar aplikasi, misalnya `HomeScreen.ts`. |
| `src/navigation/` | Tipe route dan komponen navigasi aplikasi. |
| `src/styles.ts` | Contoh style bersama untuk aplikasi. |
| `app/core/components/` | Fungsi pembuat VNode untuk komponen UI. |
| `app/core/types/` | Tipe properti, style, dan node UI. |
| `app/core/renderer.ts` | Serialisasi UI, reconciliation, event, dan viewport daftar. |
| `app/core/navigation.ts` | Implementasi stack navigator. |
| `app/core/localStorage.ts` | API penyimpanan lokal dari TypeScript. |
| `app/JuliaRuntime.cpp` | Runtime Qt, proses Node, dan bridge pesan native. |
| `app/UiRenderer.cpp` | Pembuatan serta pembaruan widget Qt. |
| `app/Style.cpp` | Konversi style menjadi aturan dan ukuran Qt. |
| `tests/` | Tes otomatis TypeScript dan komponen. |
| `doc/` | Dokumentasi yang dipisah berdasarkan topik. |

## Alur render UI

1. Fungsi aplikasi seperti `Main()` membuat VNode melalui `Widget`, `View`,
   `Text`, dan komponen lainnya.
2. `Application.run(Main)` memulai proses render.
3. Renderer TypeScript menyerialisasi VNode ke JSON dan mengirimkannya ke
   runtime native.
4. Runtime Qt membangun widget dan layout yang sesuai.
5. Aksi pengguna seperti klik, perubahan input, scroll, atau event jendela
   dikirim kembali ke proses TypeScript.
6. Perubahan state memicu render ulang. Perubahan teks tertentu dapat
   diterapkan sebagai patch tanpa membangun ulang seluruh window.

## Memisahkan layar

Untuk aplikasi dengan beberapa layar, jaga `src/main.ts` tetap berisi
konfigurasi aplikasi dan navigasi, kemudian letakkan isi layar di
`src/screens/`.

Contoh prop layar yang dikelompokkan:

```ts
interface HomeScreenProps {
  navigation: StackNavigation<AppRoutes>;
  state: {
    count: number;
    setCount: StateSetter<number>;
  };
}

export function HomeScreen({ navigation, state }: HomeScreenProps) {
  return Text(`Jumlah: ${state.count}`);
}
```

Mengelompokkan data yang berhubungan ke object `state` atau `options` membuat
fungsi lebih mudah dibaca daripada daftar parameter panjang.
