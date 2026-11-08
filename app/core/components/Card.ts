import { StyleProps, VNode } from "../types";
import { Text } from "./Text";
import { View } from "./View";

type CardProps = {
  title?: string;
  style?: StyleProps;
  headerStyle?: StyleProps;
  contentStyle?: StyleProps;
  children?: VNode[];
};

export function Card(props: CardProps): VNode {
  const children = props.children ?? [];
  const title = props.title ?? "Card Title";

  return View({
    style: {
      gap: 8,
      backgroundColor: "#ffffff",
      borderColor: "#e5e7eb",
      borderWidth: 1,
      borderRadius: 8,
      ...props.style,
    },
    children: [
      View({
        style: {
          padding: { top: 8, right: 12, bottom: 8, left: 12 },
          borderBottom: "2px solid #d5e0dc",
          ...props.headerStyle,
        },
        children: [
          Text(title, {
            style: {
              fontSize: 18,
              fontWeight: "bold",
              color: "#193b35",
            },
          }),
        ],
      }),
      View({
        style: {
          padding: { top: 0, right: 12, bottom: 12, left: 12 },
          ...props.contentStyle,
        },
        children: children,
      }),
    ],
  });
}
