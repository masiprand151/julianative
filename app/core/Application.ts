import { VNode } from "./types";
import { render, listenNativeEvents } from "./renderer";

export const Application = {
  run(main: () => VNode): void {
    listenNativeEvents();

    const root = main();

    render(root);
  },
};
