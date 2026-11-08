# State dan navigasi

## State dengan `useState`

`useState` menyimpan nilai antar render. Pemanggilan setter meminta render baru
pada window yang sama.

```ts
import { useState } from "@julia/core";
import { Button, Column, Text } from "@julia/core/components";

function Counter() {
  const [count, setCount] = useState(0);

  return Column({
    children: [
      Text(`Jumlah: ${count}`),
      Button({
        text: "Tambah",
        onClick: () => setCount((current) => current + 1),
      }),
    ],
  });
}
```

Panggil hook dengan urutan yang sama setiap render. Jangan meletakkan
`useState` di dalam kondisi atau loop. Untuk mengubah nilai berdasarkan nilai
sebelumnya, gunakan updater function seperti contoh di atas.

## Render ulang

Perubahan teks `Text`, teks `Button`, atau nilai `TextInput` dapat dikirim
sebagai patch ke widget yang sudah ada. Perubahan struktur lain dapat
membangun ulang bagian native UI.

## Stack navigation

Definisikan daftar route dan tipe param agar pemanggilan navigasi diperiksa
TypeScript:

```ts
type AppRoutes = {
  Home: undefined;
  Details: { itemId: number };
};

const Stack = createStackNavigator<AppRoutes>();
```

Buat navigator di dalam `Widget.body`:

```ts
Stack.Navigator({
  initialRouteName: "Home",
  screens: {
    Home: {
      options: { title: "Beranda" },
      component: ({ navigation }) =>
        Button({
          text: "Lihat detail",
          onClick: () => navigation.navigate("Details", { itemId: 42 }),
        }),
    },
    Details: {
      component: ({ route }) => Text(`ID: ${route.params.itemId}`),
    },
  },
});
```

Setiap screen menerima `navigation` dan `route`. Param dibaca dari
`route.params`. Aksi navigasi yang tersedia:

- `navigate(name, params?)`: membuka route; route yang sudah ada dalam stack
  dibawa ke bagian atas.
- `push(name, params?)`: mendorong route baru ke stack.
- `goBack()`: kembali satu route.
- `replace(name, params?)`: mengganti route aktif.
- `popToTop()`: kembali ke route pertama.

## Header

`screenOptions` mengatur header default untuk semua screen. Opsi pada screen
dapat menimpanya. Gunakan `headerShown: false` untuk menyembunyikan header,
`headerStyle` untuk container header, atau `headerComponent` untuk membuat
header sendiri.

Header custom menerima `navigation`, `route`, `title`, dan `canGoBack`.

## Menyusun props screen

Jika screen membutuhkan banyak nilai terkait, kelompokkan nilainya dalam
object:

```ts
HomeScreen({
  navigation,
  state: { count, setCount, email, setEmail },
});
```

Ini menjaga signature tetap ringkas ketika screen berkembang.
