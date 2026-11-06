const assert = require("node:assert/strict");
const { test } = require("node:test");

const { Card } = require("../app/build/app/core/components/Card.js");
const { Column } = require("../app/build/app/core/components/Column.js");
const { FlatList } = require("../app/build/app/core/components/FlatList.js");
const {
  Button,
  clearButtonCallbacks,
  dispatchEvent,
} = require("../app/build/app/core/components/Button.js");
const {
  clearModalCallbacks,
  dispatchModalClose,
  Modal,
} = require("../app/build/app/core/components/Modal.js");
const {
  clearTextInputCallbacks,
  dispatchChange,
  TextInput,
} = require("../app/build/app/core/components/TextInput.js");
const {
  clearWidgetCallbacks,
  dispatchWidgetEvent,
  Widget,
} = require("../app/build/app/core/components/Widget.js");
const { Row } = require("../app/build/app/core/components/Row.js");
const {
  clearTableCallbacks,
  dispatchTableResize,
  dispatchTableSort,
  Table,
} = require("../app/build/app/core/components/Table.js");
const { Text } = require("../app/build/app/core/components/Text.js");
const { View } = require("../app/build/app/core/components/View.js");
const {
  beginStateRender,
  finishStateRender,
} = require("../app/build/app/core/state.js");

test("Card composes a styled view with its title and children", () => {
  const child = { type: "Text", props: { text: "Details" } };
  const card = Card({ title: "Profile", children: [child] });

  assert.equal(card.type, "View");
  assert.equal(card.props.style.padding, 16);
  assert.equal(card.children[0].props.text, "Profile");
  assert.equal(card.children[1], child);
});

test("Text converts values and lets styles override presentation props", () => {
  const text = Text(42, {
    fontSize: 12,
    foregroundColor: "#111",
    style: { fontSize: 18, color: "#222" },
  });

  assert.equal(text.props.text, "42");
  assert.equal(text.props.fontSize, 18);
  assert.equal(text.props.foregroundColor, "#222");
});

test("Column and Row pass children and layout styles through", () => {
  const child = { type: "Text", props: { text: "Child" } };
  const column = Column({ spacing: 4, padding: 8, children: [child] });
  const row = Row({ style: { gap: 6, padding: 10 }, children: [child] });

  assert.equal(column.props.spacing, 4);
  assert.equal(column.props.padding, 8);
  assert.equal(column.children[0], child);
  assert.equal(row.props.spacing, 6);
  assert.equal(row.props.padding, 10);
  assert.equal(row.children[0], child);
});

test("View defaults to a column and exposes scroll configuration", () => {
  const view = View({
    scrollable: true,
    scrollDirection: "horizontal",
  });

  assert.equal(view.type, "View");
  assert.equal(view.props.direction, "column");
  assert.equal(view.props.scrollable, true);
  assert.equal(view.props.scrollDirection, "horizontal");
});

test("FlatList renders data with indices and separators in a scrollable View", () => {
  const separatorItems = [];
  const list = FlatList({
    data: ["Ada", "Grace", "Katherine"],
    renderItem: ({ item, index }) => Text(`${index}:${item}`),
    ItemSeparatorComponent: ({ leadingItem, trailingItem }) => {
      separatorItems.push([leadingItem, trailingItem]);
      return View({});
    },
    style: { height: 200 },
    contentContainerStyle: { padding: 12 },
  });

  assert.equal(list.type, "View");
  assert.equal(list.props.scrollable, true);
  assert.equal(list.props.scrollDirection, "vertical");
  assert.equal(list.props.style.height, 200);
  assert.equal(list.children[0].props.style.padding, 12);
  assert.deepEqual(
    list.children[0].children.map((child) =>
      child.type === "Text" ? child.props.text : "separator",
    ),
    ["0:Ada", "separator", "1:Grace", "separator", "2:Katherine"],
  );
  assert.deepEqual(separatorItems, [
    ["Ada", "Grace"],
    ["Grace", "Katherine"],
  ]);
});

test("FlatList supports horizontal lists and disabling scrolling", () => {
  const list = FlatList({
    data: [1],
    horizontal: true,
    scrollEnabled: false,
    renderItem: ({ item }) => Text(item),
  });

  assert.equal(list.props.direction, "row");
  assert.equal(list.props.scrollDirection, "horizontal");
  assert.equal(list.props.scrollable, false);
  assert.equal(list.children[0].props.direction, "row");
});

