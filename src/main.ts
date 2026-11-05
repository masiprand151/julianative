import { Application } from "@julia/core";
import {
  View,
  Widget,
  Column,
  Row,
  Text,
  TextInput,
  Button,
} from "@julia/core/components";

function Main() {
  return Widget({
    title: "Style Test",
    width: 1024,
    height: 768,
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
          TextInput({
            type: "email",
            placeholder: "Email",
            style: {
              padding: 8,
              borderColor: "#9bbab0",
              borderWidth: 1,
              borderRadius: 6,
            },
          }),
          TextInput({
            type: "password",
            placeholder: "Password",
            style: {
              padding: 8,
              borderColor: "#9bbab0",
              borderWidth: 1,
              borderRadius: 6,
            },
          }),
          TextInput({
            type: "number",
            placeholder: "Age",
            onChange: (value) => console.log("Number:", value),
            style: {
              padding: 8,
              borderColor: "#9bbab0",
              borderWidth: 1,
              borderRadius: 6,
            },
          }),
          Row({
            style: {
              gap: 12,
              alignItems: "center",
            },
            children: [
              Button({
                text: "Styled Button",
                style: {
                  padding: 10,
                  backgroundColor: "#147d72",
                  color: "#ffffff",
                  borderRadius: 6,
                },
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
