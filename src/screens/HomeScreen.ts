import { StateSetter } from "@julia/core";
import {
  View,
  Column,
  Row,
  Text,
  TextInput,
  Button,
  Image,
  FloatingActionButton,
  Card,
  FlatList,
  Modal,
  Table,
} from "@julia/core/components";
import { StackNavigation } from "../../app/core/navigation";
import { RootStackParamList } from "../navigation/types";
import { styles } from "../styles";

interface HomeScreenProps {
  navigation: StackNavigation<RootStackParamList>;
  state: {
    count: number;
    setCount: StateSetter<number>;
    email: string;
    setEmail: StateSetter<string>;
    modalVisible: boolean;
    setModalVisible: StateSetter<boolean>;
  };
}

export function HomeScreen({ navigation, state }: HomeScreenProps) {
  const { count, setCount, email, setEmail, modalVisible, setModalVisible } =
    state;

  return Column({
    style: {
      gap: 14,
      padding: { top: 0, right: 0, bottom: 18, left: 18 },
      backgroundColor: "#e8f0ee",
      borderColor: "#b8cbc5",
      borderWidth: 1,
      borderRadius: 8,
    },
    children: [
      Text("Style semua komponen", {
        style: { fontSize: 24, fontWeight: "bold", color: "#193b35" },
      }),
      Text("Widget, Column, Row, View, Text, Button", {
        style: { fontSize: 14, color: "#45635c" },
      }),
      Card({
        title: "Gambar lokal dan online",
        children: [
          Row({
            style: { gap: 16, alignItems: "center" },
            children: [
              Column({
                style: { gap: 6 },
                children: [
                  Text("SVG lokal (offline)"),
                  Image({
                    source: "assets/demo.svg",
                    alt: "Ilustrasi lokal",
                    resizeMode: "contain",
                    style: { width: 220, height: 120 },
                  }),
                ],
              }),
              Column({
                style: { gap: 6 },
                children: [
                  Text("JPG online"),
                  Image({
                    source:
                      "https://images.unsplash.com/photo-1500530855697-b586d89ba3ee?w=480&q=80",
                    alt: "Pemandangan dari Unsplash",
                    resizeMode: "cover",
                    style: { width: 220, height: 120 },
                  }),
                ],
              }),
            ],
          }),
        ],
      }),
      Card({
        title: "Contoh Card",
        children: [
          Text("Card dapat menampung komponen lain."),
          Button({ text: "Aksi Card", style: styles.primaryButton }),
        ],
      }),
      Text("Contoh View Scrollable", {
        style: { fontWeight: "bold", color: "#193b35" },
      }),
      View({
        scrollable: true,
        scrollDirection: "vertical",
        style: {
          height: 180,
          padding: { top: 12, right: 12, bottom: 12, left: 12 },
          gap: 8,
          backgroundColor: "#ffffff",
          borderColor: "#9bbab0",
          borderWidth: 1,
          borderRadius: 6,
        },
        children: [
          Text("Konten ini berada di dalam View dengan scroll vertikal."),
          ...Array.from({ length: 8 }, (_, index) => Text(`Item ${index + 1}`)),
        ],
      }),
      FlatList({
        data: [
          "Ada Lovelace",
          "Grace Hopper",
          "Katherine Johnson",
          "Margaret Hamilton",
        ],
        keyExtractor: (person) => person,
        renderItem: ({ item, index }) =>
          Text(`${index + 1}. ${item}`, {
            style: { color: "#193b35" },
          }),
        ItemSeparatorComponent: () =>
          View({ style: { height: 1, backgroundColor: "#d5e0dc" } }),
        style: {
          height: 150,
          backgroundColor: "#ffffff",
          borderColor: "#9bbab0",
          borderWidth: 1,
          borderRadius: 6,
        },
        contentContainerStyle: {
          gap: 8,
          padding: { top: 12, right: 12, bottom: 12, left: 12 },
        },
      }),
      Table({
        data: [
          { name: "Ada Lovelace", role: "Mathematician", year: 1843 },
          { name: "Grace Hopper", role: "Computer scientist", year: 1952 },
          { name: "Katherine Johnson", role: "Mathematician", year: 1953 },
          { name: "Margaret Hamilton", role: "Software engineer", year: 1961 },
        ],
        columns: [
          { key: "name", title: "Name", width: 180 },
          { key: "role", title: "Role", width: 240 },
          { key: "year", title: "Year" },
        ],
        style: {
          height: 240,
          backgroundColor: "#ffffff",
          borderColor: "#9bbab0",
          borderWidth: 1,
          borderRadius: 6,
        },
        headerStyle: { backgroundColor: "#dce9e5" },
        headerTextStyle: { color: "#193b35" },
      }),
      Button({
        text: "Buka Modal",
        onClick: () => setModalVisible(true),
        style: styles.primaryButton,
      }),
      Modal({
        visible: modalVisible,
        title: "Contoh Modal",
        width: 420,
        height: 220,
        onClose: () => setModalVisible(false),
        style: {
          padding: { top: 20, right: 20, bottom: 20, left: 20 },
          gap: 12,
          backgroundColor: "#ffffff",
        },
        children: [
          Text("Ini adalah dialog modal native."),
          Button({
            text: "Tutup",
            onClick: () => setModalVisible(false),
            style: styles.primaryButton,
          }),
        ],
      }),
      Text(`Count: ${count}`),
      Button({
        text: "Increment",
        onClick: () => setCount((currentCount) => currentCount + 1),
        style: styles.primaryButton,
      }),
      TextInput({
        type: "email",
        placeholder: "Email",
        value: email,
        onChange: setEmail,
        style: styles.textInput,
      }),
      Text(`Email: ${email}`),
      TextInput({
        type: "password",
        placeholder: "Password",
        style: styles.textInput,
      }),
      TextInput({
        type: "number",
        placeholder: "Age",
        onChange: (value) => console.log("Number:", value),
        style: styles.textInput,
      }),
      Row({
        style: {
          gap: 12,
          alignItems: "center",
          justifyContent: "center",
        },
        children: [
          Button({ text: "Styled Button", style: styles.primaryButton }),
          View({
            style: {
              width: 170,
              height: 72,
              padding: { top: 8, right: 8, bottom: 8, left: 8 },
              backgroundColor: "#ffffff",
              borderColor: "#9bbab0",
              borderWidth: 1,
              borderRadius: 6,
              justifyContent: "center",
              alignItems: "center",
            },
            children: [
              Text("Styled View", {
                style: { color: "#193b35", fontWeight: "bold" },
              }),
            ],
          }),
        ],
      }),
      Row({
        style: { gap: 8 },
        children: [
          Button({
            text: "Profil",
            onClick: () =>
              navigation.navigate("Profile", {
                name: "Julian",
                email,
              }),
            style: styles.primaryButton,
          }),
          Button({
            text: "Pengaturan",
            onClick: () =>
              navigation.navigate("Settings", {
                section: "Preferensi aplikasi",
              }),
            style: styles.primaryButton,
          }),
          Button({
            text: "Detail",
            onClick: () =>
              navigation.navigate("Details", {
                from: "Beranda",
                itemId: 42,
                email,
              }),
            style: styles.primaryButton,
          }),
        ],
      }),
      FloatingActionButton({
        icon: "+",
        imageSource: "assets/demo.svg",
        accessibilityLabel: "Tambah",
        onPress: () => setCount((currentCount) => currentCount + 1),
      }),
    ],
  });
}
