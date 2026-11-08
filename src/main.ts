import { Application, useState } from "@julia/core";
import { Button, Card, Column, Text, Widget } from "@julia/core/components";
import { styles } from "./styles";

function Main() {
  const [count, setCount] = useState(0);

  return Widget({
    title: "JuliaNative",
    width: 520,
    height: 380,
    position: "center",
    style: {
      padding: { top: 24, right: 24, bottom: 24, left: 24 },
      backgroundColor: "#f5f7f6",
    },
    body: [
      Column({
        style: {
          gap: 20,
          padding: { top: 12, right: 12, bottom: 12, left: 12 },
          alignItems: "stretch",
        },
        children: [
          Text("Welcome", {
            style: {
              fontSize: 24,
              fontWeight: "bold",
              color: "#193b35",
            },
          }),
          Text("Make a native app for everyone with JuliaNative", {
            style: { color: "#45635c" },
          }),
          Card({
            title: "Counter",
            style: {
              backgroundColor: "#ffffff",
              borderColor: "#d5e0dc",
            },
            children: [
              Text(String(count), {
                style: {
                  fontSize: 20,
                  fontWeight: "bold",
                  color: "#193b35",
                },
              }),
              Button({
                text: "add number",
                onClick: () => setCount((currentCount) => currentCount + 1),
                style: styles.primaryButton,
              }),
            ],
          }),
        ],
      }),
    ],
  });
}

Application.run(Main);
