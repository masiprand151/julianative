import { SwitchProps, VNode } from "../types";

let eventCounter = 0;

const changeCallbacks = new Map<string, (value: boolean) => void>();

export function resetSwitchEventCounter(): void {
  eventCounter = 0;
}

export function clearSwitchCallbacks(): void {
  changeCallbacks.clear();
}

export function Switch(props: SwitchProps = {}): VNode {
  const eventId = `switch_${++eventCounter}`;

  if (props.onChange) {
    changeCallbacks.set(eventId, props.onChange);
  }

  return {
    type: "Switch",
    props: {
      ...props,
      checked: props.checked ?? false,
    },
    events: {
      ...(props.onChange ? { change: eventId } : {}),
    },
  };
}

export function switchDispatchChange(id: string, checked: boolean): void {
  const callback = changeCallbacks.get(id);

  if (callback) {
    callback(checked);
  }
}
