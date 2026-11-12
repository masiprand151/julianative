// const { spawn } = require("node:child_process");
// const path = require("node:path");

// const root = process.cwd();

// const nativeApp = spawn(
//   path.join(root, "packages", "build", "JuliaNative.exe"),
//   ["--dev"],
//   { cwd: root, stdio: "inherit" },
// );

// const nodemon = spawn(
//   process.execPath,
//   [path.join(root, "node_modules", "nodemon", "bin", "nodemon.js")],
//   { cwd: root, stdio: "inherit" },
// );

// let stopping = false;

// function stop(exitCode = 0) {
//   if (stopping) return;
//   stopping = true;
//   process.exitCode = exitCode;
//   nativeApp.kill();
//   nodemon.kill();
// }

// nativeApp.on("error", (error) => {
//   console.error("Could not start JuliaNative:", error);
//   stop(1);
// });
// nativeApp.on("exit", (code) => {
//   if (!stopping) stop(code ?? 1);
// });
// nodemon.on("error", (error) => {
//   console.error("Could not start nodemon:", error);
//   stop(1);
// });
// nodemon.on("exit", (code) => {
//   if (!stopping) stop(code ?? 1);
// });

// process.on("SIGINT", () => stop());
// process.on("SIGTERM", () => stop());

const { spawn } = require("node:child_process");
const path = require("node:path");
const fs = require("node:fs");

const root = path.resolve(__dirname, "..");

const nativePath = path.join(root, "packages", "build", "JuliaNative.exe");

const nodemonPath = path.join(
  root,
  "node_modules",
  "nodemon",
  "bin",
  "nodemon.js",
);

const nodemonConfig = path.join(root, "nodemon.json");
const entryFile = path.join(root, "src", "main.ts");

if (!fs.existsSync(nativePath)) {
  throw new Error("JuliaNative.exe tidak ditemukan.");
}

if (!fs.existsSync(nodemonConfig)) {
  throw new Error("nodemon.json tidak ditemukan.");
}

if (!fs.existsSync(entryFile)) {
  throw new Error("src/main.ts tidak ditemukan.");
}

let stopping = false;
let nativeApp;
let nodemon;

function stop(exitCode = 0) {
  if (stopping) return;

  stopping = true;
  process.exitCode = exitCode;

  console.log("\n[JuliaNative] Stopping development mode...");

  nativeApp?.kill();
  nodemon?.kill();
}

// Start native application.
nativeApp = spawn(nativePath, ["--dev"], {
  cwd: root,
  stdio: "inherit",
  windowsHide: false,
});

// Start Nodemon watcher.
nodemon = spawn(
  process.execPath,
  [nodemonPath, "--config", nodemonConfig, entryFile],
  {
    cwd: root,
    stdio: "inherit",
    windowsHide: false,
  },
);

// Native events.
nativeApp.on("error", (error) => {
  console.error("[JuliaNative] Native error:", error);
  stop(1);
});

nativeApp.on("exit", (code) => {
  if (!stopping) {
    console.log("[JuliaNative] Native application exited.");
    stop(code ?? 1);
  }
});

// Nodemon events.
nodemon.on("error", (error) => {
  console.error("[JuliaNative] Nodemon error:", error);
  stop(1);
});

nodemon.on("exit", (code) => {
  if (!stopping) {
    console.log("[JuliaNative] Nodemon exited.");
    stop(code ?? 1);
  }
});

// Process events.
process.on("SIGINT", () => stop(0));
process.on("SIGTERM", () => stop(0));
