const assert = require("node:assert/strict");
const { test } = require("node:test");

const { createStackNavigator } = require("../app/build/app/core/navigation.js");
const {
  Button,
  clearButtonCallbacks,
  dispatchEvent,
} = require("../app/build/app/core/components/Button.js");
const { Text } = require("../app/build/app/core/components/Text.js");
const {
  clearTableCallbacks,
  dispatchTableSort,
  Table,
} = require("../app/build/app/core/components/Table.js");
const {
  beginStateRender,
  finishStateRender,
  useState,
} = require("../app/build/app/core/state.js");

test("Stack Navigator supports push, navigate, back, replace, and popToTop", async () => {
  const Stack = createStackNavigator();
  const screens = {
    Home: {
      component: ({ navigation }) =>
        TextButton("Open details", () =>
          navigation.push("Details", { id: 7 }),
        ),
    },
    Details: {
      component: ({ navigation, route }) => ({
        type: "Column",
        children: [
          Text(`Detail ${route.params.id}`),
          TextButton("Open profile", () => navigation.push("Profile")),
          TextButton("Replace with settings", () =>
            navigation.replace("Settings"),
          ),
          TextButton("Navigate to profile", () =>
            navigation.navigate("Profile"),
          ),
          TextButton("Back to root", navigation.popToTop),
        ],
      }),
    },
    Profile: {
      component: ({ navigation }) => ({
        type: "Column",
        children: [
          Text("Profile screen"),
          TextButton("Open details from profile", () =>
            navigation.push("Details", { id: 8 }),
          ),
        ],
      }),
    },
    Settings: {
      component: ({ navigation }) => ({
        type: "Column",
        children: [
          Text("Settings screen"),
          TextButton("Back to root", navigation.popToTop),
        ],
      }),
    },
  };

  function render() {
    clearButtonCallbacks();
    beginStateRender();
    const tree = Stack.Navigator({
      initialRouteName: "Home",
      screens,
    });
    finishStateRender();
    return tree;
  }

  function click(tree, text) {
    const button = findNode(tree, (node) => node.props?.text === text);
    assert.ok(button, `Expected to find button "${text}"`);
    dispatchEvent(button.events.click);
  }

  let tree = render();
  assert.equal(findNode(tree, (node) => node.props?.text === "Home") !== null, true);

  click(tree, "Open details");
  await Promise.resolve();
  tree = render();
  assert.ok(findNode(tree, (node) => node.props?.text === "Detail 7"));

  click(tree, "Open profile");
  await Promise.resolve();
  tree = render();
  assert.ok(findNode(tree, (node) => node.props?.text === "Profile screen"));

  click(tree, "Open details from profile");
  await Promise.resolve();
  tree = render();
  assert.ok(findNode(tree, (node) => node.props?.text === "Detail 8"));

  click(tree, "Navigate to profile");
  await Promise.resolve();
  tree = render();
  assert.ok(findNode(tree, (node) => node.props?.text === "Profile screen"));

  click(tree, "‹");
  await Promise.resolve();
  tree = render();
  assert.ok(findNode(tree, (node) => node.props?.text === "Detail 7"));

  click(tree, "Replace with settings");
  await Promise.resolve();
  tree = render();
  assert.ok(findNode(tree, (node) => node.props?.text === "Settings screen"));

  click(tree, "Back to root");
  await Promise.resolve();
  tree = render();
  assert.ok(findNode(tree, (node) => node.props?.text === "Home"));
});

