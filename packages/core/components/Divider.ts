import { DividerProps, VNode } from "../types";

export function Divider(props: DividerProps): VNode {
  return {
    type: "Divider",
    props: {
      color: props.color ?? "#D9D9D9",
      thickness: props.thickness ?? 1,
      orientation: props.orientation ?? "horizontal",
      spacing: props.spacing ?? 0,
    },
  };
}
