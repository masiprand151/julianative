# julianative

## System Requirements

To get started with this project, ensure you have the following tools and libraries:

- CMake (Version 3.14 or later): Essential for building and managing the project files.
- C++ Compiler: Compatible with Clang, GCC, or MSVC. Choose one based on your development environment.
- GTK3 Development Libraries (for Linux users): Necessary for GUI development on Linux platforms.

## Development Hot Reload

Run `npm run dev` to build and start the native app in development mode. Changes
to TypeScript files under `src` or `app/core` rebuild the UI and restart only
the Node.js renderer; the native window stays open and refreshes in place.
Changes to native C++ files require a manual rebuild and app restart.

## Widget Window Options

`Widget` supports window position, resizing, fullscreen, hiding the title bar,
always-on-top, and optional content scrolling. Fullscreen uses the native
fullscreen mode, which hides the title bar; `hideTitleBar` independently hides
the title bar while the window is not fullscreen. Its `onClose`, `onResize`,
`onMaximize`, and `onMinimize` callbacks are invoked for corresponding native
window events.

```ts
Widget({
  title: "My app",
  position: { x: 100, y: 100 },
  resizable: true,
  fullscreen: false,
  hideTitleBar: false,
  alwaysOnTop: false,
  scrollable: true,
  onResize: (width, height) => console.log(width, height),
  body: [],
});
```

## State

Use `useState` inside the root component to keep values between renders. Calling
the setter re-renders the UI in the existing native window. State hooks must be
called in the same order on every render.

```ts
function Main() {
  const [count, setCount] = useState(0);

  return Column({
    children: [
      Text(`Count: ${count}`),
      Button({
        text: "Increment",
        onClick: () => setCount((current) => current + 1),
      }),
    ],
  });
}
```

## Stack Navigation

Keep the app's single `Widget` in `Main` and render a `Stack.Navigator` inside
its body. Each screen receives `navigation` and `route` props. The navigator
provides a header with a back button and title by default. Customize the
built-in header with `headerStyle`, or replace it with `headerComponent`.

```ts
type AppStack = {
  Home: undefined;
  Details: { itemId: number };
};

const Stack = createStackNavigator<AppStack>();

function Main() {
  return Widget({
    title: "My app",
    body: [
      Stack.Navigator({
        initialRouteName: "Home",
        screens: {
          Home: {
            options: { title: "Beranda" },
            component: ({ navigation }) =>
              Button({
                text: "Lihat detail",
                onClick: () =>
                  navigation.navigate("Details", { itemId: 42 }),
              }),
          },
          Details: {
            component: ({ route }) =>
              Text(`Item ${route.params?.itemId}`),
          },
        },
      }),
    ],
  });
}
```

`navigate` takes the destination route and, when needed, a typed params object.
Read the data in the destination screen from `route.params`. Routes already in
the stack are brought to the top; otherwise, the destination is pushed.
`push`, `goBack`, `replace`, and `popToTop` are available for explicit stack
control. `screenOptions` sets defaults for screen headers; a screen can
override `title` or set `headerShown: false`.

```ts
const Stack = createStackNavigator<AppStack>();

function AppHeader({ navigation, title, canGoBack }) {
  return Row({
    style: { gap: 12, padding: 10, backgroundColor: "#193b35" },
    children: [
      ...(canGoBack
        ? [Button({ text: "Back", onClick: navigation.goBack })]
        : []),
      Text(title, { style: { color: "#fff", fontWeight: "bold" } }),
    ],
  });
}

Stack.Navigator({
  initialRouteName: "Home",
  screenOptions: {
    headerComponent: AppHeader,
    headerStyle: { padding: 6, backgroundColor: "#dce9e5" },
  },
  screens: { /* screens */ },
});
```

The custom header receives `navigation`, `route`, `title`, and `canGoBack`.
Set `headerComponent` in `screenOptions` for every screen, or in one screen's
`options` to customize that screen only. `headerStyle` styles the container
around a custom header and also overrides the built-in header's default style.
For larger apps, keep the navigator and `Widget` in `src/main.ts`, define the
route params in `src/navigation/types.ts`, and export each screen from its own
file under `src/screens/`.

## StyleSheet

Use `StyleSheet.create` to define and reuse component styles. Style properties
can use camelCase or common CSS kebab-case names. Qt-supported CSS lengths
include numbers, `px`, `pt`, `em`, and `ex`; width and height also accept
percentages. The native renderer supports `:hover`, `:focus`, `:active`,
`:pressed`, and `:disabled` states; CSS properties are limited to those
supported by Qt stylesheets.

```ts
const styles = StyleSheet.create({
  button: {
    padding: "10px",
    "background-color": "#147d72",
    ":hover": {
      backgroundColor: "#0f655c",
    },
  },
});

Button({ text: "Click me", style: styles.button });
```

