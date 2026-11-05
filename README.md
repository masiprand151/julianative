# julianative

## System Requirements

To get started with this project, ensure you have the following tools and libraries:

- CMake (Version 3.14 or later): Essential for building and managing the project files.
- C++ Compiler: Compatible with Clang, GCC, or MSVC. Choose one based on your development environment.
- GTK3 Development Libraries (for Linux users): Necessary for GUI development on Linux platforms.

## TextInput

`TextInput` supports `text`, `password`, `number`, and `email` input types.
The optional `onChange` callback receives the current value as a string.

```ts
TextInput({
  type: "email",
  placeholder: "Email",
  onChange: (value) => console.log(value),
});
```
