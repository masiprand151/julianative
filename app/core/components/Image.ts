import { ImageProps, VNode } from "../types";

export function Image(props: ImageProps): VNode {
  return {
    type: "Image",
    props: {
      source: props.source,
      alt: props.alt ?? "",
      resizeMode: props.resizeMode ?? "contain",
      style: props.style ?? {},
    },
  };
}
