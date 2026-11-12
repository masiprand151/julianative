export type ToastType = "success" | "error" | "warning" | "info";

export type ToastPosition =
  | "top-left"
  | "top-center"
  | "top-right"
  | "bottom-left"
  | "bottom-center"
  | "bottom-right";

export interface ToastOptions {
  message: string;
  type?: ToastType;
  duration?: number;
  width?: number;
  height?: number;
  position?: ToastPosition;
}

const PREFIX = "__JULIA_UI__";

export const Toast = {
  show(options: ToastOptions): void {
    if (!options.message) return;

    const message = {
      type: "command",
      command: "toast.show",
      message: options.message,
      toastType: options.type ?? "info",
      duration: options.duration ?? 3000,
      position: options.position ?? "bottom-right",
      width: options.width ?? 320,
    };

    process.stdout.write(PREFIX + JSON.stringify(message) + "\n");
  },
};