test("Stack screens keep isolated component state when navigating", async () => {
  const Stack = createStackNavigator();
  const screens = {
    Home: {
      component: ({ navigation }) => ({
        type: "Column",
        children: [
          Table({
            data: [{ value: "b" }, { value: "a" }],
            columns: [{ key: "value", title: "Value" }],
          }),
          TextButton("Open settings", () =>
            navigation.navigate("Settings"),
          ),
        ],
      }),
    },
    Settings: { component: () => Text("Settings screen") },
  };

  function render() {
    clearButtonCallbacks();
    clearTableCallbacks();
    beginStateRender();
    const tree = Stack.Navigator({
      initialRouteName: "Home",
      screens,
    });
    finishStateRender();
    return tree;
  }

  let tree = render();
  let table = findNode(tree, (node) => node.type === "Table");
  dispatchTableSort(table.events.sort, 0, true);
  await Promise.resolve();
  tree = render();
  table = findNode(tree, (node) => node.type === "Table");
  assert.equal(table.props.sortColumn, 0);

  const openSettings = findNode(
    tree,
    (node) => node.props?.text === "Open settings",
  );
  dispatchEvent(openSettings.events.click);
  await Promise.resolve();
  tree = render();
  assert.ok(findNode(tree, (node) => node.props?.text === "Settings screen"));

  const backButton = findNode(tree, (node) => node.props?.text === "‹");
  dispatchEvent(backButton.events.click);
  await Promise.resolve();
  tree = render();
  table = findNode(tree, (node) => node.type === "Table");
  assert.equal(table.props.sortColumn, 0);
});

test("Stack Navigator supports styled and custom headers", async () => {
  const Stack = createStackNavigator();
  const screens = {
    Home: {
      component: ({ navigation }) =>
        TextButton("Open details", () => navigation.navigate("Details")),
    },
    Details: { component: () => Text("Details screen") },
  };
  const headerStyle = {
    backgroundColor: "#123456",
    padding: 14,
  };
  let receivedHeaderProps;

  function render() {
    clearButtonCallbacks();
    beginStateRender();
    const tree = Stack.Navigator({
      initialRouteName: "Home",
      headerStyle,
      screenOptions: {
        headerComponent: (props) => {
          receivedHeaderProps = props;
          const [isFavorite, setIsFavorite] = useState(false);
          return {
            type: "Row",
            children: [
              TextButton("Custom back", props.navigation.goBack),
              TextButton(isFavorite ? "★" : "☆", () =>
                setIsFavorite((current) => !current),
              ),
              Text(props.title),
            ],
          };
        },
      },
      screens,
    });
    finishStateRender();
    return tree;
  }

  let tree = render();
  let headerContainer = findNode(
    tree,
    (node) => node.type === "View" && node.props.style.backgroundColor === "#123456",
  );
  assert.ok(headerContainer);
  assert.equal(headerContainer.props.style.padding, 14);
  assert.equal(receivedHeaderProps.title, "Home");
  assert.equal(receivedHeaderProps.canGoBack, false);
  const favoriteButton = findNode(
    tree,
    (node) => node.props?.text === "☆",
  );
  dispatchEvent(favoriteButton.events.click);
  await Promise.resolve();
  tree = render();
  assert.ok(findNode(tree, (node) => node.props?.text === "★"));

  const openDetails = findNode(
    tree,
    (node) => node.props?.text === "Open details",
  );
  dispatchEvent(openDetails.events.click);
  await Promise.resolve();
  tree = render();

  headerContainer = findNode(
    tree,
    (node) => node.type === "View" && node.props.style.backgroundColor === "#123456",
  );
  assert.ok(headerContainer);
  assert.equal(receivedHeaderProps.title, "Details");
  assert.equal(receivedHeaderProps.canGoBack, true);

  const customBack = findNode(
    tree,
    (node) => node.props?.text === "Custom back",
  );
  dispatchEvent(customBack.events.click);
  await Promise.resolve();
  tree = render();
  assert.equal(receivedHeaderProps.title, "Home");
});

function TextButton(text, onClick) {
  return Button({ text, onClick });
}

function findNode(node, predicate) {
  if (predicate(node)) return node;
  for (const child of node.children ?? []) {
    const result = findNode(child, predicate);
    if (result) return result;
  }
  return null;
}
