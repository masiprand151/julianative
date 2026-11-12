import { SelectProps, VNode } from "../types";

let eventCounter = 0;

const changeCallbacks = new Map<string, (value: string) => void>();

export function resetSelectEventCounter(): void {
  eventCounter = 0;
}

export function clearSelectCallbacks(): void {
  changeCallbacks.clear();
}

export function Select(props: SelectProps): VNode {
  const eventId = `select_${++eventCounter}`;

  if (props.onChange) {
    changeCallbacks.set(eventId, props.onChange);
  }
  return {
    type: "Select",
    props: {
      options: props.options ?? [],
      value: props.value ?? "",
      placeholder: props.placeholder ?? "Select...",
      disabled: props.disabled ?? false,
      style: props.style ?? {},
    },
    events: {
      ...(props.onChange ? { change: eventId } : {}),
    },
  };
}

export function SelectDispatchChange(id: string, value: string): void {
  const callback = changeCallbacks.get(id);

  if (callback) {
    callback(value);
  }
}
