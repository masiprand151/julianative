declare const process: any;

import { VNode } from "./types";
import { dispatchEvent } from "./components/Button";
import { dispatchFloatingActionButtonPress } from "./components/FloatingActionButton";
import { dispatchModalClose } from "./components/Modal";
import { dispatchChange } from "./components/TextInput";
import { dispatchTableResize, dispatchTableSort } from "./components/Table";
import { dispatchWidgetEvent } from "./components/Widget";

// Mengubah VNode menjadi JSON untuk dikirim ke C++.
function serialize(node: VNode, nodeId: string): any {
  return {
    type: node.type,
    nodeId,
    props: node.props,
    events: node.events || {},
    children: (node.children || []).map((child, index) =>
      serialize(child, `${nodeId}.${index}`),
    ),
  };
}

type LeafPatch = {
  nodeId: string;
  type: "Text" | "TextInput" | "Button";
  value: string;
};

let previousTree: any;

function sameValue(left: unknown, right: unknown): boolean {
  return JSON.stringify(left) === JSON.stringify(right);
}

function leafTextProperty(
  type: string,
): { property: string; patchType: LeafPatch["type"] } | undefined {
  if (type === "Text" || type === "Button") {
    return { property: "text", patchType: type };
  }
  if (type === "TextInput") {
    return { property: "value", patchType: type };
  }
  return undefined;
}

function collectLeafPatches(
  previous: any,
  next: any,
  patches: LeafPatch[],
  insideUnsupportedNode = false,
): boolean {
  if (
    previous.type !== next.type ||
    previous.events === undefined ||
    !sameValue(previous.events, next.events) ||
    previous.children.length !== next.children.length
  ) {
    return false;
  }

  const unsupportedDescendants =
    insideUnsupportedNode ||
    ["Table", "Modal", "Video", "Image", "FloatingActionButton"].includes(
      next.type,
    );
  const previousProps = previous.props ?? {};
  const nextProps = next.props ?? {};
  const keys = new Set([
    ...Object.keys(previousProps),
    ...Object.keys(nextProps),
  ]);
  const changedKeys = [...keys].filter(
    (key) => !sameValue(previousProps[key], nextProps[key]),
  );

  if (changedKeys.length > 0) {
    const leaf = leafTextProperty(next.type);
    if (
      unsupportedDescendants ||
      leaf === undefined ||
      changedKeys.some((key) => key !== leaf.property)
    ) {
      return false;
    }

    patches.push({
      nodeId: next.nodeId,
      type: leaf.patchType,
      value: String(nextProps[leaf.property] ?? ""),
    });
  }

  for (let index = 0; index < previous.children.length; index++) {
    if (
      !collectLeafPatches(
        previous.children[index],
        next.children[index],
        patches,
        unsupportedDescendants,
      )
    ) {
      return false;
    }
  }
  return true;
}

// Mengirim UI ke native runtime.
export function render(root: VNode): void {
  const result = serialize(root, "root");
  let message: any = result;

  if (previousTree) {
    const patches: LeafPatch[] = [];
    if (collectLeafPatches(previousTree, result, patches)) {
      if (patches.length === 0) {
        previousTree = result;
        return;
      }
      message = { type: "patch", updates: patches };
    }
  }

  previousTree = result;
  process.stdout.write("__JULIA_UI__" + JSON.stringify(message) + "\n");
}

// Menerima event dari native C++.
export function listenNativeEvents(): void {
  let buffer = "";

  process.stdin.setEncoding("utf8");

  process.stdin.on("data", (chunk: string) => {
    buffer += chunk;

    let index: number;

    while ((index = buffer.indexOf("\n")) !== -1) {
      const line = buffer.substring(0, index).trim();
      buffer = buffer.substring(index + 1);

      if (!line) continue;

      try {
        const message = JSON.parse(line);

        if (message.type === "click") {
          dispatchEvent(message.id);
          dispatchFloatingActionButtonPress(message.id);
        } else if (message.type === "change") {
          dispatchChange(message.id, message.value);
        } else if (message.type === "modalClose") {
          dispatchModalClose(message.id);
        } else if (message.type === "tableSort") {
          dispatchTableSort(
            message.id,
            message.columnIndex,
            message.ascending,
          );
        } else if (message.type === "tableResize") {
          dispatchTableResize(
            message.id,
            message.columnIndex,
            message.width,
          );
        } else if (message.type === "windowEvent") {
          try {
            if (message.event === "resize") {
              dispatchWidgetEvent(message.id, message.width, message.height);
            } else {
              dispatchWidgetEvent(message.id);
            }
          } finally {
            if (message.event === "close") {
              process.stdout.write(
                '__JULIA_UI__{"type":"command","command":"windowCloseAck"}\n',
              );
            }
          }
        }
      } catch (error) {
        console.error("Native event error:", error);
      }
    }
  });
}
