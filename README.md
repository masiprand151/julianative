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

Use `style.cursor` to choose a CSS-style mouse cursor, for example
`"pointer"`, `"text"`, `"wait"`, or `"not-allowed"`.

```ts
Button({
  text: "Click me",
  style: { cursor: "pointer" },
});
```
