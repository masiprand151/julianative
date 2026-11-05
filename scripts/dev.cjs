const { spawn } = require("node:child_process");
const path = require("node:path");

const root = path.resolve(__dirname, "..");
const nativeApp = spawn(
  path.join(root, "app", "build", "JuliaNative.exe"),
  ["--dev"],
  { cwd: root, stdio: "inherit" },
);
const nodemon = spawn(
  process.execPath,
  [path.join(root, "node_modules", "nodemon", "bin", "nodemon.js")],
  { cwd: root, stdio: "inherit" },
);

let stopping = false;

function stop(exitCode = 0) {
  if (stopping) return;
  stopping = true;
  process.exitCode = exitCode;
  nativeApp.kill();
  nodemon.kill();
}

nativeApp.on("error", (error) => {
  console.error("Could not start JuliaNative:", error);
  stop(1);
});
nativeApp.on("exit", (code) => {
  if (!stopping) stop(code ?? 1);
});
nodemon.on("error", (error) => {
  console.error("Could not start nodemon:", error);
  stop(1);
});
nodemon.on("exit", (code) => {
  if (!stopping) stop(code ?? 1);
});

process.on("SIGINT", () => stop());
process.on("SIGTERM", () => stop());
