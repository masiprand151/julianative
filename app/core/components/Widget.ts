import { WidgetProps, VNode } from "../types";

export function Widget(props: WidgetProps): VNode {
  const style = props.style ?? {};

  return {
    type: "Widget",
    props: {
      title: props.title ?? "Julia Native",
      width: style.width ?? props.width ?? 400,
      height: style.height ?? props.height ?? 300,
      style,
    },
    children: props.body,
  };
}
