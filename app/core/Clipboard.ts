import { requestNative } from "./NativeBridge";

export class Clipboard {
  static getText(): Promise<string> {
    return requestNative("clipboard.getText");
  }

  static setText(text: string): Promise<boolean> {
    return requestNative("clipboard.setText", { text });
  }

  static clear(): Promise<boolean> {
    return requestNative("clipboard.clear");
  }
}
