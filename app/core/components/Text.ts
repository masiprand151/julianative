import { TextProps, VNode } from "../types";

export function Text(text: string | number, props: TextProps = {}): VNode {
  return {
    type: "Text",
    props: {
      text: String(text),
      fontSize: props.fontSize ?? 14,
      fontWeight: props.fontWeight ?? "normal",
      foregroundColor: props.foregroundColor ?? "#000000",
    },
  };
}
