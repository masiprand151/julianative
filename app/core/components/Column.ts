import { LayoutProps, VNode } from "../types";

export function Column(props: LayoutProps): VNode {
  return {
    type: "Column",
    props: {
      spacing: props.spacing ?? 0,
      padding: props.padding ?? 0,
    },
    children: props.children ?? [],
  };
}
