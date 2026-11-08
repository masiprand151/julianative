import { ButtonProps, VNode } from "../types";

let eventCounter = 0;

const eventCallbacks = new Map<string, () => void>();

export function resetButtonEventCounter(): void {
  eventCounter = 0;
}

export function clearButtonCallbacks(): void {
  eventCallbacks.clear();
}

// Membuat VNode untuk Button dengan event click.
export function Button(props: ButtonProps): VNode {
  const eventId = `event_${++eventCounter}`;

  if (props.onClick) {
    eventCallbacks.set(eventId, props.onClick);
  }

  return {
    type: "Button",
    props,
    events: {
      ...(props.onClick ? { click: eventId } : {}),
    },
  };
}
// Mengirim event click ke C++.
export function dispatchEvent(id: string): void {
  const callback = eventCallbacks.get(id);

  if (!callback) return;

  callback();
}
