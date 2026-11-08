# Struktur proyek

Berikut bagian proyek yang paling sering disentuh saat mengembangkan aplikasi.

| Path | Kegunaan |
| --- | --- |
| `src/main.ts` | Entry point dan layar utama; membuat komponen lalu menjalankan aplikasi. |
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

1. Fungsi aplikasi seperti `Main()` membuat VNode melalui `Builder`, `View`,
   `Text`, dan komponen lainnya.
2. `Application.run(Main)` memulai proses render.
3. Renderer TypeScript menyerialisasi VNode ke JSON dan mengirimkannya ke
   runtime native.
4. Runtime Qt membangun widget dan layout yang sesuai.
5. Aksi pengguna seperti klik, perubahan input, scroll, atau event jendela
   dikirim kembali ke proses TypeScript.
6. Perubahan state memicu render ulang. Perubahan teks tertentu dapat
   diterapkan sebagai patch tanpa membangun ulang seluruh window.

## Entry point aplikasi

Saat ini contoh aplikasi menyusun tampilan utama langsung di `src/main.ts`.
State penghitung dikelola di fungsi `Main()`. Pola ini cocok untuk aplikasi
kecil atau contoh komponen. Fitur `useEffect` dan `LocalStorage` tersedia di
API, tetapi contoh `Main()` saat ini tidak memuat atau menyimpan penghitung
secara persisten.

Jika aplikasi berkembang, UI dapat dipisahkan ke modul komponen atau screen
tersendiri dan diimpor dari `main.ts`. Contoh prop dengan object:

```ts
interface DashboardProps {
  state: {
    count: number;
    setCount: StateSetter<number>;
  };
}

function Dashboard({ state }: DashboardProps) {
  return Text(`Jumlah: ${state.count}`);
}
```

Mengelompokkan data yang berhubungan ke object `state` atau `options` membuat
fungsi lebih mudah dibaca daripada daftar parameter panjang.
