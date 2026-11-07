import { FloatingActionButtonProps, VNode } from "../types";

let eventCounter = 0;
const pressCallbacks = new Map<string, () => void>();

export function clearFloatingActionButtonCallbacks(): void {
  pressCallbacks.clear();
}

export function dispatchFloatingActionButtonPress(id: string): void {
  pressCallbacks.get(id)?.();
}

export function FloatingActionButton(
  props: FloatingActionButtonProps,
): VNode {
  const eventId = `floating_action_${++eventCounter}`;

  if (props.onPress) {
    pressCallbacks.set(eventId, props.onPress);
  }

  return {
    type: "FloatingActionButton",
    props: {
      icon: props.icon ?? "+",
      imageSource: props.imageSource,
      accessibilityLabel: props.accessibilityLabel ?? props.icon ?? "Add",
      placement: props.placement ?? "bottom-right",
      margin: props.margin ?? 24,
      style: {
        width: 56,
        height: 56,
        backgroundColor: "#147d72",
        color: "#ffffff",
        borderRadius: 28,
        fontSize: 24,
        fontWeight: "bold",
        cursor: "pointer",
        ":hover": { backgroundColor: "#0f655c" },
        ":pressed": { backgroundColor: "#0a4f48" },
        ...props.style,
      },
    },
    events: props.onPress ? { click: eventId } : {},
  };
}
