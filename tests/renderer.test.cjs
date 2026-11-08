const assert = require("node:assert/strict");
const { test } = require("node:test");

const { render } = require("../app/build/app/core/renderer.js");

test("State-driven text and input changes emit leaf patches", () => {
  const messages = [];
  const originalWrite = process.stdout.write;
  process.stdout.write = (chunk) => {
    messages.push(String(chunk));
    return true;
  };

  try {
    render({
      type: "Widget",
      props: { title: "Test" },
      children: [
        {
          type: "Column",
          children: [
            { type: "Text", props: { text: "Count: 0" } },
            {
              type: "TextInput",
              props: { value: "a", type: "email" },
              events: { change: "input_1" },
            },
            {
              type: "Button",
              props: { text: "Save" },
              events: { click: "event_1" },
            },
          ],
        },
      ],
    });
    render({
      type: "Widget",
      props: { title: "Test" },
      children: [
        {
          type: "Column",
          children: [
            { type: "Text", props: { text: "Count: 1" } },
            {
              type: "TextInput",
              props: { value: "ab", type: "email" },
              events: { change: "input_1" },
            },
            {
              type: "Button",
              props: { text: "Save" },
              events: { click: "event_1" },
            },
          ],
        },
      ],
    });
  } finally {
    process.stdout.write = originalWrite;
  }

  const initial = JSON.parse(messages[0].replace("__JULIA_UI__", ""));
  const patch = JSON.parse(messages[1].replace("__JULIA_UI__", ""));

  assert.equal(initial.children[0].children[0].nodeId, "root.0.0");
  assert.equal(patch.type, "patch");
  assert.deepEqual(patch.updates, [
    { nodeId: "root.0.0", type: "Text", value: "Count: 1" },
    { nodeId: "root.0.1", type: "TextInput", value: "ab" },
  ]);
});
