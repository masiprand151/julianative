import { LayoutProps, VNode } from "../types";

export function Row(props: LayoutProps): VNode {
  return {
    type: "Row",
    props: {
      spacing: props.spacing ?? 0,
      padding: props.padding ?? 0,
    },
    children: props.children ?? [],
  };
}
