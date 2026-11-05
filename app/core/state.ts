export type StateAction<T> = T | ((previousState: T) => T);
export type StateSetter<T> = (action: StateAction<T>) => void;

let states: unknown[] = [];
let stateIndex = 0;
let expectedStateCount: number | undefined;
let isRendering = false;
let renderCallback: (() => void) | undefined;
let renderScheduled = false;

export function useState<T>(
  initialState: T | (() => T),
): [T, StateSetter<T>] {
  if (!isRendering) {
    throw new Error("useState must be called while rendering the application.");
  }

  const index = stateIndex++;
  if (index >= states.length) {
    states.push(
      typeof initialState === "function"
        ? (initialState as () => T)()
        : initialState,
    );
  }

  const setState: StateSetter<T> = (action) => {
    const previousState = states[index] as T;
    const nextState =
      typeof action === "function"
        ? (action as (previousState: T) => T)(previousState)
        : action;

    if (Object.is(previousState, nextState)) return;

    states[index] = nextState;
    scheduleRender();
  };

  return [states[index] as T, setState];
}

export function beginStateRender(): void {
  stateIndex = 0;
  isRendering = true;
}

export function finishStateRender(): void {
  isRendering = false;

  if (expectedStateCount !== undefined && stateIndex !== expectedStateCount) {
    throw new Error(
      "The number or order of useState calls changed between renders.",
    );
  }

  expectedStateCount = stateIndex;
}

export function abortStateRender(): void {
  isRendering = false;
}

export function setStateRenderCallback(callback: () => void): void {
  renderCallback = callback;
}

function scheduleRender(): void {
  if (renderScheduled) return;
  renderScheduled = true;

  queueMicrotask(() => {
    renderScheduled = false;
    if (renderCallback) {
      renderCallback();
    }
  });
}