test("FlatList renders an empty-state component for empty data", () => {
  const emptyState = Text("No results");
  const list = FlatList({
    data: [],
    renderItem: ({ item }) => Text(item),
    ListEmptyComponent: emptyState,
  });

  assert.equal(list.children[0].children.length, 1);
  assert.equal(list.children[0].children[0], emptyState);
});

test("Table creates a native table node with column sizing and cell content", () => {
  beginStateRender();
  const table = Table({
    data: [{ name: "Ada", year: 1843 }],
    columns: [
      { key: "name", title: "Name" },
      { key: "year", title: "Year", width: 100, minWidth: 60 },
    ],
    style: { height: 240 },
  });
  finishStateRender();

  assert.equal(table.type, "Table");
  assert.equal(table.props.style.height, 240);
  assert.deepEqual(table.props.rowStyle, {
    backgroundColor: "#f3f7f5",
    color: "#193b35",
  });
  assert.deepEqual(table.props.columns, [
    { title: "Name", width: undefined, minWidth: undefined, sortable: true },
    { title: "Year", width: 100, minWidth: 60, sortable: true },
  ]);
  assert.equal(table.props.rowCount, 1);
  assert.equal(table.children.length, 2);
  assert.equal(table.children[0].props.text, "Ada");
  assert.equal(table.children[1].props.text, "1843");
});

test("Table sorts rows and preserves ascending and descending state", async () => {
  const props = {
    data: [
      { name: "Grace", score: 9 },
      { name: "Ada", score: 12 },
      { name: "Katherine", score: 5 },
    ],
    columns: [{ key: "score", title: "Score" }],
  };
  const renderTable = () => {
    clearTableCallbacks();
    beginStateRender();
    const table = Table(props);
    finishStateRender();
    return table;
  };
  const initial = renderTable();
  assert.deepEqual(
    initial.children.map((cell) => cell.props.text),
    ["9", "12", "5"],
  );
  dispatchTableSort(initial.events.sort, 0, true);
  await Promise.resolve();
  const ascending = renderTable();
  assert.equal(ascending.props.sortColumn, 0);
  assert.equal(ascending.props.sortAscending, true);
  assert.deepEqual(
    ascending.children.map((cell) => cell.props.text),
    ["5", "9", "12"],
  );

  dispatchTableSort(ascending.events.sort, 0, false);
  await Promise.resolve();
  const descending = renderTable();
  assert.equal(descending.props.sortColumn, 0);
  assert.equal(descending.props.sortAscending, false);
  assert.deepEqual(
    descending.children.map((cell) => cell.props.text),
    ["12", "9", "5"],
  );
});

test("Table supports custom sorting, stable ties, and unsortable columns", async () => {
  const props = {
    data: [
      { value: "b", label: "b" },
      { value: "a", label: "a-first" },
      { value: "a", label: "a-second" },
    ],
    columns: [
      {
        key: "value",
        title: "Value",
        compare: (left, right) => left.value.localeCompare(right.value),
        render: (item) => Text(item.label),
      },
      { key: "value", title: "Fixed", sortable: false },
    ],
  };
  const renderTable = () => {
    clearTableCallbacks();
    beginStateRender();
    const table = Table(props);
    finishStateRender();
    return table;
  };

  const initial = renderTable();
  assert.deepEqual(initial.children.map((cell) => cell.props.text), [
    "b",
    "b",
    "a-first",
    "a",
    "a-second",
    "a",
  ]);
  dispatchTableSort(initial.events.sort, 0, true);
  await Promise.resolve();
  const ascending = renderTable();
  assert.deepEqual(ascending.children.map((cell) => cell.props.text), [
    "a-first",
    "a",
    "a-second",
    "a",
    "b",
    "b",
  ]);
  dispatchTableSort(ascending.events.sort, 0, false);
  await Promise.resolve();
  const descending = renderTable();
  assert.deepEqual(descending.children.map((cell) => cell.props.text), [
    "b",
    "b",
    "a-first",
    "a",
    "a-second",
    "a",
  ]);
  assert.equal(descending.props.columns[1].sortable, false);
});

