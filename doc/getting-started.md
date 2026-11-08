# Mulai menggunakan JuliaNative

Panduan ini membantu menyiapkan proyek dan menjalankan aplikasi lokal.

## Kebutuhan

- Node.js dan npm.
- CMake 3.21 atau lebih baru.
- Compiler C++17 yang didukung oleh CMake.
- Qt 6 dengan modul Core, Gui, Widgets, Network, Svg, Multimedia, dan
  MultimediaWidgets.
- Ninja jika ingin memakai konfigurasi build yang digunakan skrip proyek.

Pastikan tool tersedia di terminal:

```powershell
node --version
npm --version
cmake --version
```

Compiler dan Qt harus dapat ditemukan oleh CMake. Konfigurasi lokal mungkin
perlu menyesuaikan `CMAKE_PREFIX_PATH`, compiler, atau generator sesuai
instalasi Qt dan sistem operasi.

## Pasang dependency JavaScript

Dari root repository:

```powershell
npm install
```

Jika PowerShell Windows memblokir skrip `npm.ps1`, jalankan shim Windows:

```powershell
npm.cmd install
```

## Jalankan aplikasi

Skrip berikut membangun TypeScript, mengonfigurasi CMake, membangun aplikasi
native, lalu menjalankannya:

```powershell
npm.cmd run app
```

Untuk menjalankan mode development dengan hot reload:

```powershell
npm.cmd run dev
```

Hot reload memperbarui UI ketika file TypeScript pada `src` atau `app/core`
berubah. Perubahan C++ memerlukan build ulang dan restart aplikasi.

## Periksa TypeScript dan tes

```powershell
npm.cmd run check:ts
npm.cmd test
```

`check:ts` menjalankan pemeriksaan tipe tanpa menghasilkan output. `test`
membangun TypeScript lalu menjalankan tes komponen, renderer, navigasi, dan
penyimpanan.

## Bentuk paling sederhana

```ts
import { Application } from "@julia/core";
import { Builder, Button, Text, View } from "@julia/core/components";

function Main() {
  return Builder({
    title: "Aplikasi pertama",
    width: 480,
    height: 320,
    body: [
      View({
        style: { padding: 16, gap: 12 },
        children: [
          Text("Halo dari JuliaNative"),
          Button({ text: "Tombol" }),
        ],
      }),
    ],
  });
}

Application.run(Main);
```

`Builder` adalah jendela utama aplikasi. Komponen lain di `body` menjadi isi
jendela tersebut.
