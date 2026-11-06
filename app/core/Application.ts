import { VNode } from "./types";
import { render, listenNativeEvents } from "./renderer";
import {
  abortStateRender,
  beginStateRender,
  finishStateRender,
  setStateRenderCallback,
} from "./state";
import { clearButtonCallbacks } from "./components/Button";
import { clearFloatingActionButtonCallbacks } from "./components/FloatingActionButton";
import { clearModalCallbacks } from "./components/Modal";
import { clearTextInputCallbacks } from "./components/TextInput";
import { clearTableCallbacks } from "./components/Table";
import { clearWidgetCallbacks } from "./components/Widget";

// Mengatur ulang console.log untuk mengirim pesan ke stderr.
function setupConsole(): void {
  console.log = (...args: any[]) => {
    console.error(...args);
  };
}

export class Application {
  // sendToNative digunakan untuk mengirim pesan ke runtime native (C++).
  static sendToNative(message: any): void {
    const output = JSON.stringify(message);
    process.stdout.write("__JULIA_UI__" + output + "\n");
  }

  // exit digunakan untuk mengirim perintah keluar ke runtime native (C++).
  static exit(): void {
    Application.sendToNative({ type: "command", command: "exit" });
  }

  // run digunakan untuk menjalankan aplikasi dengan fungsi main yang diberikan.
  static run(main: () => VNode): void {
    setupConsole();

    listenNativeEvents();

    const renderApp = () => {
      beginStateRender();
      clearButtonCallbacks();
      clearFloatingActionButtonCallbacks();
      clearModalCallbacks();
      clearTableCallbacks();
      clearTextInputCallbacks();
      clearWidgetCallbacks();

      try {
        render(main());
        finishStateRender();
      } catch (error) {
        abortStateRender();
        throw error;
      }
    };

    setStateRenderCallback(renderApp);
    renderApp();
  }
}
