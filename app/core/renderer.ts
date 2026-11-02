declare const process: any;

import { VNode, EventHandler } from "./types";

const callbacks = new Map<string, EventHandler>();
let eventCounter = 0;

function serialize(node: VNode): any {
  const events: Record<string, string> = {};

  if (node.events) {
    for (const [name, callback] of Object.entries(node.events)) {
      const id = `julia_event_${++eventCounter}`;

      callbacks.set(id, callback);
      events[name] = id;
    }
  }

  return {
    type: node.type,
    props: node.props,
    events,
    children: (node.children || []).map(serialize),
  };
}
export function render(root: VNode): void {
  const result = serialize(root);
  const output = JSON.stringify(result);

  console.error("[Julia] Sending UI:", output);

  process.stdout.write("__JULIA_UI__" + output + "\n");
}
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
          const callback = callbacks.get(message.id);

          if (callback) {
            callback();
          }
        }
      } catch (error) {
        console.error("Native event error:", error);
      }
    }
  });
}
