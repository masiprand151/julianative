const assert = require("node:assert/strict");
const { test } = require("node:test");

const { listenNativeEvents } = require("../app/build/app/core/renderer.js");
const { LocalStorage } = require("../app/build/app/core/localStorage.js");

let receiveNativeData;
const originalOn = process.stdin.on;
const originalSetEncoding = process.stdin.setEncoding;
process.stdin.on = (event, callback) => {
  if (event === "data") receiveNativeData = callback;
  return process.stdin;
};
process.stdin.setEncoding = () => process.stdin;
listenNativeEvents();
process.stdin.on = originalOn;
process.stdin.setEncoding = originalSetEncoding;

async function exerciseStorageRequest(operation) {
  const messages = [];
  const originalWrite = process.stdout.write;
  process.stdout.write = (chunk) => {
    messages.push(String(chunk));
    return true;
  };
  try {
    const result = operation();
    assert.equal(messages.length, 1);
    const message = JSON.parse(messages[0].replace("__JULIA_UI__", ""));
    return { message, result };
  } finally {
    process.stdout.write = originalWrite;
  }
}

function respond(request, response) {
  receiveNativeData(
    `${JSON.stringify({
      type: "storageResponse",
      requestId: request.requestId,
      success: true,
      ...response,
    })}\n`,
  );
}

test("LocalStorage reads JSON values and returns undefined for missing keys", async () => {
  const { message, result } = await exerciseStorageRequest(() =>
    LocalStorage.getItem("profile"),
  );
  assert.equal(message.operation, "get");
  assert.equal(message.key, "profile");
  respond(message, {
    found: true,
    value: { name: "Ada", tags: ["math", "code"] },
  });
  assert.deepEqual(await result, { name: "Ada", tags: ["math", "code"] });

  const missing = await exerciseStorageRequest(() =>
    LocalStorage.getItem("missing"),
  );
  respond(missing.message, { found: false });
  assert.equal(await missing.result, undefined);
});

test("LocalStorage writes JSON values and supports remove and clear", async () => {
  const set = await exerciseStorageRequest(() =>
    LocalStorage.setItem("settings", { theme: "dark", enabled: true }),
  );
  assert.equal(set.message.operation, "set");
  assert.deepEqual(set.message.value, { theme: "dark", enabled: true });
  respond(set.message, {});
  await set.result;

  for (const [operation, expected] of [
    [() => LocalStorage.removeItem("settings"), "remove"],
    [() => LocalStorage.clear(), "clear"],
  ]) {
    const request = await exerciseStorageRequest(operation);
    assert.equal(request.message.operation, expected);
    respond(request.message, {});
    await request.result;
  }
});

test("LocalStorage rejects native persistence errors and non-JSON values", async () => {
  const request = await exerciseStorageRequest(() =>
    LocalStorage.getItem("profile"),
  );
  receiveNativeData(
    `${JSON.stringify({
      type: "storageResponse",
      requestId: request.message.requestId,
      success: false,
      error: "disk unavailable",
    })}\n`,
  );
  await assert.rejects(request.result, /disk unavailable/);
  await assert.rejects(
    LocalStorage.setItem("invalid", undefined),
    /JSON serializable/,
  );
});
