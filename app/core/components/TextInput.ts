import { TextInputProps, VNode } from "../types";

let eventCounter = 0;

const changeCallbacks = new Map<string, (value: string) => void>();

export function clearTextInputCallbacks(): void {
  changeCallbacks.clear();
}

export function TextInput(props: TextInputProps = {}): VNode {
  const eventId = `input_${++eventCounter}`;

  if (props.onChange) {
    changeCallbacks.set(eventId, props.onChange);
  }

  return {
    type: "TextInput",
    props: {
      ...props,
      type: props.type ?? "text",
    },
    events: {
      ...(props.onChange ? { change: eventId } : {}),
    },
  };
}

export function dispatchChange(id: string, value: string): void {
  const callback = changeCallbacks.get(id);

  if (callback) {
    callback(value);
  }
}
