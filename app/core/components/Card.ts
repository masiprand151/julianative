import { StyleProps, VNode } from "../types";
import { Text } from "./Text";
import { View } from "./View";

type CardProps = {
  title?: string;
  style?: StyleProps;
  children?: VNode[];
};

export function Card(props: CardProps): VNode {
  const children = props.children ?? [];
  const title = props.title ?? "Card Title";

  return View({
    style: {
      padding: 16,
      gap: 8,
      backgroundColor: "#ffffff",
      borderColor: "#e5e7eb",
      borderWidth: 1,
      borderRadius: 8,
      ...props.style,
    },
    children: [
      Text(title, {
        style: {
          fontSize: 18,
          fontWeight: "bold",
          color: "#193b35",
        },
      }),
      ...children,
    ],
  });
}