test("Table preserves resized widths in the native widget props", async () => {
  const props = {
    data: [{ name: "Ada" }],
    columns: [{ key: "name", title: "Name" }],
  };
  const renderTable = () => {
    clearTableCallbacks();
    beginStateRender();
    const table = Table(props);
    finishStateRender();
    return table;
  };

  const initial = renderTable();
  dispatchTableResize(initial.events.resize, 0, 240);
  await Promise.resolve();
  const resized = renderTable();

  assert.equal(resized.props.columns[0].width, 240);
  dispatchTableSort(resized.events.sort, 0, true);
  await Promise.resolve();
  const sorted = renderTable();

  assert.equal(sorted.props.columns[0].width, 240);
});

test("Modal exposes defaults and dispatches close once", () => {
  let closeCount = 0;
  const modal = Modal({ onClose: () => closeCount++ });

  assert.equal(modal.type, "Modal");
  assert.equal(modal.props.visible, false);
  assert.equal(modal.props.title, "");
  dispatchModalClose(modal.events.close);
  dispatchModalClose(modal.events.close);
  assert.equal(closeCount, 1);
});

test("Modal callbacks can be cleared between renders", () => {
  let closeCount = 0;
  const modal = Modal({ onClose: () => closeCount++ });

  clearModalCallbacks();
  dispatchModalClose(modal.events.close);

  assert.equal(closeCount, 0);
});

test("Modal applies explicit props and passes style and children through", () => {
  const child = { type: "Text", props: { text: "Content" } };
  const style = { width: 500, backgroundColor: "#fff" };
  const modal = Modal({
    visible: true,
    title: "Details",
    width: 400,
    style,
    children: [child],
  });

  assert.equal(modal.props.visible, true);
  assert.equal(modal.props.title, "Details");
  assert.equal(modal.props.width, 400);
  assert.equal(modal.props.style, style);
  assert.equal(modal.children[0], child);
});

test("Button dispatches its click callback", () => {
  let clickCount = 0;
  const button = Button({ text: "Click", onClick: () => clickCount++ });

  dispatchEvent(button.events.click);

  assert.equal(clickCount, 1);
});

test("Button callbacks can be cleared between renders", () => {
  let clickCount = 0;
  const button = Button({ text: "Click", onClick: () => clickCount++ });

  clearButtonCallbacks();
  dispatchEvent(button.events.click);

  assert.equal(clickCount, 0);
});

test("TextInput callbacks can be cleared between renders", () => {
  let changedValue = "";
  const input = TextInput({ onChange: (value) => (changedValue = value) });

  clearTextInputCallbacks();
  dispatchChange(input.events.change, "updated");

  assert.equal(changedValue, "");
});

test("TextInput dispatches its changed value", () => {
  let changedValue = "";
  const input = TextInput({ onChange: (value) => (changedValue = value) });

  dispatchChange(input.events.change, "updated");

  assert.equal(changedValue, "updated");
});

test("Widget does not mutate props when applying its default position", () => {
  const props = {
    position: { x: 12, y: 80 },
    hideTitleBar: false,
    body: [],
  };
  const widget = Widget(props);

  assert.deepEqual(props.position, { x: 12, y: 80 });
  assert.deepEqual(widget.props.position, { x: 12, y: 30 });
});

test("Widget fullscreen position takes precedence without mutating props", () => {
  const props = {
    position: { x: 12, y: 80 },
    fullscreen: true,
    body: [],
  };
  const widget = Widget(props);

  assert.deepEqual(props.position, { x: 12, y: 80 });
  assert.deepEqual(widget.props.position, { x: 0, y: 0 });
});

test("Widget dispatches resize callbacks and clears them between renders", () => {
  let dimensions;
  const widget = Widget({
    body: [],
    onResize: (width, height) => {
      dimensions = [width, height];
    },
  });

  dispatchWidgetEvent(widget.events.resize, 640, 480);
  assert.deepEqual(dimensions, [640, 480]);

  clearWidgetCallbacks();
  dimensions = undefined;
  dispatchWidgetEvent(widget.events.resize, 800, 600);
  assert.equal(dimensions, undefined);
});
