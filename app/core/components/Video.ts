import { VNode, VideoProps } from "../types";

export function Video(props: VideoProps): VNode {
  return {
    type: "Video",
    props: {
      source: props.source,
      autoplay: props.autoplay ?? false,
      loop: props.loop ?? false,
      muted: props.muted ?? false,
      style: props.style ?? {},
    },
  };
}
