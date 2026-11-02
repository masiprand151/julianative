import { WidgetProps, VNode } from "../types";

export function Widget(props: WidgetProps): VNode {
  return {
    type: "Widget",
    props: {
      title: props.title ?? "Julia Native",
      width: props.width ?? 400,
      height: props.height ?? 300,
    },
    children: props.body,
  };
}
