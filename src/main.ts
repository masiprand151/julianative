import { Application, useState } from "../packages/core";
import {
  Button,
  Builder,
  Card,
  Column,
  Text,
} from "../packages/core/components";
import { styles } from "./styles";

function Main() {
  const [count, setCount] = useState(0);

  return Builder({
    title: "JuliaNative",
    width: 520,
    height: 380,
    position: "center",
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
              color: "#dde7e5",
            },
          }),
          Text("Make a native app for everyone with JuliaNative", {
            style: { color: "#dde2e1" },
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
