import { TextProps, VNode } from "../types";

export function Text(props: TextProps): VNode {
  return {
    type: "Text",
    props: {
      text: props.text,
      fontSize: props.fontSize ?? 14,
      fontWeight: props.fontWeight ?? "normal",
      foregroundColor: props.foregroundColor ?? "#000000",
    },
  };
}
