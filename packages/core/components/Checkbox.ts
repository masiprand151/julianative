import type { CheckboxProps, VNode } from "../types";

let eventCounter = 0;

const changeCallbacks = new Map<string, (value: boolean) => void>();

export function resetCheckboxEventCounter(): void {
  eventCounter = 0;
}

export function clearCheckboxCallbacks(): void {
  changeCallbacks.clear();
}

export function Checkbox(props: CheckboxProps): VNode {
  const eventId = `ceckbox_${++eventCounter}`;

  if (props.onChange) {
    changeCallbacks.set(eventId, props.onChange);
  }

  return {
    type: "Checkbox",
    props,
    events: {
      ...(props.onChange ? { change: eventId } : {}),
    },
  };
}

export function checkboxDispatchChange(id: string, value: boolean): void {
  const callback = changeCallbacks.get(id);

  if (callback) {
    callback(value);
  }
}
