const fs = require("node:fs");
const path = require("node:path");

const signalPath = path.resolve(
  __dirname,
  "..",
  "packages",
  "build",
  ".dev-reload",
);

if (!fs.existsSync(signalPath)) {
  throw new Error("Development app is not running; reload signal is missing.");
}

fs.writeFileSync(signalPath, String(Date.now()));
