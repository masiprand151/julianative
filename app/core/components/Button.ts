import { ButtonProps, VNode } from "../types";

export function Button(props: ButtonProps): VNode {
  return {
    type: "Button",

    props: {
      text: props.text,
    },

    events: {
      ...(props.onClick ? { click: props.onClick } : {}),
    },
  };
}
