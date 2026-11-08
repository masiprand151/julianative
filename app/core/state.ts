export type StateAction<T> = T | ((previousState: T) => T);
export type StateSetter<T> = (action: StateAction<T>) => void;
export type EffectCleanup = () => void;
export type EffectCallback = () => void | EffectCleanup;

interface EffectRecord {
  callback: EffectCallback;
  dependencies?: readonly unknown[];
  cleanup?: EffectCleanup;
  hasRun: boolean;
}

interface PendingEffect {
  scope: StateScope;
  index: number;
  callback: EffectCallback;
  dependencies?: readonly unknown[];
}

interface StateScope {
  states: unknown[];
  stateIndex: number;
  expectedStateCount?: number;
  effects: EffectRecord[];
  effectIndex: number;
  expectedEffectCount?: number;
  pendingEffects: PendingEffect[];
}

const rootScope: StateScope = {
  states: [],
  stateIndex: 0,
  effects: [],
  effectIndex: 0,
  pendingEffects: [],
};
const stateScopes = new Map<string, StateScope>();
const activeStateScopes = new Set<StateScope>();
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

export function useEffect(
  callback: EffectCallback,
  dependencies?: readonly unknown[],
): void {
  if (!isRendering) {
    throw new Error("useEffect must be called while rendering the application.");
  }

  const scope = currentScope;
  const index = scope.effectIndex++;
  const previous = scope.effects[index];
  const dependenciesChanged =
    !previous ||
    !previous.hasRun ||
    dependencies === undefined ||
    !areDependenciesEqual(previous.dependencies, dependencies);

  if (dependenciesChanged) {
    scope.pendingEffects.push({
      scope,
      index,
      callback,
      dependencies: dependencies ? [...dependencies] : undefined,
    });
  }
}

export function beginStateRender(): void {
  activeStateScopes.clear();
  activeStateScopes.add(rootScope);
  rootScope.pendingEffects = [];
  rootScope.stateIndex = 0;
  rootScope.effectIndex = 0;
  for (const scope of stateScopes.values()) {
    scope.pendingEffects = [];
  }
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
  if (
    rootScope.expectedEffectCount !== undefined &&
    rootScope.effectIndex !== rootScope.expectedEffectCount
  ) {
    throw new Error(
      "The number or order of useEffect calls changed between renders.",
    );
  }

  rootScope.expectedStateCount = rootScope.stateIndex;
  rootScope.expectedEffectCount = rootScope.effectIndex;
  for (const [key, scope] of stateScopes) {
    if (activeStateScopes.has(scope)) {
      if (
        scope.expectedStateCount !== undefined &&
        scope.stateIndex !== scope.expectedStateCount
      ) {
        throw new Error(
          `The number or order of useState calls changed in state scope "${key}" ` +
            `(expected ${scope.expectedStateCount}, received ${scope.stateIndex}).`,
        );
      }
      if (
        scope.expectedEffectCount !== undefined &&
        scope.effectIndex !== scope.expectedEffectCount
      ) {
        throw new Error(
          `The number or order of useEffect calls changed in state scope "${key}" ` +
            `(expected ${scope.expectedEffectCount}, received ${scope.effectIndex}).`,
        );
      }
      scope.expectedStateCount = scope.stateIndex;
      scope.expectedEffectCount = scope.effectIndex;
    }
  }

  const effectsToRun = [...rootScope.pendingEffects];
  for (const scope of activeStateScopes) {
    if (scope !== rootScope) effectsToRun.push(...scope.pendingEffects);
  }
  const cleanups: EffectCleanup[] = [];
  for (const scope of [rootScope, ...stateScopes.values()]) {
    if (activeStateScopes.has(scope)) continue;
    for (const effect of scope.effects) {
      if (effect.cleanup) cleanups.push(effect.cleanup);
      effect.cleanup = undefined;
      effect.hasRun = false;
    }
  }
  for (const pending of effectsToRun) {
    const previous = pending.scope.effects[pending.index];
    if (previous?.cleanup) cleanups.push(previous.cleanup);
  }

  currentScope = rootScope;
  isRendering = false;
  for (const cleanup of cleanups) runEffectCleanup(cleanup);
  for (const pending of effectsToRun) {
    const effect: EffectRecord = {
      callback: pending.callback,
      dependencies: pending.dependencies,
      hasRun: true,
    };
    pending.scope.effects[pending.index] = effect;
    try {
      const cleanup = pending.callback();
      if (typeof cleanup === "function") effect.cleanup = cleanup;
    } catch (error) {
      console.error("useEffect callback error:", error);
    }
  }
}

export function withStateScope<T>(key: string, render: () => T): T {
  if (!isRendering) {
    throw new Error("State scopes can only be rendered during an application render.");
  }

  let scope = stateScopes.get(key);
  if (!scope) {
    scope = {
      states: [],
      stateIndex: 0,
      effects: [],
      effectIndex: 0,
      pendingEffects: [],
    };
    stateScopes.set(key, scope);
  }

  activeStateScopes.add(scope);
  const parentScope = currentScope;
  currentScope = scope;
  scope.stateIndex = 0;
  scope.effectIndex = 0;
  scope.pendingEffects = [];

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
    if (
      scope.expectedEffectCount !== undefined &&
      scope.effectIndex !== scope.expectedEffectCount
    ) {
      throw new Error(
        `The number or order of useEffect calls changed in state scope "${key}" ` +
          `(expected ${scope.expectedEffectCount}, received ${scope.effectIndex}).`,
      );
    }
    scope.expectedStateCount = scope.stateIndex;
    scope.expectedEffectCount = scope.effectIndex;
    return result;
  } finally {
    currentScope = parentScope;
  }
}

export function abortStateRender(): void {
  rootScope.pendingEffects = [];
  for (const scope of activeStateScopes) scope.pendingEffects = [];
  isRendering = false;
  currentScope = rootScope;
}

export function setStateRenderCallback(callback: () => void): void {
  renderCallback = callback;
}

export function requestRender(): void {
  scheduleRender();
}

function areDependenciesEqual(
  previous: readonly unknown[] | undefined,
  next: readonly unknown[] | undefined,
): boolean {
  if (!previous || !next || previous.length !== next.length) return false;
  return previous.every((value, index) => Object.is(value, next[index]));
}

function runEffectCleanup(cleanup: EffectCleanup): void {
  try {
    cleanup();
  } catch (error) {
    console.error("useEffect cleanup error:", error);
  }
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
