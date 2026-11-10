// import { VNode } from "./types";
// import { render, listenNativeEvents } from "./renderer";
// import {
//   abortStateRender,
//   beginStateRender,
//   finishStateRender,
//   setStateRenderCallback,
// } from "./state";
// import {
//   clearButtonCallbacks,
//   resetButtonEventCounter,
// } from "./components/Button";
// import {
//   clearFloatingActionButtonCallbacks,
//   resetFloatingActionButtonEventCounter,
// } from "./components/FloatingActionButton";
// import {
//   clearModalCallbacks,
//   resetModalEventCounter,
// } from "./components/Modal";
// import {
//   clearTextInputCallbacks,
//   resetTextInputEventCounter,
// } from "./components/TextInput";
// import {
//   clearTableCallbacks,
//   resetTableEventCounter,
// } from "./components/Table";
// import {
//   clearWidgetCallbacks,
//   resetWidgetEventCounter,
// } from "./components/Widget";

// // Mengatur ulang console.log untuk mengirim pesan ke stderr.
// function setupConsole(): void {
//   console.log = (...args: any[]) => {
//     console.error(...args);
//   };
// }

// export class Application {
//   // sendToNative digunakan untuk mengirim pesan ke runtime native (C++).
//   static sendToNative(message: any): void {
//     const output = JSON.stringify(message);
//     process.stdout.write("__JULIA_UI__" + output + "\n");
//   }

//   // exit digunakan untuk mengirim perintah keluar ke runtime native (C++).
//   static exit(): void {
//     Application.sendToNative({ type: "command", command: "exit" });
//   }

//   static screens() {
//     const s = Application.sendToNative({ type: "command", command: "screens" });
//     console.log("screens", s);
//   }

//   // run digunakan untuk menjalankan aplikasi dengan fungsi main yang diberikan.
//   static run(main: () => VNode): void {
//     setupConsole();

//     listenNativeEvents();

//     const renderApp = () => {
//       beginStateRender();
//       clearButtonCallbacks();
//       clearFloatingActionButtonCallbacks();
//       clearModalCallbacks();
//       clearTableCallbacks();
//       clearTextInputCallbacks();
//       clearWidgetCallbacks();
//       resetButtonEventCounter();
//       resetFloatingActionButtonEventCounter();
//       resetModalEventCounter();
//       resetTextInputEventCounter();
//       resetTableEventCounter();
//       resetWidgetEventCounter();

//       try {
//         render(main());
//         finishStateRender();
//       } catch (error) {
//         abortStateRender();
//         throw error;
//       }
//     };

//     setStateRenderCallback(renderApp);
//     renderApp();
//   }
// }

import { VNode } from "./types";
import { render, listenNativeEvents } from "./renderer";

import {
  abortStateRender,
  beginStateRender,
  finishStateRender,
  setStateRenderCallback,
} from "./state";

import {
  clearButtonCallbacks,
  resetButtonEventCounter,
} from "./components/Button";

import {
  clearFloatingActionButtonCallbacks,
  resetFloatingActionButtonEventCounter,
} from "./components/FloatingActionButton";

import {
  clearModalCallbacks,
  resetModalEventCounter,
} from "./components/Modal";

import {
  clearTextInputCallbacks,
  resetTextInputEventCounter,
} from "./components/TextInput";

import {
  clearTableCallbacks,
  resetTableEventCounter,
} from "./components/Table";

import {
  clearWidgetCallbacks,
  resetWidgetEventCounter,
} from "./components/Widget";
import { System } from "./system";
import {
  clearCheckboxCallbacks,
  resetCheckboxEventCounter,
} from "./components";
import {
  clearSwitchCallbacks,
  resetSwitchEventCounter,
} from "./components/Switch";

export interface Screen {
  id: number;
  name: string;
  width: number;
  height: number;
  x: number;
  y: number;
  availableWidth: number;
  availableHeight: number;
  primary: boolean;
  scaleFactor: number;
}

type PendingRequest = {
  resolve: (value: any) => void;
  reject: (error: Error) => void;
};

export class Application {
  private static requestCounter = 0;

  private static pendingRequests = new Map<string, PendingRequest>();

  static system = System;

  static sendToNative(message: any): void {
    const output = JSON.stringify(message);

    process.stdout.write("__JULIA_UI__" + output + "\n");
  }

  static requestNative<T>(command: string): Promise<T> {
    const requestId = String(++this.requestCounter);

    return new Promise<T>((resolve, reject) => {
      this.pendingRequests.set(requestId, {
        resolve,
        reject,
      });

      this.sendToNative({
        type: "command",
        command,
        requestId,
      });
    });
  }

  static handleNativeResponse(message: any): void {
    if (message.type !== "screensResponse") return;

    const request = this.pendingRequests.get(message.requestId);

    if (!request) return;

    this.pendingRequests.delete(message.requestId);

    if (message.success === false) {
      request.reject(new Error(message.error));
      return;
    }

    request.resolve(message.screens);
  }

  static getScreens(): Promise<Screen[]> {
    return this.requestNative<Screen[]>("screens");
  }

  static async getPrimaryScreen(): Promise<Screen | undefined> {
    const screens = await this.getScreens();

    return screens.find((screen) => screen.primary);
  }

  static exit(): void {
    this.sendToNative({
      type: "command",
      command: "exit",
    });
  }

  static run(main: () => VNode): void {
    console.log = (...args: any[]) => {
      console.error(...args);
    };

    listenNativeEvents();

    const renderApp = () => {
      beginStateRender();

      clearButtonCallbacks();
      clearFloatingActionButtonCallbacks();
      clearModalCallbacks();
      clearTableCallbacks();
      clearCheckboxCallbacks();
      clearTextInputCallbacks();
      clearWidgetCallbacks();
      clearSwitchCallbacks();

      resetButtonEventCounter();
      resetFloatingActionButtonEventCounter();
      resetCheckboxEventCounter();

      resetModalEventCounter();
      resetTextInputEventCounter();
      resetTableEventCounter();
      resetWidgetEventCounter();
      resetSwitchEventCounter();

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
