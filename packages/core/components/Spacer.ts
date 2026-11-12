import { SpacerProps, VNode } from "../types";

export function Spacer(props: SpacerProps = {}): VNode {
  return {
    type: "Spacer",
    props: {
      ...props,
    },
  };
}
