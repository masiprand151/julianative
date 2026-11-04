import { LayoutProps, VNode } from "../types";

export function Row(props: LayoutProps): VNode {
  const style = props.style ?? {};

  return {
    type: "Row",
    props: {
      spacing: props.spacing ?? style.gap ?? 0,
      padding: props.padding ?? style.padding ?? 0,
      style,
    },
    children: props.children ?? [],
  };
}
