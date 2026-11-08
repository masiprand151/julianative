import { ModalProps, VNode } from "../types";

let eventCounter = 0;

const closeCallbacks = new Map<string, () => void>();

export function resetModalEventCounter(): void {
  eventCounter = 0;
}

export function clearModalCallbacks(): void {
  closeCallbacks.clear();
}

export function Modal(props: ModalProps): VNode {
  const events: Record<string, string> = {};

  if (props.onClose) {
    const id = `modal_${++eventCounter}`;
    closeCallbacks.set(id, props.onClose);
    events.close = id;
  }

  return {
    type: "Modal",
    props: {
      ...props,
      visible: props.visible ?? false,
      title: props.title ?? "",
    },
    events,
    children: props.children ?? [],
  };
}

export function dispatchModalClose(id: string): void {
  const callback = closeCallbacks.get(id);
  if (!callback) return;

  closeCallbacks.delete(id);
  callback();
}
