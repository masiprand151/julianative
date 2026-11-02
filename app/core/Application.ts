import { VNode } from "./types";
import { render, listenNativeEvents } from "./renderer";

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

    const root = main();

    render(root);
  }
}
