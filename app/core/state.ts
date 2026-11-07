export type StateAction<T> = T | ((previousState: T) => T);
export type StateSetter<T> = (action: StateAction<T>) => void;

interface StateScope {
  states: unknown[];
  stateIndex: number;
  expectedStateCount?: number;
}

const rootScope: StateScope = { states: [], stateIndex: 0 };
const stateScopes = new Map<string, StateScope>();
let currentScope = rootScope;
let isRendering = false;
let renderCallback: (() => void) | undefined;
let renderScheduled = false;

export function useState<T>(
  initialState: T | (() => T),
): [T, StateSetter<T>] {
  if (!isRendering) {
    throw new Error("useState must be called while rendering the application.");
  }

  const scope = currentScope;
  const index = scope.stateIndex++;
  if (index >= scope.states.length) {
    scope.states.push(
      typeof initialState === "function"
        ? (initialState as () => T)()
        : initialState,
    );
  }

  const setState: StateSetter<T> = (action) => {
    const previousState = scope.states[index] as T;
    const nextState =
      typeof action === "function"
        ? (action as (previousState: T) => T)(previousState)
        : action;

    if (Object.is(previousState, nextState)) return;

    scope.states[index] = nextState;
    scheduleRender();
  };

  return [scope.states[index] as T, setState];
}

export function beginStateRender(): void {
  rootScope.stateIndex = 0;
  currentScope = rootScope;
  isRendering = true;
}

export function finishStateRender(): void {
  currentScope = rootScope;
  isRendering = false;

  if (
    rootScope.expectedStateCount !== undefined &&
    rootScope.stateIndex !== rootScope.expectedStateCount
  ) {
    throw new Error(
      "The number or order of useState calls changed between renders.",
    );
  }

  rootScope.expectedStateCount = rootScope.stateIndex;
}

export function withStateScope<T>(key: string, render: () => T): T {
  if (!isRendering) {
    throw new Error("State scopes can only be rendered during an application render.");
  }

  let scope = stateScopes.get(key);
  if (!scope) {
    scope = { states: [], stateIndex: 0 };
    stateScopes.set(key, scope);
  }

  const parentScope = currentScope;
  currentScope = scope;
  scope.stateIndex = 0;

  try {
    const result = render();
    if (
      scope.expectedStateCount !== undefined &&
      scope.stateIndex !== scope.expectedStateCount
    ) {
      throw new Error(
        `The number or order of useState calls changed in state scope "${key}" ` +
          `(expected ${scope.expectedStateCount}, received ${scope.stateIndex}).`,
      );
    }
    scope.expectedStateCount = scope.stateIndex;
    return result;
  } finally {
    currentScope = parentScope;
  }
}

export function abortStateRender(): void {
  isRendering = false;
  currentScope = rootScope;
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
