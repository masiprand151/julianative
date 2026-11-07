import { Application, useState } from "@julia/core";
import {
  View,
  Widget,
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
  StyleSheet,
} from "@julia/core/components";

const styles = StyleSheet.create({
  primaryButton: {
    padding: "10px",
    backgroundColor: "#147d72",
    color: "#ffffff",
    borderRadius: "6px",
    cursor: "pointer",
    ":hover": {
      backgroundColor: "#282b2a",
    },
    ":pressed": {
      backgroundColor: "#0a4f48",
    },
  },
  textInput: {
    padding: "8px",
    backgroundColor: "#ffffff",
    border: "1px solid #9bbab0",
    borderRadius: "6px",
    ":hover": {
      borderColor: "#147d72",
    },
    ":focus": {
      borderColor: "#0f655c",
    },
  },
});

function Main() {
  const [count, setCount] = useState(0);
  const [email, setEmail] = useState("");
  const [modalVisible, setModalVisible] = useState(false);

  return Widget({
    title: "Style Test",
    width: 1024,
    height: 768,
    position: { x: 0, y: 0 },
    fullscreen: false,
    resizable: true,
    hideTitleBar: false,
    alwaysOnTop: false,
    onClose: () => console.log("Window closed"),
    onResize: (width, height) =>
      console.log(`Window resized: ${width}x${height}`),
    onMaximize: () => console.log("Window maximized"),
    onMinimize: () => console.log("Window minimized"),
    style: {
      padding: 18,
      gap: 12,
      backgroundColor: "#f5f7f6",
    },

    body: [
      Column({
        style: {
          gap: 14,
          padding: 18,
          backgroundColor: "#e8f0ee",
          borderColor: "#b8cbc5",
          borderWidth: 1,
          borderRadius: 8,
        },
        children: [
          Text("Style semua komponen", {
            style: {
              fontSize: 24,
              fontWeight: "bold",
              color: "#193b35",
            },
          }),
          Text("Widget, Column, Row, View, Text, Button", {
            style: {
              fontSize: 14,
              color: "#45635c",
            },
          }),
          Card({
            title: "Gambar lokal dan online",
            children: [
              Row({
                style: {
                  gap: 16,
                  alignItems: "center",
                },
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
              Button({
                text: "Aksi Card",
                style: styles.primaryButton,
              }),
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
              padding: 12,
              gap: 8,
              backgroundColor: "#ffffff",
              borderColor: "#9bbab0",
              borderWidth: 1,
              borderRadius: 6,
            },
            children: [
              Text("Konten ini berada di dalam View dengan scroll vertikal."),
              Text("Item 1"),
              Text("Item 2"),
              Text("Item 3"),
              Text("Item 4"),
              Text("Item 5"),
              Text("Item 6"),
              Text("Item 7"),
              Text("Item 8"),
            ],
          }),
          FlatList({
            data: [
              "Ada Lovelace",
              "Grace Hopper",
              "Katherine Johnson",
              "Margaret Hamilton",
            ],
            renderItem: ({ item, index }) =>
              Text(`${index + 1}. ${item}`, {
                style: { color: "#193b35" },
              }),
            ItemSeparatorComponent: () =>
              View({
                style: {
                  height: 1,
                  backgroundColor: "#d5e0dc",
                },
              }),
            style: {
              height: 150,
              backgroundColor: "#ffffff",
              borderColor: "#9bbab0",
              borderWidth: 1,
              borderRadius: 6,
            },
            contentContainerStyle: {
              gap: 8,
              padding: 12,
            },
          }),
          Table({
            data: [
              { name: "Ada Lovelace", role: "Mathematician", year: 1843 },
              { name: "Grace Hopper", role: "Computer scientist", year: 1952 },
              { name: "Katherine Johnson", role: "Mathematician", year: 1953 },
              {
                name: "Margaret Hamilton",
                role: "Software engineer",
                year: 1961,
              },
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
            headerStyle: {
              backgroundColor: "#dce9e5",
            },
            headerTextStyle: {
              color: "#193b35",
            },
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
              padding: 20,
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
              Button({
                text: "Styled Button",
                style: styles.primaryButton,
              }),
              View({
                style: {
                  width: 170,
                  height: 72,
                  padding: 8,
                  backgroundColor: "#ffffff",
                  borderColor: "#9bbab0",
                  borderWidth: 1,
                  borderRadius: 6,
                  justifyContent: "center",
                  alignItems: "center",
                },
                children: [
                  Text("Styled View", {
                    style: {
                      color: "#193b35",
                      fontWeight: "bold",
                    },
                  }),
                ],
              }),
            ],
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

Application.run(Main);
