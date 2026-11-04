import { TextProps, VNode } from "../types";

export function Text(text: string | number, props: TextProps = {}): VNode {
  const style = props.style ?? {};

  return {
    type: "Text",
    props: {
      text: String(text),
      fontSize: style.fontSize ?? props.fontSize ?? 14,
      fontWeight: style.fontWeight ?? props.fontWeight ?? "normal",
      foregroundColor: style.color ?? props.foregroundColor ?? "#000000",
      style,
    },
  };
}
