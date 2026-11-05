declare const process: any;

import { VNode } from "./types";
import { dispatchEvent } from "./components/Button";
import { dispatchChange } from "./components/TextInput";

// Mengubah VNode menjadi JSON untuk dikirim ke C++.
function serialize(node: VNode): any {
  return {
    type: node.type,
    props: node.props,
    events: node.events || {},
    children: (node.children || []).map(serialize),
  };
}

// Mengirim UI ke native runtime.
export function render(root: VNode): void {
  const result = serialize(root);
  const output = JSON.stringify(result);

  process.stdout.write("__JULIA_UI__" + output + "\n");
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
        } else if (message.type === "change") {
          dispatchChange(message.id, message.value);
        }
      } catch (error) {
        console.error("Native event error:", error);
      }
    }
  });
}
