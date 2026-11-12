import { IconProps, VNode } from "../types";

export function Icon(props: IconProps): VNode {
  if (!props.source.endsWith(".svg")) {
    throw new Error("Source must be svg file");
  }

  return {
    type: "Image",
    props: {
      source: props.source,
      alt: props.alt ?? "",
      resizeMode: props.resizeMode ?? "contain",
      style: {
        width: 24,
        height: 25,
        ...props.style,
      },
    },
  };
}
