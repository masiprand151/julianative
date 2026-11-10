import { DatePickerProps, VNode } from "../types";

let eventCounter = 0;

const changeCallbacks = new Map<string, (date: string) => void>();

export function DatePicker(props: DatePickerProps = {}): VNode {
  const eventId = `date_${++eventCounter}`;

  if (props.onChange) {
    changeCallbacks.set(eventId, props.onChange);
  }

  return {
    type: "DatePicker",
    props: {
      ...props,
      value: props.value ?? "",
      format: props.format ?? "yyyy-MM-dd",
    },
    events: {
      ...(props.onChange ? { change: eventId } : {}),
    },
  };
}

export function DatePickerDispatchChange(id: string, value: string): void {
  const callback = changeCallbacks.get(id);

  if (callback) {
    callback(value);
  }
}
