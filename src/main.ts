import { Application, useState } from "@julia/core";
import {
  View,
  Widget,
  Column,
  Row,
  Text,
  TextInput,
  Button,
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

  return Widget({
    title: "Style Test",
    width: 1024,
    height: 768,
    position: { x: 0, y: 0 },
    fullscreen: false,
    resizable: false,
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
    ],
  });
}

Application.run(Main);
