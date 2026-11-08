const assert = require("node:assert/strict");
const { test } = require("node:test");

const { render } = require("../app/build/app/core/renderer.js");
const { FlatList } = require("../app/build/app/core/components/FlatList.js");
const { Text } = require("../app/build/app/core/components/Text.js");
const {
  abortStateRender,
  beginStateRender,
  finishStateRender,
} = require("../app/build/app/core/state.js");

function renderMessage(root) {
  const messages = [];
  const originalWrite = process.stdout.write;
  process.stdout.write = (chunk) => {
    messages.push(String(chunk));
    return true;
  };
  try {
    beginStateRender();
    render(root);
    finishStateRender();
  } catch (error) {
    abortStateRender();
    throw error;
  } finally {
    process.stdout.write = originalWrite;
  }
  return messages.length
    ? JSON.parse(messages[0].replace("__JULIA_UI__", ""))
    : undefined;
}

function listRoot(data, options = {}) {
  return {
    type: "Widget",
    props: { title: options.title ?? "Virtualized list" },
    children: [
      { type: "Text", props: { text: "prefix" } },
      FlatList({
        data,
        keyExtractor: (item) => item.id,
        estimatedItemSize: 10,
        initialNumToRender: 5,
        overscan: 2,
        renderItem: ({ item }) => Text(item.label),
        ...options,
      }),
    ],
  };
}

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

test("FlatList serializes only its initial virtual window with stable item IDs", () => {
  const data = Array.from({ length: 100 }, (_, index) => ({
    id: `item-${index}`,
    label: `Item ${index}`,
  }));
  const message = renderMessage(listRoot(data));
  const list = message.children[1];

  assert.equal(list.type, "FlatList");
  assert.equal(list.props.totalCount, 100);
  assert.equal(list.props.firstVisibleIndex, 0);
  assert.equal(list.props.visibleItemCount, 5);
  assert.equal(list.children.length, 5);
  assert.equal(
    list.children[0].nodeId,
    'root.1.key.%22item-0%22',
  );
  assert.equal(
    list.children[0].children[0].nodeId,
    'root.1.key.%22item-0%22.0',
  );
});

test("FlatList viewport events move the rendered window and keep item keys", () => {
  const data = Array.from({ length: 100 }, (_, index) => ({
    id: `item-${index}`,
    label: `Item ${index}`,
  }));
  const root = listRoot(data);
  renderMessage(root);

  let onData;
  const originalOn = process.stdin.on;
  const originalSetEncoding = process.stdin.setEncoding;
  process.stdin.on = (event, callback) => {
    if (event === "data") onData = callback;
    return process.stdin;
  };
  process.stdin.setEncoding = () => process.stdin;
  try {
    require("../app/build/app/core/renderer.js").listenNativeEvents();
    onData(
      `${JSON.stringify({
        type: "listViewport",
        nodeId: "root.1",
        offset: 150,
        viewportSize: 30,
      })}\n`,
    );
  } finally {
    process.stdin.on = originalOn;
    process.stdin.setEncoding = originalSetEncoding;
  }

  const patch = renderMessage(root);
  assert.equal(patch.type, "listPatch");
  const list = patch.updates[0];
  assert.equal(list.props.firstVisibleIndex, 13);
  assert.equal(list.children.length, 7);
  assert.equal(list.children[0].nodeId, 'root.1.key.%22item-13%22');
});

test("FlatList uses measured variable item sizes when calculating its window", () => {
  const data = Array.from({ length: 10 }, (_, index) => ({
    id: `item-${index}`,
    label: `Item ${index}`,
  }));
  const root = listRoot(data, { initialNumToRender: 3, overscan: 1 });
  renderMessage(root);

  let onData;
  const originalOn = process.stdin.on;
  const originalSetEncoding = process.stdin.setEncoding;
  process.stdin.on = (event, callback) => {
    if (event === "data") onData = callback;
    return process.stdin;
  };
  process.stdin.setEncoding = () => process.stdin;
  try {
    require("../app/build/app/core/renderer.js").listenNativeEvents();
    onData(
      `${JSON.stringify({
        type: "listViewport",
        nodeId: "root.1",
        offset: 50,
        viewportSize: 10,
        items: [
          { index: 0, size: 20 },
          { index: 1, size: 30 },
          { index: 2, size: 10 },
        ],
      })}\n`,
    );
  } finally {
    process.stdin.on = originalOn;
    process.stdin.setEncoding = originalSetEncoding;
  }

  const patch = renderMessage(root);
  const list = patch.updates[0];
  assert.equal(patch.type, "listPatch");
  assert.equal(list.props.firstVisibleIndex, 1);
  assert.equal(list.props.leadingExtent, 20);
  assert.equal(list.children[0].nodeId, 'root.1.key.%22item-1%22');
});

test("Keyed list item text changes emit patches for the matching item", () => {
  const first = [
    { id: "ada", label: "Ada" },
    { id: "grace", label: "Grace" },
  ];
  const second = [
    { id: "ada", label: "Ada" },
    { id: "grace", label: "Grace Hopper" },
  ];

  renderMessage(listRoot(first, { initialNumToRender: 5 }));
  const patch = renderMessage(listRoot(second, { initialNumToRender: 5 }));
  assert.equal(patch.type, "patch");
  assert.deepEqual(patch.updates, [
    {
      nodeId: 'root.1.key.%22grace%22.0',
      type: "Text",
      value: "Grace Hopper",
    },
  ]);
});

test("Keyed list items keep stable IDs and reorder through reconciliation", () => {
  const first = [
    { id: "ada", label: "Ada" },
    { id: "grace", label: "Grace" },
  ];
  const reordered = [
    { id: "grace", label: "Grace" },
    { id: "ada", label: "Ada" },
  ];

  renderMessage(listRoot(first, { initialNumToRender: 5 }));
  const message = renderMessage(
    listRoot(reordered, { initialNumToRender: 5 }),
  );
  assert.equal(message.type, "listPatch");
  assert.equal(message.updates[0].children[0].nodeId, 'root.1.key.%22grace%22');
  assert.equal(message.updates[0].children[1].nodeId, 'root.1.key.%22ada%22');
});

test("FlatList rejects duplicate keys instead of reconciling ambiguously", () => {
  assert.throws(
    () =>
      renderMessage(
        listRoot([
          { id: "duplicate", label: "First" },
          { id: "duplicate", label: "Second" },
        ]),
      ),
    /duplicate key/,
  );
});
