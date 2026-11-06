import { WidgetProps, VNode } from "../types";

let eventCounter = 0;

const widgetCallbacks = new Map<string, (...args: number[]) => void>();

export function clearWidgetCallbacks(): void {
  widgetCallbacks.clear();
}

export function dispatchWidgetEvent(id: string, ...args: number[]): void {
  widgetCallbacks.get(id)?.(...args);
}

export function Widget(props: WidgetProps): VNode {
  const style = props.style ?? {};
  const events: Record<string, string> = {};

  if (props.onClose) {
    const id = `window_${++eventCounter}`;
    widgetCallbacks.set(id, props.onClose);
    events.close = id;
  }
  if (props.onResize) {
    const id = `window_${++eventCounter}`;
    widgetCallbacks.set(id, props.onResize);
    events.resize = id;
  }
  if (props.onMaximize) {
    const id = `window_${++eventCounter}`;
    widgetCallbacks.set(id, props.onMaximize);
    events.maximize = id;
  }
  if (props.onMinimize) {
    const id = `window_${++eventCounter}`;
    widgetCallbacks.set(id, props.onMinimize);
    events.minimize = id;
  }

  if (!props.hideTitleBar) {
    props.position = { x: props.position?.x ?? 0, y: 30 };
  }

  if (props.fullscreen) {
    props.position = { x: 0, y: 0 };
  }

  return {
    type: "Widget",
    props: {
      title: props.title ?? "Julia Native",
      width: style.width ?? props.width ?? 400,
      height: style.height ?? props.height ?? 300,
      position: props.position ?? { x: 0, y: 0 },
      resizable: props.resizable ?? true,
      fullscreen: props.fullscreen ?? false,
      hideTitleBar: props.hideTitleBar ?? false,
      alwaysOnTop: props.alwaysOnTop ?? false,
      scrollable: props.scrollable ?? false,
      style,
    },
    events,
    children: props.body,
  };
}
