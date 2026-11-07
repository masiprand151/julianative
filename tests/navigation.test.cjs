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
  beginStateRender,
  finishStateRender,
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
