import { RadioButtonProps, VNode } from "../types";

let eventCounter = 0;

const changeCallbacks = new Map<string, (checked: boolean) => void>();

export function resetRadioEventCounter(): void {
  eventCounter = 0;
}

export function clearRadioCallbacks(): void {
  changeCallbacks.clear();
}

export function RadioButton(props: RadioButtonProps = {}): VNode {
  const eventId = `radio_${++eventCounter}`;

  if (props.onChange) {
    changeCallbacks.set(eventId, props.onChange);
  }

  return {
    type: "RadioButton",
    props: {
      ...props,
      checked: props.checked ?? false,
    },
    events: {
      ...(props.onChange ? { change: eventId } : {}),
    },
  };
}

export function radioButtonDispatchChange(id: string, checked: boolean): void {
  const callback = changeCallbacks.get(id);

  if (callback) {
    callback(checked);
  }
}
