import { ViewProps, VNode } from "../types";

export function View(props: ViewProps): VNode {
  const style = props.style ?? {};

  return {
    type: "View",
    props: {
      spacing: props.spacing ?? style.gap ?? 0,
      padding: props.padding ?? style.padding ?? 0,
      direction: props.direction ?? style.flexDirection ?? "column",
      justifyContent: props.justifyContent ?? style.justifyContent,
      alignItems: props.alignItems ?? style.alignItems,
      flex: props.flex ?? style.flex,
      style,
      scrollable: props.scrollable ?? false,
      scrollDirection: props.scrollDirection ?? "auto",
    },
    children: props.children ?? [],
  };
}
