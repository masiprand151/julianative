import { ProgressBarProps, VNode } from "../types";

let eventCounter = 0;

const changeCallbacks = new Map<string, (value: number) => void>();

export function resetProgressBarEventCounter(): void {
  eventCounter = 0;
}

export function clearProgressBarCallbacks(): void {
  changeCallbacks.clear();
}

export function ProgressBar(props: ProgressBarProps = {}): VNode {
  const eventId = `progress_${++eventCounter}`;

  if (props.onChange) {
    changeCallbacks.set(eventId, props.onChange);
  }

  return {
    type: "ProgressBar",
    props: {
      ...props,
      value: props.value ?? 0,
      minimum: props.minimum ?? 0,
      maximum: props.maximum ?? 100,
      textVisible: props.textVisible ?? false,
      seekable: props.seekable ?? false,
    },
    events: {
      ...(props.onChange ? { change: eventId } : {}),
    },
  };
}

export function ProgressBarDispatchChange(id: string, value: number): void {
  const callback = changeCallbacks.get(id);

  if (callback) {
    callback(value);
  }
}
