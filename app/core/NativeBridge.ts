declare const process: any;

type PendingRequest = {
  resolve: (value: any) => void;
  reject: (error: Error) => void;
};

const pendingRequests = new Map<string, PendingRequest>();

let requestCounter = 0;

export function requestNative<T = any>(
  command: string,
  data: Record<string, any> = {},
): Promise<T> {
  const requestId = String(++requestCounter);

  return new Promise((resolve, reject) => {
    pendingRequests.set(requestId, {
      resolve,
      reject,
    });

    const message = {
      type: "command",
      command,
      requestId,
      ...data,
    };

    process.stdout.write("__JULIA_UI__" + JSON.stringify(message) + "\n");
  });
}

export function handleNativeResponse(message: any): void {
  if (!message.requestId) return;

  const request = pendingRequests.get(String(message.requestId));

  if (!request) return;

  pendingRequests.delete(String(message.requestId));

  if (message.success === false) {
    request.reject(new Error(message.error || "Native request failed"));
    return;
  }

  request.resolve(message.data);
}
