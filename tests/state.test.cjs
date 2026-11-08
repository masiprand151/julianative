const assert = require("node:assert/strict");
const { test } = require("node:test");

const {
  beginStateRender,
  finishStateRender,
  useEffect,
  withStateScope,
} = require("../app/build/app/core/state.js");

test("useEffect runs after render, tracks dependencies, and cleans up", () => {
  const calls = [];
  const render = (dependency, includeScope = true) => {
    beginStateRender();
    if (includeScope) {
      withStateScope("use-effect-lifecycle-test", () => {
        useEffect(() => {
          calls.push(`run:${dependency}`);
          return () => calls.push(`cleanup:${dependency}`);
        }, [dependency]);
      });
    }
    finishStateRender();
  };

  render("first");
  assert.deepEqual(calls, ["run:first"]);
  render("first");
  assert.deepEqual(calls, ["run:first"]);
  render("second");
  assert.deepEqual(calls, ["run:first", "cleanup:first", "run:second"]);
  render("second", false);
  assert.deepEqual(calls, [
    "run:first",
    "cleanup:first",
    "run:second",
    "cleanup:second",
  ]);
  render("second");
  assert.deepEqual(calls, [
    "run:first",
    "cleanup:first",
    "run:second",
    "cleanup:second",
    "run:second",
  ]);
});

test("useEffect without dependencies runs after every render", () => {
  const calls = [];
  const render = () => {
    beginStateRender();
    withStateScope("use-effect-no-deps-test", () => {
      useEffect(() => calls.push("effect"));
    });
    finishStateRender();
  };

  render();
  render();
  assert.deepEqual(calls, ["effect", "effect"]);
});