`View` can scroll its contents when given a bounded size:

```ts
View({
  scrollable: true,
  scrollDirection: "vertical",
  style: { width: 320, height: 240 },
  children: [/* content */],
});
```

## FlatList

`FlatList` renders data with `renderItem`, supports horizontal or vertical
scrolling, separators, an empty-state component, and separate container styles.
The current native renderer creates every item (it does not virtualize large
lists).

```ts
FlatList({
  data: ["Ada", "Grace", "Linus"],
  renderItem: ({ item, index }) => Text(`${index + 1}. ${item}`),
  ItemSeparatorComponent: () => View({ style: { height: 1, backgroundColor: "#ddd" } }),
  style: { height: 220 },
  contentContainerStyle: { gap: 8, padding: 12 },
});
```

`keyExtractor` is not currently supported because the renderer rebuilds the
entire native view tree on each state update; stable keys have no reconciliation
behavior yet.

## Table

`Table` uses the native Qt table widget, so its header, row selection, grid,
sorting indicator, and column resizing follow the current desktop platform
style. Drag a header divider to resize any column. The rightmost column fills
any remaining width as the table or window grows. A horizontal scrollbar
appears when the other columns or their minimums exceed the viewport. Specify
`width` for an initial width and `minWidth` for the minimum. Data rows have a
subtle tinted background and contrasting text by default; set `rowStyle` to
customize it. `compare` enables custom sorting. All rows are currently rendered
at once.

```ts
Table({
  data: [
    { name: "Ada", year: 1843 },
    { name: "Grace", year: 1952 },
  ],
  columns: [
    { key: "name", title: "Name", width: 180 },
    { key: "year", title: "Year" },
  ],
  style: { height: 240 },
});
```

## TextInput

`TextInput` supports `text`, `password`, `number`, and `email` input types.
Set `autoFocus: true` to focus the input when the UI opens or refreshes. The
optional `onChange` callback receives the current value as a string.

```ts
TextInput({
  type: "email",
  placeholder: "Email",
  autoFocus: true,
  onChange: (value) => console.log(value),
});
```

## Card

Use `Card` to group content with a title and a styled container. You can
override its default container styles with `style`.

```ts
Card({
  title: "Profile",
  style: { padding: 20 },
  children: [
    Text("Manage your account details"),
    Button({ text: "Edit profile" }),
  ],
});
```

## FloatingActionButton

`FloatingActionButton` is a circular native action button anchored over the
window content. Place it directly in `Widget.body` so it floats above the
regular layout. It defaults to the bottom-right corner; choose another corner
with `placement` or adjust its distance from the edge with `margin`.
Set `imageSource` to a local image path or HTTP(S) URL to use an image instead
of the text icon.

```ts
FloatingActionButton({
  icon: "+",
  accessibilityLabel: "Add item",
  onPress: () => console.log("Add item"),
});
```

## Image

`Image` displays local image files or downloads them from an HTTP(S) URL. Local
relative paths are resolved from the app's working directory. PNG, JPG/JPEG, and
SVG are supported. Local images also work without an internet connection;
remote images require a connection when they are loaded.
For portable builds, put relative-path files in the project's `assets/`
directory; `npm run prepare:portable` copies it into the runtime.

```ts
Image({
  source: "assets/logo.svg",
  alt: "JuliaNative logo",
  resizeMode: "contain",
  style: { width: 160, height: 100 },
});

Image({
  source: "https://example.com/photo.jpg",
  resizeMode: "cover",
  style: { width: 320, height: 200 },
});
```

`resizeMode` accepts `contain` (default), `cover`, or `fill`.

## Video

`Video` renders a native player with play/pause and seek controls. Provide a
local video path or a media URL; autoplay, looping, and muted audio are
optional.
On Windows, `npm run portable` includes the Qt Multimedia and FFmpeg runtime.

```ts
Video({
  source: "assets/demo.mp4",
  autoplay: false,
  loop: true,
  muted: false,
  style: { width: 640, height: 360 },
});
```

## Modal

Use `Modal` for a native modal dialog. Control its visibility with state; closing
the dialog with its close button invokes `onClose`.

```ts
const [visible, setVisible] = useState(false);

Button({ text: "Open", onClick: () => setVisible(true) });
Modal({
  visible,
  title: "Confirmation",
  onClose: () => setVisible(false),
  children: [
    Text("Continue?"),
    Button({ text: "Close", onClick: () => setVisible(false) }),
  ],
});
```

Use `style.cursor` to choose a CSS-style mouse cursor, for example
`"pointer"`, `"text"`, `"wait"`, or `"not-allowed"`.

```ts
Button({
  text: "Click me",
  style: { cursor: "pointer" },
});
```
