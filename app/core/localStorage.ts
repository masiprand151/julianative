declare const process: any;

export type JsonValue =
  | null
  | boolean
  | number
  | string
  | JsonValue[]
  | { [key: string]: JsonValue };

type StorageResult = {
  found?: boolean;
  value?: JsonValue;
};

type PendingRequest = {
  resolve: (value: StorageResult) => void;
  reject: (error: Error) => void;
};

const pendingRequests = new Map<string, PendingRequest>();
let requestCounter = 0;

function requestStorage(
  operation: "get" | "set" | "remove" | "clear",
  key?: string,
  value?: JsonValue,
): Promise<StorageResult> {
  const requestId = `local_storage_${++requestCounter}`;
  const message: Record<string, unknown> = {
    type: "storageRequest",
    requestId,
    operation,
  };
  if (key !== undefined) message.key = key;
  if (operation === "set") message.value = value;

  return new Promise((resolve, reject) => {
    pendingRequests.set(requestId, { resolve, reject });
    try {
      process.stdout.write(
        "__JULIA_UI__" + JSON.stringify(message) + "\n",
      );
    } catch (error) {
      pendingRequests.delete(requestId);
      reject(error instanceof Error ? error : new Error(String(error)));
    }
  });
}

export function handleStorageResponse(message: {
  requestId?: string;
  success?: boolean;
  found?: boolean;
  value?: JsonValue;
  error?: string;
}): void {
  const requestId = message.requestId;
  if (!requestId) return;

  const pending = pendingRequests.get(requestId);
  if (!pending) return;
  pendingRequests.delete(requestId);

  if (message.success !== true) {
    pending.reject(
      new Error(message.error || "Local storage request failed."),
    );
    return;
  }
  pending.resolve({ found: message.found, value: message.value });
}

export const LocalStorage = {
  getItem<T extends JsonValue = JsonValue>(
    key: string,
  ): Promise<T | undefined> {
    return requestStorage("get", key).then((result) =>
      result.found ? (result.value as T) : undefined,
    );
  },

  setItem(key: string, value: JsonValue): Promise<void> {
    let serialized: string | undefined;
    try {
      serialized = JSON.stringify(value);
    } catch (error) {
      return Promise.reject(
        error instanceof Error ? error : new Error(String(error)),
      );
    }
    if (serialized === undefined) {
      return Promise.reject(
        new TypeError("LocalStorage values must be JSON serializable."),
      );
    }

    return requestStorage("set", key, JSON.parse(serialized)).then(
      () => undefined,
    );
  },

  removeItem(key: string): Promise<void> {
    return requestStorage("remove", key).then(() => undefined);
  },

  clear(): Promise<void> {
    return requestStorage("clear").then(() => undefined);
  },
};
